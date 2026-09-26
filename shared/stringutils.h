#pragma once

#include <string>
#include <vector>

namespace StringUtils
{
//! returns a lowercase copy of str
[[nodiscard]] std::string toLower(const std::string& str);

//! returns an uppercase copy of str
[[nodiscard]] std::string toUpper(const std::string& str);

//! returns a copy of str without leading/trailing whitespace
[[nodiscard]] std::string trim(const std::string& str);

//! splits str at separator, dropping empty parts
[[nodiscard]] std::vector<std::string> split(const std::string& str, char separator);
}  // namespace StringUtils
