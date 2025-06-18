/**
* Copyright 2025-2025 Shayden Seymour.
 *
 * Loop.cpp
 */

#include "Loop.h"
#include "Bwss.h"
#include "EventingSystem.h"
#include <linux/io_uring.h>
#include <string>
#include <cstring>

#include <iostream> // TMP

void bwss::loop::run() {
  while (true) {
   // Submit and wait for events
   io_uring_submit_and_wait(&es::ring, 1);

   io_uring_cqe* cqe;
   unsigned head;
   unsigned count = 0;

   io_uring_for_each_cqe(&es::ring, head, cqe) {
    count++;
    handleEvent(cqe);
   }

   io_uring_cq_advance(&es::ring, count);
  }
}

/*
 * Event handlers
 */
void bwss::handlers::disconnect(Connection* conn) {
 // Client disconnected
 std::cout << "Connection closed: " << conn->fd << std::endl;
 close(conn->fd);

 conn->decreaseActiveThreads();
 delete[] conn->buffer;
 delete conn;
}

void bwss::handlers::connection(Connection* conn,  const io_uring_cqe* cqe) {
 conn = new Connection{OperationType::READ, cqe->res, new char[4096], };

 std::cout << "New connection: " << conn->fd << std::endl;

 // Queue another accept for future connections
 es::addAcceptSetup();
 es::addRead(conn);
}

void bwss::handlers::message(Connection* conn, const io_uring_cqe* cqe) {
 // Process received data
 const std::string request(conn->buffer, cqe->res);
 std::cout << "Received from " << conn->fd << ": " << request << std::endl;

 // Send response
 const std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world!";
 es::addWrite(conn, response);
}

void bwss::handlers::error(Connection* conn, const io_uring_cqe* cqe) {
 std::cerr << "Error in operation: " << std::strerror(cqe->res) << std::endl;

 if (conn) {
  close(conn->fd);

  conn->decreaseActiveThreads();
  connCleanup::addToMap(conn);
 }
}

/*
 * Main event handler
 */

void bwss::handleEvent(io_uring_cqe* cqe) {
 Connection* conn = reinterpret_cast<Connection*>(cqe->user_data);

 // Check for any errors
 if (cqe->res < 0) {
  handlers::error(conn, cqe);
  return;
 }

 if (!conn) {
   handlers::connection(conn, cqe);
   return;
 }

 switch (conn->type) {
  case OperationType::ACCEPT: { // New connection
   handlers::connection(conn, cqe);
   return;
  }
  case OperationType::READ: {
   if (cqe->res == 0) { // Connection closed
    handlers::disconnect(conn);
  } else { // New message
   handlers::message(conn, cqe);
  }

   break;
  }
  case OperationType::WRITE: {
   // After writing, queue another read
   es::addRead(conn);
   break;
  }
 }
}