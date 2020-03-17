// Disassemble8080.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

//http://www.emulator101.com/disassembler-pt-1.html
//http://www.emulator101.com/8080-by-opcode.html

bool loadBinaryFile(const char* filename, std::vector<uint8_t>& outData) {
    std::cout << "Loading " << filename << "\n";

    std::ifstream file;
    file.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
    assert(file.is_open());

    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    outData.resize(size);

    file.read(reinterpret_cast<char*>(&(outData.front())), size);

    std::cout << "Loaded " << filename << " with " << size << " bytes\n";

    return true;
}

std::string makeOpcode3(const char* szOpcode, uint8_t dataHigh, uint8_t dataLow) {
    char buffer[64];
    sprintf_s(buffer, "%s $%02X%02X", szOpcode, dataHigh, dataLow);

    std::string opcode = buffer;

    return opcode;
}

std::string makeOpcode2(const char* szOpcode, uint8_t data) {
    char buffer[64];
    sprintf_s(buffer, "%s $%02X", szOpcode, data);

    std::string opcode = buffer;

    return opcode;
}

size_t dissassembleOpcode(uint8_t* code, std::string& outOpcode) {
    size_t numBytes = 1;
       
    switch (*code) {
    case 0x00:
        outOpcode = "NOP";
		break;
    case 0x01:
        outOpcode = makeOpcode3("LXI B,", code[2], code[1]);
        numBytes = 3;
		break;
    case 0x02:
        outOpcode = "STAX B";
		break;
    case 0x03:
        outOpcode = "INX B";
		break;
    case 0x04:
        outOpcode = "INR B";
		break;
    case 0x05:
        outOpcode = "DCR B";
		break;
    case 0x06:
        outOpcode = makeOpcode2("MVI B,", code[1]);
        numBytes = 2;
		break;
    case 0x07:
        outOpcode = "RLC";
		break;
    case 0x08:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0x09:
        outOpcode = "DAD B";
		break;
    case 0x0a:
        outOpcode = "LDAX B";
		break;
    case 0x0b:
        outOpcode = "DCX B";
		break;
    case 0x0c:
        outOpcode = "INR C";
		break;
    case 0x0d:
        outOpcode = "DCR C";
		break;
    case 0x0e:
        outOpcode = makeOpcode2("MVI C,", code[1]);
        numBytes = 2;
		break;
    case 0x0f:
        outOpcode = "RRC";
		break;
    case 0x10:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0x11:
        outOpcode = makeOpcode3("LXI D,", code[2], code[1]);
        numBytes = 3;
		break;
    case 0x12:
        outOpcode = "STAX D";
		break;
    case 0x13:
        outOpcode = "INX D";
		break;
    case 0x14:
        outOpcode = "INR D";
		break;
    case 0x15:
        outOpcode = "DCR D";
		break;
    case 0x16:
        outOpcode = makeOpcode2("MVI D,", code[1]);
        numBytes = 2;
		break;
    case 0x17:
        outOpcode = "RAL";
		break;
    case 0x18:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0x19:
        outOpcode = "DAD D";
		break;
    case 0x1a:
        outOpcode = "LDAX D";
		break;
    case 0x1b:
        outOpcode = "DCX D";
		break;
    case 0x1c:
        outOpcode = "INR E";
		break;
    case 0x1d:
        outOpcode = "DCR E";
		break;
    case 0x1e:
        outOpcode = makeOpcode2("MVI E,", code[1]);
        numBytes = 2;
		break;
    case 0x1f:
        outOpcode = "RAR";
		break;
    case 0x20:
        outOpcode = "RIM";
		break;
    case 0x21:
        outOpcode = makeOpcode3("LXI H,", code[2], code[1]);
        numBytes = 3;
		break;
    case 0x22:
        outOpcode = makeOpcode3("SHLD", code[2], code[1]);
        numBytes = 3;
		break;
    case 0x23:
        outOpcode = "INX H";
		break;
    case 0x24:
        outOpcode = "INR H";
		break;
    case 0x25:
        outOpcode = "DCR H";
		break;
    case 0x26:
        outOpcode = makeOpcode2("MVI H,", code[1]);
        numBytes = 2;
		break;
    case 0x27:
        outOpcode = "DAA";
		break;
    case 0x28:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0x29:
        outOpcode = "DAD H";
		break;
    case 0x2a:
        outOpcode = makeOpcode3("LHLD", code[2], code[1]);
        numBytes = 3;
		break;
    case 0x2b:
        outOpcode = "DCX H";
		break;
    case 0x2c:
        outOpcode = "INR L";
		break;
    case 0x2d:
        outOpcode = "DCR L";
		break;
    case 0x2e:
        outOpcode = makeOpcode2("MVI L,", code[1]);
        numBytes = 2;
		break;
    case 0x2f:
        outOpcode = "CMA";
		break;
    case 0x30:
        outOpcode = "SIM";
		break;
    case 0x31:
        outOpcode = makeOpcode3("LXI SP,", code[2], code[1]);
        numBytes = 3;
		break;
    case 0x32:
        outOpcode = makeOpcode3("STA", code[2], code[1]);
        numBytes = 3;
		break;
    case 0x33:
        outOpcode = "INX SP";
		break;
    case 0x34:
        outOpcode = "INR M";
		break;
    case 0x35:
        outOpcode = "DCR M";
		break;
    case 0x36:
        outOpcode = makeOpcode2("MVI M,", code[1]);
        numBytes = 2;
		break;
    case 0x37:
        outOpcode = "STC";
		break;
    case 0x38:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0x39:
        outOpcode = "DAD SP";
		break;
    case 0x3a:
        outOpcode = makeOpcode3("LDA", code[1], code[1]);
        numBytes = 3;
		break;
    case 0x3b:
        outOpcode = "DCX SP";
		break;
    case 0x3c:
        outOpcode = "INR A";
		break;
    case 0x3d:
        outOpcode = "DCR A";
		break;
    case 0x3e:
        outOpcode = makeOpcode2("MVIA A,", code[1]);
        numBytes = 2;
		break;
    case 0x3f:
        outOpcode = "CMC";
		break;
    case 0x40:
        outOpcode = "MOV B, B";
		break;
    case 0x41:
        outOpcode = "MOV B, C";
		break;
    case 0x42:
        outOpcode = "MOV B, D";
		break;
    case 0x43:
        outOpcode = "MOV B, E";
		break;
    case 0x44:
        outOpcode = "MOV B, H";
		break;
    case 0x45:
        outOpcode = "MOV B, L";
		break;
    case 0x46:
        outOpcode = "MOV B, M";
		break;
    case 0x47:
        outOpcode = "MOV B, A";
		break;
    case 0x48:
        outOpcode = "MOV C, B";
		break;
    case 0x49:
        outOpcode = "MOV C, C";
		break;
    case 0x4a:
        outOpcode = "MOV C, D";
		break;
    case 0x4b:
        outOpcode = "MOV C, E";
		break;
    case 0x4c:
        outOpcode = "MOV C, H";
		break;
    case 0x4d:
        outOpcode = "MOV C, L";
		break;
    case 0x4e:
        outOpcode = "MOV C, M";
		break;
    case 0x4f:
        outOpcode = "MOV C, A";
		break;
    case 0x50:
        outOpcode = "MOV D, B";
		break;
    case 0x51:
        outOpcode = "MOV D, C";
		break;
    case 0x52:
        outOpcode = "MOV D, D";
		break;
    case 0x53:
        outOpcode = "MOV D, E";
		break;
    case 0x54:
        outOpcode = "MOV D, H";
		break;
    case 0x55:
        outOpcode = "MOV D, L";
		break;
    case 0x56:
        outOpcode = "MOV D, M";
		break;
    case 0x57:
        outOpcode = "MOV D, A";
		break;
    case 0x58:
        outOpcode = "MOV E, B";
		break;
    case 0x59:
        outOpcode = "MOV E, C";
		break;
    case 0x5a:
        outOpcode = "MOV E, D";
		break;
    case 0x5b:
        outOpcode = "MOV E, E";
		break;
    case 0x5c:
        outOpcode = "MOV E, H";
		break;
    case 0x5d:
        outOpcode = "MOV E, L";
		break;
    case 0x5e:
        outOpcode = "MOV E, M";
		break;
    case 0x5f:
        outOpcode = "MOV E, A";
		break;
    case 0x60:
        outOpcode = "MOV H, B";
		break;
    case 0x61:
        outOpcode = "MOV H, C";
		break;
    case 0x62:
        outOpcode = "MOV H, D";
		break;
    case 0x63:
        outOpcode = "MOV H, E";
		break;
    case 0x64:
        outOpcode = "MOV H, H";
		break;
    case 0x65:
        outOpcode = "MOV H, L";
		break;
    case 0x66:
        outOpcode = "MOV H, M";
		break;
    case 0x67:
        outOpcode = "MOV H, A";
		break;
    case 0x68:
        outOpcode = "MOV L, B";
		break;
    case 0x69:
        outOpcode = "MOV L, C";
		break;
    case 0x6a:
        outOpcode = "MOV L, D";
		break;
    case 0x6b:
        outOpcode = "MOV L, E";
		break;
    case 0x6c:
        outOpcode = "MOV L, H";
		break;
    case 0x6d:
        outOpcode = "MOV L, L";
		break;
    case 0x6e:
        outOpcode = "MOV L, M";
		break;
    case 0x6f:
        outOpcode = "MOV L, A";
		break;
    case 0x70:
        outOpcode = "MOV M, B";
		break;
    case 0x71:
        outOpcode = "MOV M, C";
		break;
    case 0x72:
        outOpcode = "MOV M, D";
		break;
    case 0x73:
        outOpcode = "MOV M, E";
		break;
    case 0x74:
        outOpcode = "MOV M, H";
		break;
    case 0x75:
        outOpcode = "MOV M, L";
		break;
    case 0x76:
        outOpcode = "HLT";
		break;
    case 0x77:
        outOpcode = "MOV M, A";
		break;
    case 0x78:
        outOpcode = "MOV A, B";
		break;
    case 0x79:
        outOpcode = "MOV A, C";
		break;
    case 0x7a:
        outOpcode = "MOV A, D";
		break;
    case 0x7b:
        outOpcode = "MOV A, E";
		break;
    case 0x7c:
        outOpcode = "MOV A, H";
		break;
    case 0x7d:
        outOpcode = "MOV A, L";
		break;
    case 0x7e:
        outOpcode = "MOV A, M";
		break;
    case 0x7f:
        outOpcode = "MOV A, A";
		break;
    case 0x80:
        outOpcode = "ADD B";
		break;
    case 0x81:
        outOpcode = "ADD C";
		break;
    case 0x82:
        outOpcode = "ADD D";
		break;
    case 0x83:
        outOpcode = "ADD E";
		break;
    case 0x84:
        outOpcode = "ADD H";
		break;
    case 0x85:
        outOpcode = "ADD L";
		break;
    case 0x86:
        outOpcode = "ADD M";
		break;
    case 0x87:
        outOpcode = "ADD A";
		break;
    case 0x88:
        outOpcode = "ADC B";
		break;
    case 0x89:
        outOpcode = "ADC C";
		break;
    case 0x8a:
        outOpcode = "ADC D";
		break;
    case 0x8b:
        outOpcode = "ADC E";
		break;
    case 0x8c:
        outOpcode = "ADC H";
		break;
    case 0x8d:
        outOpcode = "ADC L";
		break;
    case 0x8e:
        outOpcode = "ADC M";
		break;
    case 0x8f:
        outOpcode = "ADC A";
		break;
    case 0x90:
        outOpcode = "SUB B";
		break;
    case 0x91:
        outOpcode = "SUB C";
		break;
    case 0x92:
        outOpcode = "SUB D";
		break;
    case 0x93:
        outOpcode = "SUB E";
		break;
    case 0x94:
        outOpcode = "SUB H";
		break;
    case 0x95:
        outOpcode = "SUB L";
		break;
    case 0x96:
        outOpcode = "SUB M";
		break;
    case 0x97:
        outOpcode = "SUB A";
		break;
    case 0x98:
        outOpcode = "SBB B";
		break;
    case 0x99:
        outOpcode = "SBB C";
		break;
    case 0x9a:
        outOpcode = "SBB D";
		break;
    case 0x9b:
        outOpcode = "SBB E";
		break;
    case 0x9c:
        outOpcode = "SBB H";
		break;
    case 0x9d:
        outOpcode = "SBB L";
		break;
    case 0x9e:
        outOpcode = "SBB M";
		break;
    case 0x9f:
        outOpcode = "SBB A";
		break;
    case 0xa0:
        outOpcode = "ANA B";
		break;
    case 0xa1:
        outOpcode = "ANA C";
		break;
    case 0xa2:
        outOpcode = "ANA D";
		break;
    case 0xa3:
        outOpcode = "ANA E";
		break;
    case 0xa4:
        outOpcode = "ANA H";
		break;
    case 0xa5:
        outOpcode = "ANA L";
		break;
    case 0xa6:
        outOpcode = "ANA M";
		break;
    case 0xa7:
        outOpcode = "ANA A";
		break;
    case 0xa8:
        outOpcode = "XRA B";
		break;
    case 0xa9:
        outOpcode = "XRA C";
		break;
    case 0xaa:
        outOpcode = "XRA D";
		break;
    case 0xab:
        outOpcode = "XRA E";
		break;
    case 0xac:
        outOpcode = "XRA H";
		break;
    case 0xad:
        outOpcode = "XRA L";
		break;
    case 0xae:
        outOpcode = "XRA M";
		break;
    case 0xaf:
        outOpcode = "XRA A";
		break;
    case 0xb0:
        outOpcode = "ORA B";
		break;
    case 0xb1:
        outOpcode = "ORA C";
		break;
    case 0xb2:
        outOpcode = "ORA D";
		break;
    case 0xb3:
        outOpcode = "ORA E";
		break;
    case 0xb4:
        outOpcode = "ORA H";
		break;
    case 0xb5:
        outOpcode = "ORA L";
		break;
    case 0xb6:
        outOpcode = "ORA M";
		break;
    case 0xb7:
        outOpcode = "ORA A";
		break;
    case 0xb8:
        outOpcode = "CMP B";
		break;
    case 0xb9:
        outOpcode = "CMP C";
		break;
    case 0xba:
        outOpcode = "CMP D";
		break;
    case 0xbb:
        outOpcode = "CMP E";
		break;
    case 0xbc:
        outOpcode = "CMP H";
		break;
    case 0xbd:
        outOpcode = "CMP L";
		break;
    case 0xbe:
        outOpcode = "CMP M";
		break;
    case 0xbf:
        outOpcode = "CMP A";
		break;
    case 0xc0:
        outOpcode = "RNZ";
		break;
    case 0xc1:
        outOpcode = "POP B";
		break;
    case 0xc2:
        outOpcode = makeOpcode3("JNZ", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xc3:
        outOpcode = makeOpcode3("JMP", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xc4:
        outOpcode = makeOpcode3("CNZ", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xc5:
        outOpcode = "PUSH B";
		break;
    case 0xc6:
        outOpcode = makeOpcode2("ADI", code[1]);
        numBytes = 2;
		break;
    case 0xc7:
        outOpcode = "RST 0";
		break;
    case 0xc8:
        outOpcode = "RZ";
		break;
    case 0xc9:
        outOpcode = "RET";
		break;
    case 0xca:
        outOpcode = makeOpcode3("JZ", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xcb:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0xcc:
        outOpcode = makeOpcode3("CZ", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xcd:
        outOpcode = makeOpcode3("CALL", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xce:
        outOpcode = makeOpcode2("ACI", code[1]);
        numBytes = 2;
		break;
    case 0xcf:
        outOpcode = "RST 1";
		break;
    case 0xd0:
        outOpcode = "RNC";
		break;
    case 0xd1:
        outOpcode = "POP D";
		break;
    case 0xd2:
        outOpcode = makeOpcode3("JNC", code[1], code[1]);
        numBytes = 3;
		break;
    case 0xd3:
        outOpcode = makeOpcode2("OUT", code[1]);
        numBytes = 2;
		break;
    case 0xd4:
        outOpcode = makeOpcode3("CNC", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xd5:
        outOpcode = "PUSH D";
		break;
    case 0xd6:
        outOpcode = makeOpcode2("SUI", code[1]);
        numBytes = 2;
		break;
    case 0xd7:
        outOpcode = "RST 2";
		break;
    case 0xd8:
        outOpcode = "RC";
		break;
    case 0xd9:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0xda:
        outOpcode = makeOpcode3("JC", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xdb:
        outOpcode = "IN B";
        numBytes = 2;
		break;
    case 0xdc:
        outOpcode = makeOpcode3("CC", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xdd:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0xde:
        outOpcode = makeOpcode2("SBI", code[1]);
        numBytes = 2;
		break;
    case 0xdf:
        outOpcode = "RST 3";
		break;
    case 0xe0:
        outOpcode = "RPO";
		break;
    case 0xe1:
        outOpcode = "POP H";
		break;
    case 0xe2:
        outOpcode = makeOpcode3("JPO", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xe3:
        outOpcode = "XTHL";
		break;
    case 0xe4:
        outOpcode = makeOpcode3("CPO", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xe5:
        outOpcode = "PUSH H";
		break;
    case 0xe6:
        outOpcode = makeOpcode2("ANI", code[1]);
        numBytes = 2;
		break;
    case 0xe7:
        outOpcode = "RST 4";
		break;
    case 0xe8:
        outOpcode = "RPE";
		break;
    case 0xe9:
        outOpcode = "PCHL";
		break;
    case 0xea:
        outOpcode = makeOpcode3("JPE", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xeb:
        outOpcode = "XCHG";
		break;
    case 0xec:
        outOpcode = makeOpcode3("CPE", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xed:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0xee:
        outOpcode = makeOpcode2("XRI", code[1]);
        numBytes = 2;
		break;
    case 0xef:
        outOpcode = "RST 5";
		break;
    case 0xf0:
        outOpcode = "RP";
		break;
    case 0xf1:
        outOpcode = "POP PSW";
		break;
    case 0xf2:
        outOpcode = makeOpcode3("JP", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xf3:
        outOpcode = "DI";
		break;
    case 0xf4:
        outOpcode = makeOpcode3("CP", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xf5:
        outOpcode = "PUSH PSW";
		break;
    case 0xf6:
        outOpcode = makeOpcode2("ORI", code[1]);
        numBytes = 2;
		break;
    case 0xf7:
        outOpcode = "RST 6";
		break;
    case 0xf8:
        outOpcode = "RM";
		break;
    case 0xf9:
        outOpcode = "SPHL";
		break;
    case 0xfa:
        outOpcode = makeOpcode3("JM", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xfb:
        outOpcode = "EI";
		break;
    case 0xfc:
        outOpcode = makeOpcode3("CM", code[2], code[1]);
        numBytes = 3;
		break;
    case 0xfd:
        assert(!"not supported");
        outOpcode = "<<<NOT SUPPORTED>>>";
		break;
    case 0xfe:
        outOpcode = makeOpcode2("CPI", code[1]);
        numBytes = 2;
		break;
    case 0xff:
        outOpcode = "RST 7";
		break;
    }

    return numBytes;
}

int main()
{
    const char* filename = "./roms/spaceinvaders/invaders.h";

    std::vector<uint8_t> data;

    loadBinaryFile(filename, data);

    uint8_t* ptr = &(data.front());
    size_t codeSize = data.size();
    size_t pc = 0;

    while (pc < codeSize) {
        std::string opcode;
        size_t numBytes = dissassembleOpcode(ptr + pc, opcode);

        printf("%04x %s\n", pc, opcode.c_str());
        pc += numBytes;
    } 
}


