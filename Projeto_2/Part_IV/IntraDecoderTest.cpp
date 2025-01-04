#include <iostream>
#include "opencv2/opencv.hpp"
#include "BitStream.hpp"
#include "IntraDecoder.hpp"
#include "./Predictor.hpp"
#include "./Converter.hpp"
#include <chrono>
#include <thread>

void savePPM(const Mat& image, const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Cannot open file: " << filename << endl;
        return;
    }

    // Write PPM header
    file << "P6\n" << image.cols << " " << image.rows << "\n255\n";

    // Write pixel data
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            Vec3b pixel = image.at<Vec3b>(i, j);
            file.write((char*)&pixel[2], 1); // R
            file.write((char*)&pixel[1], 1); // G
            file.write((char*)&pixel[0], 1); // B
        }
    }
    file.close();
}

int main(int argc, char const *argv[]) {
    Converter conv;
    vector<function<int(int, int, int)>> predictors = GetPredictors();

    cout << "Enter the name of the file to read (absolute path): ";
    string input;
    cin >> input;

    // Debug: Display file size and validation
    ifstream file(input, ios::binary | ios::ate);
    size_t fileSize = file.tellg();
    cout << "File size: " << fileSize << " bytes\n";
    file.close();

    // Initialize decoder and read header
    DecoderGolomb decoder(input, EncodingMode::SIGN_MAGNITUDE);
    
    int format = decoder.decode();
    int predictor = decoder.decode();
    int shift = decoder.decode();
    int n_frames = decoder.decode();
    int width = decoder.decode();
    int height = decoder.decode();

    cout << "Header values:\n"
         << "Format: " << format << "\n"
         << "Predictor: " << predictor << "\n"
         << "Shift: " << shift << "\n"
         << "Frames: " << n_frames << "\n"
         << "Width: " << width << "\n"
         << "Height: " << height << "\n";

    if (format != 2 || width <= 0 || height <= 0 || n_frames <= 0) {
        cerr << "Invalid header values\n";
        return -1;
    }

    IntraDecoder intra_decoder(decoder, shift);
    namedWindow("Decoded Frame", WINDOW_AUTOSIZE);

    for (int frame_count = 0; frame_count < n_frames; frame_count++) {
        Mat yuv_frame = Mat::zeros(height, width, CV_8UC1);
        cout << "Decoding frame " << frame_count + 1 << "/" << n_frames << "\n";
        
        intra_decoder.decode(yuv_frame, predictors[predictor]);

        if (!yuv_frame.empty() && yuv_frame.rows == height && yuv_frame.cols == width) {
            Mat rgb_frame = conv.yuv420_to_rgb(yuv_frame);
            if (!rgb_frame.empty()) {
                imshow("Decoded Frame", rgb_frame);
                
                // Save last frame as PPM
                if (frame_count == n_frames - 1) {
                    string ppm_filename = input.substr(0, input.find_last_of('.')) + "_decoded.ppm";
                    savePPM(rgb_frame, ppm_filename);
                    cout << "Saved decoded frame to: " << ppm_filename << endl;
                }
                
                char key = waitKey(10);
                if (key == 27) break;
                
                if (frame_count == n_frames - 1) {
                    this_thread::sleep_for(chrono::seconds(10));
                }
            }
        }
    }

    destroyAllWindows();
    return 0;
}