/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * BWSS.h
 */

#pragma once

#include <string>
#include <memory>
#include <linux/io_uring.h>

#define INTERNAL
#define UNREACHABLE __builtin_unreachable()

namespace bwss {
  enum class OperationType { ACCEPT, READ, WRITE };

  struct Connection {
    OperationType type;
    int fd;
    std::unique_ptr<char[]> buffer;
    size_t len;
  };

  void handleEvent(io_uring_cqe* cqe);

  void run();
  void terminate(const int& status, const std::string& reason);
}