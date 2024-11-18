#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>

#include "./Headers/Decoder.h"
#include "./Headers/BitStream.h"

class Decoder {
public:
    Decoder(const std::string& inputFilename, const std::string& outputFilename);
    void decode();
private:
    BitStream bitStream;
    std::fstream outputFile;
    int validBitsInLastByte;
};

Decoder::Decoder(const std::string& inputFilename, const std::string& outputFilename)
    : bitStream(inputFilename, false), outputFile(outputFilename, std::ios::out) {
    if (!outputFile.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    // Read the number of valid bits in the last byte from the metadata file
    std::ifstream metadataFile("output.meta", std::ios::in);
    if (!metadataFile.is_open()) {
        throw std::runtime_error("Failed to open metadata file");
    }
    metadataFile >> validBitsInLastByte;
    metadataFile.close();
}

void Decoder::decode() {
    std::string buffer;
    buffer.reserve(1024); // Reserve space to reduce reallocations

    while (!bitStream.isEndOfStream()) {
        for (int i = 0; i < 1024; ++i) {
            if (bitStream.isEndOfStream()) {
                break;
            }
            bool bit = bitStream.readBit();
            buffer.push_back(bit ? '1' : '0');
        }

        if (buffer.size() >= 1024) {
            outputFile.write(buffer.c_str(), buffer.size());
            buffer.clear();
        }
    }

    // Write any remaining bits in the buffer, ignoring the padded bits
    if (!buffer.empty()) {
        if (buffer.size() > 8) {
            outputFile.write(buffer.c_str(), buffer.size() - 8 + validBitsInLastByte);
        } else {
            outputFile.write(buffer.c_str(), validBitsInLastByte);
        }
    }

    std::cout << "Decoding completed." << std::endl;
}

int main() {
    Decoder decoder("output.bin", "output.txt");
    decoder.decode();
    return 0;
}