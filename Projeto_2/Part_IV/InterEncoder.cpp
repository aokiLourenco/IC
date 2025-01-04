#include "./Headers/InterEncoder.hpp"
#include <unistd.h>

InterEncoder::InterEncoder(EncoderGolomb& encoder, int block_size, int block_range, int shift) :
    golomb(encoder), block_size(block_size), block_range(block_range), shift(shift) {
}

int InterEncoder::get_Block_Size() const 
{
    return block_size;
}

int InterEncoder::get_Block_Range() const 
{
    return block_range;
}

void InterEncoder::set_Block_Size(int block_s) 
{
    block_size = block_s;
}

float InterEncoder::cost(Mat block) 
{
    return sum(sum(abs(block)))[0];
}

int InterEncoder::encode(Mat &old_Frame, Mat &new_Frame) 
{
    Mat min_frame_diff, frame_diff, aux_frame;
    
} 