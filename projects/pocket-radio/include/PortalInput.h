#pragma once
#include <cctype>
#include <cstddef>

// Work with Arduino String on-device and std::string in host tests.
template <class Text> bool portalCredentialsValid(const Text& ssid, const Text& password) {
  if (!ssid.length() || ssid.length() > 32) return false;
  for (size_t i = 0; i < ssid.length(); ++i) if (ssid[i] == '\0') return false;
  for (size_t i = 0; i < password.length(); ++i) if (password[i] == '\0') return false;
  if (!password.length()) return true;
  if (password.length() >= 8 && password.length() <= 63) return true;
  if (password.length() != 64) return false;
  for (size_t i = 0; i < password.length(); ++i)
    if (!std::isxdigit(static_cast<unsigned char>(password[i]))) return false;
  return true;
}

template <class Text> Text portalEscape(const Text& input) {
  Text result;
  for (size_t i = 0; i < input.length(); ++i) {
    switch (input[i]) {
      case '&': result += "&amp;"; break;
      case '<': result += "&lt;"; break;
      case '>': result += "&gt;"; break;
      case '"': result += "&quot;"; break;
      case '\'': result += "&#39;"; break;
      default: result += input[i];
    }
  }
  return result;
}

template <class Text> Text portalJsonEscape(const Text& input) {
  Text result;
  const char* hex = "0123456789abcdef";
  for (size_t i = 0; i < input.length(); ++i) {
    unsigned char c = input[i];
    if (c == '"' || c == '\\') { result += '\\'; result += char(c); }
    else if (c < 32) { result += "\\u00"; result += hex[c >> 4]; result += hex[c & 15]; }
    else result += char(c);
  }
  return result;
}
