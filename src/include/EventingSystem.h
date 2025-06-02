/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * EventingSystem.h
 */

#pragma once

// Need to include these otherwise errors occur
#ifndef IORING_OP_READV_FIXED
#define IORING_OP_READV_FIXED   3
#define IORING_OP_WRITEV_FIXED  4
#define IORING_OP_BIND          24
#define IORING_OP_LISTEN        25
#define IORING_OP_EPOLL_WAIT    26
#define IORING_OP_FTRUNCATE     47
#define IORING_RECVSEND_BUNDLE  (1 << 15)
#endif

#include <linux/io_uring.h>
#include <liburing.h>
#include "Bwss.h"

namespace bwss::es {
  constexpr int bufferSize = 4096;

  constexpr int entries = 1024;
  inline io_uring ring;

  void setup();
  void shutdown();

  bool addRead(Connection* connection) INTERNAL;
  bool addWrite(Connection* connection, const std::string& response) INTERNAL;

  void addAcceptSetup() INTERNAL;
  void addAccept(const int& fd) INTERNAL;
}

