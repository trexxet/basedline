#pragma once

#include <bitset>
#include <cctype>
#include <string>
#include <variant>

#include "Defs.hpp"

namespace Basedline {

class Console;

struct RawInput {
	enum class Type { Unknown, Key, Resize } type = Type::Unknown;
	union {
		struct {
#if defined(_WIN32)
			CHAR ch;
			WORD vkey;
			DWORD mods;
#endif
		} key;
		coord_t newSize;
	} ev;
};

class KeyInput {
	void process_control_key (const RawInput& rawInput);
public:
	int c;

	enum Flags {
		OK, HAS_CTRL, IS_EOL, IS_LEFT, IS_RIGHT, IS_UP, IS_DOWN, IS_DEL, IS_BKSPC, count
	};
	std::bitset<Flags::count> flags;
	inline bool ok       () const { return flags[Flags::OK]; }
	inline bool is_eol   () const { return flags[Flags::IS_EOL]; }
	inline bool is_left  () const { return flags[Flags::IS_LEFT]; }
	inline bool is_right () const { return flags[Flags::IS_RIGHT]; }
	inline bool is_lr    () const { return is_left() || is_right(); }
	inline bool is_up    () const { return flags[Flags::IS_UP]; }
	inline bool is_down  () const { return flags[Flags::IS_DOWN]; }
	inline bool is_del   () const { return flags[Flags::IS_DEL]; }
	inline bool is_bkspc () const { return flags[Flags::IS_BKSPC]; }
	inline bool is_print () const { return std::isprint (c); }

	static KeyInput make_from_raw (const RawInput& rawInput);
	static KeyInput make_err () { return {}; }
};

struct ResizeInput {
	coord_t newSize;
};

using InputVariant = std::variant<KeyInput, ResizeInput>;

class Input {
	static Input make (InputVariant&& var);
public:
	InputVariant value;
	static Input get (Console& con);
};

}
