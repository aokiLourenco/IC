#ifndef INTERENCODER_H
#define INTERENCODER_H

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

class InterEncoder {
private:
    int block_size;        ///< Size of the blocks for decoding
    int block_range;       ///< Range of the blocks for decoding
    int shift;             ///< Shift for decoding
    EncoderGolomb &golomb; ///< Golomb decoder for decoding

public:
    InterEncoder(EncoderGolomb& encoder, int block_size, int block_range, int shift = 0);
    int encode(Mat &old_frame, Mat &new_frame);
    int get_Block_Size() const;
    void set_Block_Size(int block_size);
    int get_Block_Range() const;
    float cost(Mat block);
};

#endif // INTERENCODER_H