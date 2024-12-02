#include <iostream>
#include <opencv2/opencv.hpp>
#include "./Headers/IntraEncoder.hpp"
#include "./Headers/IntraDecoder.hpp"
#include "./Headers/Golomb.hpp"
#include "./Headers/Predictor.hpp"

using namespace cv;
using namespace std;

int main(int argc, char const *argv[]) {

    vector<function<int(int, int, int)>> predictors = GetPredictors();

    string input_image_path = "../Data/input.png";
    string encoded_file_path = "../Data/encoded_file.dat";
    string decoded_image_path = "decoded.png";

    // Load the input image
    Mat input_image = imread(input_image_path, IMREAD_UNCHANGED);
    if (input_image.empty()) {
        cerr << "Could not open or find the image!" << endl;
        return -1;
    }

    cout << "Encoding and decoding the image..." << endl;

    {
        EncoderGolomb encoder(encoded_file_path, EncodingMode::SIGN_MAGNITUDE);
        encoder.set_M(4);
        IntraEncoder intraEncoder(&encoder);
        intraEncoder.encode(input_image, predictors[0]); // Use the first predictor for encoding
    }

    cout << "Encoding completed." << endl;
    
    {
        DecoderGolomb decoder(encoded_file_path, EncodingMode::SIGN_MAGNITUDE);
        IntraDecoder intraDecoder(&decoder);
        Mat decoded_image = Mat::zeros(input_image.size(), input_image.type());
        intraDecoder.decode(decoded_image, predictors[0]); // Use the same predictor for decoding
        imwrite(decoded_image_path, decoded_image);
    }

    cout << "Encoding and decoding completed successfully." << endl;

    return 0;
}