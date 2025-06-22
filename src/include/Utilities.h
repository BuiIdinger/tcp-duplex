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

  std::string sha1(const std::string& str) noexcept;
}