#include "Headers/InterDecoder.hpp"

InterDecoder::InterDecoder(DecoderGolomb& decoder, int blockSize, 
                         int iFrameInterval, int shift)
    : IntraDecoder(decoder, shift),
      blockSize(blockSize),
      iFrameInterval(iFrameInterval) {}

InterDecoder::~InterDecoder() {}

int InterDecoder::decode(Mat& frame, function<int(int,int,int)> predictor) {
    static int frameCount = 0;
    
    // Check frame type
    int frameType = getGolomb().decode();
    
    // I-frame
    if(frameType == 1) {
        int cost = IntraDecoder::decode(frame, predictor);
        frame.copyTo(previousFrame);
        frameCount++;
        return cost;
    }
    
    // P-frame
    Mat motionVectors = Mat::zeros(
        frame.rows/blockSize, 
        frame.cols/blockSize,
        CV_32SC2
    );
    
    // Decode motion vectors
    for(int i = 0; i < motionVectors.rows; i++) {
        for(int j = 0; j < motionVectors.cols; j++) {
            int mvx = getGolomb().decode();
            int mvy = getGolomb().decode();
            motionVectors.at<Vec2i>(i, j) = Vec2i(mvx, mvy);
        }
    }
    
    // Motion compensation
    Mat predicted = motionCompensation(previousFrame, motionVectors);
    
    // Decode residual
    Mat residual = frame.clone();
    int cost = IntraDecoder::decode(residual, predictor);
    
    // Reconstruct frame
    add(predicted, residual, frame);
    frame.copyTo(previousFrame);
    frameCount++;
    
    return cost;
}

Mat InterDecoder::motionCompensation(const Mat& referenceFrame, const Mat& motionVectors) {
    Mat compensated = Mat::zeros(referenceFrame.size(), referenceFrame.type());
    
    for(int i = 0; i < referenceFrame.rows - blockSize; i += blockSize) {
        for(int j = 0; j < referenceFrame.cols - blockSize; j += blockSize) {
            Vec2i mv = motionVectors.at<Vec2i>(i/blockSize, j/blockSize);
            
            int srcY = i + mv[1];
            int srcX = j + mv[0];
            
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

int InterDecoder::decodeVideo(const string& output, int n_frames, int width, 
                            int height, function<int(int,int,int)> predictor) {
    // Setup Y4M output
    ofstream outFile(output, ios::binary);
    outFile << "YUV4MPEG2 W" << width << " H" << height 
            << " F30:1 Ip C420\n";
            
    Mat frame;
    for(int i = 0; i < n_frames; i++) {
        frame = Mat::zeros(height, width, CV_8UC1);
        if(decode(frame, predictor) < 0) return -1;
        
        outFile << "FRAME\n";
        outFile.write((char*)frame.data, width * height);
    }
    
    outFile.close();
    return n_frames;
}