#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include "./BitStream.hpp"
#include "./IntraDecoder.hpp"
#include "./Golomb.hpp"

IntraDecoder::IntraDecoder(DecoderGolomb* decoder, int shift) : golomb(decoder), shift(shift)
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
        copyMakeBorder(old_frame, old_frame, 1, 0, 1, 0, BORDER_CONSTANT, Scalar::all(0));
    }
    else if (numnum == 1) 
    {
        copyMakeBorder(old_frame, old_frame, 1, 0, 1, 0, BORDER_CONSTANT, Scalar::all(0));
    }

    int flag = golomb->decode();
    std::cerr << "Decoded M: " << flag << std::endl;
    if (flag == 0)
    {
        Decoded_M = golomb->decode();
        if (Decoded_M <= 0)
        {
            std::cerr << "Error: Invalid Golomb parameter M: " << Decoded_M << std::endl;
            return -1;
        }
        golomb->set_M(Decoded_M);
    }
    golomb->set_M(flag * 2);

    for(int i = 1; i < old_frame.rows; i++) 
    {
        for(int j = 1; j < old_frame.cols; j++)
        {
            if (numnum == 1) // Grayscale image
            {
                x = old_frame.at<uchar>(i, j - 1);
                y = old_frame.at<uchar>(i - 1, j);
                w = old_frame.at<uchar>(i - 1, j - 1);

                error = golomb->decode();
                frame_cost += abs(error);

                if(error < 0)
                {
                    error = -1 * (abs(error) >> shift);
                }
                else 
                {
                    error = error >> shift;
                }

                old_frame.at<uchar>(i, j) = static_cast<uchar>(reconstruct_image(x, y, w) + error);
            }
            else if (numnum == 3) // Color image
            {
                Vec3b x_pixel = old_frame.at<Vec3b>(i, j - 1);
                Vec3b y_pixel = old_frame.at<Vec3b>(i - 1, j);
                Vec3b w_pixel = old_frame.at<Vec3b>(i - 1, j - 1);
                Vec3b current_pixel = old_frame.at<Vec3b>(i, j);

                for (int n = 0; n < 3; n++)
                {
                    x = x_pixel[n];
                    y = y_pixel[n];
                    w = w_pixel[n];

                    error = golomb->decode();
                    frame_cost += abs(error);

                    if(error < 0)
                    {
                        error = -1 * (abs(error) >> shift);
                    }
                    else 
                    {
                        error = error >> shift;
                    }

                    old_frame.at<Vec3b>(i, j)[n] = static_cast<uchar>(reconstruct_image(x, y, w) + error);
                }
            }
        }
    }

    int total_pixels = old_frame.cols * old_frame.rows * old_frame.channels();
    if (total_pixels == 0) {
        std::cerr << "Error: Invalid frame dimensions for division." << std::endl;
        return -1;
    }

    old_frame = old_frame(Rect(1, 1, old_frame.cols - 1, old_frame.rows - 1));
    return frame_cost / total_pixels;
}