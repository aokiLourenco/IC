#include "./Headers/InterEncoder.hpp"

InterEncoder::InterEncoder(EncoderGolomb &encoder, int shift, int block_size,
                         int search_range, int i_frame_interval)
    : IntraEncoder(encoder, shift),
      block_size(block_size),
      search_range(search_range),
      i_frame_interval(i_frame_interval) {}

InterEncoder::~InterEncoder() {}

int InterEncoder::encode(Mat &frame, function<int(int,int,int)> predictor) {
    static int frame_count = 0;
    int result;
    
    // Add validation
    if (frame.empty()) {
        throw runtime_error("Empty frame received");
    }
    
    try {
        if (frame_count % i_frame_interval == 0) {
            cout << "Encoding I-frame " << frame_count << endl;
            result = encodeIntra(frame, predictor);
        } else {
            cout << "Encoding P-frame " << frame_count << endl;
            result = encodeInter(frame, predictor);
        }
        
        updateReference(frame);
        frame_count++;
        return result;
    }
    catch (const runtime_error& e) {
        cerr << "Error encoding frame " << frame_count << ": " << e.what() << endl;
        return -1;
    }
}

int InterEncoder::encodeIntra(Mat &frame, function<int(int,int,int)> predictor) {
    Mat image, errorMat;
    int n_ch = frame.channels();
    int size = frame.rows * frame.cols * n_ch;
    int framecost = 0;

    // Add padding for prediction
    if (n_ch == 3) {
        hconcat(Mat::zeros(frame.rows, 1, CV_8UC3), frame, image);
        vconcat(Mat::zeros(1, frame.cols + 1, CV_8UC3), image, image);
        errorMat = Mat::zeros(frame.rows, frame.cols, CV_16SC3);
    } else {
        hconcat(Mat::zeros(frame.rows, 1, CV_8UC1), frame, image);
        vconcat(Mat::zeros(1, frame.cols + 1, CV_8UC1), image, image);
        errorMat = Mat::zeros(frame.rows, frame.cols, CV_16SC1);
    }

    // Calculate predictions and residuals
    for (int i = 1; i < image.rows; i++) {
        for (int j = 1; j < image.cols; j++) {
            for (int ch = 0; ch < n_ch; ch++) {
                int a = image.ptr<uchar>(i, j-1)[ch];
                int b = image.ptr<uchar>(i-1, j)[ch];
                int c = image.ptr<uchar>(i-1, j-1)[ch];

                int pred = predictor(a, b, c);
                int err = image.ptr<uchar>(i, j)[ch] - pred;

                // Shift and encode error
                if (err < 0) {
                    err = -1 * (abs(err) >> shift);
                } else {
                    err = err >> shift;
                }

                errorMat.at<short>(i-1, j-1) = err;
                golomb.encode(err);
                framecost += abs(err);
            }
        }
    }

    return framecost / size;
}

int InterEncoder::encodeInter(Mat &frame, function<int(int,int,int)> predictor) {
    vector<MotionVector> motion_vectors;
    int block_idx = 0;
    Mat residual_frame = Mat::zeros(frame.size(), frame.type());

    try {
        // Process each block
        for (int y = 0; y < frame.rows; y += block_size) {
            for (int x = 0; x < frame.cols; x += block_size) {
                // Ensure valid M value before encoding
                if (golomb.get_M() <= 0 || (golomb.get_M() & 1) == 0) {
                    golomb.set_M(1); // Set to safe default
                }
                
                // Get current block dimensions
                int current_block_height = min(block_size, frame.rows - y);
                int current_block_width = min(block_size, frame.cols - x);
                
                Rect current_block(x, y, current_block_width, current_block_height);
                Mat block = frame(current_block);
                
                // Motion estimation
                Point2i mv;
                Mat best_match = findBestMatch(block, reference_frame, mv);
                
                // Calculate residuals for inter prediction
                Mat inter_residuals;
                absdiff(block, best_match, inter_residuals);
                
                // Calculate residuals for intra prediction
                Mat intra_residuals;
                encodeIntraBlock(block, intra_residuals, predictor);
                
                // Mode decision based on residual energy
                double inter_cost = sum(inter_residuals)[0];
                double intra_cost = sum(intra_residuals)[0];
                
                MotionVector mvec;
                if (intra_cost < inter_cost) {
                    // Use intra mode
                    mvec.is_intra = true;
                    mvec.x = mvec.y = 0;
                    intra_residuals.copyTo(residual_frame(current_block));
                } else {
                    // Use inter mode
                    mvec.is_intra = false;
                    mvec.x = mv.x;
                    mvec.y = mv.y;
                    inter_residuals.copyTo(residual_frame(current_block));
                }
                
                motion_vectors.push_back(mvec);
            }
        }
        
        // Encode motion vectors
        for (const auto &mv : motion_vectors) {
            golomb.encode(mv.x);
            golomb.encode(mv.y);
            golomb.encode(mv.is_intra);
        }
        
        // Encode residuals
        for (int i = 0; i < frame.rows; i++) {
            for (int j = 0; j < frame.cols; j++) {
                golomb.encode(residual_frame.at<uchar>(i, j));
            }
        }
        
        return 0;
    }
    catch (const runtime_error& e) {
        cerr << "Error in inter-frame encoding: " << e.what() << endl;
        return -1;
    }
}

Mat InterEncoder::findBestMatch(const Mat& block, const Mat& reference, Point2i& mv) {
    int best_x = 0, best_y = 0;
    double min_sad = numeric_limits<double>::max();
    
    // Search area boundaries
    int start_x = max(-search_range, 0);
    int end_x = min(search_range, reference.cols - block.cols);
    int start_y = max(-search_range, 0);
    int end_y = min(search_range, reference.rows - block.rows);
    
    // Full search within search range
    for (int dy = start_y; dy <= end_y; dy++) {
        for (int dx = start_x; dx <= end_x; dx++) {
            Mat candidate = reference(Rect(dx, dy, block.cols, block.rows));
            double sad = sum(abs(block - candidate))[0];
            
            if (sad < min_sad) {
                min_sad = sad;
                best_x = dx;
                best_y = dy;
            }
        }
    }
    
    mv.x = best_x;
    mv.y = best_y;
    return reference(Rect(best_x, best_y, block.cols, block.rows));
}

void InterEncoder::encodeIntraBlock(const Mat& block, Mat& residuals, function<int(int,int,int)> predictor) {
    residuals = Mat::zeros(block.size(), block.type());
    
    for (int i = 0; i < block.rows; i++) {
        for (int j = 0; j < block.cols; j++) {
            int a = (j > 0) ? block.at<uchar>(i, j-1) : 0;
            int b = (i > 0) ? block.at<uchar>(i-1, j) : 0;
            int c = (i > 0 && j > 0) ? block.at<uchar>(i-1, j-1) : 0;
            
            int prediction = predictor(a, b, c);
            residuals.at<uchar>(i, j) = block.at<uchar>(i, j) - prediction;
        }
    }
}

void InterEncoder::updateReference(const Mat &frame) {
    frame.copyTo(reference_frame);
}
