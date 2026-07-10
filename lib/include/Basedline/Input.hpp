#pragma once

#include <span>

#include "BasedVT/Parser.hpp"

#include "Basedline/Editor.hpp"

namespace Basedline {

class ScreenBuf;

class Input {
	Editor ed;
	BasedVT::Parser vtparser;

	void decode (std::span<uint8_t> buf);
public:
	void read ();
	void dump (ScreenBuf& screenBuf);
};

}
