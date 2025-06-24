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
#include <algorithm>
#include <cstring>
#include <mutex>

void bwss::es::setup() {
  if (io_uring_queue_init(entries, &ring, 0)) {
    terminate(EXIT_FAILURE, "Failed to initialize queue");
    UNREACHABLE;
  }
}

void bwss::es::shutdown() {
  io_uring_queue_exit(&ring);
}

void bwss::es::addAccept(const int fd) {
  io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    terminate(EXIT_FAILURE, "Failed to get sqe for accept.");
    UNREACHABLE;
  }

  io_uring_prep_accept(sqe, fd, nullptr, nullptr, 0);
  /*
  sqe->opcode = static_cast<uint8_t>(IORING_OP_ACCEPT);
  sqe->fd = fd;
  sqe->off = reinterpret_cast<uint64_t>(nullptr);
  sqe->addr = reinterpret_cast<uint64_t>(nullptr);
  sqe->len = reinterpret_cast<uint64_t>(nullptr);
  sqe->accept_flags = static_cast<uint32_t>(0); */
  sqe->user_data = reinterpret_cast<uint64_t>(nullptr);
}

bool bwss::es::addRead(Connection* conn) {
  io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    return false;
  }

  std::scoped_lock guard(conn->mutex);

  conn->type = OperationType::READ;

  io_uring_prep_recv(sqe, conn->fd, conn->buffer, bufferSize, 0);
  sqe->user_data = reinterpret_cast<uint64_t>(conn);

  return true;
}

bool bwss::es::addWrite(Connection* conn, const std::string& response) {
  io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    return false;
  }

  std::scoped_lock guard(conn->mutex);

  memset(conn->buffer,0,bufferSize);
  conn->type = OperationType::WRITE;
  conn->len = std::min(response.size(), static_cast<size_t>(bufferSize));
  std::memcpy(conn->buffer, response.c_str(), conn->len);

  io_uring_prep_send(sqe, conn->fd, conn->buffer, conn->len, 0);
  sqe->user_data = reinterpret_cast<uint64_t>(conn);

  return true;
}

void bwss::es::addAcceptSetup() {
  addAccept(network::sock::httpSocket);
  // addAccept(network::sock::httpsSocket);
}

