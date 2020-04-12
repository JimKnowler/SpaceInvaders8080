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
#include "BuildOptions.h"

namespace {
    const uint32_t kScreenWidth = 800;
    const uint32_t kScreenHeight = 600;

#ifdef CPUDIAG
	const char* kRomFilename = "./roms/cpudiag.bin";
	const uint16_t kRomLoadAddress = 0x100;
#else
    const char* kRomFilename = "./roms/spaceinvaders/invaders.concatenated";
    const uint16_t kRomLoadAddress = 0;
#endif
}

class SpaceInvaders : public olc::PixelGameEngine
{
public:
    SpaceInvaders() {
        sAppName = "Emulator 8080 - SpaceInvaders";
    }

    bool OnUserCreate() override {
        // called once at start

        loadBinaryFile(kRomFilename, kRomLoadAddress, rom);
        uint16_t pc = kRomLoadAddress;

#ifdef CPUDIAG
        // CPU Test
        
        // fix stack pointer 
        rom[368] = 0x7;

        // skip DAA test
        rom[0x59c] = 0xc3;  // JMP
        rom[0x59d] = 0xc2;
        rom[0x59e] = 0x05;

		emulator.init(&(rom.front()), uint16_t(rom.size()), pc, 2000, 0, false, true);

		// run enough steps to complete test
		// expect to see "CPU IS OPERATIONAL" in console TTY
		step(610);

#else
		// space invaders
		emulator.init(&(rom.front()), uint16_t(rom.size()), pc, 0x2400-0x2000, 0x4000-0x2400, true, true);

		// run the first 50,000 instructions
		step(50000);		
#endif
                
        

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        // update
        if (GetKey(olc::SPACE).bReleased) { 
            if (GetKey(olc::SHIFT).bHeld) {
                if (GetKey(olc::CTRL).bHeld) {
                    step(100);
                }
                else {
                    step(10);
                }
            }
            else {
                step(1);
            }
        }


        // render
        FillRect({ 0,0 }, { ScreenWidth(), ScreenHeight() }, olc::BLUE);

        DrawCPU(10,10);
        DrawOpcodes(200,10);
        DrawMemory("HL", emulator.getHL(), 10, 200);
        DrawMemory("DE", emulator.getDE(), 10, 300);
        DrawStack(200, 200);
       
        return true;
    }

private:
    void step(int stepCount = 1) {
        for (int i = 0; i < stepCount; i++) {
            emulator.step();
        }
    }

    bool loadBinaryFile(const char* filename, uint16_t offset, std::vector<uint8_t>& outData) {
        std::cout << "Loading " << filename << "\n";

        std::ifstream file;
        file.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
        assert(file.is_open());

        size_t size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        outData.resize(size);

        file.read(reinterpret_cast<char*>(&(outData.front())), size);

        std::cout << "Loaded " << filename << " with " << size << " bytes\n";

        if (offset > 0) {
            // prepend buffer
            std::vector<uint8_t> prepend;
            prepend.resize(offset, 0xfa);
            rom.insert(outData.begin(), prepend.begin(), prepend.end());

            std::cout << "Prepended " << filename << " with " << offset << " bytes\n";
        }

        return true;
    }

    void DrawCPU(int x, int y) {

        DrawString({ x, y }, "CPU State");

        const Emulator8080::State& state = emulator.getState();
        uint64_t numSteps = emulator.getNumSteps();

        std::vector<std::string> reports = {
            FormatBuffer("step: %llu", numSteps),
            FormatBuffer("   a: 0x%02x", state.a),
            FormatBuffer("  bc: 0x%02x%02x", state.b, state.c),
            FormatBuffer("  de: 0x%02x%02x", state.d, state.e),
            FormatBuffer("  hl: 0x%02x%02x", state.h, state.l),
            FormatBuffer("  pc: 0x%04x", state.pc),
            FormatBuffer("  sp: 0x%04x", state.sp),
            FormatBuffer("   z: %u", state.cc.z),
            FormatBuffer("   s: %u", state.cc.s),
            FormatBuffer("   p: %u", state.cc.p),
            FormatBuffer("  cy: %u", state.cc.cy),
            FormatBuffer("  ac: %u", state.cc.ac),
        };

        y += 10;
        for (auto it = reports.begin(); it != reports.end(); it++) {
            DrawString({ x + 10, y }, *it);
            y += 10;
        }

    }

    void DrawOpcodes(int x, int y) {
        DrawString({ x, y }, "Opcodes");
        
        const Emulator8080::State& state = emulator.getState();        
        uint16_t pc = state.pc;

        y += 10;
        for (int i = 0; i < 10; i++) {
            std::string instruction;
            uint16_t opcodeSize = Disassemble8080::dissassembleOpcode(&(rom.front()) + pc, instruction);
            
            DrawString({ x + 10, y }, FormatBuffer("0x%04x %s", pc, instruction.c_str()));
            y += 10;

            pc += opcodeSize;
        }
    }

    void DrawMemory(const char* label, uint16_t address, int x, int y) {
        DrawString({ x, y }, FormatBuffer("Memory (%s)", label));

        // 4 byte alignment
        address &= ~3;

        if (address < emulator.getRamTop()) {
            y += 10;
            for (int i = 0; i < 8; i++) {
                DrawString({ x + 10, y }, FormatBuffer("0x%04x %02x %02x %02x %02x", address, emulator.readMemory(address), emulator.readMemory(address + 1), emulator.readMemory(address + 2), emulator.readMemory(address + 3)));

                y += 10;
                address += 4;
            }
        }
    }

    void DrawStack(int x, int y) {
        DrawString({ x, y }, "Stack");

        const Emulator8080::State& state = emulator.getState();
        uint16_t sp = state.sp;
        uint16_t address = sp & ~3;

        y += 10;
        for (int i = 0; i < 10; i++) {
			if (address >= emulator.getRamTop()) {
				break;
			}
            DrawString({ x + 10, y }, FormatBuffer("0x%04x %02x %02x %02x %02x", address, emulator.readMemory(address), emulator.readMemory(address + 1), emulator.readMemory(address + 2), emulator.readMemory(address + 3)));

            y += 10;
            address += 4;
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

int main()
{
    SpaceInvaders demo;
    if (demo.Construct(kScreenWidth, kScreenHeight, 1, 1))
        demo.Start();

    return 0;
}
