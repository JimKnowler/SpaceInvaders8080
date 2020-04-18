#include "memory/Memory.h"
#include "util/Utils.h"

#include <cassert>
#include <iostream>
#include <fstream>

namespace memory {
	Memory::Memory() {

	}

	Memory::Config::Config() : 
		sizeRom(0), sizeRam(0),
		isRomWriteable(false),
		isRamMirrored(false)
	{

	}

	bool Memory::configure(const Config& inConfig) {
		config = inConfig;

		if (memory.size() != size()) {
			memory.resize(size(), 0xfe);
		}

		return true;
	}

	bool Memory::load(const char* filename, uint16_t address) {
		std::cout << "Loading " << filename << "\n";

		size_t size = util::getFileSize(filename);

		std::ifstream file;
		file.open(filename, std::ios::in | std::ios::binary);
		assert(file.is_open());

		std::vector<uint8_t> data;
		data.resize(size);

		file.read(reinterpret_cast<char*>(&(data.front())), size);

		file.close();

		for (size_t i = 0; i < size; i++) {
			size_t writeAddress = size_t(address) + i;
			memory[writeAddress] = data[i];
		}

		std::cout << "Loaded " << filename << " with " << size << " bytes\n";

		return true;
	}

	uint16_t Memory::size() const {
		return config.sizeRam + config.sizeRom;
	}

	uint16_t Memory::translate(uint16_t inAddress) const {
		uint16_t address = inAddress;

		if (config.isRamMirrored) {
			while (address >= size()) {
				address -= config.sizeRam;
			}
		}

		assert(address <= size());

		return address;
	}

	uint8_t Memory::read(uint16_t address) const {
		return memory[address];
	}

	void Memory::write(uint16_t address, uint8_t value) {
		if (address < config.sizeRom) {
			assert(!"unable to write to address in ROM");
			return;
		}

		memory[address] = value;		
	}
}