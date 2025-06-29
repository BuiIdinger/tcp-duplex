/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * Network.h
 */

#pragma once

#include <netinet/in.h>
#include <utility>

namespace bwss::network::sock {
  /*
   * Ports that clients can connect to
   *
   * Don't change these for a public facing server.
   * A users router may have a firewall rule to block non-web
   * ports, causing the connection attempt to fail
   */
  constexpr int32_t httpPort = 80;
  constexpr int32_t httpsPort = 443;

  /*
   * TCP keep-alive
   *
   * This enables keep-alive at the TCP level when there is no data being sent after N
   * amount of seconds, these probes will detect dead connections and alert us.
   *
   * While WebSockets do use custom ping/pong frames, we could still get a TCP connection
   * being dead at the WebSocket upgrade stage. Since we are not upgraded to a WebSocket, we
   * cannot use WebSocket ping/pong frames
   */
  constexpr int32_t enableTCPKeepAlive = true;
  constexpr int32_t idle = 10;      // Start probes after 10 seconds idle
  constexpr int32_t interval = 5;   // Send probes every 5 seconds
  constexpr int32_t maxFailed = 3;  // Fail after 3 unanswered probes

  /*
   * TCP fast open
   *
   * TCP fast open allows data to be sent during the initial SYN packet of the
   * TCP handshake.
   *
   * Normally:        SYN → SYN-ACK → ACK → DATA
   * TCP fast open:   SYN → SYN-ACK → ACK
   *
   * TCP fast open requires the system setting to be enabled
   *
   * No major browsers support TCP fast open anymore as issues with middlebox compatibility
   * and privacy Concerns, so I would suggest TCP fast open should stay off.
   */
  constexpr int32_t enableTCPFastOpen = false;

  /*
   * Reuse
   *
   * Reuse address allows the program to bind to a port that is in a TIME_WAIT state. If your program
   * crashes and attempts to restart without the address reuse, then you'll probably get the
   * 'Address already in use' error.
   *
   * Port reuse allows multiple sockets to bind to the same port simultaneously. This allows for the kernel
   * to load balance incoming connections to different sockets on different threads. This is what allows
   * multithreading to work in BWSS, so don't disable it.
   */
  constexpr int32_t enableReuseAddress = true;
  constexpr int32_t enableReusePort = true;

  /*
   * Dual-stack
   *
   * Dual-stack allows a socket to support both IPV4 and IPV6 clients, rather than needing
   * two sockets, one for IPV4 and IPV6.
   */
  constexpr int32_t enableIPV6Only = false;

  /*
   * No delay
   *
   * TCP sockets by default use Nagle's algorithm to buffer up smaller messages into a larger
   * message to reduce network congestion, and wasted bandwidth. Nagle's algorithm delays messages
   * being sent which can introduce latency.
   *.
   * Enabling no delay will not buffer up any messages, and will send them immediately
   */
  constexpr int32_t enableNoDelay = false;

  /*
   * Defer accept timeout
   *
   * Defer accept timeout will defer the accept wakeup call to the eventing system if the client doesn't
   * send a packet before the timeout.
   */
  constexpr int32_t deferAcceptTimeout = 0;

  std::pair<bool, int32_t> setCongestionControl(const int32_t& sock) noexcept;
  std::pair<bool, int32_t> setDeferAcceptTimeout(const int32_t& sock) noexcept;
  std::pair<bool, int32_t> setNoDelay(const int32_t& sock) noexcept;
  std::pair<bool, int32_t> setKeepAlive(const int32_t& sock) noexcept;
  std::pair<bool, int32_t> setFastOpen(const int32_t& sock) noexcept;
  std::pair<bool, int32_t> create() noexcept;
  std::pair<bool, int32_t> setReuse(const int32_t& sock) noexcept;
  std::pair<bool, int32_t> setDualStack(const int32_t& sock) noexcept;
  void configureAddress(sockaddr_in6& address, const int32_t& port) noexcept;
  std::pair<bool, int32_t> bind(const int32_t& sock, sockaddr_in6& address) noexcept;
  std::pair<bool, int32_t> listen(const int32_t& sock) noexcept;
  std::pair<bool, int32_t> close(int32_t& sock) noexcept;
  std::pair<bool, int32_t> setNonBlocking(const int32_t& fd) noexcept;
}