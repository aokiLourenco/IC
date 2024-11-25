#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include "./BitStream.hpp"
#include "./IntraEncoder.hpp"
#include "./Headers/Golomb.hpp"


IntraEncoder::IntraEncoder(EncoderGolomb& encoder, int shift) : golomb(encoder), shift(shift) 
{
}

IntraEncoder::~IntraEncoder()
{
}

int IntraEncoder::encode(Mat &old_frame, function<int(int, int, int)> reconstruct_image) 
{
    int x, y, w, error,predicted, M, framecost, size, numnum;
    Mat image, error_of_Mat;

    // Get the frame size
    numnum = old_frame.channels();
    size = old_frame.rows * old_frame.cols * numnum;
    
    if (numnum == 3) 
    {
        hconcat(Mat::zeros(old_frame.rows, 1, CV_8UC3), old_frame, image);
        vconcat(Mat::zeros(1, old_frame.cols + 1, CV_8UC3), image, image);
        error_of_Mat = Mat::zeros(old_frame.rows, old_frame.cols, CV_16SC3);
    }
    else if (numnum == 1) 
    {
        hconcat(Mat::zeros(old_frame.rows, 1, CV_8UC1), old_frame, image);
        vconcat(Mat::zeros(1, old_frame.cols + 1, CV_8UC1), image, image);
        error_of_Mat = Mat::zeros(old_frame.rows, old_frame.cols, CV_16SC1);
    }
    else 
    {
        cout << "Error: Invalid number of channels" << endl;
        exit(1);
    }

    for(int i = 0; i < image.rows; i++) 
    {
        for(int j = 0; j < image.cols; j++)
        {
            for(int n = 0; n < numnum; n++)
            {
                x = image.ptr<uchar>(i, j -1)[n];

                y = image.ptr<uchar>(i-1,j)[n];

                w = image.ptr<uchar>(i-1,j-1)[n];

                predicted = reconstruct_image(x,y,w);
                error = image.ptr<uchar>(i,n)[n] - predicted;

                if(error < 0)
                {
                    error = -1 * (abs(error) >> shift);
                }
                else 
                {
                    error = error >> shift;
                }
                error_of_Mat.ptr<short>(i-1,j-1)[n] = error;

                if(error < 0)
                {
                    error = -1 * (abs(error) << shift);
                }
                else
                {
                    error <<= shift;
                }
                image.ptr<uchar>(i,j)[n] = (unsigned char) predicted + error; 
            }
        }
    }

    M = golomb.optimal_m(error_of_Mat);
    if(golomb.get_M() == M) 
    {
        golomb.encode(0);
    }
    else 
    {
        golomb.encode(M);
        golomb.set_M(M);
    }

    for (int i = 0; i < error_of_Mat.rows; i++)
        for (int j = 0; j < error_of_Mat.cols; j++)
            for (int n = 0; n < numnum; n++)
            {
                framecost += abs(error_of_Mat.ptr<short>(i, j)[n]);
                golomb.encode(error_of_Mat.ptr<short>(i, j)[n]);
            }

    return framecost / size;
}