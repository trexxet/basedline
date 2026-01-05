#pragma once

#include <bitset>
#include <string>

#include "ConsoleHandle.hpp"
#include "Defs.hpp"

namespace Basedline {

class TTY {
public:
	struct Input {
		enum Flags {
			OK, HAS_CTRL, IS_EOL, IS_LEFT, IS_RIGHT, IS_UP, IS_DOWN, count
		};
		std::bitset<Flags::count> flags;
		inline bool ok ()       const { return flags[Flags::OK]; }
		inline bool is_eol ()   const { return flags[Flags::IS_EOL]; }
		inline bool is_left ()  const { return flags[Flags::IS_LEFT]; }
		inline bool is_right () const { return flags[Flags::IS_RIGHT]; }
		inline bool is_lr ()    const { return is_left() || is_right(); }
		inline bool is_up ()    const { return flags[Flags::IS_UP]; }
		inline bool is_down ()  const { return flags[Flags::IS_DOWN]; }
		int c;
		static Input make_err () { return {}; }
	};

private:
	void process_control_key (Input& input, const ConsoleHandle::RawInput& rawInput);

public:
	Input getc (ConsoleHandle& con);

	TTY () { }
	BASEDLINE_CLASS_NO_COPY_MOVE (TTY);
};

}
