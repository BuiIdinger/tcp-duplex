/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * BWSS.cpp
 */

#include "BWSS.h"
#include "Network.h"
#include "EventingSystem.h"
#include <string>
#include <iostream>
#include <cstring>

void bwss::terminate(const int& status, const std::string& reason) {
  std::cerr << "Server terminate. Status: " << std::to_string(status) << ". Reason: " << reason << std::flush;
  network::sock::shutdown();
  es::shutdown();
  exit(status);
}

void bwss::handleEvent(io_uring_cqe* cqe) {
  Connection* conn = reinterpret_cast<Connection*>(cqe->user_data);

  if (cqe->res < 0) {
    std::cerr << "Error in operation: " << std::strerror(-cqe->res) << std::endl;
    close(conn->fd);
    delete conn;
    return;
  }

  switch (conn->type) {
    case OperationType::ACCEPT: {
      int clientFd = cqe->res;
      std::cout << "New connection: " << clientFd << std::endl;

      // Queue another accept for future connections
      es::addAcceptSetup();
      es::addRead(clientFd);
      break;
    }
    case OperationType::READ: {
      if (cqe->res == 0) {
        // Client disconnected
        std::cout << "Connection closed: " << conn->fd << std::endl;
        close(conn->fd);
      } else {
        // Process received data
        std::string request(conn->buffer.get(), cqe->res);
        std::cout << "Received from " << conn->fd << ": " << request << std::endl;

        // Send response
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world!";
        es::addWrite(conn->fd, response);
      }
      break;
    }
    case OperationType::WRITE: {
      // After writing, queue another read
      es::addRead(conn->fd);
      break;
    }
  }



  delete conn;
}

void bwss::run() {
  network::sock::create();
  network::sock::allowReuseAddress();
  network::sock::allowDualStackSocket();
  network::sock::configureServerAddresses();
  network::sock::bind();
  network::sock::listen();
}

