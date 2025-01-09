#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>

class BitStream {
public:
    BitStream(const std::string& filename, bool writeMode);
    ~BitStream();
    void writeBit(bool bit);
    bool readBit();
    void writeBits(uint64_t value, int n);
    uint64_t readBits(int n);
    void writeString(const std::string& str);
    std::string readString(size_t length);
    void flushBuffer();
    void fillBuffer();
    bool isEndOfStream();
    std::fstream& getFile();
    int getBufferPos();
    void close();
private:
    std::fstream file;
    bool writeMode;
    uint8_t buffer;
    int bufferPos;
};