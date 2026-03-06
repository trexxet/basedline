#pragma once

#include <optional>
#include <string>

namespace Basedline {

struct ReadState {
	std::string prompt;
	std::string linebuf = "";

	inline size_t len () const { return prompt.length() + linebuf.length(); }

	ReadState (const std::string& prompt) : prompt (prompt) { }
};

using OptReadState = std::optional<ReadState>;

}
