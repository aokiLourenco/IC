#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "./Headers/BitStream.hpp"
#include "./Headers/IntraEncoder.hpp"
#include "./Headers/IntraDecoder.hpp"
#include "./Headers/Predictor.hpp"
#include "./Headers/Golomb.hpp"

using namespace cv;
using namespace std;

int main(int argc, char const *argv[]) 
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <encoded_file>" << std::endl;
        return -1;
    }

    std::string encoded_file_path = argv[1];

    // Decode the image
    Predictor predictor;
    Mat decoded_image = predictor.decode(encoded_file_path);

    // Save the decoded image
    imwrite("decoded.png", decoded_image);

    // Display the decoded image
    namedWindow("Decoded Image", WINDOW_AUTOSIZE);
    imshow("Decoded Image", decoded_image);

    waitKey(0);

    return 0;
}