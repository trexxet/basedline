#pragma once

#include <string>
#include <optional>

namespace Basedline {

using OptString = std::optional<std::string>;

struct consize_t { int y, x; };
struct conarea_t { int y1, x1, y2, x2; };

}
