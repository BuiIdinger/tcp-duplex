/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * BWSS.cpp
 */

#include "BWSS.h"
#include "Network.h"
void bwss::terminate(const int& status, const std::string& reason) {
  std::cerr << "Server terminate. Status: " << std::to_string(status) << ". Reason: " << reason << std::flush;
  network::sock::shutdown();
  es::shutdown();
  exit(status);
}

void bwss::run() {
  network::sock::create();
  network::sock::allowReuseAddress();
  network::sock::allowDualStackSocket();
  network::sock::configureServerAddresses();
  network::sock::bind();
  network::sock::listen();
}

