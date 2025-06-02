/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * EventingSystem.cpp
 */

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
#include "EventingSystem.h"
#include <liburing.h>
#include "Bwss.h"
#include "Network.h"

void bwss::es::setup() {
  if (io_uring_queue_init(entries, &ring, 0)) {
    terminate(EXIT_FAILURE, "Failed to initialize queue");
    UNREACHABLE;
  }
}

void bwss::es::shutdown() {
  io_uring_queue_exit(&ring);
}

void bwss::es::addAccept(const int& fd) {
  io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    terminate(EXIT_FAILURE, "Failed to get sqe for accept.");
    UNREACHABLE;
  }

  const Connection* data = new Connection{OperationType::ACCEPT, network::sock::httpSocket, nullptr, 0};

  io_uring_prep_accept(sqe, fd, nullptr, nullptr, 0);
  /*
  sqe->opcode = static_cast<uint8_t>(IORING_OP_ACCEPT);
  sqe->fd = fd;
  sqe->off = reinterpret_cast<uint64_t>(nullptr);
  sqe->addr = reinterpret_cast<uint64_t>(nullptr);
  sqe->len = reinterpret_cast<uint64_t>(nullptr);
  sqe->accept_flags = static_cast<uint32_t>(0); */
  sqe->user_data = reinterpret_cast<uint64_t>(data);
}

bool bwss::es::addRead(const int fd) {
  io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    return false;
  }

  Connection* data = new Connection{
    OperationType::READ,
    fd,
    std::make_unique<char[]>(bufferSize),
    bufferSize
  };

  io_uring_prep_recv(sqe, fd, data->buffer.get(), bufferSize, 0);
  sqe->user_data = reinterpret_cast<uint64_t>(data);

  return true;
}

bool bwss::es::addWrite(const int fd, const std::string& response) {
  auto* sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    return false;
  }

  Connection* data = new Connection{
    OperationType::WRITE,
    fd,
    std::make_unique<char[]>(response.size()),
    response.size()
  };

  std::ranges::copy(response, data->buffer.get());
  io_uring_prep_send(sqe, fd, data->buffer.get(), response.size(), 0);
  sqe->user_data = reinterpret_cast<uint64_t>(data);

  return true;
}

void bwss::es::addAcceptSetup() {
  addAccept(network::sock::httpSocket);
  // addAccept(network::sock::httpsSocket);
}

