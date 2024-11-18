#include <fstream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>

#ifndef GOLOMB_H
#define GOLOMB_H

using namespace cv;

class EncoderGolomb {
private:
    BitStream bitStream;
    int M;
    int b;  
public:
    EncoderGolomb(string file_path);
    void set_M(int m);
    int get_M();
    int optimal_m(Mat &frame);
    void finishEncoding();
    void encode(int num);
};

class DecoderGolomb{
private:
    BitStream bitStream;
    int M;
    int b;
public:
    DecoderGolomb(string file_path);
    void set_M(int m);
    int get_M();
    int decode();
};

#endif