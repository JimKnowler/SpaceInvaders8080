#pragma once

#include <string>

#include "memory/IMemory.h"

struct Disassemble {
	static std::string stringFromOpcode(memory::IMemory* memory, uint16_t pc, uint16_t& outOpcodeSize);
};
