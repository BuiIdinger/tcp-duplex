#include <iostream>
#include <sys/resource.h>
#include "BWSS.h"

void shutdown(const int& status) {
  exit(status);
}

static void enableCoreDumps() {
  rlimit coreLimit{};
  coreLimit.rlim_cur = RLIM_INFINITY;
  coreLimit.rlim_max = RLIM_INFINITY;

  if (setrlimit(RLIMIT_CORE, &coreLimit) != 0) {
    std::cerr << "Unable to make program core dumpfile\n";
  }
}

int main() {
  // Early check to enable core dumps
  enableCoreDumps();

  bwss::run();

  std::cout << "Hello" << "\n";
}