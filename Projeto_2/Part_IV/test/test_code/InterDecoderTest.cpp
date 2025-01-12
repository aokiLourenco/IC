#include <iostream>
#include "opencv2/opencv.hpp"
#include "../../src/headers/BitStream.hpp"
#include "../../src/headers/InterDecoder.hpp"
#include "../../src/headers/Predictor.hpp"
#include "../../src/headers/Converter.hpp"

int main(int argc, char const *argv[]) {
    Converter conv;
    vector<function<int(int, int, int)>> predictors = GetPredictors();
    
    cout << "Enter the encoded file path: ";
    string input;
    cin >> input;

    // Initialize decoder and read header
    DecoderGolomb decoder(input, EncodingMode::SIGN_MAGNITUDE);
    
    // Read header parameters
    int format = decoder.decode();
    int predictor = decoder.decode();
    int shift = decoder.decode();
    int n_frames = decoder.decode();
    int blockSize = decoder.decode();
    int searchRange = decoder.decode();
    int iFrameInterval = decoder.decode();
    int width = decoder.decode();
    int height = decoder.decode();

    cout << "Header values:\n"
         << "Format: " << format << "\n"
         << "Predictor: " << predictor << "\n"
         << "Shift: " << shift << "\n"
         << "Frames: " << n_frames << "\n"
         << "Block Size: " << blockSize << "\n"
         << "I-Frame Interval: " << iFrameInterval << "\n"
         << "Width: " << width << "\n"
         << "Height: " << height << "\n";

    if (width <= 0 || height <= 0 || n_frames <= 0) {
        cerr << "Invalid header values\n";
        return -1;
    }

    InterDecoder inter_decoder(decoder, blockSize, iFrameInterval, shift);
    string output_video = input.substr(0, input.find_last_of('.')) + "_decoded.y4m";
    
    if (inter_decoder.decodeVideo(output_video, n_frames, width, height, predictors[predictor]) < 0) {
        cerr << "Error decoding video" << endl;
        return -1;
    }
    
    cout << "Video decoded successfully to: " << output_video << endl;
    return 0;
}