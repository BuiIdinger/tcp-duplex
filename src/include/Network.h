/**
 * Copyright 2025-2025 Shayden Seymour.
 *
 * Network.h
 */

#pragma once

#include <netinet/in.h>

namespace network {
  namespace sock {
    inline int httpSocket;
    inline int httpsSocket;

    constexpr int httpPort = 80;
    constexpr int httpsPort = 443;

    inline sockaddr_in6 httpSocketAddress;
    inline sockaddr_in6 httpsSocketAddress;

    void create();
    void allowReuseAddress();
    void allowDualStackSocket();
    void configureServerAddresses();
    void bind();
    void listen();
    void shutdown();
  }
}