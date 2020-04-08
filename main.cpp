#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "Disassemble8080.h"
#include "Emulator8080.h"

//#define ENABLE_DISASSEMBLER

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

void disassemble(std::vector<uint8_t>& data, uint16_t inPc) {
    uint8_t* ptr = &(data.front());
    size_t codeSize = data.size();
    size_t pc = size_t(inPc);

    while (pc < codeSize) {
        std::string opcode;
        size_t numBytes = Disassemble8080::dissassembleOpcode(ptr + pc, opcode);

        printf("%04x %s\n", pc, opcode.c_str());
        pc += numBytes;
    }
}

void emulate(std::vector<uint8_t>& data, uint16_t pc) {
    Emulator8080 emulator;
    uint8_t* ptr = &(data.front());
    emulator.init(ptr, data.size(), 0);

    uint32_t step = 0;
    while (true) {
        emulator.step();

        step += 1;
    }
}

int main()
{
    const char* filename = "./roms/spaceinvaders/invaders.concatenated";

    std::vector<uint8_t> data;

    loadBinaryFile(filename, data);

    uint16_t pc = 0;

#ifdef ENABLE_DISASSEMBLER
    disassemble(data, pc);
#endif

    emulate(data, pc);

    return 0;
}

