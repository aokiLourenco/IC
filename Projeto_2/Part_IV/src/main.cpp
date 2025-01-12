#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <iomanip>
#include <iterator>
#include <opencv2/opencv.hpp>
#include "headers/Converter.hpp"
#include "headers/Golomb.hpp"
#include "headers/Predictor.hpp"
#include "headers/InterEncoder.hpp"
#include "headers/InterDecoder.hpp"
#include "headers/IntraEncoder.hpp"
#include "headers/IntraDecoder.hpp"

using namespace std;
using namespace cv;


void printUsage() {
    cout << "Video Codec Usage:" << endl;
    cout << "./codec <mode> <codec> <input> <output> [options]" << endl;
    cout << "\nModes:" << endl;
    cout << "  -e    Encode" << endl;
    cout << "  -d    Decode" << endl;
    cout << "\nCodec Types:" << endl;
    cout << "  -p    Predictor only" << endl;
    cout << "  -i    Intra-frame" << endl;
    cout << "  -v    Inter-frame (video)" << endl;
    cout << "\nCommon Options:" << endl;
    cout << "  --predictor <1-8>   Prediction mode (default: 8)" << endl;
    cout << "  --m <value>         Initial M value (default: 4)" << endl;
    cout << "  --color <space>     Color space (yuv420/yuv422/yuv444)" << endl;
    cout << "\nInter-frame Options:" << endl;
    cout << "  --block <size>      Block size (default: 16)" << endl;
    cout << "  --search <range>    Search range (default: 16)" << endl;
    cout << "  --keyframe <int>    Keyframe interval (default: 30)" << endl;
}

bool isVideo(const string& filename) {
    string ext = filename.substr(filename.find_last_of(".") + 1);
    vector<string> videoExts = {"mp4", "avi", "mkv", "mov", "webm", "y4m"};
    return find(videoExts.begin(), videoExts.end(), ext) != videoExts.end();
}

bool isImage(const string& filename) {
    string ext = filename.substr(filename.find_last_of(".") + 1);
    vector<string> imageExts = {"png", "jpg", "jpeg", "bmp", "tiff", "ppm"};
    return find(imageExts.begin(), imageExts.end(), ext) != imageExts.end();
}

bool isBinary(const string& filename) {
    string ext = filename.substr(filename.find_last_of(".") + 1);
    return ext == "bin";
}



bool verifyInput(const string& filename, const string& codec, bool isEncoding) {
    if (isEncoding) {
        if (codec == "-v" && !isVideo(filename)) {
            cerr << "Error: Video codec requires video input file" << endl;
            return false;
        }
        if (codec == "-i" && !isImage(filename) && !isVideo(filename)) {
            cerr << "Error: Invalid input file format for Intra codec" << endl;
            return false;
        }
        if (codec == "-p" && !isImage(filename)) {
            cerr << "Error: Predictor codec requires image input file" << endl;
            return false;
        }
    } else {
        if (!isBinary(filename)) {
            cerr << "Error: Decoder requires .bin input file" << endl;
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printUsage();
        return 1;
    }

    string mode = argv[1];
    string codec = argv[2];
    string input = argv[3];
    string output = argv[4];

    // Default parameters
    int predictor_num = 8;
    int m_value = 4;
    string colorspace = "yuv420";
    int block_size = 16;
    int search_range = 16;
    int keyframe_interval = 30;

    // Parse options
    for (int i = 5; i < argc; i += 2) {
        string param = argv[i];
        if (i + 1 >= argc) break;

        if (param == "--predictor") predictor_num = stoi(argv[i+1]);
        else if (param == "--m") m_value = stoi(argv[i+1]);
        else if (param == "--color") colorspace = argv[i+1];
        else if (param == "--block") block_size = stoi(argv[i+1]);
        else if (param == "--search") search_range = stoi(argv[i+1]);
        else if (param == "--keyframe") keyframe_interval = stoi(argv[i+1]);
    }

    if (!verifyInput(input, codec, mode == "-e")) {
        printUsage();
        return 1;
    }

    try {
        Converter converter;
        auto predictor = GetPredictors()[predictor_num-1];

        if (mode == "-e") {  // Encode
            EncoderGolomb encoder(output, EncodingMode::INTERLEAVING);
            encoder.set_M(m_value);

            if (codec == "-p") {  // Predictor only
                Mat frame = imread(input);
                if (frame.empty()) {
                    throw runtime_error("Cannot open input image");
                }
                
                if (colorspace == "yuv420")
                    frame = converter.rgb_to_yuv420(frame);
                else if (colorspace == "yuv422")
                    frame = converter.rgb_to_yuv422(frame);
                else if (colorspace == "yuv444")
                    frame = converter.rgb_to_yuv444(frame);

                // Process image with predictor
                for (int i = 0; i < frame.rows; i++) {
                    for (int j = 0; j < frame.cols; j++) {
                        for (int c = 0; c < frame.channels(); c++) {
                            encoder.encode(frame.at<uchar>(i,j,c));
                        }
                    }
                }
            }
            else if (codec == "-i") {  // Intra-frame
                Converter conv;
                vector<function<int(int, int, int)>> predictors = GetPredictors();

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
            }
            else if (codec == "-v") {  // Inter-frame
                VideoCapture cap(input);
                if (!cap.isOpened()) 
                    throw runtime_error("Cannot open input video");

                InterEncoder interEncoder(encoder, block_size, search_range, keyframe_interval);
                
                Mat frame;
                while (cap.read(frame)) {
                    if (colorspace == "yuv420")
                        frame = converter.rgb_to_yuv420(frame);
                    else if (colorspace == "yuv422")
                        frame = converter.rgb_to_yuv422(frame);
                    else if (colorspace == "yuv444")
                        frame = converter.rgb_to_yuv444(frame);

                    interEncoder.encode(frame, predictor);
                }
                cap.release();
            }
        }
        else if (mode == "-d") {  // Decode
            DecoderGolomb decoder(input, EncodingMode::INTERLEAVING);
            decoder.set_M(m_value);

            if (codec == "-p") {  // Predictor only
                //! not asked
            }
            else if (codec == "-i") {  // Intra-frame
                vector<function<int(int, int, int)>> predictors = GetPredictors();
                DecoderGolomb decoder(input, EncodingMode::SIGN_MAGNITUDE);
                int format = decoder.decode();
                int predictor = decoder.decode();
                int shift = decoder.decode();
                int n_frames = decoder.decode();
                int width = decoder.decode();
                int height = decoder.decode();

                IntraDecoder intra_decoder(decoder, shift);
                intra_decoder.decodeVideo(output, n_frames, width, height, predictors[predictor]);
            }
            else if (codec == "-v") {  // Inter-frame
                InterDecoder interDecoder(decoder, block_size, keyframe_interval);
                vector<function<int(int, int, int)>> predictors = GetPredictors();
                int width = decoder.decode();
                int height = decoder.decode();
                int frames = decoder.decode();
                int predictor = decoder.decode();
                interDecoder.decodeVideo(output, frames, width, height, predictors[predictor]);
                
                // DecoderGolomb decoder(input, EncodingMode::INTERLEAVING);
                // decoder.set_M(m_value);
                
                // InterDecoder inter_decoder(decoder, block_size, keyframe_interval);
                // auto predictor = GetPredictors()[predictor_num-1];
                
                // // Decode header info and process video
                // int result = inter_decoder.decodeVideo(output, 0, 0, 0, predictor);
                // if (result < 0) {
                //     cerr << "Error decoding video" << endl;
                //     return 1;
                // }
            }
        }

        cout << "Operation completed successfully" << endl;
        return 0;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}