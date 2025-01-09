#include "Headers/InterEncoder.hpp"

InterEncoder::InterEncoder(EncoderGolomb& encoder, int blockSize, int searchRange, 
                         int iFrameInterval, int shift)
    : IntraEncoder(encoder, shift), 
      blockSize(blockSize), 
      searchRange(searchRange),
      iFrameInterval(iFrameInterval) {
    previousFrame = Mat();
}

InterEncoder::~InterEncoder() = default;

Mat InterEncoder::motionEstimation(const Mat& currentFrame, const Mat& referenceFrame) {
    Mat motionVectors = Mat::zeros(
        currentFrame.rows/blockSize, 
        currentFrame.cols/blockSize, 
        CV_32SC2
    );
    
    for(int i = 0; i < currentFrame.rows - blockSize; i += blockSize) {
        for(int j = 0; j < currentFrame.cols - blockSize; j += blockSize) {
            Rect currentBlock(j, i, blockSize, blockSize);
            Mat block = currentFrame(currentBlock);
            
            Point2i bestMatch(0, 0);
            int minSAD = INT_MAX;
            
            // Search in reference frame
            for(int dy = -searchRange; dy <= searchRange; dy++) {
                for(int dx = -searchRange; dx <= searchRange; dx++) {
                    if(i+dy < 0 || i+dy+blockSize > referenceFrame.rows ||
                       j+dx < 0 || j+dx+blockSize > referenceFrame.cols)
                        continue;
                        
                    Rect searchBlock(j+dx, i+dy, blockSize, blockSize);
                    Mat candidateBlock = referenceFrame(searchBlock);
                    
                    // Calculate Sum of Absolute Differences (SAD)
                    Mat diff;
                    absdiff(block, candidateBlock, diff);
                    int sad = sum(diff)[0];
                    
                    if(sad < minSAD) {
                        minSAD = sad;
                        bestMatch = Point2i(dx, dy);
                    }
                }
            }
            
            motionVectors.at<Vec2i>(i/blockSize, j/blockSize) = Vec2i(bestMatch.x, bestMatch.y);
        }
    }
    return motionVectors;
}

Mat InterEncoder::motionCompensation(const Mat& referenceFrame, const Mat& motionVectors) {
    Mat compensated = Mat::zeros(referenceFrame.size(), referenceFrame.type());
    
    for(int i = 0; i < referenceFrame.rows - blockSize; i += blockSize) {
        for(int j = 0; j < referenceFrame.cols - blockSize; j += blockSize) {
            Vec2i mv = motionVectors.at<Vec2i>(i/blockSize, j/blockSize);
            
            int srcY = i + mv[1];
            int srcX = j + mv[0];
            
            // Boundary check
            if(srcY >= 0 && srcY + blockSize <= referenceFrame.rows &&
               srcX >= 0 && srcX + blockSize <= referenceFrame.cols) {
                Rect srcBlock(srcX, srcY, blockSize, blockSize);
                Rect dstBlock(j, i, blockSize, blockSize);
                
                referenceFrame(srcBlock).copyTo(compensated(dstBlock));
            }
        }
    }
    return compensated;
}

int InterEncoder::encode(Mat& frame, function<int(int,int,int)> predictor) {
    static int frameCount = 0;
    static Mat previousFrame;
    
    // For I-frames, use intra-frame coding
    if(frameCount % iFrameInterval == 0) {
        golomb.encode(1); // Flag for I-frame
        frameCount++;
        return IntraEncoder::encode(frame, predictor);
    }
    
    // P-frame
    golomb.encode(0); // Flag for P-frame
    
    Mat motionVectors = motionEstimation(frame, previousFrame);
    Mat predicted = motionCompensation(previousFrame, motionVectors);
    
    // Encode motion vectors
    for(int i = 0; i < motionVectors.rows; i++) {
        for(int j = 0; j < motionVectors.cols; j++) {
            Vec2i mv = motionVectors.at<Vec2i>(i, j);
            golomb.encode(mv[0]);
            golomb.encode(mv[1]);
        }
    }
    
    // Encode residual
    Mat residual;
    absdiff(frame, predicted, residual);
    
    int totalCost = IntraEncoder::encode(residual, predictor);
    
    frame.copyTo(previousFrame);
    frameCount++;
    
    return totalCost;
}

int InterEncoder::encodeVideo(const string& input, const string& output, 
                            function<int(int,int,int)> predictor) {
    VideoCapture cap(input);
    if (!cap.isOpened()) {
        throw runtime_error("Cannot open input video");
    }

    // Get video properties
    int width = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);
    int totalFrames = cap.get(CAP_PROP_FRAME_COUNT);

    // Write header
    golomb.encode(width);
    golomb.encode(height);
    golomb.encode(totalFrames);
    golomb.encode(blockSize);
    golomb.encode(iFrameInterval);

    Mat frame;
    int frameCount = 0;
    while (cap.read(frame)) {
        if (encode(frame, predictor) < 0) {
            throw runtime_error("Error encoding frame " + to_string(frameCount));
        }
        frameCount++;
    }

    return frameCount;
}