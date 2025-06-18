#pragma once

#include "Bwss.h"
#include <optional>

namespace bwss::utilities {
  namespace convert {
    template<typename T>
    requires std::integral<T> || std::floating_point<T>
    std::optional<T> stringToNumber(const std::string& str) noexcept;
  }
}