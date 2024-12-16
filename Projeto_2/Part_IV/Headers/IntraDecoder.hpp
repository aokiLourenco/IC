#ifndef INTRADECODER_H
#define INTERDECODER_H

#include <fstream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include "./BitStream.hpp"
#include "./Golomb.hpp"
#include "./Predictor.hpp"

using namespace cv;
using namespace std;

class IntraDecoder {
private:
    int shift;             
    DecoderGolomb* golomb; 

public:
    IntraDecoder(DecoderGolomb* decoder, int shift = 0);
    ~IntraDecoder();
    int decode(Mat &old_frame, function<int(int, int, int)> reconstruct_image);
};
#endif