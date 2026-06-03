#pragma once

#include <string_view>
#include <vector>

namespace Basedline {

int grapheme_width (std::u8string_view u8bytes);
ssize_t u8string_width (std::u8string_view u8str);
std::vector<std::u8string_view> wrap (std::u8string_view str, size_t width);

}
