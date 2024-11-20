#include "./Headers/BitStream.hpp"

using namespace std;

BitStream::BitStream()
    : writeMode(false), buffer(0), bufferPos(0), buff(0), mode(0), eof(false), bitCount(0) {
}

BitStream::BitStream(const std::string& filename, bool writeMode)
    : writeMode(writeMode), buffer(0), bufferPos(0), buff(0), mode(0), eof(false), bitCount(0) {
    if (writeMode) {
        file.open(filename, std::ios::out | std::ios::binary);
    } else {
        file.open(filename, std::ios::in | std::ios::binary);
    }
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
}

BitStream::~BitStream() {
    if (writeMode && bufferPos > 0) {
        flushBuffer();
    }
    file.close();
}

bool BitStream::readBit() {
    if (bufferPos == 0) {
        fillBuffer();
    }
    if (bufferPos == 0) {
        throw std::runtime_error("Attempt to read beyond end of file");
    }
    bool bit = (buffer & 0x80) != 0;
    buffer <<= 1;
    bufferPos--;
    return bit;
}

uint64_t BitStream::readBits(int n) {
    uint64_t value = 0;

    for (int i = 0; i < n; ++i) {
        if (isEndOfStream()) {
            throw std::runtime_error("Attempt to read beyond end of file while reading bits");
        }
        value = (value << 1) | readBit();
    }
    return value;
}

void BitStream::writeBit(bool bit) {
    buffer = (buffer << 1) | bit;
    bufferPos++;
    if (bufferPos == 8) {
        flushBuffer();
    }
}

void BitStream::writeBits(uint64_t value, int n) {
    for (int i = n - 1; i >= 0; --i) {
        writeBit((value >> i) & 1);
    }
}

void BitStream::writeString(const std::string& str) {
    for (char c : str) {
        writeBits(static_cast<uint8_t>(c), 8);
    }
}

std::string BitStream::readString(size_t length) {
    std::string str;
    for (size_t i = 0; i < length; ++i) {
        str.push_back(static_cast<char>(readBits(8)));
    }
    return str;
}

void BitStream::flushBuffer() {
    if (bufferPos > 0) {
        buffer <<= (8 - bufferPos);
        file.put(buffer);
        buffer = 0;
        bufferPos = 0;
    }
}

void BitStream::fillBuffer() {
    if (file.eof()) {
        bufferPos = 0;
        return;
    }
    buffer = file.get();
    // printf("Buffer : %d\n", buffer);
    if (file.eof()) {
        bufferPos = 0;
        return;
    }
    bufferPos = 8;
}

bool BitStream::isEndOfStream() {
    // printf("Buffer Pos on IEFS : %d\n", bufferPos);
    if (bufferPos > 0) {
        return false;
    }
    if (file.peek() == EOF) {
        return true;
    }
    return false;
}

std::fstream& BitStream::getFile() {
    return file;
}

int BitStream::getBufferPos() {
    return bufferPos;
}

int BitStream::setToWrite(const string& filename) {
    file.close();
    file.open(filename, fstream::binary | fstream::out | fstream::app);
    if (!file.is_open()) {
        cerr << "Couldn't open specified file for writing!" << endl;
        exit(1);
    }

    mode = 1;
    buff = 0;
    bitCount = 0;

    return 0;
}

int BitStream::setToRead(const string& filename) {
    file.close();
    file.open(filename, fstream::binary | fstream::in);
    if (!file.is_open()) {
        cerr << "Couldn't open specified file for reading!" << endl;
        exit(1);
    }

    mode = 0;
    buff = 0;
    bitCount = 8;
    eof = false;

    return 0;
}

int BitStream::getValidBitsInLastByte() {
    return bufferPos;
}

std::fstream& BitStream::getFileStream() {
    return file;
}

void BitStream::close() {
    if (mode && bitCount != 0)
    {
        while (bitCount)
            writeBit(0);
    }

    file.close();
}

bool BitStream::getEOF() {
    return eof;
}