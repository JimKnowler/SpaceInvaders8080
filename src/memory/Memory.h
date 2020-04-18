#pragma once

#include "memory/IMemory.h"

#include <vector>

namespace memory {

	class Memory : public IMemory {
	public:
		Memory();

		/// @struct Config
		/// @brief Describe a contiguous memory map, starting with ROM and followed by RAM
		struct Config {
			Config();

			uint16_t sizeRom;
			uint16_t sizeRam;
			bool isRomWriteable;
			bool isRamMirrored;
		};

		// Configure memory with the supplied configuration
		bool configure(const Config& config);

		// Load a ROM from filename at specified address
		bool load(const char* filename, uint16_t address = 0);

		// Return total size of memory map
		uint16_t size() const;

	public: // IMemory
		uint16_t translate(uint16_t address) const override;
		void write(uint16_t address, uint8_t value) override;
		uint8_t read(uint16_t address) const override;

	private:
		Config config;

		std::vector<uint8_t> memory;
	};
}