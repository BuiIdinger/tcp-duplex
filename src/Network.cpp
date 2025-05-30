/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * Network.cpp
 */

#include "Network.h"
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include "BWSS.h"

void network::sock::allowReuseAddress() {
  static constexpr int reuse = 1;
  if (setsockopt(httpSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
    std::cerr << std::strerror(errno) << std::flush;
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }

  if (setsockopt(httpsSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
    std::cerr << std::strerror(errno) << std::flush;
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }
}

void network::sock::create() {
  httpSocket = socket(AF_INET6, SOCK_STREAM, 0);
  if (httpSocket == -1) {
    std::cerr << std::strerror(errno) << std::flush;
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }

  httpsSocket = socket(AF_INET6, SOCK_STREAM, 0);
  if (httpsSocket == -1) {
    std::cerr << std::strerror(errno) << std::flush;
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }
}

void network::sock::allowDualStackSocket() {
  static constexpr int flag = 1;
  if (setsockopt(httpSocket, IPPROTO_IPV6, IPV6_V6ONLY, &flag, sizeof(flag)) == -1) {
    std::cerr << std::strerror(errno) << std::flush;
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }

  if (setsockopt(httpsSocket, IPPROTO_IPV6, IPV6_V6ONLY, &flag, sizeof(flag)) == -1) {
    std::cerr << std::strerror(errno) << std::flush;
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }
}

static void configureAddress(sockaddr_in6& address, const int& port) {
  memset(&address, 0, sizeof(address));
  address.sin6_family = AF_INET6;
  address.sin6_addr = in6addr_any;
  address.sin6_port = htons(port);
}

void network::sock::configureServerAddresses() {
  configureAddress(httpSocketAddress, httpPort);
  configureAddress(httpsSocketAddress, httpsPort);
}

static bool bindSock(const int sock, sockaddr_in6 address) {
  if (bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
    std::cerr << std::strerror(errno) << std::flush;
    return false;
  }

  return true;
}

void network::sock::bind() {
  if (!bindSock(httpSocket, httpSocketAddress)) {
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }

  if (!bindSock(httpsSocket, httpsSocketAddress)) {
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }
}

static bool listenSock(const int sock) {
  if (::listen(sock, 0) == -1) {
    std::cerr <<  std::strerror(errno) << std::flush;
    return false;
  }

  return true;
}

void network::sock::listen() {
  if (!listenSock(httpSocket)) {
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }

  if (!listenSock(httpsSocket)) {
    exit(EXIT_FAILURE);
    UNREACHABLE;
  }
}