#pragma once

#include <cstdio>
#include <string>
#include <string_view>

namespace Basedline {

class ScreenBuf {
	const size_t defaultSize = 1024;
	const size_t maxSize = 16 * defaultSize;
	std::string buf;
public:
	void push (std::string_view val) {
		buf.append (val);
	}

	void flush () {
		if (buf.empty()) [[unlikely]] return;

		std::fwrite (buf.data(), 1, buf.size(), stdout);
		std::fflush (stdout);

		buf.clear();
		if (buf.capacity() > maxSize) {
			std::string tmp;
			tmp.reserve (defaultSize);
			buf.swap (tmp);
		}
	}

	ScreenBuf () {
		buf.reserve (defaultSize);
	}
};

}
