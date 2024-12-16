#ifndef GOLOMB_H
#define GOLOMB_H

#include <fstream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include "./BitStream.hpp"

using namespace cv;

enum class EncodingMode {
    SIGN_MAGNITUDE,
    INTERLEAVING
};

class EncoderGolomb {
private:
    BitStream bitStream;
    int M;
    int b;
    EncodingMode mode;
public:
    EncoderGolomb(std::string file_path, EncodingMode mode);
    void set_M(int m);
    int get_M();
    int optimal_m(Mat &frame);
    void finishEncoding();
    void encode(int num);
};

class DecoderGolomb {
private:
    BitStream bitStream;
    int M;
    int b;
    EncodingMode mode;
public:
    DecoderGolomb(std::string file_path, EncodingMode mode);
    void set_M(int m);
    int get_M();
    int decode();
    BitStream * getBitStream();
};

#endif // GOLOMB_H