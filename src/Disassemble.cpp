#include <cstdio>
#include <cassert>

#include "Disassemble.h"

//http://www.emulator101.com/disassembler-pt-1.html
//http://www.emulator101.com/8080-by-opcode.html

namespace {

    std::string makeOpcode(const char* szOpcode, const char* argsPrefix) {
        char buffer[64];
        sprintf(buffer, "%-7s%s", szOpcode, argsPrefix);

        std::string opcode = buffer;

        return opcode;
    }

    std::string makeOpcodeD16(const char* szOpcode, const char* argsPrefix, uint8_t dataHigh, uint8_t dataLow) {
        char buffer[64];
        sprintf(buffer, "%-7s%s$%02x%02x", szOpcode, argsPrefix, dataHigh, dataLow);

        std::string opcode = buffer;

        return opcode;
    }

    std::string makeOpcodeD8(const char* szOpcode, const char* argsPrefix, uint8_t data) {
        char buffer[64];
        sprintf(buffer, "%-7s%s$%02x", szOpcode, argsPrefix, data);

        std::string opcode = buffer;

        return opcode;
    }

    std::string makeOpcodeNotSupported() {
        //assert(!"not supported");

        std::string opcode = "-";

        return opcode;
    }
}

std::string Disassemble::stringFromOpcode(memory::IMemory* memory, uint16_t pc, uint16_t& outOpcodeSize) {
    uint16_t opcodeSize = 1;

    uint8_t opcode = memory->read(pc);

    std::string strOpcode;

    switch (opcode) {
    case 0x00:
        strOpcode = "NOP";
        break;
    case 0x01:
        strOpcode = makeOpcodeD16("LXI", "B, #", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0x02:
        strOpcode = makeOpcode("STAX", "B");
        break;
    case 0x03:
        strOpcode = makeOpcode("INX", "B");
        break;
    case 0x04:
        strOpcode = makeOpcode("INR", "B");
        break;
    case 0x05:
        strOpcode = makeOpcode("DCR", "B");
        break;
    case 0x06:
        strOpcode = makeOpcodeD8("MVI", "B, #", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0x07:
        strOpcode = "RLC";
        break;
    case 0x08:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0x09:
        strOpcode = makeOpcode("DAD", "B");
        break;
    case 0x0a:
        strOpcode = makeOpcode("LDAX", "B");
        break;
    case 0x0b:
        strOpcode = makeOpcode("DCX", "B");
        break;
    case 0x0c:
        strOpcode = makeOpcode("INR", "C");
        break;
    case 0x0d:
        strOpcode = makeOpcode("DCR", "C");
        break;
    case 0x0e:
        strOpcode = makeOpcodeD8("MVI", "C, #", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0x0f:
        strOpcode = "RRC";
        break;
    case 0x10:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0x11:
        strOpcode = makeOpcodeD16("LXI", "D, #", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0x12:
        strOpcode = makeOpcode("STAX", "D");
        break;
    case 0x13:
        strOpcode = makeOpcode("INX", "D");
        break;
    case 0x14:
        strOpcode = makeOpcode("INR", "D");
        break;
    case 0x15:
        strOpcode = makeOpcode("DCR", "D");
        break;
    case 0x16:
        strOpcode = makeOpcodeD8("MVI", "D, #", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0x17:
        strOpcode = "RAL";
        break;
    case 0x18:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0x19:
        strOpcode = makeOpcode("DAD", "D");
        break;
    case 0x1a:
        strOpcode = makeOpcode("LDAX", "D");
        break;
    case 0x1b:
        strOpcode = makeOpcode("DCX", "D");
        break;
    case 0x1c:
        strOpcode = makeOpcode("INR", "E");
        break;
    case 0x1d:
        strOpcode = makeOpcode("DCR", "E");
        break;
    case 0x1e:
        strOpcode = makeOpcodeD8("MVI", "E, #", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0x1f:
        strOpcode = "RAR";
        break;
    case 0x20:
        strOpcode = "RIM";
        break;
    case 0x21:
        strOpcode = makeOpcodeD16("LXI", "H, #", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0x22:
        strOpcode = makeOpcodeD16("SHLD", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0x23:
        strOpcode = makeOpcode("INX", "H");
        break;
    case 0x24:
        strOpcode = makeOpcode("INR", "H");
        break;
    case 0x25:
        strOpcode = makeOpcode("DCR", "H");
        break;
    case 0x26:
        strOpcode = makeOpcodeD8("MVI", "H, #", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0x27:
        strOpcode = "DAA";
        break;
    case 0x28:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0x29:
        strOpcode = makeOpcode("DAD", "H");
        break;
    case 0x2a:
        strOpcode = makeOpcodeD16("LHLD", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0x2b:
        strOpcode = makeOpcode("DCX", "H");
        break;
    case 0x2c:
        strOpcode = makeOpcode("INR", "L");
        break;
    case 0x2d:
        strOpcode = makeOpcode("DCR", "L");
        break;
    case 0x2e:
        strOpcode = makeOpcodeD8("MVI", "L, #", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0x2f:
        strOpcode = "CMA";
        break;
    case 0x30:
        strOpcode = "SIM";
        break;
    case 0x31:
        strOpcode = makeOpcodeD16("LXI", "SP, #", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0x32:
        strOpcode = makeOpcodeD16("STA", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0x33:
        strOpcode = makeOpcode("INX", "SP");
        break;
    case 0x34:
        strOpcode = makeOpcode("INR", "M");
        break;
    case 0x35:
        strOpcode = makeOpcode("DCR", "M");
        break;
    case 0x36:
        strOpcode = makeOpcodeD8("MVI", "M, #", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0x37:
        strOpcode = "STC";
        break;
    case 0x38:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0x39:
        strOpcode = makeOpcode("DAD", "SP");
        break;
    case 0x3a:
        strOpcode = makeOpcodeD16("LDA", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0x3b:
        strOpcode = makeOpcode("DCX", "SP");
        break;
    case 0x3c:
        strOpcode = makeOpcode("INR", "A");
        break;
    case 0x3d:
        strOpcode = makeOpcode("DCR", "A");
        break;
    case 0x3e:
        strOpcode = makeOpcodeD8("MVI", "A,", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0x3f:
        strOpcode = "CMC";
        break;
    case 0x40:
        strOpcode = makeOpcode("MOV", "B, B");
        break;
    case 0x41:
        strOpcode = makeOpcode("MOV", "B, C");
        break;
    case 0x42:
        strOpcode = makeOpcode("MOV", "B, D");
        break;
    case 0x43:
        strOpcode = makeOpcode("MOV", "B, E");
        break;
    case 0x44:
        strOpcode = makeOpcode("MOV", "B, H");
        break;
    case 0x45:
        strOpcode = makeOpcode("MOV", "B, L");
        break;
    case 0x46:
        strOpcode = makeOpcode("MOV", "B, M");
        break;
    case 0x47:
        strOpcode = makeOpcode("MOV", "B, A");
        break;
    case 0x48:
        strOpcode = makeOpcode("MOV", "C, B");
        break;
    case 0x49:
        strOpcode = makeOpcode("MOV", "C, C");
        break;
    case 0x4a:
        strOpcode = makeOpcode("MOV", "C, D");
        break;
    case 0x4b:
        strOpcode = makeOpcode("MOV", "C, E");
        break;
    case 0x4c:
        strOpcode = makeOpcode("MOV", "C, H");
        break;
    case 0x4d:
        strOpcode = makeOpcode("MOV", "C, L");
        break;
    case 0x4e:
        strOpcode = makeOpcode("MOV", "C, M");
        break;
    case 0x4f:
        strOpcode = makeOpcode("MOV", "C, A");
        break;
    case 0x50:
        strOpcode = makeOpcode("MOV", "D, B");
        break;
    case 0x51:
        strOpcode = makeOpcode("MOV", "D, C");
        break;
    case 0x52:
        strOpcode = makeOpcode("MOV", "D, D");
        break;
    case 0x53:
        strOpcode = makeOpcode("MOV", "D, E");
        break;
    case 0x54:
        strOpcode = makeOpcode("MOV", "D, H");
        break;
    case 0x55:
        strOpcode = makeOpcode("MOV", "D, L");
        break;
    case 0x56:
        strOpcode = makeOpcode("MOV", "D, M");
        break;
    case 0x57:
        strOpcode = makeOpcode("MOV", "D, A");
        break;
    case 0x58:
        strOpcode = makeOpcode("MOV", "E, B");
        break;
    case 0x59:
        strOpcode = makeOpcode("MOV", "E, C");
        break;
    case 0x5a:
        strOpcode = makeOpcode("MOV", "E, D");
        break;
    case 0x5b:
        strOpcode = makeOpcode("MOV", "E, E");
        break;
    case 0x5c:
        strOpcode = makeOpcode("MOV", "E, H");
        break;
    case 0x5d:
        strOpcode = makeOpcode("MOV", "E, L");
        break;
    case 0x5e:
        strOpcode = makeOpcode("MOV", "E, M");
        break;
    case 0x5f:
        strOpcode = makeOpcode("MOV", "E, A");
        break;
    case 0x60:
        strOpcode = makeOpcode("MOV", "H, B");
        break;
    case 0x61:
        strOpcode = makeOpcode("MOV", "H, C");
        break;
    case 0x62:
        strOpcode = makeOpcode("MOV", "H, D");
        break;
    case 0x63:
        strOpcode = makeOpcode("MOV", "H, E");
        break;
    case 0x64:
        strOpcode = makeOpcode("MOV", "H, H");
        break;
    case 0x65:
        strOpcode = makeOpcode("MOV", "H, L");
        break;
    case 0x66:
        strOpcode = makeOpcode("MOV", "H, M");
        break;
    case 0x67:
        strOpcode = makeOpcode("MOV", "H, A");
        break;
    case 0x68:
        strOpcode = makeOpcode("MOV", "L, B");
        break;
    case 0x69:
        strOpcode = makeOpcode("MOV", "L, C");
        break;
    case 0x6a:
        strOpcode = makeOpcode("MOV", "L, D");
        break;
    case 0x6b:
        strOpcode = makeOpcode("MOV", "L, E");
        break;
    case 0x6c:
        strOpcode = makeOpcode("MOV", "L, H");
        break;
    case 0x6d:
        strOpcode = makeOpcode("MOV", "L, L");
        break;
    case 0x6e:
        strOpcode = makeOpcode("MOV", "L, M");
        break;
    case 0x6f:
        strOpcode = makeOpcode("MOV", "L, A");
        break;
    case 0x70:
        strOpcode = makeOpcode("MOV", "M, B");
        break;
    case 0x71:
        strOpcode = makeOpcode("MOV", "M, C");
        break;
    case 0x72:
        strOpcode = makeOpcode("MOV", "M, D");
        break;
    case 0x73:
        strOpcode = makeOpcode("MOV", "M, E");
        break;
    case 0x74:
        strOpcode = makeOpcode("MOV", "M, H");
        break;
    case 0x75:
        strOpcode = makeOpcode("MOV", "M, L");
        break;
    case 0x76:
        strOpcode = "HLT";
        break;
    case 0x77:
        strOpcode = makeOpcode("MOV", "M, A");
        break;
    case 0x78:
        strOpcode = makeOpcode("MOV", "A, B");
        break;
    case 0x79:
        strOpcode = makeOpcode("MOV", "A, C");
        break;
    case 0x7a:
        strOpcode = makeOpcode("MOV", "A, D");
        break;
    case 0x7b:
        strOpcode = makeOpcode("MOV", "A, E");
        break;
    case 0x7c:
        strOpcode = makeOpcode("MOV", "A, H");
        break;
    case 0x7d:
        strOpcode = makeOpcode("MOV", "A, L");
        break;
    case 0x7e:
        strOpcode = makeOpcode("MOV", "A, M");
        break;
    case 0x7f:
        strOpcode = makeOpcode("MOV", "A, A");
        break;
    case 0x80:
        strOpcode = makeOpcode("ADD", "B");
        break;
    case 0x81:
        strOpcode = makeOpcode("ADD", "C");
        break;
    case 0x82:
        strOpcode = makeOpcode("ADD", "D");
        break;
    case 0x83:
        strOpcode = makeOpcode("ADD", "E");
        break;
    case 0x84:
        strOpcode = makeOpcode("ADD", "H");
        break;
    case 0x85:
        strOpcode = makeOpcode("ADD", "L");
        break;
    case 0x86:
        strOpcode = makeOpcode("ADD", "M");
        break;
    case 0x87:
        strOpcode = makeOpcode("ADD", "A");
        break;
    case 0x88:
        strOpcode = makeOpcode("ADC", "B");
        break;
    case 0x89:
        strOpcode = makeOpcode("ADC", "C");
        break;
    case 0x8a:
        strOpcode = makeOpcode("ADC", "D");
        break;
    case 0x8b:
        strOpcode = makeOpcode("ADC", "E");
        break;
    case 0x8c:
        strOpcode = makeOpcode("ADC", "H");
        break;
    case 0x8d:
        strOpcode = makeOpcode("ADC", "L");
        break;
    case 0x8e:
        strOpcode = makeOpcode("ADC", "M");
        break;
    case 0x8f:
        strOpcode = makeOpcode("ADC", "A");
        break;
    case 0x90:
        strOpcode = makeOpcode("SUB", "B");
        break;
    case 0x91:
        strOpcode = makeOpcode("SUB", "C");
        break;
    case 0x92:
        strOpcode = makeOpcode("SUB", "D");
        break;
    case 0x93:
        strOpcode = makeOpcode("SUB", "E");
        break;
    case 0x94:
        strOpcode = makeOpcode("SUB", "H");
        break;
    case 0x95:
        strOpcode = makeOpcode("SUB", "L");
        break;
    case 0x96:
        strOpcode = makeOpcode("SUB", "M");
        break;
    case 0x97:
        strOpcode = makeOpcode("SUB", "A");
        break;
    case 0x98:
        strOpcode = makeOpcode("SBB", "B");
        break;
    case 0x99:
        strOpcode = makeOpcode("SBB", "C");
        break;
    case 0x9a:
        strOpcode = makeOpcode("SBB", "D");
        break;
    case 0x9b:
        strOpcode = makeOpcode("SBB", "E");
        break;
    case 0x9c:
        strOpcode = makeOpcode("SBB", "H");
        break;
    case 0x9d:
        strOpcode = makeOpcode("SBB", "L");
        break;
    case 0x9e:
        strOpcode = makeOpcode("SBB", "M");
        break;
    case 0x9f:
        strOpcode = makeOpcode("SBB", "A");
        break;
    case 0xa0:
        strOpcode = makeOpcode("ANA", "B");
        break;
    case 0xa1:
        strOpcode = makeOpcode("ANA", "C");
        break;
    case 0xa2:
        strOpcode = makeOpcode("ANA", "D");
        break;
    case 0xa3:
        strOpcode = makeOpcode("ANA", "E");
        break;
    case 0xa4:
        strOpcode = makeOpcode("ANA", "H");
        break;
    case 0xa5:
        strOpcode = makeOpcode("ANA", "L");
        break;
    case 0xa6:
        strOpcode = makeOpcode("ANA", "M");
        break;
    case 0xa7:
        strOpcode = makeOpcode("ANA", "A");
        break;
    case 0xa8:
        strOpcode = makeOpcode("XRA", "B");
        break;
    case 0xa9:
        strOpcode = makeOpcode("XRA", "C");
        break;
    case 0xaa:
        strOpcode = makeOpcode("XRA", "D");
        break;
    case 0xab:
        strOpcode = makeOpcode("XRA", "E");
        break;
    case 0xac:
        strOpcode = makeOpcode("XRA", "H");
        break;
    case 0xad:
        strOpcode = makeOpcode("XRA", "L");
        break;
    case 0xae:
        strOpcode = makeOpcode("XRA", "M");
        break;
    case 0xaf:
        strOpcode = makeOpcode("XRA", "A");
        break;
    case 0xb0:
        strOpcode = makeOpcode("ORA", "B");
        break;
    case 0xb1:
        strOpcode = makeOpcode("ORA", "C");
        break;
    case 0xb2:
        strOpcode = makeOpcode("ORA", "D");
        break;
    case 0xb3:
        strOpcode = makeOpcode("ORA", "E");
        break;
    case 0xb4:
        strOpcode = makeOpcode("ORA", "H");
        break;
    case 0xb5:
        strOpcode = makeOpcode("ORA", "L");
        break;
    case 0xb6:
        strOpcode = makeOpcode("ORA", "M");
        break;
    case 0xb7:
        strOpcode = makeOpcode("ORA", "A");
        break;
    case 0xb8:
        strOpcode = makeOpcode("CMP", "B");
        break;
    case 0xb9:
        strOpcode = makeOpcode("CMP", "C");
        break;
    case 0xba:
        strOpcode = makeOpcode("CMP", "D");
        break;
    case 0xbb:
        strOpcode = makeOpcode("CMP", "E");
        break;
    case 0xbc:
        strOpcode = makeOpcode("CMP", "H");
        break;
    case 0xbd:
        strOpcode = makeOpcode("CMP", "L");
        break;
    case 0xbe:
        strOpcode = makeOpcode("CMP", "M");
        break;
    case 0xbf:
        strOpcode = makeOpcode("CMP", "A");
        break;
    case 0xc0:
        strOpcode = "RNZ";
        break;
    case 0xc1:
        strOpcode = makeOpcode("POP", "B");
        break;
    case 0xc2:
        strOpcode = makeOpcodeD16("JNZ", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xc3:
        strOpcode = makeOpcodeD16("JMP", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xc4:
        strOpcode = makeOpcodeD16("CNZ", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xc5:
        strOpcode = makeOpcode("PUSH", "B");
        break;
    case 0xc6:
        strOpcode = makeOpcodeD8("ADI", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xc7:
        strOpcode = makeOpcode("RST", "0");
        break;
    case 0xc8:
        strOpcode = "RZ";
        break;
    case 0xc9:
        strOpcode = "RET";
        break;
    case 0xca:
        strOpcode = makeOpcodeD16("JZ", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xcb:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0xcc:
        strOpcode = makeOpcodeD16("CZ", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xcd:
        strOpcode = makeOpcodeD16("CALL", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xce:
        strOpcode = makeOpcodeD8("ACI", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xcf:
        strOpcode = makeOpcode("RST", "1");
        break;
    case 0xd0:
        strOpcode = "RNC";
        break;
    case 0xd1:
        strOpcode = makeOpcode("POP", "D");
        break;
    case 0xd2:
        strOpcode = makeOpcodeD16("JNC", "", memory->read(pc + 1), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xd3:
        strOpcode = makeOpcodeD8("OUT", "", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xd4:
        strOpcode = makeOpcodeD16("CNC", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xd5:
        strOpcode = makeOpcode("PUSH", "D");
        break;
    case 0xd6:
        strOpcode = makeOpcodeD8("SUI", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xd7:
        strOpcode = makeOpcode("RST", "2");
        break;
    case 0xd8:
        strOpcode = "RC";
        break;
    case 0xd9:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0xda:
        strOpcode = makeOpcodeD16("JC", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xdb:
        strOpcode = makeOpcodeD8("IN", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xdc:
        strOpcode = makeOpcodeD16("CC", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xdd:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0xde:
        strOpcode = makeOpcodeD8("SBI", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xdf:
        strOpcode = makeOpcode("RST", "3");
        break;
    case 0xe0:
        strOpcode = "RPO";
        break;
    case 0xe1:
        strOpcode = makeOpcode("POP", "H");
        break;
    case 0xe2:
        strOpcode = makeOpcodeD16("JPO", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xe3:
        strOpcode = "XTHL";
        break;
    case 0xe4:
        strOpcode = makeOpcodeD16("CPO", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xe5:
        strOpcode = makeOpcode("PUSH", "H");
        break;
    case 0xe6:
        strOpcode = makeOpcodeD8("ANI", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xe7:
        strOpcode = makeOpcode("RST", "4");
        break;
    case 0xe8:
        strOpcode = "RPE";
        break;
    case 0xe9:
        strOpcode = "PCHL";
        break;
    case 0xea:
        strOpcode = makeOpcodeD16("JPE", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xeb:
        strOpcode = "XCHG";
        break;
    case 0xec:
        strOpcode = makeOpcodeD16("CPE", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xed:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0xee:
        strOpcode = makeOpcodeD8("XRI", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xef:
        strOpcode = makeOpcode("RST", "5");
        break;
    case 0xf0:
        strOpcode = "RP";
        break;
    case 0xf1:
        strOpcode = makeOpcode("POP", "PSW");
        break;
    case 0xf2:
        strOpcode = makeOpcodeD16("JP", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xf3:
        strOpcode = "DI";
        break;
    case 0xf4:
        strOpcode = makeOpcodeD16("CP", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xf5:
        strOpcode = makeOpcode("PUSH", "PSW");
        break;
    case 0xf6:
        strOpcode = makeOpcodeD8("ORI", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xf7:
        strOpcode = makeOpcode("RST", "6");
        break;
    case 0xf8:
        strOpcode = "RM";
        break;
    case 0xf9:
        strOpcode = "SPHL";
        break;
    case 0xfa:
        strOpcode = makeOpcodeD16("JM", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xfb:
        strOpcode = "EI";
        break;
    case 0xfc:
        strOpcode = makeOpcodeD16("CM", "", memory->read(pc + 2), memory->read(pc + 1));
        opcodeSize = 3;
        break;
    case 0xfd:
        strOpcode = makeOpcodeNotSupported();
        break;
    case 0xfe:
        strOpcode = makeOpcodeD8("CPI", "#", memory->read(pc + 1));
        opcodeSize = 2;
        break;
    case 0xff:
        strOpcode = makeOpcode("RST", "7");
        break;
    }

    outOpcodeSize = opcodeSize;

    return strOpcode;
}
