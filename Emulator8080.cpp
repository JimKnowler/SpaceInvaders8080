#include "Emulator8080.h"
#include "Disassemble8080.h"

#include <cstring>
#include <cassert>

namespace {
	bool parity(uint16_t value) {
		int size = 16;
		int count = 0;

		for (int i = 0; i < size; i++) {
			if (value & 0x01) {
				count += 1;
			}
			
			value >>= 1;
		}

		return (0 == (count & 0x1));
	}

	uint16_t makeWord(uint8_t hi, uint8_t lo) {
		return (uint16_t(hi) << 8) | uint16_t(lo);
	}

	void swap(uint8_t& a, uint8_t& b) {
		uint8_t temp = a;
		a = b;
		b = temp;
	}
}

Emulator8080::Emulator8080() {
	memset(&state, sizeof(State), 0);
	rom = 0;
	romSize = 0;
	interuptsEnabled = false;

	work.resize(0x400, 0xfe);
	video.resize(0x1c00, 0xfe);	

	numSteps = 0;
}

void Emulator8080::init(uint8_t* inRom, size_t inRomSize, uint16_t inPc) {
	rom = inRom;
	romSize = inRomSize;
	state.pc = inPc;

	numSteps = 0;
}

uint64_t Emulator8080::getNumSteps() const {
	return numSteps;
}

const Emulator8080::State& Emulator8080::getState() const {
	return state;
}

void Emulator8080::step() {
	numSteps += 1;

	uint8_t* opcode = rom + state.pc;
	
	size_t opcodeSize = 1;

#if 0
	std::string strOpcode;
	size_t numBytes = Disassemble8080::dissassembleOpcode(opcode, strOpcode);
	printf("pc[0x%04x] opcode[0x%02x] %s\n", state.pc, *opcode, strOpcode.c_str());
#endif

	switch (*opcode) {
		case 0x00:						// NOP 
			break;
		case 0x01:						// LXI B, D16
		{
			state.c = opcode[1];
			state.b = opcode[2];
			opcodeSize = 3;
			break;
		}

		case 0x05:						// DCR B
		{
			uint8_t value = state.b - 1;
			updateZSP(value);
			state.b = value;
			break;
		}
		case 0x06:						// MVI B, D8
		{
			state.b = opcode[1];
			opcodeSize = 2;
			break;
		}

		case 0x09:						// DAD B
		{
			uint32_t hl = getHL();
			uint32_t bc = getBC();
			uint32_t value = hl + bc;
			updateWordCY(value);
			setHL(value & 0xffff);
			break;
		}

		case 0x0D:						// DCR C
		{
			uint8_t value = state.c - 1;
			updateZSP(value);
			state.c = value;
			break;
		}
		case 0x0E:						// MVI C, D8
		{
			state.c = opcode[1];
			opcodeSize = 2;
			break;
		}
		case 0x0F:						// RRC
		{
			uint8_t bit0 = state.a & 0x01;
			state.a >>= 1;
			state.a |= (bit0 << 7);
			state.cc.cy = bit0;
			break;
		}

		case 0x11:						// LXI D, D16
		{
			state.d = opcode[2];
			state.e = opcode[1];			
			opcodeSize = 3;
			break;
		}

		case 0x13:						// INX D
		{
			uint16_t value = getDE();
			value += 1;
			setDE(value);
			break;
		}

		case 0x19:						// DAD D
		{
			uint32_t de = getDE();
			uint32_t hl = getHL();
			uint32_t value = hl + de;
			updateWordCY(value);
			setHL(value& 0xffff);
			break;
		}

		case 0x1A:						// LDAX D
		{
			uint16_t address = getDE();
			state.a = readMemory(address);
			break;
		}
#if 0
		case 0x1B:						// DCX D
		{
			uint16_t value = getDE();
			value -= 1;
			setDE(value);
			break;
		}
#endif

		case 0x21:						// LXI H, D16
		{
			state.h = opcode[2];
			state.l = opcode[1];			
			opcodeSize = 3;
			break;
		}

		case 0x23:						// INX H
		{
			uint16_t value = getHL();
			value += 1;
			setHL(value);
			break;
		}

		case 0x26:						// MVI H, D8
		{
			state.l = opcode[1];
			opcodeSize = 2;
			break;
		}

		case 0x29:						// DAD H
		{
			uint32_t hl = getHL();
			uint32_t value = hl + hl;
			updateWordCY(value);
			setHL(value & 0xffff);
			break;
		}

		case 0x31:						// LXI SP, D16
		{
			state.sp = readOpcodeD16(opcode);
			opcodeSize = 3;
			break;
		}
		case 0x32:						// STA adr
		{
			uint16_t address = readOpcodeD16(opcode);
			writeMemory(address, state.a);
			opcodeSize = 3;
			break;
		}

		case 0x36:						// MVI M, D8
		{
			uint8_t value = opcode[1];
			uint16_t address = getHL();
			writeMemory(address, value);
			opcodeSize = 2;
			break;
		}

		case 0x3a:						// LDA word
		{
			uint16_t address = readOpcodeD16(opcode);
			state.a = readMemory(address);

			opcodeSize = 3;
			break;
		}
		

		case 0x3e:						// MVI A, byte
		{
			uint8_t value = opcode[1];
			state.a = value;

			opcodeSize = 2;
			break;
		}

#if 0
		case 0x41: state.b = state.c; break;    //MOV B,C    
		case 0x42: state.b = state.d; break;    //MOV B,D    
		case 0x43: state.b = state.e; break;    //MOV B,E   					

		case 0x46:						// MOV B, M
		{
			uint16_t address = getHL();
			state.b = readMemory(address);
			break;
		}

		case 0x4E:						// MOV C, M
		{
			uint16_t address = getHL();
			state.c = readMemory(address);
			break;
		}
#endif

		case 0x56:						// MOV D, M
		{
			uint16_t address = getHL();
			state.d = readMemory(address);
			break;
		}

		case 0x5E:						// MOV E, M
		{
			uint16_t address = getHL();
			state.e = readMemory(address);			
			break;
		}

#if 0

		case 0x61:						// MOV H, C
		{
			state.h = state.c;
			break;
		}
#endif

		case 0x66:						// MOV H, M
		{
			uint16_t address = getHL();
			state.h = readMemory(address);
			break;
		}

		case 0x6F:						// MOV L, A
		{
			state.l = state.a;
			break;
		}

		case 0x77:						// MOV M, A
		{
			uint16_t address = getHL();
			writeMemory(address, state.a);
			break;
		}

		case 0x7A:						// MOV A, D
		{
			state.a = state.d;
			break;
		}

		case 0x7B:						// MOV A, E
		{
			state.a = state.e;
			break;
		}
		
		case 0x7C:						// MOV A, H
		{
			state.a = state.h;
			break;
		}

		case 0x7E:						// MOV A, M
		{
			uint16_t address = getHL();
			state.a = readMemory(address);
			break;
		}

#if 0
		case 0x80:						// ADD B
		{
			uint16_t answer = uint16_t(state.a) + uint16_t(state.b);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x81:						// ADD C
		{
			uint16_t answer = uint16_t(state.a) + uint16_t(state.c);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
#endif
		case 0xA7:						// ANA A
		{
			state.a &= state.a;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		
		case 0xAf:						// XRA A
		{
			state.a = state.a ^ state.a;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}

		case 0xC1:						// POP B
		{
			state.c = readMemory(state.sp);
			state.b = readMemory(state.sp + 1);
			state.sp += 2;
			break;
		}
		case 0xC2:						// JNZ adr
		{
			if (0 == state.cc.z) {
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
		}
		case 0xC3:						// JMP adr
		{
			uint16_t address = readOpcodeD16(opcode);
			state.pc = address;
			return;
		}

		case 0xC5:						// PUSH B
		{
			writeMemory(state.sp - 2, state.c);
			writeMemory(state.sp - 1, state.b);
			state.sp -= 2;
			break;
		}
		case 0xC6:						// ADI byte
		{
			uint16_t answer = uint16_t(state.a) + uint16_t(opcode[1]);
			updateZSP(answer);
			updateCY(answer);
			state.a = uint8_t(answer & 0xff);
			opcodeSize = 2;
			break;
		}

		case 0xC9:						// RET
		{
			uint8_t pclo = readMemory(state.sp);
			uint8_t pchi = readMemory(state.sp + 1);
			state.pc = makeWord(pchi, pclo);
			state.sp += 2;
			return;
		}
		case 0xCA:						// JZ adr
		{
			if (state.cc.z != 0) {
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				return;
			} else {
				opcodeSize = 3;
				break;
			}
		}

		case 0xCD:						// CALL adr
		{

			uint16_t address = readOpcodeD16(opcode);
#if 1    
			if (5 == address)
			{
				if (state.c == 9)
				{
					uint16_t offset = getDE();

					offset += 3;		// skip prefix bytes

					uint8_t c = readMemory(offset);
					while (c != '$') {
						printf("%c", c);
						offset += 1;
						c = readMemory(offset);
					}

					printf("\n");
				}
				else if (state.c == 2)
				{
					//saw this in the inspected code, never saw it called    
					printf("print char routine called\n");
				}
			}
			else if (0 == address)
			{
				exit(0);
			}
			else
#endif  
			{
				uint16_t ret = state.pc + 3;
				uint8_t rethi = uint8_t((ret >> 8) & 0xff);
				uint8_t retlo = uint8_t(ret & 0xff);
				writeMemory(state.sp - 1, rethi);
				writeMemory(state.sp - 2, retlo);
				state.sp = state.sp - 2;

				state.pc = address;
			}

			return;
		}

		case 0xCE:						// ACI D8
		{
			uint16_t value = state.a;
			value += uint16_t(opcode[1]);
			value += uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = value;

			opcodeSize = 2;
			break;
		}

		case 0xD1:						// POP D
		{
			state.e = readMemory(state.sp);
			state.d = readMemory(state.sp + 1);
			state.sp += 2;
			break;
		}
		case 0xD2:						// JNC adr
		{
			if (!state.cc.cy) {
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
		}
		case 0xD3:						// OUT D8 (special)
		{
			uint8_t data = opcode[1];
			printf("OUT 0x%02x\n", data);
			opcodeSize = 2;
			break;
		}

		case 0xD5:						// PUSH D
		{
			writeMemory(state.sp - 2, state.e);
			writeMemory(state.sp - 1, state.d);
			state.sp -= 2;
			break;
		}
		case 0xD6:						// SUI D8
		{
			uint8_t data = opcode[1];
			uint16_t value = uint16_t(state.a) - uint16_t(data);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);

			opcodeSize = 2;
			break;
		}

		case 0xDA:						// JC addr
		{
			if (state.cc.cy == 1) {
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
			break;
		}

		case 0xDE:						// SBI D8
		{
			uint8_t data = opcode[1];
			uint16_t value = uint16_t(state.a) - uint16_t(data) - uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);

			opcodeSize = 2;
		}

		case 0xE1:						// POP H
		{
			state.l = readMemory(state.sp);
			state.h = readMemory(state.sp + 1);
			state.sp += 2;
			break;
		}
		case 0xE2:						// JPO
		{
			if (state.cc.p == 0) {
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
			break;
		}

		case 0xE5:						// PUSH H
		{
			writeMemory(state.sp - 2, state.l);
			writeMemory(state.sp - 1, state.h);
			state.sp -= 2;
			break;
		}
		case 0xE6:						// ANI D8
		{
			uint16_t value = uint16_t(state.a) & uint16_t(opcode[1]);
			updateZSP(value);
			updateCY(value);
			state.a = uint8_t(value & 0xff);
			opcodeSize = 2;
			break;
		}

		case 0xEA:						// JPE adr
		{
			if (state.cc.p == 1) {
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
		}
		case 0xEB:						// XCHG
		{
			swap(state.h, state.d);
			swap(state.l, state.e);
			break;
		}

		case 0xF1:						// POP PSW
		{
			uint8_t flags = readMemory(state.sp);
			state.cc = *reinterpret_cast<ConditionCodes*>(&flags);
			state.a = readMemory(state.sp + 1);
			state.sp += 2;

			break;
		}
		case 0xF2:						// JP
		{
			if (state.cc.s == 0) {
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
			break;
		}

#if 0
		case 0xF3:						// DI (special)
		{
			printf("DI\n");
			break;
		}
#endif
		case 0xF5:						// PUSH PSW
		{
			writeMemory(state.sp - 2, *reinterpret_cast<uint8_t*>(&state.cc));
			writeMemory(state.sp - 1, state.a);
			state.sp -= 2;
			break;
		}
		case 0xF6:						// ORI D8
		{
			uint8_t data = opcode[1];
			uint8_t value = state.a | data;
			updateCY(value);
			updateZSP(value);
			state.a = value;
			opcodeSize = 2;
			break;
		}

		case 0xFA:						// JM
		{
			if (state.cc.s == 1) {
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
			break;
		}
		case 0xFB:						// EI
		{
			interuptsEnabled = true;
			break;
		}

		case 0xFE:						// CPI D8
		{
			uint16_t value = uint16_t(state.a) - uint16_t(opcode[1]);
			updateZSP(value);
			updateCY(value);

			opcodeSize = 2;
			break;
		}

		default:
			opcodeSize = unimplementedOpcode(state.pc);
			break;
	}

	state.pc += uint16_t(opcodeSize);


}

void Emulator8080::updateZSP(uint16_t answer) {
	state.cc.z = ((answer & 0xff) == 0);
	state.cc.s = ((answer & 0x80) == 0x80);	
	state.cc.p = parity(answer & 0xff);

	// ac (auxilliary carry) not implemented - not required for space invaders
}

void Emulator8080::updateCY(uint16_t value) {
	state.cc.cy = (value > 0xff);
}

void Emulator8080::updateWordCY(uint32_t value) {
	state.cc.cy = (value > 0xffff);
}

size_t Emulator8080::unimplementedOpcode(uint16_t pc) {
	assert(pc < romSize);
	
	uint8_t* opcode = rom + pc;

	std::string strOpcode;
	size_t numBytes = Disassemble8080::dissassembleOpcode(opcode, strOpcode);

	
	printf("unimplemented Instruction: %04x 0x%02x %s\n", pc, *opcode, strOpcode.c_str());
	
	return numBytes;
}

uint16_t Emulator8080::readOpcodeD16(uint8_t* opcode) {
	uint16_t value = makeWord(opcode[2], opcode[1]);

	return value;
}

void Emulator8080::writeMemory(uint16_t address, uint8_t value) {
	while (address >= 0x4000) {
		// handle mirroring of RAM above 0x4000
		address -= 0x2000;
	}

	assert(address >= romSize);
	assert(address <= 0x3fff);

	if (address < 0x2400) {
		// work RAM
		uint16_t workAddress = address - romSize;
		work[workAddress] = value;
	}
	else {
		// video RAM
		uint16_t videoAddress = address - 0x2400;
		video[videoAddress] = value;
	}
}

uint8_t Emulator8080::readMemory(uint16_t address) const {
	while (address >= 0x4000) {
		// handle mirroring of RAM above 0x4000
		address -= 0x2000;
	}

	assert(address <= 0x3fff);

	if (address < romSize) {
		return rom[address];
	}

	if (address < 0x2400) {
		// work RAM
		uint16_t workAddress = address - romSize;
		
		return work[workAddress];
	}
	else {
		// video RAM

		uint16_t videoAddress = address - 0x2400;

		return video[videoAddress];
	}
}

uint16_t Emulator8080::getDE() const {
	uint16_t value = makeWord(state.d, state.e);

	return value;
}

void Emulator8080::setDE(uint16_t value) {
	state.d = uint8_t((value >> 8) & 0xff);
	state.e = uint8_t(value & 0xff);
}

uint16_t Emulator8080::getHL() const {
	uint16_t value = makeWord(state.h, state.l);

	return value;
}

void Emulator8080::setHL(uint16_t value) {
	state.h = uint8_t((value >> 8) & 0xff);
	state.l = uint8_t(value & 0xff);
}

uint16_t Emulator8080::getBC() const {
	uint16_t value = makeWord(state.b, state.c);

	return value;
}
