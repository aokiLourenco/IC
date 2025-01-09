#ifndef INTERFRAME_H
#define INTERFRAME_H

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

struct MotionVector {
    int x;
    int y;
    bool is_intra;  // true if block uses intra coding
};

class InterFrame {
protected:
    int block_size;
    int search_range;
    int i_frame_interval;
    
public:
    InterFrame(int block_size, int search_range, int i_frame_interval);
    virtual ~InterFrame() = default;
    
    // Configuration
    void setBlockSize(int size) { block_size = size; }
    void setSearchRange(int range) { search_range = range; }
    void setIFrameInterval(int interval) { i_frame_interval = interval; }
    
    // Motion estimation
    vector<MotionVector> estimateMotion(const Mat& current, const Mat& reference);
    
    // Block matching
    Mat findBestMatch(const Mat& block, const Mat& reference, Point& motion);
    
    // Mode decision
    bool shouldUseIntra(const Mat& block, const Mat& prediction, int threshold);
};

#endif