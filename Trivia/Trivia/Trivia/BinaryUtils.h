#pragma once
#include <vector>

typedef unsigned char Byte;
typedef std::vector<Byte> Buffer;

// Simple function to convert int to 4 bytes in big-endian format
inline Buffer ConvertToBinaryFourBytes(int val) {
    Buffer binaryNumber(4);
    binaryNumber[0] = static_cast<Byte>((val >> 24) & 0xFF);
    binaryNumber[1] = static_cast<Byte>((val >> 16) & 0xFF);
    binaryNumber[2] = static_cast<Byte>((val >> 8) & 0xFF);
    binaryNumber[3] = static_cast<Byte>(val & 0xFF);
    return binaryNumber;
}