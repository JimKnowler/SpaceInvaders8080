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
	
	numSteps = 0;
}

void Emulator8080::init(uint8_t* inRom, uint16_t inRomSize, uint16_t inPc, uint16_t inWorkSize, uint16_t inVideoSize, bool inIsRamMirrorEnabled, bool inIsRomWriteable) {
	rom = inRom;
	romSize = inRomSize;
	state.pc = inPc;

	work.resize(inWorkSize, 0xfe);
	video.resize(inVideoSize, 0xfe);

	workTop = inRomSize + inWorkSize;
	videoTop = workTop + inVideoSize;
	isRamMirrorEnabled = inIsRamMirrorEnabled;
	isRomWriteable = inIsRomWriteable;
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
		case 0x02:						// STAX B
		{
			uint16_t address = getBC();
			writeMemory(address, state.a);
			break;
		}
		case 0x03:						// INX B
		{
			uint16_t value = getBC();
			value += 1;
			setBC(value);
			break;
		}
		case 0x04:						// INR B
		{
			state.b += 1;
			updateZSP(state.b);
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
		case 0x07:						// RLC
		{
			uint8_t bit7 = ((state.a & 128) == 128) ? 1 : 0;
			state.a = (state.a << 1) | bit7;
			state.cc.cy = bit7;
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
		case 0x0A:						// LDAX B
		{
			uint16_t address = getBC();
			state.a = readMemory(address);
			break;
		}
		case 0x0B:						// DCX B
		{
			uint16_t value = getBC();
			value -= 1;
			setBC(value);
			break;
		}
		case 0x0C:						// INR C
		{
			state.c += 1;
			updateZSP(state.c);
			break;
		}
		case 0x0D:						// DCR C
		{
			state.c -= 1;
			updateZSP(state.c);
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
		case 0x12:						// STAX D
		{
			uint16_t address = getDE();
			writeMemory(address, state.a);
			break;
		}
		case 0x13:						// INX D
		{
			uint16_t value = getDE();
			value += 1;
			setDE(value);
			break;
		}
		case 0x14:						// INR D
		{
			state.d += 1;
			updateZSP(state.d);
			break;
		}
		case 0x15:						// DCR D
		{
			state.d -= 1;
			updateZSP(state.d);
			break;
		}
		case 0x16:						// MVI D, D8
		{
			state.d = opcode[1];
			opcodeSize = 2;
			break;
		}
		case 0x17:						// RAL
		{
			uint8_t bit7 = ((state.a & 128) == 128) ? 1 : 0;
			uint8_t bit0 = state.cc.cy;
			state.a = (state.a << 1) | bit0;
			state.cc.cy = bit7;
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
		case 0x1B:						// DCX D
		{
			uint16_t value = getDE();
			value -= 1;
			setDE(value);
			break;
		}
		case 0x1C:						// INR E
		{
			state.e += 1;
			updateZSP(state.e);
			break;
		}
		case 0x1D:						// DCR E
		{
			state.e -= 1;
			updateZSP(state.e);
			break;
		}
		case 0x1E:						// MVI E, D8
		{
			state.e = opcode[1];
			opcodeSize = 2;
			break;
		}
		case 0x1F:						// RAR
		{
			uint8_t bit0 = state.a & 1;
			state.a = (state.a >> 1) | (bit0 << 7);
			state.cc.cy = bit0;
			break;
		}

		case 0x21:						// LXI H, D16
		{
			state.h = opcode[2];
			state.l = opcode[1];			
			opcodeSize = 3;
			break;
		}
		case 0x22:						// SHLD
		{
			uint16_t address = readOpcodeD16(opcode);
			writeMemory(address, state.l);
			writeMemory(address + 1, state.h);
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
		case 0x24:						// INR H
		{
			state.h += 1;
			updateZSP(state.h);
			break;
		}
		case 0x25:						// DCR H
		{
			state.h -= 1;
			updateZSP(state.h);
			break;
		}
		case 0x26:						// MVI H, D8
		{
			state.h = opcode[1];
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
		case 0x2A:						// LHLD
		{
			uint16_t address = readOpcodeD16(opcode);
			state.l = readMemory(address);
			state.h = readMemory(address + 1);
			opcodeSize = 3;
			break;
		}
		case 0x2B:						// DCX H
		{
			uint16_t value = getHL();
			value -= 1;
			setHL(value);
			break;
		}
		case 0x2C:						// INR L
		{
			state.l += 1;
			updateZSP(state.l);
			break;
		}
		case 0x2D:						// DCR L
		{
			state.l -= 1;
			updateZSP(state.l);
			break;
		}
		case 0x2E:						// MVI L, D8
		{
			state.l = opcode[1];
			opcodeSize = 2;
			break;
		}
		case 0x2F:						// CMA
		{
			state.a = ~state.a;
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
		case 0x33:						// INX SP
		{
			state.sp += 1;
			break;
		}
		case 0x34:						// INR M
		{
			uint16_t address = getHL();
			uint16_t value = readMemory(address);
			value += 1;
			updateZSP(value);
			writeMemory(address, value);
			break;
		}
		case 0x35:						// DCR M
		{
			uint16_t address = getHL();
			uint16_t value = readMemory(address);
			value -= 1;
			updateZSP(value);
			writeMemory(address, value);
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
		case 0x37:						// STC
		{
			state.cc.cy = 1;
			break;
		}

		case 0x39:						// DAD SP
		{
			uint32_t value = uint32_t(getHL()) + uint32_t(state.sp);
			updateWordCY(value);
			setHL(uint16_t(value & 0xffff));
			break;
		}
		case 0x3a:						// LDA word
		{
			uint16_t address = readOpcodeD16(opcode);
			state.a = readMemory(address);

			opcodeSize = 3;
			break;
		}
		case 0x3B:						// DCX SP
		{
			state.sp -= 1;
			break;
		}
		case 0x3c:						// INR A
		{
			state.a += 1;
			updateZSP(state.a);
			break;
		}
		case 0x3d:						// DCR A
		{
			state.a -= 1;
			updateZSP(state.a);
			break;
		}
		

		case 0x3e:						// MVI A, byte
		{
			uint8_t value = opcode[1];
			state.a = value;

			opcodeSize = 2;
			break;
		}
		case 0x3f:						// CMC
		{
			state.cc.cy = 1 - state.cc.cy;
			break;
		}

		case 0x41: state.b = state.c; break;    //MOV B,C    
		case 0x42: state.b = state.d; break;    //MOV B,D    
		case 0x43: state.b = state.e; break;    //MOV B,E   					
		case 0x44:						// MOV B, H
		{
			state.b = state.h;
			break;
		}
		case 0x45:						// MOV B, L
		{
			state.b = state.l;
			break;
		}
		case 0x46:						// MOV B, M
		{
			uint16_t address = getHL();
			state.b = readMemory(address);
			break;
		}
		case 0x47:						// MOV B, A
		{
			state.b = state.a;
			break;
		}
		case 0x48:						// MOV C, B
		{
			state.c = state.b;
			break;
		}

		case 0x4A:						// MOV C, D
		{
			state.c = state.d;
			break;
		}
		case 0x4B:						// MOV C, E
		{
			state.c = state.e;
			break;
		}
		case 0x4C:						// MOV C, H
		{
			state.c = state.h;
			break;
		}
		case 0x4D:						// MOV C, L
		{
			state.c = state.l;
			break;
		}
#if 0

		case 0x4E:						// MOV C, M
		{
			uint16_t address = getHL();
			state.c = readMemory(address);
			break;
		}
#endif

		case 0x4f:						// MOV C, A
		{
			state.c = state.a;
			break;
		}
		case 0x50:						// MOV D, B
		{
			state.d = state.b;
			break;
		}
		case 0x51:						// MOV D, C
		{
			state.d = state.c;
			break;
		}

		case 0x53:						// MOV D, E
		{
			state.d = state.e;
			break;
		}
		case 0x54:						// MOV D, H
		{
			state.d = state.h;
			break;
		}
		case 0x55:						// MOV D, L
		{
			state.d = state.l;
			break;
		}
		case 0x56:						// MOV D, M
		{
			uint16_t address = getHL();
			state.d = readMemory(address);
			break;
		}
		case 0x57:						// MOV D, A
		{
			state.d = state.a;
			break;
		}
		case 0x58:						// MOV E, B
		{
			state.e = state.b;
			break;
		}
		case 0x59:						// MOV E, C
		{
			state.e = state.c;
			break;
		}
		case 0x5A:						// MOV E, D
		{
			state.e = state.d;
			break;
		}

		case 0x5C:						// MOV E, H
		{
			state.e = state.h;
			break;
		}
		case 0x5D:						// MOV E, L
		{
			state.e = state.l;
			break;
		}
		case 0x5E:						// MOV E, M
		{
			uint16_t address = getHL();
			state.e = readMemory(address);			
			break;
		}
		case 0x5F:						// MOV E, A
		{
			state.e = state.a;
			break;
		}
		case 0x60:						// MOV H, B
		{
			state.h = state.b;
			break;
		}
		case 0x61:						// MOV H, C
		{
			state.h = state.c;
			break;
		}
		case 0x62:						// MOV H, D
		{
			state.h = state.d;
			break;
		}
		case 0x63:						// MOV H, E
		{
			state.h = state.e;
			break;
		}

		case 0x65:						// MOV H, L
		{
			state.h = state.l;
			break;
		}
		case 0x66:						// MOV H, M
		{
			uint16_t address = getHL();
			state.h = readMemory(address);
			break;
		}
		case 0x67:						// MOV H, A
		{
			state.h = state.a;
			break;
		}
		case 0x68:						// MOV L, B
		{
			state.l = state.b;
			break;
		}
		case 0x69:						// MOV L, C
		{
			state.l = state.c;
			break;
		}
		case 0x6A:						// MOV L, D
		{
			state.l = state.d;
			break;
		}
		case 0x6B:						// MOV L, E
		{
			state.l = state.e;
			break;
		}
		case 0x6C:						// MOV L, H
		{
			state.l = state.h;
			break;
		}

		case 0x6E:						// MOV L, M
		{
			uint16_t address = getHL();
			state.l = readMemory(address);
			break;
		}
		case 0x6F:						// MOV L, A
		{
			state.l = state.a;
			break;
		}
		case 0x70:						// MOV M, B
		{
			uint16_t address = getHL();
			writeMemory(address, state.b);
			break;
		}

		case 0x72:						// MOV M, D
		{
			uint16_t address = getHL();
			writeMemory(address, state.d);
			break;
		}
		case 0x73:						// MOV M, E
		{
			uint16_t address = getHL();
			writeMemory(address, state.e);
			break;
		}
		case 0x74:						// MOV M, H
		{
			uint16_t address = getHL();
			writeMemory(address, state.h);
			break;
		}
		case 0x75:						// MOV M, L
		{
			uint16_t address = getHL();
			writeMemory(address, state.l);
			break;
		}

		case 0x77:						// MOV M, A
		{
			uint16_t address = getHL();
			writeMemory(address, state.a);
			break;
		}
		case 0x78:						// MOV A, B
		{
			state.a = state.b;
			break;
		}
		case 0x79:						// MOV A, C
		{
			state.a = state.c;
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
		case 0x7D:						// MOV A, L
		{
			state.a = state.l;
			break;
		}
		case 0x7E:						// MOV A, M
		{
			uint16_t address = getHL();
			state.a = readMemory(address);
			break;
		}
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
		case 0x82:						// ADD D
		{
			uint16_t answer = uint16_t(state.a) + uint16_t(state.d);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x83:						// ADD E
		{
			uint16_t answer = uint16_t(state.a) + uint16_t(state.e);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x84:						// ADD H
		{
			uint16_t answer = uint16_t(state.a) + uint16_t(state.h);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x85:						// ADD L
		{
			uint16_t answer = uint16_t(state.a) + uint16_t(state.l);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x86:						// ADD M
		{
			uint16_t address = getHL();
			uint16_t answer = uint16_t(state.a) + uint16_t(readMemory(address));
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x87:						// ADD A
		{
			uint16_t answer = uint16_t(state.a) + uint16_t(state.a);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x88:						// ADC B
		{
			uint16_t value = state.a + uint16_t(state.b) + uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);
			break;
		}
		case 0x89:						// ADC C
		{
			uint16_t value = state.a + uint16_t(state.c) + uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);
			break;
		}
		case 0x8a:						// ADC D
		{
			uint16_t value = state.a + uint16_t(state.d) + uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);
			break;
		}
		case 0x8b:						// ADC E
		{
			uint16_t value = state.a + uint16_t(state.e) + uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);
			break;
		}
		case 0x8c:						// ADC H
		{
			uint16_t value = state.a + uint16_t(state.h) + uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);
			break;
		}
		case 0x8d:						// ADC L
		{
			uint16_t value = state.a + uint16_t(state.l) + uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);
			break;
		}
		case 0x8e:
		{								// ADC M
			uint16_t address = getHL();
			uint16_t value = state.a + uint16_t(readMemory(address)) + uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);
			break;
		}
		case 0x8f:						// ADC A
		{
			uint16_t value = state.a + uint16_t(state.a) + uint16_t(state.cc.cy);
			updateCY(value);
			updateZSP(value);
			state.a = uint8_t(value & 0xff);
			break;
		}
		case 0x90:						// SUB B
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.b);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x91:						// SUB C
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.c);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x92:						// SUB D
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.d);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x93:						// SUB E
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.e);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x94:						// SUB H
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.h);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x95:						// SUB L
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.l);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x96:						// SUB M
		{
			uint16_t address = getHL();
			uint16_t answer = uint16_t(state.a) - uint16_t(readMemory(address));
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x97:						// SUB A
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.a);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x98:						// SBB B
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.b) - uint16_t(state.cc.cy);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x99:						// SBB C
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.c) - uint16_t(state.cc.cy);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x9a:						// SBB D
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.d) - uint16_t(state.cc.cy);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x9b:						// SBB E
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.e) - uint16_t(state.cc.cy);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x9c:						// SBB H
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.h) - uint16_t(state.cc.cy);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x9d:						// SBB L
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.l) - uint16_t(state.cc.cy);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x9e:						// SBB M
		{
			uint16_t address = getHL();
			uint16_t answer = uint16_t(state.a) - uint16_t(readMemory(address)) - uint16_t(state.cc.cy);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0x9f:						// SBB A
		{
			uint16_t answer = uint16_t(state.a) - uint16_t(state.a) - uint16_t(state.cc.cy);
			updateZSP(answer);
			updateCY(answer);
			state.a = answer & 0xff;
			break;
		}
		case 0xa0:						// ANA B
		{
			state.a &= state.b;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xa1:						// ANA C
		{
			state.a &= state.c;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xa2:						// ANA D
		{
			state.a &= state.d;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xa3:						// ANA E
		{
			state.a &= state.e;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xa4:						// ANA H
		{
			state.a &= state.h;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xa5:						// ANA L
		{
			state.a &= state.l;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xA6:						// ANA M
		{
			uint16_t address = getHL();
			state.a &= readMemory(address);
			updateZSP(state.a);
			updateCY(state.a);
			break;
			break;
		}
		case 0xA7:						// ANA A
		{
			state.a &= state.a;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xA8:						// XRA B
		{
			state.a = state.a ^ state.b;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xA9:						// XRA C
		{
			state.a = state.a ^ state.c;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xAA:						// XRA D
		{
			state.a = state.a ^ state.d;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xAB:						// XRA E
		{
			state.a = state.a ^ state.e;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xAC:						// XRA H
		{
			state.a = state.a ^ state.h;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xAD:						// XRA L
		{
			state.a = state.a ^ state.l;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xAE:						// XRA M
		{
			uint16_t address = getHL();
			state.a = state.a ^ readMemory(address);
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
		case 0xB0:						// ORA B
		{
			state.a = state.a | state.b;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xB1:						// ORA C
		{
			state.a = state.a | state.c;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xB2:						// ORA D
		{
			state.a = state.a | state.d;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xB3:						// ORA E
		{
			state.a = state.a | state.e;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xB4:						// ORA H
		{
			state.a = state.a | state.h;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xB5:						// ORA L
		{
			state.a = state.a | state.l;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xB6:						// ORA M
		{
			uint16_t address = getHL();
			state.a = state.a | readMemory(address);
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xB7:						// ORA a
		{
			state.a = state.a | state.a;
			updateZSP(state.a);
			updateCY(state.a);
			break;
		}
		case 0xB8:						// CMP B
		{
			uint16_t value = uint16_t(state.a) - uint16_t(state.b);
			updateZSP(value);
			updateCY(value);
			break;
		}
		case 0xB9:						// CMP C
		{
			uint16_t value = uint16_t(state.a) - uint16_t(state.c);
			updateZSP(value);
			updateCY(value);
			break;
		}
		case 0xBA:						// CMP D
		{
			uint16_t value = uint16_t(state.a) - uint16_t(state.d);
			updateZSP(value);
			updateCY(value);
			break;
		}
		case 0xBB:						// CMP E
		{
			uint16_t value = uint16_t(state.a) - uint16_t(state.e);
			updateZSP(value);
			updateCY(value);
			break;
		}
		case 0xBC:						// CMP H
		{
			uint16_t value = uint16_t(state.a) - uint16_t(state.h);
			updateZSP(value);
			updateCY(value);
			break;
		}
		case 0xBD:						// CMP L
		{
			uint16_t value = uint16_t(state.a) - uint16_t(state.l);
			updateZSP(value);
			updateCY(value);
			break;
		}
		case 0xBE:						// CMP M
		{
			uint16_t address = getHL();
			uint16_t value = uint16_t(state.a) - uint16_t(readMemory(address));
			updateZSP(value);
			updateCY(value);
			break;
		}
		case 0xBF:						// CMP A
		{
			uint16_t value = uint16_t(state.a) - uint16_t(state.a);
			updateZSP(value);
			updateCY(value);
			break;
		}
		case 0xC0:						// RNZ
		{
			if (state.cc.z == 0) {
				ret();
				return;
			}
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
		case 0xC4:						// CNZ adr
		{
			if (state.cc.z == 0) {
				uint16_t address = readOpcodeD16(opcode);
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
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

		case 0xC8:						// RZ
		{
			if (state.cc.z == 1) {
				ret();
				return;
			}
			break;
		}
		case 0xC9:						// RET
		{
			ret();
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

		case 0xCC:						// CZ adr
		{
			if (state.cc.z == 1) {
				uint16_t address = readOpcodeD16(opcode);
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);
				return;
			}
			else {
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
				// cpudiag - print function

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
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);				
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
			state.a = uint8_t(value & 0xff);

			opcodeSize = 2;
			break;
		}

		case 0xD0:						// RNC
		{
			if (state.cc.cy == 0) {
				ret();
				return;
			}
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
			if (state.cc.cy == 0) {
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
		case 0xD4:						// CNC adr
		{
			if (state.cc.cy == 0) {
				uint16_t address = readOpcodeD16(opcode);
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
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

		case 0xD8:						// RC
		{
			if (state.cc.cy == 1) {
				ret();
				return;
			}

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

		case 0xDC:						// CC adr
		{
			if (state.cc.cy == 1) {
				uint16_t address = readOpcodeD16(opcode);
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
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

		case 0xE0:						// RPO
		{
			if (state.cc.p == 0) {
				ret();
				return;
			}
			break;
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
		case 0xE3:						// XTHL
		{
			uint8_t l = state.l;
			uint8_t h = state.h;
			state.l = readMemory(state.sp);
			state.h = readMemory(state.sp + 1);
			writeMemory(state.sp, l);
			writeMemory(state.sp + 1, h);
			break;
		}
		case 0xE4:						// CPO adr
		{
			if (state.cc.p == 0) {
				uint16_t address = readOpcodeD16(opcode);
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);
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
		case 0xE8:						// RPE
		{
			if (state.cc.p == 1) {
				ret();
				return;
			}

			break;
		}
		case 0xE9:						// PCHL
		{
			state.pc = makeWord(state.h, state.l);
			return;
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
		case 0xEC:						// CPE adr
		{
			if (state.cc.p == 1) {
				uint16_t address = readOpcodeD16(opcode);
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
		}

		case 0xEE:						// XRI
		{
			state.a ^= opcode[1];
			updateCY(state.a);
			updateZSP(state.a);

			opcodeSize = 2;
			break;
		}

		case 0xF0:						// RP
		{
			if (state.cc.s == 0) {
				ret();
				return;
			}

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
		case 0xF4:						// CP
		{
			if (state.cc.s == 0) {
				uint16_t address = readOpcodeD16(opcode);
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
		}
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

		case 0xF8:						// RM
		{
			if (state.cc.s == 1) {
				ret();
				return;
			}
			break;
		}
		case 0xF9:						// SPHL
		{
			state.sp = getHL();
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
		case 0xFC:						// CM addr
		{
			if (state.cc.s == 1) {
				uint16_t address = readOpcodeD16(opcode);
				uint16_t returnAddress = state.pc + 3;
				call(address, returnAddress);
				return;
			}
			else {
				opcodeSize = 3;
				break;
			}
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
	if (isRamMirrorEnabled) {
		while (address >= videoTop) {
			// handle mirroring of RAM above end of video ram
			address -= ramSize;
		}
	}

	assert(address <= videoTop);

	if (address < romSize) {
		if (isRomWriteable) {
			rom[address] = value;
		}
		else {
			assert(!"unable to write to address in ROM");
		}
	} else if (address < workTop) {
		// work RAM
		uint16_t workAddress = address - romSize;
		work[workAddress] = value;
	}
	else {
		// video RAM
		uint16_t videoAddress = address - workTop;
		video[videoAddress] = value;
	}
}

uint8_t Emulator8080::readMemory(uint16_t address) const {
	if (isRamMirrorEnabled) {
		while (address >= videoTop) {
			// handle mirroring of RAM above video RAM
			address -= ramSize;
		}
	}

	assert(address <= videoTop);

	if (address < romSize) {
		return rom[address];
	} else if (address < workTop) {
		// work RAM
		uint16_t workAddress = address - romSize;
		
		return work[workAddress];
	}
	else {
		// video RAM

		uint16_t videoAddress = address - workTop;

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

void Emulator8080::setBC(uint16_t value) {
	state.b = uint8_t((value >> 8) & 0xff);
	state.c = uint8_t(value & 0xff);
}

void Emulator8080::call(uint16_t address, uint16_t returnAddress) {
	uint8_t rethi = uint8_t((returnAddress >> 8) & 0xff);
	uint8_t retlo = uint8_t(returnAddress & 0xff);
	writeMemory(state.sp - 1, rethi);
	writeMemory(state.sp - 2, retlo);
	state.sp = state.sp - 2;
	state.pc = address;
}

void Emulator8080::ret() {
	uint8_t pclo = readMemory(state.sp);
	uint8_t pchi = readMemory(state.sp + 1);
	state.pc = makeWord(pchi, pclo);
	state.sp += 2;
}

uint16_t Emulator8080::getRamTop() const {
	return videoTop;
}