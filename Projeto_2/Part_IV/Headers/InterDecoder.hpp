#ifndef INTERDECODER_H
#define INTERDECODER_H

#include "IntraDecoder.hpp"

class InterDecoder : public IntraDecoder {
private:
    int blockSize;
    int iFrameInterval;
    Mat previousFrame;
    
    Mat motionCompensation(const Mat& referenceFrame, const Mat& motionVectors);

public:
    InterDecoder(DecoderGolomb& decoder, int blockSize, 
                int iFrameInterval, int shift = 0);
    virtual ~InterDecoder();
    
    int decode(Mat& frame, function<int(int,int,int)> predictor) override;
    int decodeVideo(const string& output, int n_frames, int width, 
                   int height, function<int(int,int,int)> predictor);
};

#endif