#include "../headers/DCTUtils.hpp"

Mat performDCT(const Mat& block) {
    Mat floatBlock;
    block.convertTo(floatBlock, CV_32F);
    Mat dctBlock;
    dct(floatBlock, dctBlock);
    return dctBlock;
}

Mat performIDCT(const Mat& block) {
    Mat idctBlock;
    idct(block, idctBlock);
    return idctBlock;
}

Mat quantizeDCT(const Mat& dctBlock, int quantLevel) {
    Mat quantized;
    dctBlock.convertTo(quantized, CV_32S, 1.0 / quantLevel);
    return quantized;
}

Mat inverseQuantizeDCT(const Mat& quantizedBlock, int quantLevel) {
    Mat dctBlock;
    quantizedBlock.convertTo(dctBlock, CV_32F, quantLevel);
    return dctBlock;
}

vector<int> zigzagOrder(const Mat& block) {
    vector<int> result;
    int n = block.rows;
    int m = block.cols;
    for (int i = 0; i < n + m - 1; ++i) {
        if (i % 2 == 0) {
            for (int y = min(i, n - 1); y >= max(0, i - m + 1); --y) {
                result.push_back(block.at<int>(y, i - y));
            }
        } else {
            for (int x = min(i, m - 1); x >= max(0, i - n + 1); --x) {
                result.push_back(block.at<int>(i - x, x));
            }
        }
    }
    return result;
}

Mat inverseZigzagOrder(const vector<int>& zigzag, int rows, int cols) {
    Mat block(rows, cols, CV_32S);
    int n = rows;
    int m = cols;
    int index = 0;
    for (int i = 0; i < n + m - 1; ++i) {
        if (i % 2 == 0) {
            for (int y = min(i, n - 1); y >= max(0, i - m + 1); --y) {
                block.at<int>(y, i - y) = zigzag[index++];
            }
        } else {
            for (int x = min(i, m - 1); x >= max(0, i - n + 1); --x) {
                block.at<int>(i - x, x) = zigzag[index++];
            }
        }
    }
    return block;
}