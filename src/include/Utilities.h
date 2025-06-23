#pragma once

#include <optional>
#include <string>
#include <concepts>

namespace bwss::utilities {
  namespace convert {
    template<typename T>
    requires std::integral<T> || std::floating_point<T>
    std::optional<T> stringToNumber(const std::string& str) noexcept {
      try {
        if constexpr (std::integral<T>) {
          if constexpr (std::signed_integral<T>) {
            return static_cast<T>(std::stoll(str));
          } else {
            return static_cast<T>(std::stoull(str));
          }
        } else if constexpr (std::floating_point<T>) {
          return static_cast<T>(std::stod(str));
        }
      } catch (...) {
        return std::nullopt;
      }

      return std::nullopt;
    }
  }

  inline constexpr char base64Chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

  /**
   * Hashes a string using SHA1 and returns the raw value of the SHA1
   */
  std::string sha1Raw(const std::string& str) noexcept;

  /**
   * Hashes a string using SHA1 and returns the hexadecimal string representation
   */
  std::string sha1(const std::string& str) noexcept;

  /**
   * Base64 encodes a string, returns the base64 encoded string
   */
  std::string base64(const std::string& str) noexcept;
}