#ifndef PREDICTOR_HPP
#define PREDICTOR_HPP

#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <functional>

using namespace std;
using namespace cv;

class Predictor {
public:
    Predictor();
    ~Predictor();
    void encode(const Mat& image, const std::string& output_file);
    Mat decode(const std::string& input_file);
    vector<function<int(int, int, int)>> Predicting();

private:
    int predict(const Mat& image, int x, int y);
    std::vector<int> calculate_residuals(const Mat& image);
    Mat reconstruct_image(const std::vector<int>& residuals, int rows, int cols, int channels);
};

int Predictor_One(int x, int y, int z);
int Predictor_Two(int x, int y, int z);
int Predictor_Three(int x, int y, int z);
int Predictor_Four(int x, int y, int z);
int Predictor_Five(int x, int y, int z);
int Predictor_Six(int x, int y, int z);
int Predictor_Seven(int x, int y, int z);
int LS_Predictor(int x, int y, int z);
vector<function<int(int, int, int)>> GetPredictors();

#endif // PREDICTOR_HPP