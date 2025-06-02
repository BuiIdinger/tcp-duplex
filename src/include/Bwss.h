/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * Bwss.h
 */

#pragma once

#include <string>
#include <linux/io_uring.h>
#include <mutex>
#include <atomic>
#include <unordered_set>

#define INTERNAL
#define UNREACHABLE __builtin_unreachable()

namespace bwss {
  enum class OperationType { ACCEPT, READ, WRITE } INTERNAL;

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
    OperationType type INTERNAL;
    int fd INTERNAL;
    char* buffer INTERNAL;
    size_t len INTERNAL;
    std::mutex mutex INTERNAL;

    /*
     * Userdata, anything can pretty much be stored in here, create your
     * own mutex in your own userdata to prevent race conditions, don't
     * use the mutex located in this class, as it's for internal operations.
     *
     * This variable isn't accessed in any internal function, nor will any
     * allocated memory will be deleted, so use the on close handler to delete
     * allocated memory.
     */
    void* userdata;

    /*
     * Active threads tracking
     *
     * This is used in order to prevent a use-after-free vulnerabilities, by
     * tracking the active threads that are currently using this connection.
     *
     * Even in callback functions this should not be decreased, since the connection
     * is still being stored to be passed to that callback function.
     *
     * When the client disconnects or an error occurs, this connection will be only
     * deleted when no active threads using this connection, if threads are still
     * using the connection, its deletion will be delayed until no active threads
     * are using this connection.
     *
     * If your using callback functions or passing this connection to a different
     * thread, create a class and wrap these counters in the constructor and
     * deconstructor, and then just manage the lifetime yourself.
     *
     * By default, there is one active thread, which is the eventing system, without it
     * the connection will delete itself.
     */
    std::atomic<uint64_t> activeThreads = 1;

    // Increases and decreased the active threads
    void incrementActiveThreads();
    void decreaseActiveThreads();
  };

  /*
   * This is an automated way of tracking a connections thread lifetime,
   * usually used when passing a connection to callbacks or passing a
   * connection onto another thread
   *
   * Object for tracking a connections lifetime in a thread, only
   * use this object when passing onto another thread, or using
   * non-blocking functions that may cause the main thread to return.
   */
  struct ThreadTracker {
    Connection* m_conn;

    explicit ThreadTracker(Connection* conn) : m_conn(conn) {
      ++m_conn->activeThreads;
    }

    ~ThreadTracker() {
      --m_conn->activeThreads;
    }
  };

  /*
   * Creates a thread tracker to manage the connection lifetime
   */
  ThreadTracker createThreadTracker(Connection* conn);

  /*
   * Connection cleanup, each connection that is no longer needed
   * because of a disconnect or an error, will be placed into this
   * map. A thread then checks if the connections active threads are
   * zero, if so then we are safe to delete the connection.
   */
  namespace connCleanup {
    inline std::unordered_set<Connection*> conns;
    inline std::mutex mapMutex;

    void addToMap(Connection* conn);
    void removeFromMap(Connection* conn);
    void runCleanup(Connection* conn);
  }

  void handleEvent(io_uring_cqe* cqe);

  /*
   * Run the server, will block the main thread
   */
  void run();

  void terminate(const int& status, const std::string& reason);
}