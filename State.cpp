#include "State.h"
#include "Utils.h"

uint16_t State::getDE() const {
	uint16_t value = makeWord(d, e);

	return value;
}

void State::setDE(uint16_t value) {
	d = uint8_t((value >> 8) & 0xff);
	e = uint8_t(value & 0xff);
}

uint16_t State::getHL() const {
	uint16_t value = makeWord(h, l);

	return value;
}

void State::setHL(uint16_t value) {
	h = uint8_t((value >> 8) & 0xff);
	l = uint8_t(value & 0xff);
}

uint16_t State::getBC() const {
	uint16_t value = makeWord(b, c);

	return value;
}

void State::setBC(uint16_t value) {
	b = uint8_t((value >> 8) & 0xff);
	c = uint8_t(value & 0xff);
}