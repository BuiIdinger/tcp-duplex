/**
* Copyright 2025-2025 Shayden Seymour.
 *
 * Loop.h
 */

#pragma once

#include "Bwss.h"

namespace bwss::loop {
  /*
   * Runs the main loop, this will block the current main thread
   */
  void run() INTERNAL;
}