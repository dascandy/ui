#include <ui/Time.hpp>
#include <sys/time.h>

Val<int64_t> currentTimeMs = val((int64_t)0);
static int64_t startTime = 0;

void updateTime() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  int64_t current = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  if (startTime == 0) {
    startTime = current;
  }
  currentTimeMs = val(current - startTime);
}


