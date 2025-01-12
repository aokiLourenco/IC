#include <iostream>
#include <iterator>
#include "opencv2/opencv.hpp"
#include "../../src/headers/BitStream.hpp"
#include "../../src/headers/InterEncoder.hpp"
#include "../../src/headers/Predictor.hpp"
#include "../../src/headers/Converter.hpp"

int main(int argc, char const *argv[]) {
    Converter conv;
    vector<function<int(int, int, int)>> predictors = GetPredictors();
    
    // Get parameters
    cout << "Enter the input video path: ";
    string input;
    cin >> input;
    
    cout << "Enter the output file path: ";
    string output;
    cin >> output;
    
    cout << "Enter block size (e.g., 16): ";
    int blockSize;
    cin >> blockSize;
    
    cout << "Enter search range (e.g., 16): ";
    int searchRange;
    cin >> searchRange;
    
    cout << "Enter I-frame interval (e.g., 30): ";
    int iFrameInterval;
    cin >> iFrameInterval;

    try {
        EncoderGolomb encoder(output, EncodingMode::SIGN_MAGNITUDE);
        encoder.set_M(3);
        
        int shift = 0;
        InterEncoder inter_encoder(encoder, blockSize, searchRange, iFrameInterval, shift);

        // Open input video and verify format
        VideoCapture cap(input);
        if (!cap.isOpened()) {
            throw runtime_error("Cannot open input video");
        }

        if (inter_encoder.encodeVideo(input, output, predictors[0]) < 0) {
            throw runtime_error("Error encoding video");
        }

    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }

    cout << "Video encoded successfully!" << endl;
    return 0;
}