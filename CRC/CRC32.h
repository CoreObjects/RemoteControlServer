#pragma once
#include <vector>
class CRC32 {
private:
	std::vector<uint32_t> table;

public:
	CRC32() {
		// Precompute the CRC-32 table
		table.resize(256);
		for (uint32_t i = 0; i < table.size(); ++i) {
			uint32_t remainder = i;
			for (uint32_t bit = 0; bit < 8; ++bit) {
				if (remainder & 1)
					remainder = (remainder >> 1) ^ 0xedb88320;
				else
					remainder >>= 1;
			}
			table[i] = remainder;
		}
	}

	uint32_t calculate(const char* data, size_t size) {
		uint32_t crc = 0xffffffff;
		for (size_t i = 0; i < size; ++i)
			crc = (crc >> 8) ^ table[(crc & 0xff) ^ (uint8_t)data[i]];
		return crc ^ 0xffffffff;
	}
};
