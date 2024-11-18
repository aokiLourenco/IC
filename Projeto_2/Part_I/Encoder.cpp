#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>

#include "./Headers/Encoder.h"
#include "./Headers/BitStream.h"

class Encoder {
public:
    Encoder(const std::string& inputFilename, const std::string& outputFilename);
    void encode();
private:
    BitStream bitStream;
    std::fstream inputFile;
};

Encoder::Encoder(const std::string& inputFilename, const std::string& outputFilename)
    : bitStream(outputFilename, true), inputFile(inputFilename, std::ios::in) {
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
}

void Encoder::encode() {
    char c;
    while (inputFile.get(c)) {
        if (c == '0') {
            bitStream.writeBit(false);
        } else if (c == '1') {
            bitStream.writeBit(true);
        } else {
            throw std::runtime_error("Invalid character in input file");
        }
    }

    // Write the number of valid bits in the last byte to a metadata file
    std::ofstream metadataFile("output.meta", std::ios::out);
    if (!metadataFile.is_open()) {
        throw std::runtime_error("Failed to open metadata file");
    }
    metadataFile << bitStream.getBufferPos();
    bitStream.flushBuffer();
    metadataFile.close();
}

int main() {
    Encoder Encoder("../Data/Fake_Binary.txt", "output.bin");
    Encoder.encode();
    return 0;
}