#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "./Headers/BitStream.hpp"
#include "BitStream.cpp"

#include "./Headers/Golomb.hpp"

// This class should include functions for encoding and decoding
// integers, utilizing the BitStream class for efficient bit-level manipulation. The Golomb coding scheme
// requires a parameter m, which controls the encoding length, making it adaptable to specific probability
// distributions. Your implementation should handle both positive and negative integers. For negative values,
// provide two encoding approaches (configurable by the user):
// 1. Sign and magnitude: encode the sign separately from the magnitude.
// 2. Positive/negative interleaving: Use a zigzag or odd-even mapping to interleave positive and negative values, so all numbers map to non-negative integers.

// Specifications:
// • Bit efficiency: use the BitStream class for efficient bit manipulation.
// • Parameterization: ensure the class can easily switch between modes and values of m for flexibility
// in encoding different distributions.
// • Handling negative values: Implement both sign and magnitude and interleaving for representing
// negative numbers
using namespace cv;

EncoderGolomb::EncoderGolomb(string file_path) {
    bitStream.setToWrite(file_path);
    this->set_M(3);
}

void EncoderGolomb::set_M(int m) {
    if (m == 0) {
        return;
    }
    this->M = m;
    this->b = ceil(log2(m));
}

int EncoderGolomb::get_M() {
    return M;
}

int EncoderGolomb::optimal_m(cv::Mat &frame) {
    double u = 0;
    cv::Scalar mean_values = cv::mean(cv::abs(frame));

    for (int n = 0; n < frame.channels(); n++)
        u += mean_values[n];
    u /= frame.channels();
    if (u < 0.01)
        return 2;
    int s = ceil(log2(u) - 0.05 + 0.6 / u);

    s = (0 > s) ? 0 : s;
    return pow(2, s);
}

void EncoderGolomb::encode(int Number) {
    int r, q;

    if (Number < 0)
        bitStream.writeBit(1);
    else
        bitStream.writeBit(0);

    Number = abs(Number);

    q = Number / M; 

    r = Number % M;

    for (int i = 0; i < q; i++)
        bitStream.writeBit(1);

    bitStream.writeBit(0);

    if (M % 2 == 0) {
        bitStream.writeBits(r, b);
    } else if (r < pow(2, b + 1) - M) {
        bitStream.writeBits(r, b);
    } else {
        bitStream.writeBits(r + pow(2, b + 1) - M, b + 1);
    }
}

DecoderGolomb::DecoderGolomb(string file_path)
{
    bitStream.setToRead(file_path);
    this->set_M(3);
}

void DecoderGolomb::set_M(int m) {
    if (m == 0) {
        return;
    }
    this->M = m;
    this->b = ceil(log2(m));
}

int DecoderGolomb::get_M() {
    return M;
}

int DecoderGolomb::decode() {
    int r, r2, q;

    q = 0;

    if(bitStream.isEndOfStream())
        return NULL;

    while (true)
    {
        if ((bitStream.readBit() & 1) == 0)
            break;
        q++;
    }

    
}