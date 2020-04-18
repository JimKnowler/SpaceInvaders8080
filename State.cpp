#include "State.h"
#include "Utils.h"

State::State() :
	hl(h, l), de(d, e), bc(b, c),
	a(0), b(0), c(0), d(0), e(0), h(0), l(0),
	sp(0), pc(0),
	interruptsEnabled(false)
{
}

void State::reset() {
	cc.reset();

	a = 0;
	bc = 0;
	de = 0;
	hl = 0;
	sp = 0;
	pc = 0;

	interruptsEnabled = false;
}
