#include "cpu/CPU.h"
#include "util/Utils.h"

#include "Disassemble8080.h"
#include "BuildOptions.h"

#include <cstring>
#include <cassert>

namespace cpu {

	CPU::CPU() {	
		state.reset();

		rom = 0;
		romSize = 0;		
		numSteps = 0;

		isRamMirrorEnabled = false;
		isRomWriteable = false;

		ramSize = 0;
		workTop = 0;
		videoTop = 0;
	}

	void CPU::init(uint8_t* inRom, uint16_t inRomSize, uint16_t inPc, uint16_t inWorkSize, uint16_t inVideoSize, bool inIsRamMirrorEnabled, bool inIsRomWriteable) {
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

	void CPU::setCallbackIn(CallbackIn callback) {
		callbackIn = callback;
	}

	void CPU::setCallbackOut(CallbackOut callback) {
		callbackOut = callback;
	}

	uint64_t CPU::getNumSteps() const {
		return numSteps;
	}

	const State& CPU::getState() const {
		return state;
	}

	void CPU::step() {
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
				uint16_t address = state.bc;
				writeMemory(address, state.a);
				break;
			}
			case 0x03:						// INX B
			{
				uint16_t value = state.bc;
				value += 1;
				state.bc = value;
				break;
			}
			case 0x04:						// INR B
			{
				state.b += 1;
				state.cc.updateByteZSP(state.b);
				break;
			}
			case 0x05:						// DCR B
			{
				uint8_t value = state.b - 1;
				state.cc.updateByteZSP(value);
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
				uint32_t hl = state.hl;
				uint32_t bc = state.bc;
				uint32_t value = hl + bc;
				state.cc.updateWordCY(value);
				state.hl = value & 0xffff;
				break;
			}
			case 0x0A:						// LDAX B
			{
				uint16_t address = state.bc;
				state.a = readMemory(address);
				break;
			}
			case 0x0B:						// DCX B
			{
				uint16_t value = state.bc;
				value -= 1;
				state.bc = value;
				break;
			}
			case 0x0C:						// INR C
			{
				state.c += 1;
				state.cc.updateByteZSP(state.c);
				break;
			}
			case 0x0D:						// DCR C
			{
				state.c -= 1;
				state.cc.updateByteZSP(state.c);
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
				uint16_t address = state.de;
				writeMemory(address, state.a);
				break;
			}
			case 0x13:						// INX D
			{
				uint16_t value = state.de;
				value += 1;
				state.de = value;
				break;
			}
			case 0x14:						// INR D
			{
				state.d += 1;
				state.cc.updateByteZSP(state.d);
				break;
			}
			case 0x15:						// DCR D
			{
				state.d -= 1;
				state.cc.updateByteZSP(state.d);
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
				uint32_t de = state.de;
				uint32_t hl = state.hl;
				uint32_t value = hl + de;
				state.cc.updateWordCY(value);
				state.hl = value & 0xffff;
				break;
			}

			case 0x1A:						// LDAX D
			{
				uint16_t address = state.de;
				state.a = readMemory(address);
				break;
			}
			case 0x1B:						// DCX D
			{
				uint16_t value = state.de;
				value -= 1;
				state.de = value;
				break;
			}
			case 0x1C:						// INR E
			{
				state.e += 1;
				state.cc.updateByteZSP(state.e);
				break;
			}
			case 0x1D:						// DCR E
			{
				state.e -= 1;
				state.cc.updateByteZSP(state.e);
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
				uint16_t value = state.hl;
				value += 1;
				state.hl = value;
				break;
			}
			case 0x24:						// INR H
			{
				state.h += 1;
				state.cc.updateByteZSP(state.h);
				break;
			}
			case 0x25:						// DCR H
			{
				state.h -= 1;
				state.cc.updateByteZSP(state.h);
				break;
			}
			case 0x26:						// MVI H, D8
			{
				state.h = opcode[1];
				opcodeSize = 2;
				break;
			}
			case 0x27:						// DAA
			{
				//http://z80-heaven.wikidot.com/instructions-set:daa

				// todo: implement auxilliary carry flag for DAA instruction
				if (/*(1 == state.cc.ac) ||*/ (state.a & 0x0f) > 9) {
					state.a += 6;

					// todo: deal with this step causing state.cc.cy
				}

				if ((1 == state.cc.cy) || ((state.a & 0xf0) > 0x90)) {
					state.a += 0x60;
					state.cc.cy = 1;
					state.cc.updateByteZSP(state.a);
				}

				break;
			}

			case 0x29:						// DAD H
			{
				uint32_t hl = state.hl;
				uint32_t value = hl + hl;
				state.cc.updateWordCY(value);
				state.hl = value & 0xffff;
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
				uint16_t value = state.hl;
				value -= 1;
				state.hl = value;
				break;
			}
			case 0x2C:						// INR L
			{
				state.l += 1;
				state.cc.updateByteZSP(state.l);
				break;
			}
			case 0x2D:						// DCR L
			{
				state.l -= 1;
				state.cc.updateByteZSP(state.l);
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
				uint16_t address = state.hl;
				uint16_t value = readMemory(address);
				value += 1;
				state.cc.updateByteZSP(value);
				writeMemory(address, uint8_t(value & 0xff));
				break;
			}
			case 0x35:						// DCR M
			{
				uint16_t address = state.hl;
				uint16_t value = readMemory(address);
				value -= 1;
				state.cc.updateByteZSP(value);
				writeMemory(address, uint8_t(value & 0xff));
				break;
			}
			case 0x36:						// MVI M, D8
			{
				uint8_t value = opcode[1];
				uint16_t address = state.hl;
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
				uint32_t value = uint32_t(state.hl) + uint32_t(state.sp);
				state.cc.updateWordCY(value);
				state.hl = uint16_t(value & 0xffff);
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
				state.cc.updateByteZSP(state.a);
				break;
			}
			case 0x3d:						// DCR A
			{
				state.a -= 1;
				state.cc.updateByteZSP(state.a);
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
				uint16_t address = state.hl;
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
			case 0x4E:						// MOV C, M
			{
				uint16_t address = state.hl;
				state.c = readMemory(address);
				break;
			}
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
				uint16_t address = state.hl;
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
				uint16_t address = state.hl;
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
				uint16_t address = state.hl;
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
				uint16_t address = state.hl;
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
				uint16_t address = state.hl;
				writeMemory(address, state.b);
				break;
			}
			case 0x71:						// MOV M, C
			{
				uint16_t address = state.hl;
				writeMemory(address, state.c);
				break;
			}
			case 0x72:						// MOV M, D
			{
				uint16_t address = state.hl;
				writeMemory(address, state.d);
				break;
			}
			case 0x73:						// MOV M, E
			{
				uint16_t address = state.hl;
				writeMemory(address, state.e);
				break;
			}
			case 0x74:						// MOV M, H
			{
				uint16_t address = state.hl;
				writeMemory(address, state.h);
				break;
			}
			case 0x75:						// MOV M, L
			{
				uint16_t address = state.hl;
				writeMemory(address, state.l);
				break;
			}

			case 0x77:						// MOV M, A
			{
				uint16_t address = state.hl;
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
				uint16_t address = state.hl;
				state.a = readMemory(address);
				break;
			}
			case 0x80:						// ADD B
			{
				uint16_t answer = uint16_t(state.a) + uint16_t(state.b);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x81:						// ADD C
			{
				uint16_t answer = uint16_t(state.a) + uint16_t(state.c);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x82:						// ADD D
			{
				uint16_t answer = uint16_t(state.a) + uint16_t(state.d);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x83:						// ADD E
			{
				uint16_t answer = uint16_t(state.a) + uint16_t(state.e);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x84:						// ADD H
			{
				uint16_t answer = uint16_t(state.a) + uint16_t(state.h);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x85:						// ADD L
			{
				uint16_t answer = uint16_t(state.a) + uint16_t(state.l);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x86:						// ADD M
			{
				uint16_t address = state.hl;
				uint16_t answer = uint16_t(state.a) + uint16_t(readMemory(address));
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x87:						// ADD A
			{
				uint16_t answer = uint16_t(state.a) + uint16_t(state.a);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x88:						// ADC B
			{
				uint16_t value = state.a + uint16_t(state.b) + uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);
				break;
			}
			case 0x89:						// ADC C
			{
				uint16_t value = state.a + uint16_t(state.c) + uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);
				break;
			}
			case 0x8a:						// ADC D
			{
				uint16_t value = state.a + uint16_t(state.d) + uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);
				break;
			}
			case 0x8b:						// ADC E
			{
				uint16_t value = state.a + uint16_t(state.e) + uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);
				break;
			}
			case 0x8c:						// ADC H
			{
				uint16_t value = state.a + uint16_t(state.h) + uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);
				break;
			}
			case 0x8d:						// ADC L
			{
				uint16_t value = state.a + uint16_t(state.l) + uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);
				break;
			}
			case 0x8e:
			{								// ADC M
				uint16_t address = state.hl;
				uint16_t value = state.a + uint16_t(readMemory(address)) + uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);
				break;
			}
			case 0x8f:						// ADC A
			{
				uint16_t value = state.a + uint16_t(state.a) + uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);
				break;
			}
			case 0x90:						// SUB B
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.b);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x91:						// SUB C
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.c);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x92:						// SUB D
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.d);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x93:						// SUB E
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.e);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x94:						// SUB H
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.h);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x95:						// SUB L
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.l);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x96:						// SUB M
			{
				uint16_t address = state.hl;
				uint16_t answer = uint16_t(state.a) - uint16_t(readMemory(address));
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x97:						// SUB A
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.a);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x98:						// SBB B
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.b) - uint16_t(state.cc.cy);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x99:						// SBB C
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.c) - uint16_t(state.cc.cy);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x9a:						// SBB D
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.d) - uint16_t(state.cc.cy);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x9b:						// SBB E
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.e) - uint16_t(state.cc.cy);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x9c:						// SBB H
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.h) - uint16_t(state.cc.cy);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x9d:						// SBB L
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.l) - uint16_t(state.cc.cy);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x9e:						// SBB M
			{
				uint16_t address = state.hl;
				uint16_t answer = uint16_t(state.a) - uint16_t(readMemory(address)) - uint16_t(state.cc.cy);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0x9f:						// SBB A
			{
				uint16_t answer = uint16_t(state.a) - uint16_t(state.a) - uint16_t(state.cc.cy);
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = answer & 0xff;
				break;
			}
			case 0xa0:						// ANA B
			{
				state.a &= state.b;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xa1:						// ANA C
			{
				state.a &= state.c;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xa2:						// ANA D
			{
				state.a &= state.d;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xa3:						// ANA E
			{
				state.a &= state.e;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xa4:						// ANA H
			{
				state.a &= state.h;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xa5:						// ANA L
			{
				state.a &= state.l;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xA6:						// ANA M
			{
				uint16_t address = state.hl;
				state.a &= readMemory(address);
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xA7:						// ANA A
			{
				state.a &= state.a;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xA8:						// XRA B
			{
				state.a = state.a ^ state.b;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xA9:						// XRA C
			{
				state.a = state.a ^ state.c;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xAA:						// XRA D
			{
				state.a = state.a ^ state.d;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xAB:						// XRA E
			{
				state.a = state.a ^ state.e;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xAC:						// XRA H
			{
				state.a = state.a ^ state.h;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xAD:						// XRA L
			{
				state.a = state.a ^ state.l;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xAE:						// XRA M
			{
				uint16_t address = state.hl;
				state.a = state.a ^ readMemory(address);
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xAf:						// XRA A
			{
				state.a = state.a ^ state.a;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB0:						// ORA B
			{
				state.a = state.a | state.b;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB1:						// ORA C
			{
				state.a = state.a | state.c;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB2:						// ORA D
			{
				state.a = state.a | state.d;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB3:						// ORA E
			{
				state.a = state.a | state.e;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB4:						// ORA H
			{
				state.a = state.a | state.h;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB5:						// ORA L
			{
				state.a = state.a | state.l;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB6:						// ORA M
			{
				uint16_t address = state.hl;
				state.a = state.a | readMemory(address);
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB7:						// ORA a
			{
				state.a = state.a | state.a;
				state.cc.updateByteZSP(state.a);
				state.cc.updateByteCY(state.a);
				break;
			}
			case 0xB8:						// CMP B
			{
				uint16_t value = uint16_t(state.a) - uint16_t(state.b);
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				break;
			}
			case 0xB9:						// CMP C
			{
				uint16_t value = uint16_t(state.a) - uint16_t(state.c);
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				break;
			}
			case 0xBA:						// CMP D
			{
				uint16_t value = uint16_t(state.a) - uint16_t(state.d);
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				break;
			}
			case 0xBB:						// CMP E
			{
				uint16_t value = uint16_t(state.a) - uint16_t(state.e);
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				break;
			}
			case 0xBC:						// CMP H
			{
				uint16_t value = uint16_t(state.a) - uint16_t(state.h);
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				break;
			}
			case 0xBD:						// CMP L
			{
				uint16_t value = uint16_t(state.a) - uint16_t(state.l);
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				break;
			}
			case 0xBE:						// CMP M
			{
				uint16_t address = state.hl;
				uint16_t value = uint16_t(state.a) - uint16_t(readMemory(address));
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				break;
			}
			case 0xBF:						// CMP A
			{
				uint16_t value = uint16_t(state.a) - uint16_t(state.a);
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				break;
			}
			case 0xC0:						// RNZ
			{
				if (state.cc.z == 0) {
					ret();
					opcodeSize = 0;
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
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
			}
			case 0xC3:						// JMP adr
			{
				uint16_t address = readOpcodeD16(opcode);
				state.pc = address;
				opcodeSize = 0;
				break;
			}
			case 0xC4:						// CNZ adr
			{
				if (state.cc.z == 0) {
					uint16_t address = readOpcodeD16(opcode);
					uint16_t returnAddress = state.pc + 3;
					call(address, returnAddress);
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
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
				state.cc.updateByteZSP(answer);
				state.cc.updateByteCY(answer);
				state.a = uint8_t(answer & 0xff);
				opcodeSize = 2;
				break;
			}

			case 0xC8:						// RZ
			{
				if (state.cc.z == 1) {
					ret();
					opcodeSize = 0;
				}
				break;
			}
			case 0xC9:						// RET
			{
				ret();
				opcodeSize = 0;
				break;
			}
			case 0xCA:						// JZ adr
			{
				if (state.cc.z != 0) {
					uint16_t address = readOpcodeD16(opcode);
					state.pc = address;
					opcodeSize = 0;
				} else {
					opcodeSize = 3;
				}

				break;
			}

			case 0xCC:						// CZ adr
			{
				if (state.cc.z == 1) {
					uint16_t address = readOpcodeD16(opcode);
					uint16_t returnAddress = state.pc + 3;
					call(address, returnAddress);
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
			}
			case 0xCD:						// CALL adr
			{

				uint16_t address = readOpcodeD16(opcode);
	#ifdef CPUDIAG
				if (5 == address)
				{
					// cpudiag - print function

					if (state.c == 9)
					{
						uint16_t offset = state.de;

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

				opcodeSize = 0;
				break;
			}

			case 0xCE:						// ACI D8
			{
				uint16_t value = state.a;
				value += uint16_t(opcode[1]);
				value += uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);

				opcodeSize = 2;
				break;
			}

			case 0xD0:						// RNC
			{
				if (state.cc.cy == 0) {
					ret();
					opcodeSize = 0;
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
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
			}
			case 0xD3:						// OUT D8 (special)
			{
				uint8_t data = opcode[1];
				if (callbackOut) {
					callbackOut(data, state.a);
				}
				opcodeSize = 2;
				break;
			}
			case 0xD4:						// CNC adr
			{
				if (state.cc.cy == 0) {
					uint16_t address = readOpcodeD16(opcode);
					uint16_t returnAddress = state.pc + 3;
					call(address, returnAddress);
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

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
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);

				opcodeSize = 2;
				break;
			}

			case 0xD8:						// RC
			{
				if (state.cc.cy == 1) {
					ret();
					opcodeSize = 0;
				}

				break;
			}

			case 0xDA:						// JC addr
			{
				if (state.cc.cy == 1) {
					uint16_t address = readOpcodeD16(opcode);
					state.pc = address;
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}
				break;
			}
			case 0xDB:						// IN D8 (special)
			{
				uint8_t port = opcode[1];
				if (callbackIn) {
					state.a = callbackIn(port);
				}
				opcodeSize = 2;
				break;
			}
			case 0xDC:						// CC adr
			{
				if (state.cc.cy == 1) {
					uint16_t address = readOpcodeD16(opcode);
					uint16_t returnAddress = state.pc + 3;
					call(address, returnAddress);
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
			}

			case 0xDE:						// SBI D8
			{
				uint8_t data = opcode[1];
				uint16_t value = uint16_t(state.a) - uint16_t(data) - uint16_t(state.cc.cy);
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = uint8_t(value & 0xff);

				opcodeSize = 2;
				break;
			}

			case 0xE0:						// RPO
			{
				if (state.cc.p == 0) {
					ret();
					opcodeSize = 0;
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
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
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
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
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
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);
				state.a = uint8_t(value & 0xff);
				opcodeSize = 2;
				break;
			}
			case 0xE8:						// RPE
			{
				if (state.cc.p == 1) {
					ret();
					opcodeSize = 0;
				}

				break;
			}
			case 0xE9:						// PCHL
			{
				state.pc = util::makeWord(state.h, state.l);
				opcodeSize = 0;
				break;
			}
			case 0xEA:						// JPE adr
			{
				if (state.cc.p == 1) {
					uint16_t address = readOpcodeD16(opcode);
					state.pc = address;
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}
				break;
			}
			case 0xEB:						// XCHG
			{
				util::swap(state.h, state.d);
				util::swap(state.l, state.e);
				break;
			}
			case 0xEC:						// CPE adr
			{
				if (state.cc.p == 1) {
					uint16_t address = readOpcodeD16(opcode);
					uint16_t returnAddress = state.pc + 3;
					call(address, returnAddress);
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
			}

			case 0xEE:						// XRI
			{
				state.a ^= opcode[1];
				state.cc.updateByteCY(state.a);
				state.cc.updateByteZSP(state.a);

				opcodeSize = 2;
				break;
			}

			case 0xF0:						// RP
			{
				if (state.cc.s == 0) {
					ret();
					opcodeSize = 0;
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
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}
				break;
			}

	#if 0
			case 0xF3:						// DI (special)
			{
				state.interruptsEnabled = false;
				break;
			}
	#endif
			case 0xF4:						// CP
			{
				if (state.cc.s == 0) {
					uint16_t address = readOpcodeD16(opcode);
					uint16_t returnAddress = state.pc + 3;
					call(address, returnAddress);
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
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
				state.cc.updateByteCY(value);
				state.cc.updateByteZSP(value);
				state.a = value;
				opcodeSize = 2;
				break;
			}

			case 0xF8:						// RM
			{
				if (state.cc.s == 1) {
					ret();
					opcodeSize = 0;
				}
				break;
			}
			case 0xF9:						// SPHL
			{
				state.sp = state.hl;
				break;
			}
			case 0xFA:						// JM
			{
				if (state.cc.s == 1) {
					uint16_t address = readOpcodeD16(opcode);
					state.pc = address;
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
			}
			case 0xFB:						// EI (special)
			{
				state.interruptsEnabled = true;
				break;
			}
			case 0xFC:						// CM addr
			{
				if (state.cc.s == 1) {
					uint16_t address = readOpcodeD16(opcode);
					uint16_t returnAddress = state.pc + 3;
					call(address, returnAddress);
					opcodeSize = 0;
				}
				else {
					opcodeSize = 3;
				}

				break;
			}

			case 0xFE:						// CPI D8
			{
				uint16_t value = uint16_t(state.a) - uint16_t(opcode[1]);
				state.cc.updateByteZSP(value);
				state.cc.updateByteCY(value);

				opcodeSize = 2;
				break;
			}

			default:
				opcodeSize = unimplementedOpcode(state.pc);
				break;
		}

		state.pc += uint16_t(opcodeSize);

		if (!breakpoints.opcode.empty() && (breakpoints.opcode.find(state.pc) != breakpoints.opcode.end())) {
			if (callbackBreakpoint) {
				Breakpoint breakpoint(Breakpoint::Type::Opcode, state.pc);
				callbackBreakpoint(breakpoint, 0);
			}
		}
	}

	size_t CPU::unimplementedOpcode(uint16_t pc) {
		assert(pc < romSize);

		uint8_t* opcode = rom + pc;

		std::string strOpcode;
		size_t numBytes = Disassemble8080::dissassembleOpcode(opcode, strOpcode);


		printf("unimplemented Instruction: %04x 0x%02x %s\n", pc, *opcode, strOpcode.c_str());

		return numBytes;
	}

	uint16_t CPU::readOpcodeD16(uint8_t* opcode) {
		uint16_t value = util::makeWord(opcode[2], opcode[1]);

		return value;
	}

	void CPU::writeMemory(uint16_t address, uint8_t value) {
		if (isRamMirrorEnabled) {
			while (address >= videoTop) {
				// handle mirroring of RAM above end of video ram
				address -= ramSize;
			}
		}

		assert(address <= videoTop);
	
		if (!breakpoints.memoryWrite.empty() && (breakpoints.memoryWrite.find(address) != breakpoints.memoryWrite.end())) {
			if (callbackBreakpoint) {
				Breakpoint breakpoint(Breakpoint::Type::MemoryWrite, address);
				callbackBreakpoint(breakpoint, value);
			}
		}
	
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

	uint8_t CPU::readMemory(uint16_t address) const {
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

	void CPU::call(uint16_t address, uint16_t returnAddress) {
		uint8_t rethi = uint8_t((returnAddress >> 8) & 0xff);
		uint8_t retlo = uint8_t(returnAddress & 0xff);
		writeMemory(state.sp - 1, rethi);
		writeMemory(state.sp - 2, retlo);
		state.sp = state.sp - 2;
		state.pc = address;
	}

	void CPU::ret() {
		uint8_t pclo = readMemory(state.sp);
		uint8_t pchi = readMemory(state.sp + 1);
		state.pc = util::makeWord(pchi, pclo);
		state.sp += 2;
	}

	uint16_t CPU::getRamTop() const {
		return videoTop;
	}

	void CPU::interrupt(int interruptNum) {
		if (!state.interruptsEnabled) {
			return;
		}
		// push PC to stack
		uint8_t pclo = uint8_t(state.pc & 0xff);
		uint8_t pchi = uint8_t((state.pc >> 8) & 0xff);
		writeMemory(state.sp - 1, pchi);
		writeMemory(state.sp - 2, pclo);
		state.sp -= 2;

		// jump to interrupt vector
		state.pc = 8 * interruptNum;
	}

	const std::vector<uint8_t> CPU::getVideoRam() const {
		return video;
	}

	void CPU::addBreakpoint(const Breakpoint& breakpoint) {
		switch (breakpoint.type) {
		case Breakpoint::Type::MemoryWrite:
			breakpoints.memoryWrite.insert(breakpoint.address);
			break;
		case Breakpoint::Type::Opcode:
			breakpoints.opcode.insert(breakpoint.address);
			break;
		}	
	}

	void CPU::setCallbackBreakpoint(CallbackBreakpoint callback) {
		callbackBreakpoint = callback;
	}

}
