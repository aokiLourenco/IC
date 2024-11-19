#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "./Headers/BitStream.hpp"
#include "./Headers/Golomb.hpp"

using namespace cv;

EncoderGolomb::EncoderGolomb(std::string file_path, EncodingMode mode) : bitStream(file_path, true), mode(mode) {
    this->set_M(3);
}

void EncoderGolomb::set_M(int m) {
    if (m <= 0) {
        throw std::invalid_argument("M must be greater than 0");
    }
    this->M = m;
    this->b = static_cast<int>(ceil(log2(m)));
}

int EncoderGolomb::get_M() {
    return M;
}

int EncoderGolomb::optimal_m(cv::Mat &frame) {
    double u = 0;
    cv::Scalar mean_values = cv::mean(cv::abs(frame));

    for (int n = 0; n < frame.channels(); n++)
        u += mean_values[n];
    u /= frame.channels();
    if (u < 0.01)
        return 2;
    int s = static_cast<int>(ceil(log2(u) - 0.05 + 0.6 / u));

    s = (0 > s) ? 0 : s;
    return 1 << s; // Use bitwise shift instead of pow
}

void EncoderGolomb::encode(int Number) {
    int mappedNumber;
    if (mode == EncodingMode::SIGN_MAGNITUDE) {
        mappedNumber = Number < 0 ? (-Number << 1) | 1 : Number << 1;
    } else { // INTERLEAVING
        mappedNumber = (Number < 0) ? (-Number << 1) - 1 : Number << 1;
    }

    int q = mappedNumber / M;
    int r = mappedNumber % M;

    // Debug print
    std::cout << "Number: " << Number << ", Mapped Number: " << mappedNumber << ", Quotient: " << q << ", Remainder: " << r << std::endl;

    // Encode the quotient using unary coding
    for (int i = 0; i < q; ++i) {
        bitStream.writeBit(1);
    }
    bitStream.writeBit(0);

    // Encode the remainder using binary coding
    if (M % 2 == 0) {
        bitStream.writeBits(r, b);
    } else {
        if (r < (1 << b) - M) {
            bitStream.writeBits(r, b);
        } else {
            bitStream.writeBits(r + (1 << b) - M, b + 1);
        }
    }
}


void EncoderGolomb::finishEncoding() {
    bitStream.close();
}

DecoderGolomb::DecoderGolomb(std::string file_path, EncodingMode mode) : bitStream(file_path, false), mode(mode) {
    this->set_M(3);
}

void DecoderGolomb::set_M(int m) {
    if (m <= 0) {
        throw std::invalid_argument("M must be greater than 0");
    }
    this->M = m;
    this->b = static_cast<int>(ceil(log2(m)));
}

int DecoderGolomb::get_M() {
    return M;
}

int DecoderGolomb::decode() {
    int r, q, r2, N;

    q = 0;
    // Read unary part (q = count of 1s before the 0)
    while (true) {
        if (bitStream.isEndOfStream()) {
            throw std::runtime_error("Attempt to read beyond end of file while reading unary part");
        }
        if ((bitStream.readBit() & 1) == 0)
            break;
        q++;
    }

    // Read remainder bits
    if (M % 2 == 0) {
        if (bitStream.isEndOfStream()) {
            throw std::runtime_error("Attempt to read beyond end of file while reading remainder bits (even M)");
        }
        r = bitStream.readBits(b);
    } else {
        r2 = bitStream.readBits(b);
        if (r2 < (1 << b) - M) {
            r = r2;
        } else {
            if (bitStream.isEndOfStream()) {
                throw std::runtime_error("Attempt to read beyond end of file while reading remainder bits (odd M, part 2)");
            }
            r = (r2 << 1) | bitStream.readBit();
            r -= (1 << b) - M; // Adjusting the decrement
        }
    }

    N = q * M + r;

    // Debug print
    std::cout << "Quotient: " << q << ", Remainder: " << r << ", N: " << N << std::endl;

    if (mode == EncodingMode::SIGN_MAGNITUDE) {
        std::cout << "Decoded Number (SIGN_MAGNITUDE): " << ((N & 1) ? -(N >> 1) : (N >> 1)) << std::endl;
        return (N & 1) ? -(N >> 1) : (N >> 1);
    } else { // INTERLEAVING
        std::cout << "Decoded Number (INTERLEAVING): " << ((N & 1) ? -(N >> 1) - 1 : (N >> 1)) << std::endl;
        return (N & 1) ? -(N >> 1) - 1 : (N >> 1);
    }
}