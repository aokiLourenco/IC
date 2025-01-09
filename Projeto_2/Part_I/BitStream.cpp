#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <bitset>

#include "./Headers/BitStream.h"

BitStream::BitStream(const std::string& filename, bool writeMode)
    : writeMode(writeMode), buffer(0), bufferPos(0) {
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

void BitStream::writeBit(bool bit) {
    std::cout << "Writing bit: " << bit << std::endl;
    buffer = (buffer << 1) | bit;
    bufferPos++;
    if (bufferPos == 8) {
        flushBuffer();
    }
}

bool BitStream::readBit() {
    if (bufferPos == 0) {
        fillBuffer();
    }
    if (isEndOfStream()) {
        throw std::runtime_error("Attempt to read beyond end of file");
    }
    bool bit = (buffer & 0x80) != 0;
    buffer <<= 1;
    bufferPos--;
    std::cout << "Reading bit: " << bit << std::endl;
    return bit;
}

void BitStream::writeBits(uint64_t value, int n) {
    for (int i = n - 1; i >= 0; --i) {
        writeBit((value >> i) & 1);
    }
}

uint64_t BitStream::readBits(int n) {
    uint64_t value = 0;
    for (int i = 0; i < n; ++i) {
        value = (value << 1) | readBit();
    }
    return value;
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
        std::cout << "Flushing buffer: " << std::bitset<8>(buffer) << std::endl;
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
    if (file.eof()) {
        bufferPos = 0;
        return;
    }
    bufferPos = 8;
    std::cout << "Filling buffer: " << std::bitset<8>(buffer) << std::endl;
}

bool BitStream::isEndOfStream() {
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

void BitStream::close() {
    file.close();
}