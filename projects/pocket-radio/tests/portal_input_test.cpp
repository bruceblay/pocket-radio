#include <cassert>
#include <string>
#include "PortalInput.h"

int main() {
  using std::string;
  assert(portalCredentialsValid(string("Home"), string("password")));
  assert(portalCredentialsValid(string("Open cafe"), string()));
  assert(!portalCredentialsValid(string(), string("password")));
  assert(!portalCredentialsValid(string(33, 's'), string("password")));
  assert(portalCredentialsValid(string(32, 's'), string(63, 'p')));
  assert(!portalCredentialsValid(string("Home"), string("short")));
  assert(portalCredentialsValid(string("Home"), string(64, 'a')));
  assert(!portalCredentialsValid(string("Home"), string(64, 'z')));
  assert(!portalCredentialsValid(string("Home"), string(65, 'a')));
  assert(!portalCredentialsValid(string("a\0b", 3), string("password")));
  assert(!portalCredentialsValid(string("Home"), string("pass\0word", 9)));
  assert(portalCredentialsValid(string("  Home  "), string("  pass  ")));
  string unicode;
  for (int i = 0; i < 16; ++i) unicode += "é";
  assert(portalCredentialsValid(unicode, string("password")));
  unicode += "é";
  assert(!portalCredentialsValid(unicode, string("password")));
  assert(portalEscape(string("\"><script>&'")) == "&quot;&gt;&lt;script&gt;&amp;&#39;");
  assert(portalEscape(string("Café Wi-Fi")) == "Café Wi-Fi");
  assert(portalJsonEscape(string("\"\\\n\t")) == "\\\"\\\\\\u000a\\u0009");
  assert(portalJsonEscape(string("a\0b", 3)) == "a\\u0000b");
  assert(portalJsonEscape(string("Café Wi-Fi")) == "Café Wi-Fi");
}
