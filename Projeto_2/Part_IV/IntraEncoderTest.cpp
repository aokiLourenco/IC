#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "./Headers/BitStream.hpp"
#include "./Headers/IntraEncoder.hpp"
#include "./Headers/IntraDecoder.hpp"
#include "./Headers/Predictor.hpp"

using namespace cv;
using namespace std;

int main(int argc, char const *argv[]) 
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_image>" << std::endl;
        return -1;
    }

    std::string input_image_path = argv[1];
    std::string encoded_file_path = "../Data/encoded_file.dat";

    // Load the input image
    Mat image = imread(input_image_path, IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }

    // Encode the image
    Predictor predictor;
    predictor.encode(image, encoded_file_path);

    // Load the encoded file to verify encoding
    std::ifstream encoded_file(encoded_file_path, std::ios::binary);
    if (!encoded_file.is_open()) {
        std::cerr << "Could not open the encoded file!" << std::endl;
        return -1;
    }

    // Display success message
    std::cout << "Image successfully encoded to " << encoded_file_path << std::endl;

    return 0;
}