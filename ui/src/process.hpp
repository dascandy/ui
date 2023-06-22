#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <span>
#include <cstdint>

template <typename... Ts>
struct FirstOf;
template <typename T, typename... Ts>
struct FirstOf<T, Ts...> { using type = T; };
template <typename... Ts>
using FirstOf_v = typename FirstOf<Ts...>::type;

struct Process {
  template <typename... Ts>
  Process(std::filesystem::path command, Ts... ts) 
  requires ((sizeof...(Ts) == 0 or sizeof...(Ts) > 1) or std::is_convertible_v<FirstOf_v<Ts...>, std::string>)
  : command(command)
  , commandLine{ts...}
  {
  }
  template <typename Range>
  Process(std::filesystem::path command, Range r) 
  requires (not std::is_convertible_v<Range, std::string>)
  : command(command)
  , commandLine(r)
  {
  }
  ~Process();
  Process&& in(std::filesystem::path path) &&;
  Process&& in(std::span<const uint8_t>& data) &&;
  Process&& out(std::filesystem::path path) &&;
  Process&& out(std::vector<uint8_t>& data) &&;
  Process&& err(std::filesystem::path path) &&;
  Process&& err(std::vector<uint8_t>& data) &&;

  std::string command;
  std::vector<std::string> commandLine;
  int inpipe = -1, outpipe = -1, errpipe = -1;
  int infd = -1, outfd = -1, errfd = -1;
  size_t inoffset = 0, outoffset = 0, erroffset = 0;
  std::span<const uint8_t> indata;
  std::vector<uint8_t>* outdata = nullptr;
  std::vector<uint8_t>* errdata = nullptr;
  Process* prev = nullptr;
  Process&& operator|(Process&& rhs) && {
    rhs.prev = this;
    return std::move(rhs);
  }
};

int Run(Process&& process);


