/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * Bwss.cpp
 */

#include "Bwss.h"
#include "Network.h"
#include "EventingSystem.h"
#include <string>
#include <iostream>
#include <cstring>
#include "Loop.h"

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

  es::setup();
  es::addAcceptSetup();

  loop::run();
}

/*
 * Active thread tracking
 */
void bwss::Connection::incrementActiveThreads() {
  ++activeThreads;
}

void bwss::Connection::decreaseActiveThreads() {
  --activeThreads;
}

bwss::ThreadTracker bwss::createThreadTracker(Connection *conn) {
  return ThreadTracker(conn);
}

/*
 * Connection cleanup
 */
void bwss::connCleanup::addToMap(Connection* conn) {
  std::scoped_lock guard(mapMutex);
  conns.insert(conn);
}

void bwss::connCleanup::removeFromMap(Connection* conn) {
  std::scoped_lock guard(mapMutex);
  conns.erase(conn);
}

/*
 * Server options
 */
void bwss::serverConfig::set(const Config& options) {
 config = options;
}

bool bwss::serverConfig::checkIfOverridden() {
  return changed;
}
