#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <opencv2/opencv.hpp>
#include "./Headers/InterEncoder.hpp"
#include "./Headers/InterDecoder.hpp"
#include "./Headers/Predictor.hpp"

using namespace cv;
using namespace std;

// Utility function to calculate PSNR
double calculatePSNR(const Mat& original, const Mat& decoded) {
    Mat diff;
    absdiff(original, decoded, diff);
    diff.convertTo(diff, CV_32F);
    diff = diff.mul(diff);
    Scalar mse = mean(diff);
    if(mse[0] < 1e-10) return 100;
    return 10.0 * log10((255 * 255) / mse[0]);
}

// Test function
void runTest(const string& videoPath, int blockSize = 16, int searchRange = 16, int iFrameInterval = 5) {
    VideoCapture cap(videoPath);
    if(!cap.isOpened()) {
        cerr << "Error: Could not open video file" << endl;
        return;
    }

    // Setup codec parameters
    EncoderGolomb encoder("test.bin", EncodingMode::SIGN_MAGNITUDE);
    DecoderGolomb decoder("test.bin", EncodingMode::SIGN_MAGNITUDE);
    InterEncoder interEncoder(encoder, 0, blockSize, searchRange, iFrameInterval);
    InterDecoder interDecoder(decoder, 0, blockSize, searchRange, iFrameInterval);
    
    vector<function<int(int,int,int)>> predictors = GetPredictors();
    Mat frame, decoded;
    vector<double> psnrValues;
    int frameCount = 0;
    
    while(true) {
        cap >> frame;
        if(frame.empty()) break;
        
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        decoded = Mat::zeros(frame.size(), frame.type());
        
        // Encode and decode frame
        cout << "Processing frame " << frameCount << endl;
        int encodedSize = interEncoder.encode(frame, predictors[0]);
        interDecoder.decode(decoded, predictors[0]);
        
        // Calculate and store PSNR
        double psnr = calculatePSNR(frame, decoded);
        psnrValues.push_back(psnr);
        
        cout << "Frame " << frameCount << " PSNR: " << psnr << "dB" << endl;
        cout << "Compression ratio: " << (frame.total() * 8.0) / encodedSize << endl;
        
        frameCount++;
    }
    
    // Print summary
    double avgPSNR = accumulate(psnrValues.begin(), psnrValues.end(), 0.0) / psnrValues.size();
    cout << "\nTest Summary:" << endl;
    cout << "Average PSNR: " << avgPSNR << "dB" << endl;
    cout << "Total frames processed: " << frameCount << endl;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input.y4m>" << endl;
        return -1;
    }

    try {
        // Debug Golomb encoder with odd M value
        EncoderGolomb encoder("encoded.bin", EncodingMode::SIGN_MAGNITUDE);
        encoder.set_M(3); // Use M=3 (odd value)
        cout << "Using M value: " << encoder.get_M() << endl;
        
        DecoderGolomb decoder("encoded.bin", EncodingMode::SIGN_MAGNITUDE);

        // Load and prepare single frame
        VideoCapture cap(argv[1]);
        if (!cap.isOpened()) {
            throw runtime_error("Could not open video file");
        }

        Mat frame;
        cap >> frame;
        if(frame.empty()) {
            throw runtime_error("Could not read first frame");
        }

        cvtColor(frame, frame, COLOR_BGR2GRAY);
        Mat decoded = Mat::zeros(frame.size(), frame.type());
        
        // Setup codec with debug output
        int block_size = 16;
        int search_range = 16;
        int i_frame_interval = 30;
        
        cout << "Codec parameters:" << endl
             << "Block size: " << block_size << endl
             << "Search range: " << search_range << endl
             << "I-frame interval: " << i_frame_interval << endl;

        InterEncoder interEncoder(encoder, 0, block_size, search_range, i_frame_interval);
        InterDecoder interDecoder(decoder, 0, block_size, search_range, i_frame_interval);

        vector<function<int(int,int,int)>> predictors = GetPredictors();
        
        // Test encoding
        cout << "Testing single frame encoding..." << endl;
        int encodedSize = interEncoder.encode(frame, predictors[0]);
        if (encodedSize < 0) throw runtime_error("Encoding failed");
        
        cout << "Encoded size: " << encodedSize << " bytes" << endl;
        
        // Test decoding
        cout << "Testing single frame decoding..." << endl;
        if (interDecoder.decode(decoded, predictors[0]) < 0) {
            throw runtime_error("Decoding failed");
        }

        // Save frames for inspection
        imwrite("debug_original.png", frame);
        imwrite("debug_decoded.png", decoded);
        
        // Calculate quality metrics
        double psnr = calculatePSNR(frame, decoded);
        double compressionRatio = (frame.total() * 8.0) / encodedSize;
        
        cout << "Results:" << endl
             << "PSNR: " << psnr << "dB" << endl
             << "Compression ratio: " << compressionRatio << ":1" << endl;

        return 0;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }
}