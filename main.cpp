#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

//#define OLC_PGE_GAMEPAD
//#include "olcPGEX_Gamepad.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "Disassemble8080.h"
#include "Emulator8080.h"

namespace {
    const uint32_t kScreenWidth = 800;
    const uint32_t kScreenHeight = 600;

    const char* kRomFilename = "./roms/spaceinvaders/invaders.concatenated";
}

class SpaceInvaders : public olc::PixelGameEngine
{
public:
    SpaceInvaders() {
        sAppName = "Emulator 8080 - SpaceInvaders";
    }

    bool OnUserCreate() override {
        // called once at start

        loadBinaryFile(kRomFilename, rom);
        uint16_t pc = 0;

        emulator.init(&(rom.front()), rom.size(), pc);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        // update
        if (GetKey(olc::SPACE).bReleased) {
            emulator.step();
        }

        // render
        
        FillRect({ 0,0 }, { ScreenWidth(), ScreenHeight() }, olc::BLUE);

        DrawState();
       
        return true;
    }

private:
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

    void DrawState() {
        
        DrawString({ 10, 10 }, "CPU State");

        const Emulator8080::State& state = emulator.getState();
        uint64_t numSteps = emulator.getNumSteps();

        std::vector<std::string> reports = {
            FormatBuffer("step: %llu", numSteps),
            FormatBuffer("   a: 0x%02x", state.a),
            FormatBuffer("   b: 0x%02x", state.b),
            FormatBuffer("   c: 0x%02x", state.c),
            FormatBuffer("   d: 0x%02x", state.d),
            FormatBuffer("   e: 0x%02x", state.e),
            FormatBuffer("   h: 0x%02x", state.h),
            FormatBuffer("   l: 0x%02x", state.l),
            FormatBuffer("  sp: %04x", state.sp),
            FormatBuffer("  pc: %04x", state.pc),
            FormatBuffer("   z: %u", state.cc.z),
            FormatBuffer("   s: %u", state.cc.s),
            FormatBuffer("   p: %u", state.cc.p),
            FormatBuffer("  cy: %u", state.cc.cy),
            FormatBuffer("  ac: %u", state.cc.ac),
        };
        
        {
            int y = 20;
            for (auto it = reports.begin(); it != reports.end(); it++) {
                DrawString({ 20, y }, *it);
                y += 10;
            }
        }


        DrawString({ 200, 10 }, "Instructions");
        
        uint16_t pc = state.pc;

        int y = 20;
        for (int i = 0; i < 10; i++) {
            std::string instruction;
            uint16_t opcodeSize = Disassemble8080::dissassembleOpcode(&(rom.front()) + pc, instruction);
            
            DrawString({ 210, y }, FormatBuffer("0x%04x %s", pc, instruction.c_str()));
            y += 10;

            pc += opcodeSize;
        }
    }

    std::string FormatBuffer(const char* format, ...) {        
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsprintf_s(buffer, format, args);        
        va_end(args);
        
        std::string ret = buffer;

        return ret;
    }
    
    std::vector<uint8_t> rom;
    Emulator8080 emulator;

};

#if 0
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
#endif



int main()
{
    SpaceInvaders demo;
    if (demo.Construct(kScreenWidth, kScreenHeight, 1, 1))
        demo.Start();

    return 0;
}
