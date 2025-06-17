#include <iostream>
#include <sys/resource.h>
#include "Bwss.h"

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
  enableCoreDumps();

  bwss::serverConfig::Config config;

  config.bufferSize = 4096;

  bwss::serverConfig::set(config);


  bwss::run();
  std::cout << "Fallthrough" << "\n";
}