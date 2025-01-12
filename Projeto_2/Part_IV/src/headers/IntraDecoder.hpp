#ifndef INTRADECODER_H
#define INTRADECODER_H

#include <fstream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include "BitStream.hpp"
#include "Golomb.hpp"
#include "Predictor.hpp"

using namespace cv;
using namespace std;

class IntraDecoder {
private:
    int shift;             
    DecoderGolomb &golomb; 

public:
    IntraDecoder(DecoderGolomb &decoder, int shift = 0);
    ~IntraDecoder();
    virtual int decode(Mat &frame, function<int(int, int, int)> reconstruct_image);
    int decodeVideo(const string &output, int n_frames, int width, int height, function<int(int, int, int)> reconstruct_image);

    DecoderGolomb& getGolomb() { return golomb; }
};
#endif