#include <iostream>
#include "opencv2/opencv.hpp"
#include "../../src/headers/BitStream.hpp"
#include "../../src/headers/IntraEncoder.hpp"
#include "../../src/headers/Predictor.hpp"
#include <chrono>
#include <iomanip>
#include <iterator>
#include "../../src/headers/Converter.hpp"

int main(int argc, char const *argv[]) {
    Converter conv;
    vector<function<int(int, int, int)>> predictors = GetPredictors();
    
    cout << "Enter the name of the file to save to (absolute path): ";
    string output;
    cin >> output;

    cout << "Enter the name of the file to read from (absolute path): ";
    string input;
    cin >> input;

    // Read format from file header
    ifstream file(input, ios::binary);
    string file_header;
    getline(file, file_header);
    file.close();

    istringstream iss(file_header);
    vector<string> tokens{istream_iterator<string>{iss},
                         istream_iterator<string>{}};

    // Determine format
    int format;
    if (tokens.size() > 6) {
        if (tokens[6].compare("C444") == 0) format = 0;
        else if (tokens[6].compare("C422") == 0) format = 1;
        else format = 2;
    } else {
        format = 2;
    }
    cout << "Format: " << format << endl;

    // Count total frames
    VideoCapture frame_counter(input);
    if (!frame_counter.isOpened()) {
        cout << "Error opening video for frame counting" << endl;
        return -1;
    }

    Mat temp_frame;
    int num_frames = 0;
    while (frame_counter.read(temp_frame)) {
        num_frames++;
    }
    frame_counter.release();
    cout << "Total frames counted: " << num_frames << endl;

    // Open video for processing
    VideoCapture cap(input);
    if (!cap.isOpened()) {
        cout << "Error opening video for processing" << endl;
        return -1;
    }

    // Initialize encoder
    int shift = 0;
    int predictor = 0;
    EncoderGolomb encoder(output, EncodingMode::SIGN_MAGNITUDE);
    IntraEncoder intra_encoder(encoder, shift);

    // Write header information
    encoder.encode(format);
    encoder.encode(predictor);
    encoder.encode(shift);
    encoder.encode(num_frames);

    // Process frames
    Mat frame;
    int count = 0;
    switch (format) {
        case 0: {
            while (true) {
                cap >> frame;
                if (frame.empty()) break;
                
                frame = conv.rgb_to_yuv444(frame);
                if (count == 0) {
                    encoder.encode(frame.cols);
                    encoder.encode(frame.rows);
                }
                intra_encoder.encode(frame, predictors[predictor]);
                count++;
                cout << "\rProcessing frame " << count << "/" << num_frames 
                     << " (" << (count * 100.0 / num_frames) << "%)" << flush;
            }
            break;
        }
        case 1: {
            while (true) {
                cap >> frame;
                if (frame.empty()) break;
                
                frame = conv.rgb_to_yuv422(frame);
                if (count == 0) {
                    encoder.encode(frame.cols);
                    encoder.encode(frame.rows);
                }
                intra_encoder.encode(frame, predictors[predictor]);
                count++;
                cout << "\rProcessing frame " << count << "/" << num_frames 
                     << " (" << (count * 100.0 / num_frames) << "%)" << flush;
            }
            break;
        }
        case 2: {
            while (true) {
                cap >> frame;
                if (frame.empty()) break;
                
                frame = conv.rgb_to_yuv420(frame);
                if (count == 0) {
                    encoder.encode(frame.cols);
                    encoder.encode(frame.rows);
                }
                intra_encoder.encode(frame, predictors[predictor]);
                count++;
                cout << "\rProcessing frame " << count << "/" << num_frames 
                     << " (" << (count * 100.0 / num_frames) << "%)" << flush;
            }
            break;
        }
    }
    cout << endl << "Encoding complete!" << endl;
    encoder.finishEncoding();
    return 0;
}
