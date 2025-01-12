#ifndef CONVERTER_HPP
#define CONVERTER_HPP


#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

class Converter
{
    public:
        Converter();
        Mat yuv420_to_rgb(Mat &frame);
        Mat rgb_to_yuv420(Mat &frame);

        Mat yuv422_to_rgb(Mat &frame);
        Mat rgb_to_yuv422(Mat &frame);
        
        Mat yuv444_to_rgb(Mat &frame);
        Mat rgb_to_yuv444(Mat &frame);
    private:
};

#endif