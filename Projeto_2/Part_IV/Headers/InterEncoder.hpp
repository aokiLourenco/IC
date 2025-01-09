#ifndef INTERENCODER_H
#define INTERENCODER_H

#include "IntraEncoder.hpp"
#include "InterFrame.hpp"

class InterEncoder : public IntraEncoder {
private:
    int block_size;
    int search_range;
    int i_frame_interval;
    Mat reference_frame;
    vector<MotionVector> motion_vectors;

public:
    InterEncoder(EncoderGolomb &encoder, int shift, int block_size,

                 int search_range, int i_frame_interval);

    ~InterEncoder();
    int encode(Mat &frame, function<int(int,int,int)> predictor);
    int encodeIntra(Mat &frame, function<int(int,int,int)> predictor);
    int encodeInter(Mat &frame, function<int(int,int,int)> predictor);
    void encodeIntraBlock(const Mat& block, Mat& residuals, function<int(int,int,int)> predictor);
    void updateReference(const Mat &frame);
    Mat findBestMatch(const Mat& block, const Mat& reference, Point2i& mv);
};

#endif