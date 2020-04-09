#pragma once

#include <cstdint>
#include <vector>

class Emulator8080 {
public:
    Emulator8080();

    // initialise the emulator
    void init(uint8_t* rom, size_t romSize,  uint16_t pc);

    // step through an instruction at current address in pc
    void step();

private:
    void updateZSP(uint16_t answer);
    void updateCY(uint16_t value);
    size_t unimplementedOpcode(uint16_t pc);
    uint16_t readOpcodeD16(uint8_t* opcode);
    void writeMemory(uint16_t address, uint8_t value);
    uint8_t readMemory(uint16_t address);
    uint16_t getDE();
    void setDE(uint16_t value);
    uint16_t getHL();
    void setHL(uint16_t value);
    uint16_t getBC();

    struct ConditionCodes {
        uint8_t    z : 1;
        uint8_t    s : 1;
        uint8_t    p : 1;
        uint8_t    cy : 1;
        uint8_t    ac : 1;
        uint8_t    pad : 3;
    };

    struct State {
        uint8_t     a;
        uint8_t     b;
        uint8_t     c;
        uint8_t     d;
        uint8_t     e;
        uint8_t     h;
        uint8_t     l;
        uint16_t    sp;
        uint16_t    pc;
    };

    State           state;
    ConditionCodes  cc;
    uint8_t*        rom;  
    size_t          romSize;
    bool            interuptsEnabled;

    std::vector<uint8_t> work;
    std::vector<uint8_t> video;
};