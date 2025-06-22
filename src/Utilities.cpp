#include "Bwss.h"
#include "Utilities.h"
#include <openssl/sha.h>
#include <sstream>
#include <utility>
#include <iomanip>

std::string bwss::utilities::sha1(const std::string& str) noexcept {
  unsigned char hash[SHA_DIGEST_LENGTH];

  SHA1(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), hash);

  std::stringstream output;
  for (const unsigned char& i : hash) {
    output << std::hex << std::setw(2) << std::setfill('') << static_cast<int>(i);
  }

  return std::move(output.str());
}