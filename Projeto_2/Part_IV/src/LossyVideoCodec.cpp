#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <opencv2/opencv.hpp>
#include "headers/BitStream.hpp"
#include "headers/Golomb.hpp"
#include "headers/Predictor.hpp"
#include "headers/Converter.hpp"
#include "headers/IntraDecoder.hpp"
#include "headers/IntraEncoder.hpp"

using namespace std;
using namespace cv;

// Function to perform frame prediction (using the previous frame)
int selectPredictor(const vector<function<int(int, int, int)>>& predictors, int frameNumber) {
    // Example: Select predictor based on frame number
    return frameNumber % predictors.size();
}

// Function to calculate residuals
Mat calculateResiduals(const Mat& currentFrame, const Mat& predictedFrame) {
    Mat current32S, predicted32S, residual;
    
    // Convert to CV_32S without modifying the original frames
    currentFrame.convertTo(current32S, CV_32S);
    predictedFrame.convertTo(predicted32S, CV_32S);
    
    // Subtract the predicted frame from the current frame
    subtract(current32S, predicted32S, residual);
    
    return residual;
}

// Function to quantize residuals
Mat quantizeResiduals(const Mat& residuals, int quantLevel) {
    Mat quantized;
    residuals.convertTo(quantized, CV_32S, 1.0 / quantLevel, 0);
    // Round to nearest integer
    return quantized;
}

// Function to inverse quantize residuals
Mat inverseQuantizeResiduals(const Mat& quantizedResiduals, int quantLevel) {
    Mat residuals;
    quantizedResiduals.convertTo(residuals, CV_32S, quantLevel, 0);
    return residuals;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cout << "Usage: ./LossyVideoCodec <input_video> <output_encoded> <quant_level> <predictor_id>" << endl;
        return -1;
    }

    string inputVideoPath = argv[1];
    string outputEncodedPath = argv[2];
    int quantLevel = stoi(argv[3]);
    int predictorId = stoi(argv[4]);

    if (quantLevel <= 0) {
        cerr << "Quantization level must be a positive integer." << endl;
        return -1;
    }

    // Open the input video
    VideoCapture cap(inputVideoPath);
    if (!cap.isOpened()) {
        cerr << "Error: Cannot open the video file." << endl;
        return -1;
    }

    // Get video properties
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    int totalFrames = static_cast<int>(cap.get(CAP_PROP_FRAME_COUNT));
    double fps = cap.get(CAP_PROP_FPS);

    cout << "Video Properties:" << endl;
    cout << "Resolution: " << frameWidth << "x" << frameHeight << endl;
    cout << "Total Frames: " << totalFrames << endl;
    cout << "FPS: " << fps << endl;

    // Initialize Golomb Encoder
    EncodingMode mode = EncodingMode::INTERLEAVING; // Choose encoding mode as needed
    EncoderGolomb encoder(outputEncodedPath, mode);

    // Initialize Predictors
    vector<function<int(int, int, int)>> predictors = GetPredictors();
    if (predictorId < 0 || predictorId >= predictors.size()) {
        cerr << "Invalid predictor ID. Available predictors: 0 to " << predictors.size()-1 << endl;
        return -1;
    }
    auto predictor = predictors[predictorId];
    cout << "Selected Predictor ID: " << predictorId << endl;

    // Initialize Converter
    Converter converter;

    // Read the first frame
    Mat frame;
    cap >> frame;
    if (frame.empty()) {
        cerr << "Error: Empty frame encountered." << endl;
        return -1;
    }
    Mat grayFrame;
    cvtColor(frame, grayFrame, COLOR_BGR2GRAY); // Convert to grayscale

    encoder.set_M(encoder.optimal_m(grayFrame));
    cout << "Optimal M set to: " << encoder.get_M() << endl;

    Mat previousFrame = grayFrame.clone();

    // Encode the first frame (no prediction)
    Mat residuals = calculateResiduals(grayFrame, Mat::zeros(grayFrame.size(), grayFrame.type()));
    Mat quantizedResiduals = quantizeResiduals(residuals, quantLevel);

    // Flatten and encode residuals
    for (int y = 0; y < quantizedResiduals.rows; ++y) {
        for (int x = 0; x < quantizedResiduals.cols; ++x) {
            int residual = quantizedResiduals.at<int>(y, x);
            encoder.encode(residual);
        }
    }

    // Process remaining frames
    int frameCount = 1;
    while (cap.read(frame)) {
        cvtColor(frame, grayFrame, COLOR_BGR2GRAY); // Convert to grayscale

        // Predict the current frame
        int prevY = 0, prevX = 0, prevZ = 0;
        Mat predictedFrame(grayFrame.size(), grayFrame.type(), Scalar(0));
        for (int y = 1; y < grayFrame.rows - 1; ++y) {
            for (int x = 1; x < grayFrame.cols - 1; ++x) {
                prevY = grayFrame.at<uchar>(y-1, x);
                prevX = grayFrame.at<uchar>(y, x-1);
                prevZ = grayFrame.at<uchar>(y-1, x-1);
                predictedFrame.at<uchar>(y, x) = static_cast<uchar>(predictor(prevY, prevX, prevZ));
            }
        }

        // Calculate residuals and quantize
        residuals = calculateResiduals(grayFrame, predictedFrame);
        quantizedResiduals = quantizeResiduals(residuals, quantLevel);

        // Flatten and encode residuals
        for (int y = 0; y < quantizedResiduals.rows; ++y) {
            for (int x = 0; x < quantizedResiduals.cols; ++x) {
                int residual = quantizedResiduals.at<int>(y, x);
                encoder.encode(residual);
            }
        }

        previousFrame = grayFrame.clone();
        frameCount++;
        cout << "Encoded frame: " << frameCount << "/" << totalFrames << "\r" << flush;
    }
    cout << endl;

    // Finalize encoding
    encoder.finishEncoding();
    cout << "Encoding completed. Encoded data saved to " << outputEncodedPath << endl;

    cap.release();
    return 0;
}