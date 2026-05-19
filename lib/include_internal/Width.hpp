#pragma once

#include <string_view>

namespace Basedline {

int grapheme_width (std::u8string_view u8bytes);
ssize_t u8string_width (std::u8string_view u8str);

}
