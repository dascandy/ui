#include "process.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

Process::~Process() {

}

Process&& Process::in(std::filesystem::path path) && {
  infd = open(path.c_str(), O_RDONLY);
  if (infd == -1) throw std::runtime_error("Could not open input stream for reading");
  return std::move(*this);
}

Process&& Process::in(std::span<const uint8_t>& data) && {
  int fd_pair[2];
  pipe2(fd_pair, O_NONBLOCK);
  infd = fd_pair[0];
  inpipe = fd_pair[1];
  indata = data;
  return std::move(*this);
}

Process&& Process::out(std::filesystem::path path) && {
  outfd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
  if (outfd == -1) throw std::runtime_error("Could not open output stream for writing");
  return std::move(*this);
}

Process&& Process::out(std::vector<uint8_t>& data) && {
  int fd_pair[2];
  pipe2(fd_pair, O_NONBLOCK);
  outpipe = fd_pair[0];
  outfd = fd_pair[1];
  outdata = &data;
  return std::move(*this);
}

Process&& Process::err(std::filesystem::path path) && {
  errfd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
  if (errfd == -1) throw std::runtime_error("Could not open error stream for writing");
  return std::move(*this);
}

Process&& Process::err(std::vector<uint8_t>& data) && {
  int fd_pair[2];
  pipe2(fd_pair, O_NONBLOCK);
  errpipe = fd_pair[0];
  errfd = fd_pair[1];
  errdata = &data;
  return std::move(*this);
}

int RunController(pid_t pid, Process& process) {
  int status;
  int id = waitpid(pid, &status, WNOHANG);
  while (id == 0) {
    fd_set r, w;

    FD_ZERO(&w);
    FD_ZERO(&r);
    Process* current = &process;
    while (current) {
      if (current->inpipe != -1) FD_SET(current->inpipe, &w);
      if (current->outpipe != -1) FD_SET(current->outpipe, &r);
      if (current->errpipe != -1) FD_SET(current->errpipe, &r);
      current = current->prev;
    }
    timeval timeout = {1, 0};
    int rv = select(1023, &r, &w, nullptr, &timeout);
    (void)rv;
    
    current = &process;
    while (current) {
      if (current->inpipe >= 0 && FD_ISSET(current->inpipe, &w)) {
        int written = write(current->inpipe, current->indata.data() + current->inoffset, current->indata.size() - current->inoffset);
        if (written >= 0) {
          current->inoffset += written;
          if (current->inoffset == current->indata.size()) {
            close(current->inpipe);
          }
        } else {
          // Some error
        }
      }
      if (current->outpipe >= 0 && FD_ISSET(current->outpipe, &r)) {
        int bytesread = 0;
        do {
          current->outdata->resize(current->outoffset + 16384);
          bytesread = read(current->outpipe, current->outdata->data() + current->outoffset, 16384);
          if (bytesread >= 0) {
            current->outoffset += bytesread;
          } else {
            // Some error
          }
        } while (bytesread == 16384);
      }
      if (current->errpipe >= 0 && FD_ISSET(current->errpipe, &r)) {
        int bytesread = 0;
        do {
          current->errdata->resize(current->erroffset + 16384);
          bytesread = read(current->errpipe, current->errdata->data() + current->erroffset, 16384);
          if (bytesread >= 0) {
            current->erroffset += bytesread;
          } else {
            // Some error
          }
        } while (bytesread == 16384);
      }
      current = current->prev;
    }
    id = waitpid(pid, &status, WNOHANG);
  }
  return status;
}

[[noreturn]] void RunProcess(Process& process) {
  if (process.prev) {
    int p[2];
    pipe(p);
    pid_t newPid = fork();
    if (newPid == 0) {
      close(p[0]);
      process.prev->outfd = p[1];
      RunProcess(*process.prev);
    } else {
      process.infd = p[0];
      close(p[1]);
    }
  }

  std::vector<const char*> argv;
  argv.push_back(process.command.c_str());
  for (auto& arg : process.commandLine) argv.push_back(arg.c_str());
  argv.push_back(nullptr);

  if (process.infd != -1) {
    dup2(process.infd, 0);
  } else {
    close(0);
  }

  if (process.outfd != -1) {
    dup2(process.outfd, 1);
  } else {
    close(1);
  }

  if (process.errfd != -1) {
    dup2(process.errfd, 2);
  } else {
    close(2);
  }

  for (size_t n = 3; n < 1024; n++) {
    close(n);
  }

  execvp(argv[0], (char* const*)argv.data());
  std::abort();
}

int Run(Process&& process) {
  pid_t newPid = fork();
  if (newPid) {
    return RunController(newPid, process);
  } else {
    RunProcess(process);
  }
}


