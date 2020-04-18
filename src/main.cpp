#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

//#define OLC_PGE_GAMEPAD
//#include "olcPGEX_Gamepad.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <chrono>

#include "cpu/CPU.h"

#include "Disassemble8080.h"
#include "BuildOptions.h"

namespace {
    const uint32_t kScreenWidth = 1000;
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
		
		mode = Mode::Debugger;

#ifdef CPUDIAG
        // CPU Test
        
        // fix stack pointer 
        rom[368] = 0x7;

		emulator.init(&(rom.front()), uint16_t(rom.size()), pc, 2000, 0, false, true);

		// run enough steps to complete test
		// expect to see "CPU IS OPERATIONAL" in console TTY
		step(650);

#else
		// space invaders
        initIO();

		emulator.init(&(rom.front()), uint16_t(rom.size()), pc, 0x2400-0x2000, 0x4000-0x2400, true, true);


# if 0
		// debugging 'credits' 

		// address of 'credits' in memory discovered by using old school 'Game Genie' method of looking for byte that 
		//   changed when number of credits was incremented / decremented
		emulator.addBreakpoint(Breakpoint(Breakpoint::Type::MemoryWrite, 8192 + 235));

		// PC where credits is incremented
		emulator.addBreakpoint(Breakpoint(Breakpoint::Type::Opcode, 0x0038));

		// PC where credits is decremented
		emulator.addBreakpoint(Breakpoint(Breakpoint::Type::Opcode, 0x079b));
# endif

		// callback invoked when a breakpoint is reached
		emulator.setCallbackBreakpoint([&](const cpu::Breakpoint& breakpoint, uint16_t value) {
			switch (breakpoint.type) {
			case cpu::Breakpoint::Type::MemoryWrite:
				printf("PC [0x%04x] Memory Write - address [0x%04x] - changing value from [%u] to [%u]\n", emulator.getState().pc, breakpoint.address, emulator.readMemory(breakpoint.address), value);
				break;
			case cpu::Breakpoint::Type::Opcode:
				printf("PC [0x%04x] Opcode\n", breakpoint.address);
				break;
			default:
				printf("unknown breakpoint\n");
			}

			mode = Mode::Debugger;
		});
#endif
                
		timeLastInterrupt = getCurrentTime();
		interruptNum = 1;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        // update
		updateInput();
		
		switch (mode) {
			case Mode::Debugger:
				updateStep();
				break;
			case Mode::Run:
				updateFrame();
				break;
		}
	
        // render
        FillRect({ 0,0 }, { ScreenWidth(), ScreenHeight() }, olc::BLUE);

		DrawString({ 10,10 }, FormatBuffer("Mode [%s]", (mode == Mode::Debugger) ? "DEBUGGER" : "RUN"));
        DrawCPU(10,40);
        DrawOpcodes(200,40);
        DrawMemory("HL", emulator.getState().hl, 10, 200);
        DrawMemory("DE", emulator.getState().de, 10, 300);
        DrawStack(200, 200);
		
#ifndef CPUDIAG
		// space invaders
		DrawVideoRam(400, 10);
#endif

        return true;
    }

	void updateFrame() {
		bool isFrameComplete = false;

		while (!isFrameComplete) {
			const int kNumStepsPerChunk = 10000;

			for (int i = 0; (i < kNumStepsPerChunk) && (mode == Mode::Run); i++) {
				emulator.step();
			}

			if (mode == Mode::Debugger) {
				// a breakpoint was fired
				break;
			}

			auto currentTime = getCurrentTime();
			const float elapsedTime = std::chrono::duration<float>(currentTime - timeLastInterrupt).count();
			static const float kFrameDuration = 1.0f / 60.0f;
			
			// Space Invaders - vblank/vhalf interrupts
			if (elapsedTime > kFrameDuration) {
				timeLastInterrupt = currentTime;
				
				emulator.interrupt(interruptNum);

				alternateInterruptNum();

				isFrameComplete = true;
			}
		}
	}

	void updateStep() {
		if (GetKey(olc::SPACE).bReleased) {
			if (GetKey(olc::SHIFT).bHeld) {
				if (GetKey(olc::CTRL).bHeld) {
					// CTRL + SHIFT + SPACE
					step(100);
				}
				else {
					// SHIFT + SPACE
					step(10);
				}
			}
			else {
				// SPACE
				step(1);
			}
		}
	}

	void updateInput() {
		if (GetKey(olc::D).bPressed) {
			mode = Mode::Debugger;
		}
		else if (GetKey(olc::R).bPressed) {
			mode = Mode::Run;
		}
	}

private:
    void initIO() {
		// space invaders 
		emulator.setCallbackIn([this](uint8_t port) -> uint8_t {
			//printf("IN port %u\n", port);

			uint8_t a = 0;
			switch (port) {
			case 0:
				a = (1 << 1) |									// always 1
					(1 << 2) |									// always 1
					(1 << 3) |									// always 1
					((GetKey(olc::SPACE).bHeld ? 1 : 0) << 4) |	// P1 Shoot
					((GetKey(olc::LEFT).bHeld ? 1 : 0) << 5) |	// P1 Left
					((GetKey(olc::RIGHT).bHeld ? 1 : 0) << 6);	// P1 Right
				break;
			case 1:
				a =
					(GetKey(olc::C).bHeld ? 0 : 1) |			// Coin
					((GetKey(olc::K2).bHeld ? 1 : 0) << 1) |		// P2 Start Button
					((GetKey(olc::K1).bHeld ? 1 : 0) << 2) |		// P1 Start Button
					(1 << 3) |									// always 1
					((GetKey(olc::SPACE).bHeld ? 1 : 0) << 4) |	// P1 Shoot
					((GetKey(olc::LEFT).bHeld ? 1 : 0) << 5) |	// P1 Left
					((GetKey(olc::RIGHT).bHeld ? 1 : 0) << 6);	// P1 Right
				break;
			case 2:
				a =
					(GetKey(olc::SHIFT).bHeld ? 1 : 0) << 4,	// P2 Shoot
					(GetKey(olc::Z).bHeld ? 1 : 0) << 5,		// P2 Left
					(GetKey(olc::X).bHeld ? 1 : 0) << 6;		// P2 Right
				break;
			case 3:
				// read from shift register
				{
					uint8_t shiftAmount = 8 - shiftRegisterResultOffset;
					uint16_t shiftedResult = shiftRegister >> shiftAmount;
					a = uint8_t(shiftedResult & 0xff);
				}
				break;
			default:
				break;
			}

			return a;
		});

		emulator.setCallbackOut([this](uint8_t port, uint8_t value) -> void {
			//printf("OUT port %u value %u\n", port, value);

			switch (port) {
			case 2:
				// 3 bit shift register offset
				shiftRegisterResultOffset = value & 0x7;
				break;
			case 4:
				// push to high byte of shift register
				shiftRegister >>= 8;
				shiftRegister |= (uint16_t(value) << 8);
				break;
			default:
				break;
			}
		});
    }

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

        const cpu::State& state = emulator.getState();
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
        
        const cpu::State& state = emulator.getState();
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

        const cpu::State& state = emulator.getState();
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

	void DrawVideoRam(int x, int y) {
		const std::vector<uint8_t> videoRam = emulator.getVideoRam();

		int i = 0;
		const int kVideoRamWidth = 224;
		const int kVideoRamHeight = 256;

		for (int ix = 0; ix < kVideoRamWidth; ix++) {
			for (int iy = 0; iy < kVideoRamHeight; iy +=8) {
				uint8_t byte = videoRam[i++];

				for (int b = 0; b < 8; b++) {
					FillRect({ x + ( ix << 1), y + ((kVideoRamHeight - (iy + b)) << 1) }, { 2,2 }, ((byte & 0x1) == 0x1) ? olc::WHITE : olc::BLACK);
					byte >>= 1;
				}
			}
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

	std::chrono::time_point<std::chrono::system_clock> getCurrentTime() const {
		return std::chrono::system_clock::now();
	}

	void alternateInterruptNum() {
		// interruptNum 1 => simulate vsync when beam is near the middle of the screen
		// interruptNum 2 => simulate vsync when beam is near the bottom of the screen

		interruptNum = (interruptNum == 1) ? 2 : 1;
	}
    
    std::vector<uint8_t> rom;
	cpu::CPU emulator;

	uint8_t shiftRegisterResultOffset;
	uint16_t shiftRegister;
	
	enum class Mode {
		Debugger,
		Run
	};
	Mode mode;

	std::chrono::time_point<std::chrono::system_clock> timeLastInterrupt;
	int interruptNum;
};

int main()
{
    SpaceInvaders demo;
    if (demo.Construct(kScreenWidth, kScreenHeight, 1, 1))
        demo.Start();

    return 0;
}