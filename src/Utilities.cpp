#include "Bwss.h"
#include "Utilities.h"
#include <concepts>
#include <optional>

template<typename T>
requires std::integral<T> || std::floating_point<T>
std::optional<T> bwss::utilities::convert::stringToNumber(const std::string& str) noexcept {
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