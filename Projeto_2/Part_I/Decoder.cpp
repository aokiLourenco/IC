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

void runTestCase1() {
    BitStream bitStream("test.bin", true);
    std::string writeSequence = "10101010";
    for(char bit : writeSequence) {
        bitStream.writeBit(bit == '1');
    }

    BitStream readStream("test.bin", false);
    std::string readSequence;
    while (!readStream.isEndOfStream()) {
        bool bit = readStream.readBit();
        readSequence += bit ? '1' : '0';
    }

    if(writeSequence == readSequence) {
        std::cout << "Test Case 1 Passed" << std::endl;
    } else {
        std::cout << "Test Case 1 Failed" << std::endl;
    }
}

void runTestCase2() {
    BitStream bitStream("test_multiple_bits.bin", true);
    
    std::vector<std::pair<uint32_t, int>> testValues = {
        {0xF, 4},    // 4 bits
        {0xAA, 8},   // 8 bits
        {0x1234, 16} // 16 bits
    };
    
    for(const auto& [value, bitLength] : testValues) {
        bitStream.writeBits(value, bitLength);
    }
    bitStream.flushBuffer();
    bitStream.close();
    

    // Read bits back from the file
    BitStream readStream("test_multiple_bits.bin", false);
    std::vector<std::pair<uint32_t, int>> readValues;
    try {
        for(const auto& [value, bitLength] : testValues) {
            uint32_t readValue = readStream.readBits(bitLength);
            readValues.emplace_back(readValue, bitLength);
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error during reading bits: " << e.what() << std::endl;
    }
    readStream.close();

    bool passed = (testValues.size() == readValues.size());
    if(passed) {
        for(size_t i = 0; i < testValues.size(); ++i) {
            if(testValues[i].first != readValues[i].first || testValues[i].second != readValues[i].second) {
                passed = false;
                break;
            }
        }
    }

    std::cout << "Test Case 2 " << (passed ? "Passed" : "Failed") << std::endl;
}

void runTestCase3() {
    BitStream bitStream("test_error_handling.bin", true);
    
    std::string writeSequence = "1101"; // Writing 4 bits
    for(char bit : writeSequence) {
        bitStream.writeBit(bit == '1');
    }
    bitStream.flushBuffer();
    bitStream.close();

    // Attempt to read 9 bits (5 more than written)
    BitStream readStream("test_error_handling.bin", false);
    try {
        for(int i = 0; i < 9; ++i) {
            bool bit = readStream.readBit();
            std::cout << "Reading bit: " << bit << std::endl;
        }
        std::cout << "Test Case 3 Failed: No exception thrown when reading beyond EOF" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Test Case 3 Passed: " << e.what() << std::endl;
    }
    readStream.close();
}

int main() {
    // runTestCase1();
    // runTestCase2();
    // runTestCase3();
    
    std::cout << "Enter the name of the file to save to (txt)(absolute path): ";
    std::string output;
    std::cin >> output;

    std::cout << "Enter the name of the file to read from (bin)(absolute path): ";
    std::string input;
    std::cin >> input;

    Decoder decoder(input, output);
    decoder.decode();
    return 0;
}
