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
    // Add M validation
    if (M <= 0 || (M & 1) == 0) {
        throw std::runtime_error("Invalid M value: M must be positive and odd");
    }

    // Map number according to encoding mode
    uint32_t mappedNumber;
    if (mode == EncodingMode::SIGN_MAGNITUDE) {
        if (Number == INT_MIN) {
            // Special case for INT_MIN
            mappedNumber = static_cast<uint32_t>(INT_MAX) + 1;
            mappedNumber = (mappedNumber << 1) | 1;
        } else {
            mappedNumber = static_cast<uint32_t>(Number < 0 ? -Number : Number) << 1;
            if (Number < 0) mappedNumber |= 1;
        }
    } else { // INTERLEAVING
        if (Number == INT_MIN) {
            // Special case for INT_MIN
            mappedNumber = static_cast<uint32_t>(INT_MAX) + 1;
            mappedNumber = (mappedNumber << 1) - 1;
        } else {
            mappedNumber = static_cast<uint32_t>(Number < 0 ? -Number : Number) << 1;
            if (Number < 0) mappedNumber -= 1;
        }
    }

    // Calculate quotient and remainder using unsigned arithmetic
    uint32_t q = mappedNumber / static_cast<uint32_t>(M);
    uint32_t r = mappedNumber % static_cast<uint32_t>(M);

    try {
        // Encode quotient in unary
        for (int i = 0; i < q; ++i) {
            bitStream.writeBit(1);
        }
        bitStream.writeBit(0);

        // Encode remainder
        if (M % 2 == 0) {
            if (r >= M) {
                throw std::runtime_error("Invalid remainder for even M");
            }
            bitStream.writeBits(r, b);
        } else {
            if (r < (1 << b) - M) {
                bitStream.writeBits(r, b);
            } else {
                if (r >= M) {
                    throw std::runtime_error("Invalid remainder for odd M");
                }
                bitStream.writeBits(r + (1 << b) - M, b + 1);
            }
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Bit stream error: ") + e.what());
    }
}

void EncoderGolomb::finishEncoding() {
    bitStream.flushBuffer();
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
    if (M <= 0) {
        throw std::invalid_argument("M must be positive");
    }

    int r, q = 0;

    // Read unary part
    while (true) {
        if (bitStream.isEndOfStream()) {
            throw std::runtime_error("Unexpected end of stream while reading unary");
        }
        if ((bitStream.readBit() & 1) == 0) {
            break;
        }
        q++;
    }

    // Read remainder bits
    if (M % 2 == 0) {
        if (bitStream.isEndOfStream()) {
            throw std::runtime_error("Unexpected end of stream while reading remainder (even M)");
        }
        r = bitStream.readBits(b);
        if (r >= M) {
            throw std::runtime_error("Invalid remainder value for even M");
        }
    } else {
        if (bitStream.isEndOfStream()) {
            throw std::runtime_error("Unexpected end of stream while reading remainder (odd M)");
        }
        int r2 = bitStream.readBits(b);
        
        if (r2 < (1 << b) - M) {
            r = r2;
        } else {
            if (bitStream.isEndOfStream()) {
                throw std::runtime_error("Unexpected end of stream while reading extra bit");
            }
            r = (r2 << 1) | bitStream.readBit();
            r -= (1 << b) - M;
            if (r >= M) {
                throw std::runtime_error("Invalid remainder value for odd M");
            }
        }
    }

    // Check for overflow
    if (q > (INT_MAX - r) / M) {
        throw std::overflow_error("Integer overflow in Golomb decoding");
    }

    int N = q * M + r;

    // Sign handling
    if (mode == EncodingMode::SIGN_MAGNITUDE) {
        return (N & 1) ? -(N >> 1) : (N >> 1);
    } else { // INTERLEAVING
        return (N & 1) ? -(N >> 1) - 1 : (N >> 1);
    }
}

BitStream* DecoderGolomb::getBitStream() {
    return &bitStream;
}