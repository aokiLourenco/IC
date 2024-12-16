#include <opencv2/opencv.hpp>
#include <vector>
#include <stdexcept>
#include <map>
#include <functional>
#include <iostream>
#include "./Headers/Golomb.hpp"
#include "./Headers/Predictor.hpp"

using namespace cv;

Predictor::Predictor() {}

Predictor::~Predictor() {}

int Predictor::predict(const Mat& image, int x, int y) {
    if (x == 0) return 0; // No left neighbor
    return image.at<uchar>(y, x - 1);
}

std::vector<int> Predictor::calculate_residuals(const Mat& image) {
    std::vector<int> residuals;
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            int predicted = predict(image, x, y);
            int actual = image.at<uchar>(y, x);
            residuals.push_back(actual - predicted);
        }
    }
    return residuals;
}

void Predictor::encode(const Mat& image, const std::string& output_file) {
    std::vector<int> residuals = calculate_residuals(image);
    EncoderGolomb encoder(output_file, EncodingMode::SIGN_MAGNITUDE);
    encoder.set_M(256);
    for (int residual : residuals) {
        encoder.encode(residual);
    }
    encoder.finishEncoding();
}

Mat Predictor::decode(const std::string& input_file) {
    DecoderGolomb decoder(input_file, EncodingMode::SIGN_MAGNITUDE);
    decoder.set_M(256);
    std::vector<int> residuals;
    int rows = 512;
    int cols = 512;
    int total_pixels = rows * cols;
    try {
        for (int i = 0; i < total_pixels; ++i) {
            residuals.push_back(decoder.decode());
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error decoding: " << e.what() << std::endl;
    }

    int channels = 1;
    return reconstruct_image(residuals, rows, cols, channels);
}

Mat Predictor::reconstruct_image(const std::vector<int>& residuals, int rows, int cols, int channels) {
    Mat image(rows, cols, CV_8UC(channels));
    int index = 0;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int predicted = predict(image, x, y);
            int residual = residuals[index++];
            image.at<uchar>(y, x) = predicted + residual;
        }
    }
    return image;
}

int Predictor_One(int x, int y, int z)
{
    return x;
}

int Predictor_Two(int x, int y, int z)
{
    return y;
}

int Predictor_Three(int x, int y, int z)
{
    return z;
}

int Predictor_Four(int x, int y, int z)
{
    return x + y - z;
}

int Predictor_Five(int x, int y, int z)
{
    return x + (y - z) / 2;
}

int Predictor_Six(int x, int y, int z)
{
    return y + (x - z) / 2;
}

int Predictor_Seven(int x, int y, int z)
{
    return (x + y) / 2;
}

int LS_Predictor(int x, int y, int z)
{
    int prediction = x + y - z;

    int minimum = min(x, y);
    int maximum = max(x, y);

    if (z >= maximum)
    {
        prediction = minimum;
    }
    if (z <= minimum)
        prediction = maximum;

    return prediction;
}

vector<function<int(int, int, int)>> GetPredictors()
{
    vector<function<int(int, int, int)>> predictorFunctions;

    predictorFunctions.push_back(Predictor_One);
    predictorFunctions.push_back(Predictor_Two);
    predictorFunctions.push_back(Predictor_Three);
    predictorFunctions.push_back(Predictor_Four);
    predictorFunctions.push_back(Predictor_Five);
    predictorFunctions.push_back(Predictor_Six);
    predictorFunctions.push_back(Predictor_Seven);
    predictorFunctions.push_back(LS_Predictor);

    return predictorFunctions;
}