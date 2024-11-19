#ifndef BITSTREAM_HPP
#define BITSTREAM_HPP

#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>

class BitStream {
public:
    BitStream();
    BitStream(const std::string& filename, bool writeMode);
    ~BitStream();
    void writeBit(bool bit);
    bool readBit();
    void close();
    bool getEOF();
    void writeBits(uint64_t value, int n);
    uint64_t readBits(int n);
    void writeString(const std::string& str);
    std::string readString(size_t length);
    void flushBuffer();
    void fillBuffer();
    bool isEndOfStream();
    std::fstream& getFile();
    int getBufferPos();
    int setToWrite(const std::string& filename);
    int setToRead(const std::string& filename);
    int getValidBitsInLastByte(); // Add this method
    std::fstream& getFileStream(); // Add this method

private:
    std::fstream file;
    bool writeMode;
    uint8_t buffer;
    int bufferPos;
    int buff;
    int mode;
    bool eof;
    int bitCount;
};

#endif // BITSTREAM_HPP