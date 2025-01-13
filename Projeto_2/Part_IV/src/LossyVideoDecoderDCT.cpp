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
#include "headers/DCTUtils.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Usage: ./lossy_decoder_DCT <input_encoded> <output_video> <quant_level>" << endl;
        return -1;
    }

    string inputEncodedPath = argv[1];
    string outputVideoPath = argv[2];
    int quantLevel = stoi(argv[3]);

    if (quantLevel <= 0) {
        cerr << "Quantization level must be a positive integer." << endl;
        return -1;
    }

    // Initialize Golomb Decoder
    EncodingMode mode = EncodingMode::INTERLEAVING; // Choose encoding mode as needed
    DecoderGolomb decoder(inputEncodedPath, mode);

    // Initialize Video Writer
    VideoWriter writer(outputVideoPath, VideoWriter::fourcc('M', 'P', '4', 'V'), 30, Size(640, 480), false);

    if (!writer.isOpened()) {
        cerr << "Error: Cannot open the video writer." << endl;
        return -1;
    }

    // Decode frames
    while (!decoder.getBitStream()->isEndOfStream()) {
        Mat frame(480, 640, CV_8UC1);

        for (int y = 0; y < frame.rows; y += BLOCK_SIZE) {
            for (int x = 0; x < frame.cols; x += BLOCK_SIZE) {
                vector<int> zigzag;
                for (int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; ++i) {
                    zigzag.push_back(decoder.decode());
                }
                Mat quantizedBlock = inverseZigzagOrder(zigzag, BLOCK_SIZE, BLOCK_SIZE);
                Mat dctBlock = inverseQuantizeDCT(quantizedBlock, quantLevel);
                Mat block = performIDCT(dctBlock);
                block.convertTo(block, CV_8U);
                block.copyTo(frame(Rect(x, y, BLOCK_SIZE, BLOCK_SIZE)));
            }
        }

        writer.write(frame);
    }

    cout << "Decoding completed. Decoded video saved to " << outputVideoPath << endl;

    writer.release();
    return 0;
}