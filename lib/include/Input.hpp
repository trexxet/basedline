#pragma once

#include <bitset>
#include <cctype>
#include <string>
#include <variant>

#include "Basedlib/Class.hpp"
#include "Basedlib/RingBuffer.hpp"
#include "wrap/ncurses.h"
#include "wrap/windows.h"

#include "Pad.hpp"
#include "ReadState.hpp"

namespace Basedline {

class Console;

struct RawInputEvent {
	enum class Type { Unknown, Key, Resize } type = Type::Unknown;
	union {
		struct {
#if BLWIN
			CHAR ch;
			WORD vkey;
			DWORD mods;
#endif
		} key;
		consize_t newSize;
	} ev;
};

class KeyInputEvent {
	//void apply_line_edit (Console& con, ReadState& rs);
	void process_control_key (const RawInputEvent& rawInput);
public:
	int c;

	enum Flags {
		IS_OK, HAS_CTRL, IS_EOL, IS_LEFT, IS_RIGHT, IS_UP, IS_DOWN, IS_DEL, IS_BKSPC, IS_SPACE, count
	};
	std::bitset<Flags::count> flags;
	inline bool ok       () const { return flags[Flags::IS_OK]; }
	inline bool has_ctrl () const { return flags[Flags::HAS_CTRL]; }
	inline bool is_eol   () const { return flags[Flags::IS_EOL]; }
	inline bool is_left  () const { return flags[Flags::IS_LEFT]; }
	inline bool is_right () const { return flags[Flags::IS_RIGHT]; }
	inline bool is_lr    () const { return is_left() || is_right(); }
	inline bool is_up    () const { return flags[Flags::IS_UP]; }
	inline bool is_down  () const { return flags[Flags::IS_DOWN]; }
	inline bool is_del   () const { return flags[Flags::IS_DEL]; }
	inline bool is_bkspc () const { return flags[Flags::IS_BKSPC]; }
	inline bool is_space () const { return flags[Flags::IS_SPACE]; }
	inline bool is_print () const { return std::isprint (c); }

	static KeyInputEvent make_from_raw (const RawInputEvent& rawInput);
	static KeyInputEvent make_err () { return {}; }

	bool process (Console& con, ReadState& rs);
};

struct ResizeInputEvent {
	consize_t newSize;

	bool process (Console& con);
};

using InputVariant = std::variant<KeyInputEvent, ResizeInputEvent>;

class InputEvent {
	static InputEvent make (InputVariant&& var) { return InputEvent (std::move (var)); }
public:
	InputVariant value;
	static InputEvent get (Console& con);

	/// @return true if should continue fetching input
	bool process (Console& con, OptReadState& rs);
};

class InputHandler {
	Console& con;
	Pad pad;
	std::wstring acLine;

public:
	void sync ();

	InputHandler (Console& con, size_t bufLines);

	BASED_CLASS_NO_COPY_MOVE (InputHandler);
};

}
