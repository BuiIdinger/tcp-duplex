#pragma once

#include "Bwss.h"
#include <string>

namespace bwss::http {
  /*
   * Gets a wHTTP headers value from a key
   */
  std::string getHeader(const std::string& key, const std::string& request) noexcept;

 std::string calculateSecWebSocketKey(const std::string& secWebsocketKeyHeader);
  void upgradeHttpRequest(Connection* ws, std::string request);
}