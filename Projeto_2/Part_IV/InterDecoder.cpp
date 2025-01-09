#include "InterDecoder.hpp"
#include "Golomb.hpp"

InterDecoder::InterDecoder(DecoderGolomb &decoder, int shift, int block_size,
                         int search_range, int i_frame_interval)
    : IntraDecoder(decoder, shift),
      block_size(block_size),
      search_range(search_range),
      i_frame_interval(i_frame_interval) {}

InterDecoder::~InterDecoder() {}

int InterDecoder::decode(Mat &frame, function<int(int,int,int)> reconstruct_image) {
    static int frame_count = 0;
    int result;
    
    if (frame_count % i_frame_interval == 0) {
        result = decodeIntra(frame, reconstruct_image);
    } else {
        result = decodeInter(frame, reconstruct_image);
    }
    
    updateReference(frame);
    frame_count++;
    return result;
}

int InterDecoder::decodeInter(Mat &frame, function<int(int,int,int)> reconstruct_image) {
    // Decode motion vectors
    int num_blocks = (frame.rows/block_size) * (frame.cols/block_size);
    motion_vectors.clear();
    
    for (int i = 0; i < num_blocks; i++) {
        MotionVector mv;
        mv.x = getGolomb().decode();
        mv.y = getGolomb().decode();
        mv.is_intra = getGolomb().decode();
        motion_vectors.push_back(mv);
    }
    
    // Process each block
    int block_idx = 0;
    for (int y = 0; y < frame.rows; y += block_size) {
        for (int x = 0; x < frame.cols; x += block_size) {
            // Get current block dimensions
            int current_block_height = min(block_size, frame.rows - y);
            int current_block_width = min(block_size, frame.cols - x);
            
            Rect current_block(x, y, current_block_width, current_block_height);
            MotionVector& mv = motion_vectors[block_idx++];

            if (mv.is_intra) {
                // Decode intra block
                Mat block = frame(current_block);
                decodeIntraBlock(block, reconstruct_image);
            } else {
                // Apply motion compensation
                Point2i ref_pos(x + mv.x, y + mv.y);
                Rect ref_block(ref_pos.x, ref_pos.y, current_block_width, current_block_height);
                
                // Ensure reference block is within bounds
                ref_block = ref_block & Rect(0, 0, frame.cols, frame.rows);
                
                // Copy motion compensated block
                reference_frame(ref_block).copyTo(frame(current_block));
                
                // Decode and add residuals
                Mat residual = frame(current_block);
                decodeResiduals(residual);
                
                // Add residuals to motion compensated block
                add(frame(current_block), residual, frame(current_block));
            }
        }
    }

    return 0;
}

void InterDecoder::decodeIntraBlock(Mat& block, function<int(int,int,int)> reconstruct_image) {
    for (int i = 0; i < block.rows; i++) {
        for (int j = 0; j < block.cols; j++) {
            int a = (j > 0) ? block.at<uchar>(i, j-1) : 0;
            int b = (i > 0) ? block.at<uchar>(i-1, j) : 0;
            int c = (i > 0 && j > 0) ? block.at<uchar>(i-1, j-1) : 0;
            
            int prediction = reconstruct_image(a, b, c);
            int residual = getGolomb().decode();
            
            block.at<uchar>(i, j) = saturate_cast<uchar>(prediction + residual);
        }
    }
}

void InterDecoder::decodeResiduals(Mat& residual) {
    for (int i = 0; i < residual.rows; i++) {
        for (int j = 0; j < residual.cols; j++) {
            residual.at<uchar>(i, j) = getGolomb().decode();
        }
    }
}

void InterDecoder::updateReference(const Mat &frame) {
    frame.copyTo(reference_frame);
}

int InterDecoder::decodeIntra(Mat &frame, function<int(int,int,int)> reconstruct_image) {
    // Reuse IntraDecoder's decode method
    return IntraDecoder::decode(frame, reconstruct_image);
}