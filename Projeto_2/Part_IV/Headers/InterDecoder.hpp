#ifndef INTERDECODER_H
#define INTERDECODER_H

#include "IntraDecoder.hpp"
#include "InterFrame.hpp"

class InterDecoder : public IntraDecoder {
private:
    int block_size;
    int search_range;
    int i_frame_interval;
    Mat reference_frame;
    vector<MotionVector> motion_vectors;

public:
    InterDecoder(DecoderGolomb &decoder, int shift, int block_size,
                int search_range, int i_frame_interval);
    ~InterDecoder();

    int decode(Mat &frame, function<int(int,int,int)> reconstruct_image) override;
    int decodeIntra(Mat &frame, function<int(int,int,int)> reconstruct_image);
    int decodeInter(Mat &frame, function<int(int,int,int)> reconstruct_image);
    void decodeIntraBlock(Mat& block, function<int(int,int,int)> reconstruct_image);
    void decodeResiduals(Mat& residual);
    void updateReference(const Mat &frame);
};

#endif // INTERDECODER_H