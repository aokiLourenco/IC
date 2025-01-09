#include "./Headers/Golomb.hpp"
#include "./Headers/BitStream.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <unistd.h>


using namespace std;

void calculate_and_save_histogram_image(const Mat& image, const string& filename) {
    // Number of bins
    int histSize = 256;    // 256 bins for pixel values [0,255]

    // Set the ranges (for B,G,R)
    float range[] = { 0, 256 }; // the upper boundary is exclusive
    const float* histRange = { range };

    bool uniform = true, accumulate = false;

    Mat hist;

    // Calculate the histogram for the grayscale image
    calcHist(&image, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

    // Normalize the result to [0, histImage.rows]
    Mat histImage(400, 512, CV_8UC3, Scalar(255, 255, 255));
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    // Draw the histogram
    int binWidth = cvRound((double) histImage.cols / histSize);
    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(binWidth * (i - 1), histImage.rows - cvRound(hist.at<float>(i - 1))),
             Point(binWidth * i, histImage.rows - cvRound(hist.at<float>(i))),
             Scalar(0, 0, 0), 2, 8, 0);
    }

    // Save the histogram image
    imwrite(filename, histImage);
    cout << "Histogram image saved to " << filename << endl;
}

int main() {
    // -----------------------------------------------------------------------------------//
    // string inputFilePath = "../data/Tests.txt";
    // cout << "Opening input file: " << inputFilePath << endl;
    // ifstream inputFile(inputFilePath);
    // if (!inputFile.is_open()) {
    //     throw runtime_error("Failed to open input file");
    // }

    // vector<int> inputNumbers;
    // int num;
    // while (inputFile >> num) {
    //     inputNumbers.push_back(num);
    // }
    // inputFile.close();

    // // Step 2: Encode integers using the Golomb encoder
    // EncoderGolomb encoder("../data/output.bin", EncodingMode::SIGN_MAGNITUDE);
    // encoder.set_M(3); // Set the value of M
    // for (int number : inputNumbers) {
    //     encoder.encode(number);
    // }
    // encoder.finishEncoding();

    // // Step 3: Decode the encoded integers

    // printf("Decoding\n");

    // DecoderGolomb decoder("../data/output.bin", EncodingMode::SIGN_MAGNITUDE);
    // decoder.set_M(3); // Set the value of M
    // vector<int> decodedNumbers;
    // for (size_t i = 0; i < inputNumbers.size(); ++i) {
    //     int decodedNumber = decoder.decode();
    //     printf("Decoded Number : %d\n", decodedNumber);
    //     decodedNumbers.push_back(decodedNumber);
    // }

    // // Step 4: Output the results
    // bool allMatch = true;
    // for (size_t i = 0; i < inputNumbers.size(); ++i) {
    //     if (inputNumbers[i] != decodedNumbers[i]) {
    //         allMatch = false;
    //         cout << "Mismatch at index " << i << ": original = " << inputNumbers[i]
    //              << ", decoded = " << decodedNumbers[i] << endl;
    //     }
    // }

    // if (allMatch) {
    //     cout << "All numbers were encoded and decoded correctly." << endl;
    // } else {
    //     cout << "There were mismatches in the encoding/decoding process." << endl;
    // }

    // return 0;
    // -----------------------------------------------------------------------------------//


    // -----------------------------------------------------------------------------------//
    // !First Test
    // // Step 1: Initialize the BitStream for writing with an output file
    // EncoderGolomb encoder("../data/output.bin", EncodingMode::SIGN_MAGNITUDE);
    
    // // Step 2: Instantiate the EncoderGolomb with SIGN_MAGNITUDE encoding mode
    // encoder.set_M(3); // Set parameter M = 3
    
    // // Step 3: Encode a predefined sequence of positive integers
    // vector<int> inputNumbers = {1, 2, 3, 4, 5};
    // for (int number : inputNumbers) {
    //     encoder.encode(number);
    // }
    
    // // Step 4: Finalize encoding by flushing the BitStream buffer
    // encoder.finishEncoding();
    
    // // Step 5: Initialize the DecoderGolomb with the encoded file path and the same encoding mode
    // DecoderGolomb decoder("../data/output.bin", EncodingMode::SIGN_MAGNITUDE);
    // decoder.set_M(3);
    
    // // Step 6: Decode the integers and store them in a vector
    // vector<int> decodedNumbers;
    // for (size_t i = 0; i < inputNumbers.size(); ++i) {
    //     int decodedNumber = decoder.decode();
    //     cout << "Decoded Number : " << decodedNumber << endl;
    //     decodedNumbers.push_back(decodedNumber);
    // }
    
    // // Step 7: Compare the decoded sequence with the original input sequence
    // bool allMatch = true;
    // for (size_t i = 0; i < inputNumbers.size(); ++i) {
    //     if (inputNumbers[i] != decodedNumbers[i]) {
    //         allMatch = false;
    //         cout << "Mismatch at index " << i << ": original = " << inputNumbers[i]
    //              << ", decoded = " << decodedNumbers[i] << endl;
    //     }
    // }
    
    // if (allMatch) {
    //     cout << "All numbers were encoded and decoded correctly." << endl;
    // } else {
    //     cout << "There were mismatches in the encoding/decoding process." << endl;
    // }
    
    // return 0;
    // -----------------------------------------------------------------------------------//

    // -----------------------------------------------------------------------------------//
    // !Second Test
    // // Test Case 2: Encoding and Decoding Negative Numbers
    // // Step 1: Initialize the BitStream for writing with an output file
    // EncoderGolomb encoderNeg("../data/output_neg.bin", EncodingMode::SIGN_MAGNITUDE);
    
    // // Step 2: Instantiate the EncoderGolomb with SIGN_MAGNITUDE encoding mode
    // encoderNeg.set_M(3); // Set parameter M = 3
    
    // // Step 3: Encode a sequence of integers containing both positive and negative values
    // vector<int> inputNumbersNeg = {-1, 2, -3, 4, -5};
    // for (int number : inputNumbersNeg) {
    //     encoderNeg.encode(number);
    // }
    
    // // Step 4: Finalize encoding by flushing the BitStream buffer
    // encoderNeg.finishEncoding();
    
    // // Step 5: Initialize the DecoderGolomb with the encoded file path and the same encoding mode
    // DecoderGolomb decoderNeg("../data/output_neg.bin", EncodingMode::SIGN_MAGNITUDE);
    // decoderNeg.set_M(3);
    
    // // Step 6: Decode the integers and store them in a vector
    // vector<int> decodedNumbersNeg;
    // for (size_t i = 0; i < inputNumbersNeg.size(); ++i) {
    //     int decodedNumber = decoderNeg.decode();
    //     cout << "Decoded Number : " << decodedNumber << endl;
    //     decodedNumbersNeg.push_back(decodedNumber);
    // }
    
    // // Step 7: Compare the decoded sequence with the original input sequence
    // bool allMatchNeg = true;
    // for (size_t i = 0; i < inputNumbersNeg.size(); ++i) {
    //     if (inputNumbersNeg[i] != decodedNumbersNeg[i]) {
    //         allMatchNeg = false;
    //         cout << "Mismatch at index " << i << ": original = " << inputNumbersNeg[i]
    //              << ", decoded = " << decodedNumbersNeg[i] << endl;
    //     }
    // }
    
    // if (allMatchNeg) {
    //     cout << "All numbers were encoded and decoded correctly." << endl;
    // } else {
    //     cout << "There were mismatches in the encoding/decoding process." << endl;
    // }
    
    // return 0;
    // -----------------------------------------------------------------------------------//

    // -----------------------------------------------------------------------------------//
    // !Third Test

    // Step 1: Load a dataset with known statistical properties (e.g., pixel intensity values from an image)
    Mat image = imread("../data/test_image.png", IMREAD_GRAYSCALE);
    if (image.empty()) {
        throw runtime_error("Failed to load image from ../data/test_image.png");
    }

    cout << "Image loaded successfully. Dimensions: " << image.rows << "x" << image.cols << endl;

    // Step 2: Calculate the optimal M using the EncoderGolomb class
    EncoderGolomb tempEncoder("../data/temp_optimize.bin", EncodingMode::INTERLEAVING); // Temporary encoder with valid path
    int optimalM = tempEncoder.optimal_m(image);
    cout << "Optimal M calculated: " << optimalM << endl;

    // Optionally, remove the temporary file after calculation
    if (std::remove("../data/temp_optimize.bin") != 0) {
        perror("Error deleting temporary file");
    } else {
        cout << "Temporary file deleted successfully." << endl;
    }

    // Step 3: Initialize the BitStream for writing with an output file
    EncoderGolomb encoderOpt("../data/output_opt.bin", EncodingMode::INTERLEAVING);
    cout << "Encoder initialized with output file: ../data/output_opt.bin" << endl;

    // Step 4: Instantiate the EncoderGolomb with the output file path and INTERLEAVING encoding mode
    encoderOpt.set_M(optimalM);
    cout << "Encoder parameter M set to: " << optimalM << endl;

    // Variables to track encoding statistics
    long long totalQuotient = 0;
    long long totalRemainder = 0;
    long long totalNumbers = image.rows * image.cols;

    // Step 5: Encode the dataset using the selected M
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            int pixelValue = static_cast<int>(image.at<uchar>(row, col));
            // You can modify the encode function to return quotient and remainder if needed
            encoderOpt.encode(pixelValue);
            // Placeholder for quotient and remainder
            // Assuming encode returns a pair<int, int>
            // auto [quotient, remainder] = encoderOpt.encode(pixelValue);
            // totalQuotient += quotient;
            // totalRemainder += remainder;
        }
    }
    cout << "Encoding completed for " << totalNumbers << " numbers." << endl;

    // Step 6: Finalize encoding by flushing the BitStream buffer
    encoderOpt.finishEncoding();
    cout << "BitStream buffer flushed. Encoding finalized." << endl;

    // Step 7: Initialize the DecoderGolomb with the encoded file path and the same encoding mode
    DecoderGolomb decoderOpt("../data/output_opt.bin", EncodingMode::INTERLEAVING);
    decoderOpt.set_M(optimalM);
    cout << "Decoder initialized with file: ../data/output_opt.bin and M: " << optimalM << endl;

    // Step 8: Decode the dataset and store the results
    vector<int> decodedData;
    decodedData.reserve(totalNumbers);
    for (int i = 0; i < totalNumbers; ++i) {
        int decodedValue = decoderOpt.decode();
        decodedData.push_back(decodedValue);
    }
    cout << "Decoding completed for " << decodedData.size() << " numbers." << endl;

    // Step 9: Compare the decoded data with the original dataset to ensure integrity
    bool integrity = true;
    long long mismatchCount = 0;

    for (int row = 0; row < image.rows && integrity; ++row) {
        for (int col = 0; col < image.cols && integrity; ++col) {
            int original = static_cast<int>(image.at<uchar>(row, col));
            int decoded = decodedData[row * image.cols + col];
            if (original != decoded) {
                integrity = false;
                mismatchCount++;
                cout << "Mismatch at (" << row << ", " << col << "): original = " << original
                     << ", decoded = " << decoded << endl;
            }
        }
    }

    // Step 10: Output the results
    if (integrity) {
        cout << "Test Result: SUCCESS - Optimal M encoding and decoding successful. Data integrity preserved." << endl;
    } else {
        cout << "Test Result: FAILURE - Data integrity check failed. Total mismatches: " << mismatchCount << endl;
    }

    // Step 11: Calculate and Save Histogram Data
    string histImageFilename = "../data/histogram_data.png";
    try {
        calculate_and_save_histogram_image(image, histImageFilename);
    } catch (const exception& e) {
        cerr << "Error calculating histogram: " << e.what() << endl;
        return 1;
    }

    // Summary Statistics (Optional)
    // Uncomment and implement if you track quotient and remainder
    /*
    double averageQuotient = static_cast<double>(totalQuotient) / totalNumbers;
    double averageRemainder = static_cast<double>(totalRemainder) / totalNumbers;

    cout << "Summary Statistics:" << endl;
    cout << "Total Numbers Encoded: " << totalNumbers << endl;
    cout << "Average Quotient: " << averageQuotient << endl;
    cout << "Average Remainder: " << averageRemainder << endl;
    */

    return 0;
    // -----------------------------------------------------------------------------------//
}