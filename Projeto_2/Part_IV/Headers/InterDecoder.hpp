#ifndef INTERDECODER_H
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

using namespace cv;
using namespace std;

class InterDecoder {
private:
    int block_size;        ///< Size of the blocks for decoding
    int block_range;       ///< Range of the blocks for decoding
    int shift;             ///< Shift for decoding
    EncoderGolomb &golomb; ///< Golomb decoder for decoding

public:
    InterDecoder(DecoderGolomb& encoder, int block_size, int block_range, int shift = 0);
    int decode(Mat &old_frame, Mat &new_frame);
};

#endif // INTERDECODER_H