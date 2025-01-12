#ifndef INTERENCODER_H
#define INTERENCODER_H

#include "IntraEncoder.hpp"

class InterEncoder : public IntraEncoder {
private:
    int blockSize;
    int searchRange;
    int iFrameInterval;
    Mat previousFrame;
    
    Mat motionEstimation(const Mat& currentFrame, const Mat& referenceFrame);
    Mat motionCompensation(const Mat& referenceFrame, const Mat& motionVectors);
    bool shouldUseIntraMode(const Mat& block, const Mat& predictedBlock);

    EncoderGolomb& golomb;

public:
    InterEncoder(EncoderGolomb& encoder, int blockSize, int searchRange, 
                int iFrameInterval, int shift = 0);
    virtual ~InterEncoder() override;
    
    int encode(Mat& frame, function<int(int,int,int)> predictor) override;
    int encodeVideo(const string& input, const string& output, 
                   function<int(int,int,int)> predictor);
};

#endif