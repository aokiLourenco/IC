#ifndef INTRAENCODER_H
#define INTRAENCODER_H

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

class IntraEncoder {
private:
    int shift;  
    EncoderGolomb &golomb; 

public:
    IntraEncoder(EncoderGolomb &encoder, int shift = 0);
    virtual ~IntraEncoder();
    virtual int encode(Mat &old_frame, function<int(int, int, int)> reconstruct_image);

    EncoderGolomb& getGolomb() { return golomb; }
};

#endif // INTRAENCODER_H