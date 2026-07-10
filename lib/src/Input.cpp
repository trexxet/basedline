#include "Basedline/Input.hpp"

#include <print>

#ifdef __WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "Basedline/ScreenBuf.hpp"

#include "InputDispatcher.hpp"

namespace Basedline {

static bool needs_flush (std::span<uint8_t> buf, uint8_t currByte) noexcept {
	return (buf.size() == 1 && buf[0] == '\e') // single ESC
	    || (buf.size() == 2 && buf[0] == '\e' && currByte == '[')  // CSI entry
	    || (buf.size() == 2 && buf[0] == '\e' && currByte == 'O'); // SS3 entry
}

void Input::decode (std::span<uint8_t> buf) {
	for (uint8_t b : buf) {
		vtparser.feed (b);
		auto event = needs_flush (buf, b) ? vtparser.flush() : vtparser.get();
		if (event) dispatch_input_event (*event, ed);
	}
}

void Input::read () {
	uint8_t buf[64];
	ssize_t n = ::read (STDIN_FILENO, buf, sizeof (buf));
	decode (std::span (buf, n));
}

void Input::dump (ScreenBuf& screenBuf) {
	screenBuf.push (ed.buf());
}

}
