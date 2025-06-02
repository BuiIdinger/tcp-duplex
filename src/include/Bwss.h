/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * BWSS.h
 */

#pragma once

#define INTERNAL_API
#define UNREACHABLE __builtin_unreachable()

namespace bwss {
  void run();
  void terminate(const int& status, const std::string& reason);
}