// Disassemble8080.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdio>
#include <cassert>

#include "Disassemble8080.h"

//http://www.emulator101.com/disassembler-pt-1.html
//http://www.emulator101.com/8080-by-opcode.html

namespace {

    std::string makeOpcode(const char* szOpcode, const char* argsPrefix) {
        char buffer[64];
        sprintf_s(buffer, "%-7s%s", szOpcode, argsPrefix);

        std::string opcode = buffer;

        return opcode;
    }

    std::string makeOpcodeD16(const char* szOpcode, const char* argsPrefix, uint8_t dataHigh, uint8_t dataLow) {
        char buffer[64];
        sprintf_s(buffer, "%-7s%s$%02x%02x", szOpcode, argsPrefix, dataHigh, dataLow);

        std::string opcode = buffer;

        return opcode;
    }

    std::string makeOpcodeD8(const char* szOpcode, const char* argsPrefix, uint8_t data) {
        char buffer[64];
        sprintf_s(buffer, "%-7s%s$%02x", szOpcode, argsPrefix, data);

        std::string opcode = buffer;

        return opcode;
    }

    std::string makeOpcodeNotSupported() {
        //assert(!"not supported");

        std::string opcode = "-";

        return opcode;
    }
}

namespace Disassemble8080 {

    size_t dissassembleOpcode(uint8_t* code, std::string& outOpcode) {
        size_t numBytes = 1;

        switch (*code) {
        case 0x00:
            outOpcode = "NOP";
            break;
        case 0x01:
            outOpcode = makeOpcodeD16("LXI", "B, #", code[2], code[1]);
            numBytes = 3;
            break;
        case 0x02:
            outOpcode = makeOpcode("STAX", "B");
            break;
        case 0x03:
            outOpcode = makeOpcode("INX", "B");
            break;
        case 0x04:
            outOpcode = makeOpcode("INR", "B");
            break;
        case 0x05:
            outOpcode = makeOpcode("DCR", "B");
            break;
        case 0x06:
            outOpcode = makeOpcodeD8("MVI", "B, #", code[1]);
            numBytes = 2;
            break;
        case 0x07:
            outOpcode = "RLC";
            break;
        case 0x08:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0x09:
            outOpcode = makeOpcode("DAD", "B");
            break;
        case 0x0a:
            outOpcode = makeOpcode("LDAX", "B");
            break;
        case 0x0b:
            outOpcode = makeOpcode("DCX", "B");
            break;
        case 0x0c:
            outOpcode = makeOpcode("INR", "C");
            break;
        case 0x0d:
            outOpcode = makeOpcode("DCR", "C");
            break;
        case 0x0e:
            outOpcode = makeOpcodeD8("MVI", "C, #", code[1]);
            numBytes = 2;
            break;
        case 0x0f:
            outOpcode = "RRC";
            break;
        case 0x10:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0x11:
            outOpcode = makeOpcodeD16("LXI", "D, #", code[2], code[1]);
            numBytes = 3;
            break;
        case 0x12:
            outOpcode = makeOpcode("STAX", "D");
            break;
        case 0x13:
            outOpcode = makeOpcode("INX", "D");
            break;
        case 0x14:
            outOpcode = makeOpcode("INR", "D");
            break;
        case 0x15:
            outOpcode = makeOpcode("DCR", "D");
            break;
        case 0x16:
            outOpcode = makeOpcodeD8("MVI", "D, #", code[1]);
            numBytes = 2;
            break;
        case 0x17:
            outOpcode = "RAL";
            break;
        case 0x18:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0x19:
            outOpcode = makeOpcode("DAD", "D");
            break;
        case 0x1a:
            outOpcode = makeOpcode("LDAX", "D");
            break;
        case 0x1b:
            outOpcode = makeOpcode("DCX", "D");
            break;
        case 0x1c:
            outOpcode = makeOpcode("INR", "E");
            break;
        case 0x1d:
            outOpcode = makeOpcode("DCR", "E");
            break;
        case 0x1e:
            outOpcode = makeOpcodeD8("MVI", "E, #", code[1]);
            numBytes = 2;
            break;
        case 0x1f:
            outOpcode = "RAR";
            break;
        case 0x20:
            outOpcode = "RIM";
            break;
        case 0x21:
            outOpcode = makeOpcodeD16("LXI", "H, #", code[2], code[1]);
            numBytes = 3;
            break;
        case 0x22:
            outOpcode = makeOpcodeD16("SHLD", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0x23:
            outOpcode = makeOpcode("INX", "H");
            break;
        case 0x24:
            outOpcode = makeOpcode("INR", "H");
            break;
        case 0x25:
            outOpcode = makeOpcode("DCR", "H");
            break;
        case 0x26:
            outOpcode = makeOpcodeD8("MVI", "H, #", code[1]);
            numBytes = 2;
            break;
        case 0x27:
            outOpcode = "DAA";
            break;
        case 0x28:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0x29:
            outOpcode = makeOpcode("DAD", "H");
            break;
        case 0x2a:
            outOpcode = makeOpcodeD16("LHLD", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0x2b:
            outOpcode = makeOpcode("DCX", "H");
            break;
        case 0x2c:
            outOpcode = makeOpcode("INR", "L");
            break;
        case 0x2d:
            outOpcode = makeOpcode("DCR", "L");
            break;
        case 0x2e:
            outOpcode = makeOpcodeD8("MVI", "L, #", code[1]);
            numBytes = 2;
            break;
        case 0x2f:
            outOpcode = "CMA";
            break;
        case 0x30:
            outOpcode = "SIM";
            break;
        case 0x31:
            outOpcode = makeOpcodeD16("LXI", "SP, #", code[2], code[1]);
            numBytes = 3;
            break;
        case 0x32:
            outOpcode = makeOpcodeD16("STA", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0x33:
            outOpcode = makeOpcode("INX", "SP");
            break;
        case 0x34:
            outOpcode = makeOpcode("INR", "M");
            break;
        case 0x35:
            outOpcode = makeOpcode("DCR", "M");
            break;
        case 0x36:
            outOpcode = makeOpcodeD8("MVI", "M, #", code[1]);
            numBytes = 2;
            break;
        case 0x37:
            outOpcode = "STC";
            break;
        case 0x38:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0x39:
            outOpcode = makeOpcode("DAD", "SP");
            break;
        case 0x3a:
            outOpcode = makeOpcodeD16("LDA", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0x3b:
            outOpcode = makeOpcode("DCX", "SP");
            break;
        case 0x3c:
            outOpcode = makeOpcode("INR", "A");
            break;
        case 0x3d:
            outOpcode = makeOpcode("DCR", "A");
            break;
        case 0x3e:
            outOpcode = makeOpcodeD8("MVI", "A,", code[1]);
            numBytes = 2;
            break;
        case 0x3f:
            outOpcode = "CMC";
            break;
        case 0x40:
            outOpcode = makeOpcode("MOV", "B, B");
            break;
        case 0x41:
            outOpcode = makeOpcode("MOV", "B, C");
            break;
        case 0x42:
            outOpcode = makeOpcode("MOV", "B, D");
            break;
        case 0x43:
            outOpcode = makeOpcode("MOV", "B, E");
            break;
        case 0x44:
            outOpcode = makeOpcode("MOV", "B, H");
            break;
        case 0x45:
            outOpcode = makeOpcode("MOV", "B, L");
            break;
        case 0x46:
            outOpcode = makeOpcode("MOV", "B, M");
            break;
        case 0x47:
            outOpcode = makeOpcode("MOV", "B, A");
            break;
        case 0x48:
            outOpcode = makeOpcode("MOV", "C, B");
            break;
        case 0x49:
            outOpcode = makeOpcode("MOV", "C, C");
            break;
        case 0x4a:
            outOpcode = makeOpcode("MOV", "C, D");
            break;
        case 0x4b:
            outOpcode = makeOpcode("MOV", "C, E");
            break;
        case 0x4c:
            outOpcode = makeOpcode("MOV", "C, H");
            break;
        case 0x4d:
            outOpcode = makeOpcode("MOV", "C, L");
            break;
        case 0x4e:
            outOpcode = makeOpcode("MOV", "C, M");
            break;
        case 0x4f:
            outOpcode = makeOpcode("MOV", "C, A");
            break;
        case 0x50:
            outOpcode = makeOpcode("MOV", "D, B");
            break;
        case 0x51:
            outOpcode = makeOpcode("MOV", "D, C");
            break;
        case 0x52:
            outOpcode = makeOpcode("MOV", "D, D");
            break;
        case 0x53:
            outOpcode = makeOpcode("MOV", "D, E");
            break;
        case 0x54:
            outOpcode = makeOpcode("MOV", "D, H");
            break;
        case 0x55:
            outOpcode = makeOpcode("MOV", "D, L");
            break;
        case 0x56:
            outOpcode = makeOpcode("MOV", "D, M");
            break;
        case 0x57:
            outOpcode = makeOpcode("MOV", "D, A");
            break;
        case 0x58:
            outOpcode = makeOpcode("MOV", "E, B");
            break;
        case 0x59:
            outOpcode = makeOpcode("MOV", "E, C");
            break;
        case 0x5a:
            outOpcode = makeOpcode("MOV", "E, D");
            break;
        case 0x5b:
            outOpcode = makeOpcode("MOV", "E, E");
            break;
        case 0x5c:
            outOpcode = makeOpcode("MOV", "E, H");
            break;
        case 0x5d:
            outOpcode = makeOpcode("MOV", "E, L");
            break;
        case 0x5e:
            outOpcode = makeOpcode("MOV", "E, M");
            break;
        case 0x5f:
            outOpcode = makeOpcode("MOV", "E, A");
            break;
        case 0x60:
            outOpcode = makeOpcode("MOV", "H, B");
            break;
        case 0x61:
            outOpcode = makeOpcode("MOV", "H, C");
            break;
        case 0x62:
            outOpcode = makeOpcode("MOV", "H, D");
            break;
        case 0x63:
            outOpcode = makeOpcode("MOV", "H, E");
            break;
        case 0x64:
            outOpcode = makeOpcode("MOV", "H, H");
            break;
        case 0x65:
            outOpcode = makeOpcode("MOV", "H, L");
            break;
        case 0x66:
            outOpcode = makeOpcode("MOV", "H, M");
            break;
        case 0x67:
            outOpcode = makeOpcode("MOV", "H, A");
            break;
        case 0x68:
            outOpcode = makeOpcode("MOV", "L, B");
            break;
        case 0x69:
            outOpcode = makeOpcode("MOV", "L, C");
            break;
        case 0x6a:
            outOpcode = makeOpcode("MOV", "L, D");
            break;
        case 0x6b:
            outOpcode = makeOpcode("MOV", "L, E");
            break;
        case 0x6c:
            outOpcode = makeOpcode("MOV", "L, H");
            break;
        case 0x6d:
            outOpcode = makeOpcode("MOV", "L, L");
            break;
        case 0x6e:
            outOpcode = makeOpcode("MOV", "L, M");
            break;
        case 0x6f:
            outOpcode = makeOpcode("MOV", "L, A");
            break;
        case 0x70:
            outOpcode = makeOpcode("MOV", "M, B");
            break;
        case 0x71:
            outOpcode = makeOpcode("MOV", "M, C");
            break;
        case 0x72:
            outOpcode = makeOpcode("MOV", "M, D");
            break;
        case 0x73:
            outOpcode = makeOpcode("MOV", "M, E");
            break;
        case 0x74:
            outOpcode = makeOpcode("MOV", "M, H");
            break;
        case 0x75:
            outOpcode = makeOpcode("MOV", "M, L");
            break;
        case 0x76:
            outOpcode = "HLT";
            break;
        case 0x77:
            outOpcode = makeOpcode("MOV", "M, A");
            break;
        case 0x78:
            outOpcode = makeOpcode("MOV", "A, B");
            break;
        case 0x79:
            outOpcode = makeOpcode("MOV", "A, C");
            break;
        case 0x7a:
            outOpcode = makeOpcode("MOV", "A, D");
            break;
        case 0x7b:
            outOpcode = makeOpcode("MOV", "A, E");
            break;
        case 0x7c:
            outOpcode = makeOpcode("MOV", "A, H");
            break;
        case 0x7d:
            outOpcode = makeOpcode("MOV", "A, L");
            break;
        case 0x7e:
            outOpcode = makeOpcode("MOV", "A, M");
            break;
        case 0x7f:
            outOpcode = makeOpcode("MOV", "A, A");
            break;
        case 0x80:
            outOpcode = makeOpcode("ADD", "B");
            break;
        case 0x81:
            outOpcode = makeOpcode("ADD", "C");
            break;
        case 0x82:
            outOpcode = makeOpcode("ADD", "D");
            break;
        case 0x83:
            outOpcode = makeOpcode("ADD", "E");
            break;
        case 0x84:
            outOpcode = makeOpcode("ADD", "H");
            break;
        case 0x85:
            outOpcode = makeOpcode("ADD", "L");
            break;
        case 0x86:
            outOpcode = makeOpcode("ADD", "M");
            break;
        case 0x87:
            outOpcode = makeOpcode("ADD", "A");
            break;
        case 0x88:
            outOpcode = makeOpcode("ADC", "B");
            break;
        case 0x89:
            outOpcode = makeOpcode("ADC", "C");
            break;
        case 0x8a:
            outOpcode = makeOpcode("ADC", "D");
            break;
        case 0x8b:
            outOpcode = makeOpcode("ADC", "E");
            break;
        case 0x8c:
            outOpcode = makeOpcode("ADC", "H");
            break;
        case 0x8d:
            outOpcode = makeOpcode("ADC", "L");
            break;
        case 0x8e:
            outOpcode = makeOpcode("ADC", "M");
            break;
        case 0x8f:
            outOpcode = makeOpcode("ADC", "A");
            break;
        case 0x90:
            outOpcode = makeOpcode("SUB", "B");
            break;
        case 0x91:
            outOpcode = makeOpcode("SUB", "C");
            break;
        case 0x92:
            outOpcode = makeOpcode("SUB", "D");
            break;
        case 0x93:
            outOpcode = makeOpcode("SUB", "E");
            break;
        case 0x94:
            outOpcode = makeOpcode("SUB", "H");
            break;
        case 0x95:
            outOpcode = makeOpcode("SUB", "L");
            break;
        case 0x96:
            outOpcode = makeOpcode("SUB", "M");
            break;
        case 0x97:
            outOpcode = makeOpcode("SUB", "A");
            break;
        case 0x98:
            outOpcode = makeOpcode("SBB", "B");
            break;
        case 0x99:
            outOpcode = makeOpcode("SBB", "C");
            break;
        case 0x9a:
            outOpcode = makeOpcode("SBB", "D");
            break;
        case 0x9b:
            outOpcode = makeOpcode("SBB", "E");
            break;
        case 0x9c:
            outOpcode = makeOpcode("SBB", "H");
            break;
        case 0x9d:
            outOpcode = makeOpcode("SBB", "L");
            break;
        case 0x9e:
            outOpcode = makeOpcode("SBB", "M");
            break;
        case 0x9f:
            outOpcode = makeOpcode("SBB", "A");
            break;
        case 0xa0:
            outOpcode = makeOpcode("ANA", "B");
            break;
        case 0xa1:
            outOpcode = makeOpcode("ANA", "C");
            break;
        case 0xa2:
            outOpcode = makeOpcode("ANA", "D");
            break;
        case 0xa3:
            outOpcode = makeOpcode("ANA", "E");
            break;
        case 0xa4:
            outOpcode = makeOpcode("ANA", "H");
            break;
        case 0xa5:
            outOpcode = makeOpcode("ANA", "L");
            break;
        case 0xa6:
            outOpcode = makeOpcode("ANA", "M");
            break;
        case 0xa7:
            outOpcode = makeOpcode("ANA", "A");
            break;
        case 0xa8:
            outOpcode = makeOpcode("XRA", "B");
            break;
        case 0xa9:
            outOpcode = makeOpcode("XRA", "C");
            break;
        case 0xaa:
            outOpcode = makeOpcode("XRA", "D");
            break;
        case 0xab:
            outOpcode = makeOpcode("XRA", "E");
            break;
        case 0xac:
            outOpcode = makeOpcode("XRA", "H");
            break;
        case 0xad:
            outOpcode = makeOpcode("XRA", "L");
            break;
        case 0xae:
            outOpcode = makeOpcode("XRA", "M");
            break;
        case 0xaf:
            outOpcode = makeOpcode("XRA", "A");
            break;
        case 0xb0:
            outOpcode = makeOpcode("ORA", "B");
            break;
        case 0xb1:
            outOpcode = makeOpcode("ORA", "C");
            break;
        case 0xb2:
            outOpcode = makeOpcode("ORA", "D");
            break;
        case 0xb3:
            outOpcode = makeOpcode("ORA", "E");
            break;
        case 0xb4:
            outOpcode = makeOpcode("ORA", "H");
            break;
        case 0xb5:
            outOpcode = makeOpcode("ORA", "L");
            break;
        case 0xb6:
            outOpcode = makeOpcode("ORA", "M");
            break;
        case 0xb7:
            outOpcode = makeOpcode("ORA", "A");
            break;
        case 0xb8:
            outOpcode = makeOpcode("CMP", "B");
            break;
        case 0xb9:
            outOpcode = makeOpcode("CMP", "C");
            break;
        case 0xba:
            outOpcode = makeOpcode("CMP", "D");
            break;
        case 0xbb:
            outOpcode = makeOpcode("CMP", "E");
            break;
        case 0xbc:
            outOpcode = makeOpcode("CMP", "H");
            break;
        case 0xbd:
            outOpcode = makeOpcode("CMP", "L");
            break;
        case 0xbe:
            outOpcode = makeOpcode("CMP", "M");
            break;
        case 0xbf:
            outOpcode = makeOpcode("CMP", "A");
            break;
        case 0xc0:
            outOpcode = "RNZ";
            break;
        case 0xc1:
            outOpcode = makeOpcode("POP", "B");
            break;
        case 0xc2:
            outOpcode = makeOpcodeD16("JNZ", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xc3:
            outOpcode = makeOpcodeD16("JMP", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xc4:
            outOpcode = makeOpcodeD16("CNZ", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xc5:
            outOpcode = makeOpcode("PUSH", "B");
            break;
        case 0xc6:
            outOpcode = makeOpcodeD8("ADI", "#", code[1]);
            numBytes = 2;
            break;
        case 0xc7:
            outOpcode = makeOpcode("RST", "0");
            break;
        case 0xc8:
            outOpcode = "RZ";
            break;
        case 0xc9:
            outOpcode = "RET";
            break;
        case 0xca:
            outOpcode = makeOpcodeD16("JZ", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xcb:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0xcc:
            outOpcode = makeOpcodeD16("CZ", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xcd:
            outOpcode = makeOpcodeD16("CALL", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xce:
            outOpcode = makeOpcodeD8("ACI", "#", code[1]);
            numBytes = 2;
            break;
        case 0xcf:
            outOpcode = makeOpcode("RST", "1");
            break;
        case 0xd0:
            outOpcode = "RNC";
            break;
        case 0xd1:
            outOpcode = makeOpcode("POP", "D");
            break;
        case 0xd2:
            outOpcode = makeOpcodeD16("JNC", "", code[1], code[1]);
            numBytes = 3;
            break;
        case 0xd3:
            outOpcode = makeOpcodeD8("OUT", "", code[1]);
            numBytes = 2;
            break;
        case 0xd4:
            outOpcode = makeOpcodeD16("CNC", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xd5:
            outOpcode = makeOpcode("PUSH", "D");
            break;
        case 0xd6:
            outOpcode = makeOpcodeD8("SUI", "#", code[1]);
            numBytes = 2;
            break;
        case 0xd7:
            outOpcode = makeOpcode("RST", "2");
            break;
        case 0xd8:
            outOpcode = "RC";
            break;
        case 0xd9:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0xda:
            outOpcode = makeOpcodeD16("JC", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xdb:
            outOpcode = makeOpcodeD8("IN", "#", code[1]);
            numBytes = 2;
            break;
        case 0xdc:
            outOpcode = makeOpcodeD16("CC", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xdd:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0xde:
            outOpcode = makeOpcodeD8("SBI", "#", code[1]);
            numBytes = 2;
            break;
        case 0xdf:
            outOpcode = makeOpcode("RST", "3");
            break;
        case 0xe0:
            outOpcode = "RPO";
            break;
        case 0xe1:
            outOpcode = makeOpcode("POP", "H");
            break;
        case 0xe2:
            outOpcode = makeOpcodeD16("JPO", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xe3:
            outOpcode = "XTHL";
            break;
        case 0xe4:
            outOpcode = makeOpcodeD16("CPO", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xe5:
            outOpcode = makeOpcode("PUSH", "H");
            break;
        case 0xe6:
            outOpcode = makeOpcodeD8("ANI", "#", code[1]);
            numBytes = 2;
            break;
        case 0xe7:
            outOpcode = makeOpcode("RST", "4");
            break;
        case 0xe8:
            outOpcode = "RPE";
            break;
        case 0xe9:
            outOpcode = "PCHL";
            break;
        case 0xea:
            outOpcode = makeOpcodeD16("JPE", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xeb:
            outOpcode = "XCHG";
            break;
        case 0xec:
            outOpcode = makeOpcodeD16("CPE", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xed:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0xee:
            outOpcode = makeOpcodeD8("XRI", "#", code[1]);
            numBytes = 2;
            break;
        case 0xef:
            outOpcode = makeOpcode("RST", "5");
            break;
        case 0xf0:
            outOpcode = "RP";
            break;
        case 0xf1:
            outOpcode = makeOpcode("POP", "PSW");
            break;
        case 0xf2:
            outOpcode = makeOpcodeD16("JP", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xf3:
            outOpcode = "DI";
            break;
        case 0xf4:
            outOpcode = makeOpcodeD16("CP", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xf5:
            outOpcode = makeOpcode("PUSH", "PSW");
            break;
        case 0xf6:
            outOpcode = makeOpcodeD8("ORI", "#", code[1]);
            numBytes = 2;
            break;
        case 0xf7:
            outOpcode = makeOpcode("RST", "6");
            break;
        case 0xf8:
            outOpcode = "RM";
            break;
        case 0xf9:
            outOpcode = "SPHL";
            break;
        case 0xfa:
            outOpcode = makeOpcodeD16("JM", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xfb:
            outOpcode = "EI";
            break;
        case 0xfc:
            outOpcode = makeOpcodeD16("CM", "", code[2], code[1]);
            numBytes = 3;
            break;
        case 0xfd:
            outOpcode = makeOpcodeNotSupported();
            break;
        case 0xfe:
            outOpcode = makeOpcodeD8("CPI", "#", code[1]);
            numBytes = 2;
            break;
        case 0xff:
            outOpcode = makeOpcode("RST", "7");
            break;
        }

        return numBytes;
    }

}


