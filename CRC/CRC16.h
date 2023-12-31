#pragma once
#include <vector>

class CRC16 {
private:
	std::vector<uint16_t> table;

	// Private constructor so that no objects can be created.
	CRC16() {
		// Precompute the CRC-16 table
		table.resize(256);
		for (uint16_t i = 0; i < table.size(); ++i) {
			uint16_t remainder = i;
			for (uint16_t bit = 0; bit < 8; ++bit) {
				if (remainder & 1)
					remainder = (remainder >> 1) ^ 0xA001;
				else
					remainder >>= 1;
			}
			table[i] = remainder;
		}
	}

public:
	// Deleted copy constructor and assignment operator to prevent copying
	CRC16(const CRC16&) = delete;
	CRC16& operator=(const CRC16&) = delete;

	// Static access method
	static CRC16& getInstance() {
		static CRC16 instance;
		return instance;
	}

	uint16_t calculate(const char* data, size_t size) {
		uint16_t crc = 0xFFFF;
		for (size_t i = 0; i < size; ++i)
			crc = (crc >> 8) ^ table[(crc & 0xff) ^ (uint8_t)data[i]];
		return crc;
	}
};