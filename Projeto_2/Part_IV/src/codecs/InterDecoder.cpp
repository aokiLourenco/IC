#include <iostream>
#include "../headers/InterDecoder.hpp"
#include "../headers/Golomb.hpp"

InterDecoder::InterDecoder(DecoderGolomb& decoder, int blockSize, 
                         int iFrameInterval, int shift)
    : IntraDecoder(decoder, shift), golomb(decoder),
      blockSize(blockSize),
      iFrameInterval(iFrameInterval) {
    std::cout << "InterDecoder initialized with blockSize: " << blockSize 
              << ", iFrameInterval: " << iFrameInterval 
              << ", shift: " << shift << std::endl;
    std::cout.flush();
}

InterDecoder::~InterDecoder() {}

int InterDecoder::decode(Mat& frame, function<int(int,int,int)> predictor) {
    static int frameCount = 0;
    
    // Check frame type
    int frameType = golomb.decode();
    std::cout << "Decoding frame " << frameCount << ", type: " << frameType << std::endl;
    std::cout.flush();
    
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
            int mvx = golomb.decode();
            int mvy = golomb.decode();
            motionVectors.at<Vec2i>(i, j) = Vec2i(mvx, mvy);
        }
    }
    
    // Motion compensation
    if (previousFrame.empty()) {
        std::cerr << "Error: previousFrame is empty during motion compensation" << std::endl;
        std::cerr.flush();
        return -1;
    }
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
    std::cout << "AAAAAAAAA" << std::endl;
    Mat compensated = Mat::zeros(referenceFrame.size(), referenceFrame.type());
    std::cout << "BBBBBBBBBB" << std::endl;
    
    std::cout << "Performing motion compensation" << std::endl;
    std::cout << "Reference frame size: " << referenceFrame.size() << std::endl;
    std::cout << "Motion vectors size: " << motionVectors.size() << std::endl;
    std::cout.flush();
    
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
    std::cout << "CCCCCCCCCCCCCCCCCCCC" << std::endl;
    // Setup Y4M output
    ofstream outFile(output, ios::binary);
    outFile << "YUV4MPEG2 W" << width << " H" << height 
            << " F30:1 Ip C420\n";
    
    std::cout << "Decoding video with dimensions: " << width << "x" << height << std::endl;
    std::cout << "Number of frames: " << n_frames << std::endl;
    std::cout.flush();
    
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid frame dimensions: " << width << "x" << height << std::endl;
        std::cerr.flush();
        return -1;
    }
    
    Mat frame;
    for(int i = 0; i < n_frames; i++) {
        std::cout << "Decoding frame " << i << " with dimensions: " << width << "x" << height << std::endl;
        std::cout.flush();
        frame = Mat::zeros(height, width, CV_8UC1);
        if(decode(frame, predictor) < 0) return -1;
        
        outFile << "FRAME\n";
        outFile.write((char*)frame.data, width * height);
    }
    
    outFile.close();
    return n_frames;
}