#ifndef DCTUTILS_HPP
#define DCTUTILS_HPP

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

const int BLOCK_SIZE = 8;

Mat performDCT(const Mat& block);
Mat performIDCT(const Mat& block);
Mat quantizeDCT(const Mat& dctBlock, int quantLevel);
Mat inverseQuantizeDCT(const Mat& quantizedBlock, int quantLevel);
vector<int> zigzagOrder(const Mat& block);
Mat inverseZigzagOrder(const vector<int>& zigzag, int rows, int cols);

#endif // DCTUTILS_HPP