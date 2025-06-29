/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * Network.cpp
 */

#include "Network.h"
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include "Bwss.h"
#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <netinet/tcp.h>

std::pair<bool, int32_t> bwss::network::sock::setCongestionControl(const int32_t& sock) noexcept {
  constexpr const char* bbr = "bbr";
  constexpr const char* cubic = "cubic";
  constexpr const char* reno = "reno";

  if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, bbr, strlen(bbr)) == 0) {
    return { true, errno };
  }
  if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, cubic, strlen(cubic)) == 0) {
    return { true, errno };
  }
  if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, reno, strlen(reno)) == 0) {
    return { true, errno };
  }

  return { false, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::setDeferAcceptTimeout(const int32_t& sock) noexcept {
  if (setsockopt(sock, IPPROTO_TCP, TCP_DEFER_ACCEPT, &deferAcceptTimeout, sizeof(deferAcceptTimeout)) == -1) {
    return { false, errno };
  }

  return { true, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::setNoDelay(const int32_t& sock) noexcept {
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &enableNoDelay, sizeof(enableNoDelay)) == -1) {
    return { false, errno };
  }

  return { true, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::setKeepAlive(const int32_t& sock) noexcept {
  if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &enableTCPKeepAlive, sizeof(enableTCPKeepAlive)) == -1) {
    return { false, errno };
  }

  /*
   * Three system calls are avoided here, as their default values can be changed in /etc/sysctl.conf
   *
   * net.ipv4.tcp_keepalive_time = N    // N amount of seconds before keep-alive probes start
   * net.ipv4.tcp_keepalive_intvl = N   // N amount of seconds between of intervals between keep-alive probes
   * net.ipv4.tcp_keepalive_probes = N  // N amount of probes that can fail before connection is considered dead
   */

  return { true, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::setFastOpen(const int32_t& sock) noexcept {
  if (setsockopt(sock, IPPROTO_TCP, TCP_FASTOPEN, &enableTCPFastOpen, sizeof(enableTCPFastOpen)) == -1) {
    return { false, errno };
  }

  return { true, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::setReuse(const int32_t& sock) noexcept {
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddress, sizeof(enableReuseAddress)) == -1) {
    return { false, errno };
  }
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &enableReusePort, sizeof(enableReusePort)) == -1) {
    return { false, errno };
  }

  return { true, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::create() noexcept {
  const int32_t sock = socket(AF_INET6, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  if (sock == -1) {
    return { false, errno };
  }

  return { true, sock };
}

std::pair<bool, int32_t> bwss::network::sock::setDualStack(const int32_t& sock) noexcept {
  constexpr int32_t flag = 0;
  if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &flag, sizeof(flag)) == -1) {
    return { false, errno };
  }

  return { true, 0 };
}

void bwss::network::sock::configureAddress(sockaddr_in6& address, const int32_t& port) noexcept {
  std::memset(&address, 0, sizeof(address));
  address.sin6_family = AF_INET6;
  address.sin6_addr = in6addr_any;
  address.sin6_port = htons(port);
  address.sin6_flowinfo = 0;
  address.sin6_scope_id = 0;
}

std::pair<bool, int32_t> bwss::network::sock::bind(const int32_t& sock, sockaddr_in6& address) noexcept {
  if (bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
    return { false, errno };
  }

  return { true, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::listen(const int32_t& sock) noexcept {
  if (::listen(sock, 0) == -1) {
    return { false, errno };
  }

  return { true, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::close(int32_t& sock) noexcept {
  if (sock != -1) {
    ::close(sock);
    sock = -1;
    return { true, 0 };
  }

  return { false, 0 };
}

std::pair<bool, int32_t> bwss::network::sock::setNonBlocking(const int32_t& sock) noexcept {
  const int32_t flags = fcntl(sock, F_GETFL, 0);
  if (flags == -1) {
    return { false, errno };
  }
  if (fcntl(sock, F_SETFL, flags | O_NONBLOCK)) {
    return { true, errno };
  }

  return { true, 0 };
}