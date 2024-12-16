#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include "./BitStream.hpp"
#include "./IntraEncoder.hpp"
#include "./Headers/Golomb.hpp"

IntraEncoder::IntraEncoder(EncoderGolomb* encoder, int shift) : golomb(encoder), shift(shift) 
{
}

IntraEncoder::~IntraEncoder()
{
}

int IntraEncoder::encode(Mat &old_frame, function<int(int, int, int)> reconstruct_image) 
{
    int x, y, w, error, predicted, M, framecost = 0, size, numnum;
    Mat image, error_of_Mat;

    numnum = old_frame.channels();
    size = old_frame.rows * old_frame.cols * numnum;
    
    if (numnum == 3) 
    {
        copyMakeBorder(old_frame, image, 1, 0, 1, 0, BORDER_CONSTANT, Scalar::all(0));
        error_of_Mat = Mat::zeros(old_frame.size(), CV_16SC3);
    }
    else if (numnum == 1) 
    {
        copyMakeBorder(old_frame, image, 1, 0, 1, 0, BORDER_CONSTANT, Scalar::all(0));
        error_of_Mat = Mat::zeros(old_frame.size(), CV_16SC1);
    }
    else 
    {
        cout << "Error: Invalid number of channels" << endl;
        return -1;
    }

    for(int i = 1; i < image.rows; i++) 
    {
        for(int j = 1; j < image.cols; j++)
        {
            if (numnum == 1) // Grayscale image
            {
                x = image.at<uchar>(i, j - 1);
                y = image.at<uchar>(i - 1, j);
                w = image.at<uchar>(i - 1, j - 1);

                uchar current_pixel = image.at<uchar>(i, j);

                predicted = reconstruct_image(x, y, w);
                error = current_pixel - predicted;

                if(error < 0)
                {
                    error = -1 * (abs(error) >> shift);
                }
                else 
                {
                    error = error >> shift;
                }

                error_of_Mat.at<short>(i - 1, j - 1) = error;

                if(error < 0)
                {
                    error = -1 * (abs(error) << shift);
                }
                else
                {
                    error <<= shift;
                }
                image.at<uchar>(i, j) = static_cast<uchar>(predicted + error);
            }
            else if (numnum == 3) // Color image
            {
                Vec3b x_pixel = image.at<Vec3b>(i, j - 1);
                Vec3b y_pixel = image.at<Vec3b>(i - 1, j);
                Vec3b w_pixel = image.at<Vec3b>(i - 1, j - 1);
                Vec3b current_pixel = image.at<Vec3b>(i, j);

                for (int n = 0; n < 3; n++)
                {
                    x = x_pixel[n];
                    y = y_pixel[n];
                    w = w_pixel[n];

                    predicted = reconstruct_image(x, y, w);
                    error = current_pixel[n] - predicted;

                    if(error < 0)
                    {
                        error = -1 * (abs(error) >> shift);
                    }
                    else 
                    {
                        error = error >> shift;
                    }

                    error_of_Mat.at<Vec3s>(i - 1, j - 1)[n] = static_cast<short>(error);

                    if(error < 0)
                    {
                        error = -1 * (abs(error) << shift);
                    }
                    else
                    {
                        error <<= shift;
                    }
                    image.at<Vec3b>(i, j)[n] = static_cast<uchar>(predicted + error);
                }
            }
        }
    }

    M = golomb->optimal_m(error_of_Mat);
    printf("M: %d\n", M);
    if(golomb->get_M() == M) 
    {
        golomb->encode(1);
    }
    else 
    {
        golomb->encode(0);
        golomb->encode(M);
        golomb->set_M(M);
    }

    int channels = error_of_Mat.channels();
    for (int i = 0; i < error_of_Mat.rows; i++)
    {
        for (int j = 0; j < error_of_Mat.cols; j++)
        {
            if (channels == 1)
            {
                short value = error_of_Mat.at<short>(i, j);
                framecost += abs(value);
                golomb->encode(value);
            }
            else if (channels == 3)
            {
                Vec<short, 3> pixel = error_of_Mat.at<Vec<short, 3>>(i, j);
                for (int n = 0; n < 3; n++)
                {
                    short value = pixel[n];
                    framecost += abs(value);
                    golomb->encode(value);
                }
            }
        }
    }

    golomb->finishEncoding(); // Ensure the encoding is properly finalized

    return framecost / size;
}