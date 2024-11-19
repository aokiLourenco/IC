#include "./Headers/Golomb.hpp"
#include "./Headers/BitStream.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <unistd.h> // For getcwd

using namespace std;

int main() {
    string inputFilePath = "../Data/input.txt";
    cout << "Opening input file: " << inputFilePath << endl;
    ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        throw runtime_error("Failed to open input file");
    }

    vector<int> inputNumbers;
    int num;
    while (inputFile >> num) {
        inputNumbers.push_back(num);
    }
    inputFile.close();

    // Step 2: Encode integers using the Golomb encoder
    EncoderGolomb encoder("../Data/output.bin", EncodingMode::SIGN_MAGNITUDE);
    encoder.set_M(3); // Set the value of M
    for (int number : inputNumbers) {
        encoder.encode(number);
    }
    encoder.finishEncoding();

    // Step 3: Decode the encoded integers
    DecoderGolomb decoder("../Data/output.bin", EncodingMode::SIGN_MAGNITUDE);
    decoder.set_M(3); // Set the value of M
    vector<int> decodedNumbers;
    for (size_t i = 0; i < inputNumbers.size(); ++i) {
        int decodedNumber = decoder.decode();
        decodedNumbers.push_back(decodedNumber);
    }

    // Step 4: Output the results
    bool allMatch = true;
    for (size_t i = 0; i < inputNumbers.size(); ++i) {
        if (inputNumbers[i] != decodedNumbers[i]) {
            allMatch = false;
            cout << "Mismatch at index " << i << ": original = " << inputNumbers[i]
                 << ", decoded = " << decodedNumbers[i] << endl;
        }
    }

    if (allMatch) {
        cout << "All numbers were encoded and decoded correctly." << endl;
    } else {
        cout << "There were mismatches in the encoding/decoding process." << endl;
    }

    return 0;
}