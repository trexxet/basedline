#include "Width.hpp"

#include <vector>

extern "C" {
#include <grapheme.h>
}
#include "widechar_width.h"

namespace Basedline {

static std::vector<char32_t> u8_to_u32 (std::u8string_view u8) {
	std::vector<char32_t> u32;
	u32.reserve (u8.size());
	size_t pos = 0;
	while (pos < u8.size()) {
		uint_least32_t cp = GRAPHEME_INVALID_CODEPOINT;
		size_t processed = grapheme_decode_utf8 (reinterpret_cast<const char*> (u8.data() + pos), u8.size() - pos, &cp);
		if (processed == 0 || processed > u8.size() - pos || cp == GRAPHEME_INVALID_CODEPOINT)
			break;
		pos += processed;
		u32.push_back (static_cast<char32_t> (cp));
	}
	return u32;
}

static int cp_width (char32_t cp) {
	switch (cp) {
		case U'\u200C': // ZWNJ
		case U'\u200D': // ZWJ
			return 0;
		default: [[likely]] break;
	}

	int w = widechar_wcwidth (static_cast<uint32_t> (cp));
	if (w > 0) return w;

	switch (w) {
		case widechar_combining:
			return 0;
		case widechar_ambiguous: // TODO: make ambigous width configurable
		case widechar_private_use:
		case widechar_unassigned:
			return 1;
		case widechar_widened_in_9:
			return 2;
	}
	return -1;
}

int grapheme_width (std::u8string_view u8grapheme) {
	if (u8grapheme.empty()) [[unlikely]] return 0;

	std::vector<char32_t> cps = u8_to_u32 (u8grapheme);
	if (cps.empty()) [[unlikely]] return 0;

	int sum = 0;
	bool vs16 = false;
	bool keycap = false;

	for (char32_t cp : cps) {
		if (cp == U'\uFE0F') [[unlikely]] vs16 = true;
		if (cp == U'\u20E3') [[unlikely]] keycap = true;
		int w = cp_width (cp);
		if (w < 0) return -1;
		sum += w;
	}

	if (sum == 0) return 0;
	if (vs16 || keycap) return 2;
	return std::min (sum, 2);
}

}
