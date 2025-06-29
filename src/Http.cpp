#include "Bwss.h"
#include "Http.h"
#include <string>
#include <string_view>
#include <algorithm>
#include "Utilities.h"
#include <optional>
#include <openssl/sha.h>
#include <sstream>

#include <iostream> // TMP

std::string bwss::http::getHeader(const std::string& key, const std::string& request) noexcept {
  std::string keyWithColon = key + ":";
  auto it = std::ranges::search(request, keyWithColon,[](char a, char b) {
    return std::tolower(a) == std::tolower(b);
  }).begin();

  if (it == request.end()) {
    return "";
  }

  it += keyWithColon.size();

  while (it != request.end() && (*it == ' ' || *it == '\t')) {
    ++it;
  }

  const auto lineEnd = std::find_if(it, request.end(), [](char c) {
      return c == '\r' || c == '\n';
  });

  return std::string(it, lineEnd);
}

std::string bwss::http::calculateSecWebSocketKey(const std::string& secWebsocketKeyHeader) {
  std::string secWebSocketAccept = secWebsocketKeyHeader + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

  secWebSocketAccept = utilities::sha1Raw(secWebSocketAccept);
  secWebSocketAccept = utilities::base64(secWebSocketAccept);

  return secWebSocketAccept;
}

void bwss::http::upgradeHttpRequest(Connection* ws, std::string request) {
  /*
   * Request's method must be a GET request, should be the first info in the request
   */
  const std::string_view method = request.substr(0, 3);
  if (method != "GET") {
    ws->close("Invalid HTTP request.");
    return;
  }

  /*
   * Check if this is a valid HTTP request, we check if the request contains
   * the HTTP identifier
   */
  const size_t httpVersionStartPos = request.find("HTTP/");
  if (httpVersionStartPos == std::string::npos) {
    ws->close("Invalid HTTP request.");
    return;
  }

  /*
   * Find the first line ending position, this is determinate by \r\n
   */
  const size_t requestLineEndPos = request.find("\r\n");
  if (requestLineEndPos == std::string_view::npos) {
    ws->close("Invalid HTTP request.");
    return;
  }

  /*
   * Get the HTTP version
   */
  const std::string httpVersionStr = request.substr(httpVersionStartPos + 5, requestLineEndPos);

  const std::optional<float> httpVersion = utilities::convert::stringToNumber<float>(httpVersionStr);
  if (!httpVersion.has_value()) {
    ws->close("Invalid HTTP request.");
    return;
  }

  /*
   * Minimum HTTP version is 1.1 as defined by IETF
   * Maximum HTTP version is 3 as that's the latest version
   */
  if (httpVersion.value() < 1.1 || httpVersion.value() > 3) {
    ws->close("Invalid HTTP request.");
    return;
  }

  /*
   * Client must set the upgrade header to websocket
   */
  const std::string upgradeHeader = getHeader("Upgrade", request);
  if (upgradeHeader.empty()) {
    ws->close("Invalid HTTP request.");
    return;
  }

  /*
   * Must contain Sec-WebSocket-Key, this is used for the response back to the
   * client to verify that the request wasn't malformed in the process of transmitting
   */
  std::string secWebsocketKeyHeader = getHeader("Sec-WebSocket-Key", request);
  if (secWebsocketKeyHeader.empty()) {
    ws->close("Invalid HTTP request.");
    return;
  }

  /*
   * Must contain Sec-WebSocket-Version. The value must be set to 13
   */
  const std::string secWebSocketVersionStr = getHeader("Sec-WebSocket-Version", request);
  if (secWebSocketVersionStr.empty()) {
    ws->close("Invalid HTTP request.");
    return;
  }

  const std::optional<float> secWebSocketVersion = utilities::convert::stringToNumber<float>(secWebSocketVersionStr);
  if (!secWebSocketVersion.has_value()) {
    ws->close("Invalid HTTP request.");
    return;
  }

  if (secWebSocketVersion != 13) {
    ws->close("Invalid HTTP request.");
    return;
  }

  /*
   * Optional headers
   */
  const std::string secWebSocketProtocol = getHeader("Sec-WebSocket-Protocol", request);
  const std::string secWebSocketExtensions = getHeader("Sec-WebSocket-Extensions", request);

  /*
   * Everything is valid, begin to upgrade this request
   */
  std::stringstream response;

  // Headers that are necessary to complete upgrade
  response << "HTTP/1.1 101 Switching Protocols\r\n";
  response << "Connection: Upgrade\r\n";
  response << "Upgrade: websocket\r\n";
  response << "Sec-WebSocket-Accept: " << calculateSecWebSocketKey(secWebsocketKeyHeader) << "\r\n";
  response << "\r\n";

  ws->send(response.str(), response.str().size());
}