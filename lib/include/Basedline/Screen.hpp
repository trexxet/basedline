#pragma once

#include "Basedline/Input.hpp"
#include "Basedline/TTYConf.hpp"

namespace Basedline {

class Screen {
	TTYConf ttyconf;

	void clear ();
public:
	Input in;

	void render ();

	Screen ();
};

}
