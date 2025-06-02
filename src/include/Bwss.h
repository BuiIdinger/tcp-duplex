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
  /*
   * The server can be configured by using the config
   * class
   */
  namespace serverConfig {
      struct Config {
      /*
       * Maximum size in a message, usually this is 16MB in most
       * browsers
       */
      uint32_t bufferSize = 4096;

      /*
       * Server name, will be shown to others on the server
       * header, as well  as get requests, post requests etc.
       */
      std::string serverName = "Buildinger WebSocket Server";

      /*
       * Cleanup delay, delay in seconds on when a cleanup cycle for
       * deallocating old connection memory will occur, don't put this
       * to low otherwise you'll just be causing loops every somewhat
       * milliseconds, generally anything above 10 seconds pretty good.
       */
      uint16_t cleanupDelay = 10000; // Milliseconds
    };

    // Currently configured server config options
    inline Config config INTERNAL;
    inline bool changed = false;

    /*
     * Sets servers options, invoke this with configured server options
     * before before invoking run()
     */
    void set(const Config& options);

    /* Returns true if the server options have been overridden by the set()
     * function
     */
    bool checkIfOverridden() INTERNAL;
  }

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