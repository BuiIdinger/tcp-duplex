/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * BWSS.cpp
 */

#include "BWSS.h"
#include "Network.h"

void bwss::run() {
  network::sock::create();
  network::sock::allowReuseAddress();
  network::sock::allowDualStackSocket();
  network::sock::configureServerAddresses();
  network::sock::bind();
  network::sock::listen();
}

