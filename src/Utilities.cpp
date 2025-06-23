#include "Utilities.h"
#include <openssl/sha.h>
#include <sstream>
#include <utility>
#include <iomanip>
#include <string>
#include <iostream>

std::string bwss::utilities::base64(const std::string& str) noexcept {
  std::string out;
  int val = 0;
  int valb = -6;

  for (const char i : str) {
    val = (val << 8) + i;
    valb += 8;
    while (valb >= 0) {
      out.push_back(base64Chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }

  if (valb > -6) {
    out.push_back(base64Chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }

  while (out.size() % 4) {
    out.push_back('=');
  }

  return std::move(out);
}

std::string bwss::utilities::sha1Raw(const std::string& str) noexcept {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), hash);
  return std::string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);
}

std::string bwss::utilities::sha1(const std::string& str) noexcept {
  unsigned char hash[SHA_DIGEST_LENGTH];

  SHA1(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), hash);

  std::stringstream output;
  for (const unsigned char& i : hash) {
    output << std::hex << std::setw(2) << std::setfill('') << static_cast<int>(i);
  }

  return std::move(output.str());
}