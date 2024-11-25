#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include "./BitStream.hpp"
#include "./IntraDecoder.hpp"
#include "./Golomb.hpp"

IntraDecoder::IntraDecoder(DecoderGolomb& decoder, int shift) : golomb(decoder), shift(shift)
{
}

IntraDecoder::~IntraDecoder()
{
}

int IntraDecoder::decode(Mat &old_frame, function<int(int, int, int)> reconstruct_image)
{
    int x, y, w, error, Decoded_M, frame_cost = 0, numnum = old_frame.channels();
    
    if (numnum == 3) 
    {
        hconcat(Mat::zeros(old_frame.rows, 1, CV_8UC3), old_frame, old_frame);
        vconcat(Mat::zeros(1, old_frame.cols, CV_8UC3), old_frame, old_frame);
    }
        else if (numnum == 1) 
    {
        hconcat(Mat::zeros(old_frame.rows, 1, CV_8UC1), old_frame, old_frame);
        vconcat(Mat::zeros(1, old_frame.cols, CV_8UC1), old_frame, old_frame);
    }

    Decoded_M = golomb.decode();

    if (Decoded_M != 0)
        golomb.set_M(Decoded_M);

    for(int i = 0; i < old_frame.rows; i++) 
    {
        for(int j = 0; j < old_frame.cols; j++)
        {
            for(int n = 0; n < numnum; n++)
            {
                x = old_frame.ptr<uchar>(i, j -1)[n];

                y = old_frame.ptr<uchar>(i-1,j)[n];

                w = old_frame.ptr<uchar>(i-1,j-1)[n];

                error = golomb.decode();
                frame_cost += abs(error);

                if(error < 0)
                {
                    error = -1 * (abs(error) >> shift);
                }
                else 
                {
                    error = error >> shift;
                }

                old_frame.ptr<uchar>(i, j)[n] = (unsigned char)reconstruct_image(x, y, w) + error;
            }
        }
    }
    old_frame = old_frame(Rect(1, 1, old_frame.cols - 1, old_frame.rows - 1));
    return frame_cost / (old_frame.cols * old_frame.rows * old_frame.channels());
}