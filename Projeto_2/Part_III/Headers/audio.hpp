#include <vector>
#include <string>
#include <iostream>
#include <SFML/Audio.hpp>

#include "./Golomb.hpp"


class audio_codec
{
private:
    std::string filename;
    std::string output;
    sf::SoundBuffer buffer;

    //Lossless vs Lossy
    int mode;
    
    // Dinamic M
    bool dynamic_M = false;

    // Target bitrate for the lossy mode in bits per sample
    int target_bitrate;

    // Basic information about the audio file
    const sf::Int16 *samples;
    std::size_t sampleCount;
    unsigned int sampleRate;
    unsigned int channelCount;
    float duration;

    int calculate_optimal_m(const std::vector<int> &residuals)
    {
        if(residuals.empty()){
            return 512;
        }

        double sum = 0;
        for (int residual : residuals)
        {
            sum += abs(residual);
        }

        sum = sum / residuals.size();

        int m = pow(2,ceil(log2(sum)));

        return std::max(1, m);
    }

public:
    EncoderGolomb encoder;
    DecoderGolomb decoder;
    audio_codec(std::string file, std::string output_file, int M, int target_bits,int encode_mode);
    ~audio_codec();


    // Diferece between samples in the same channel
    std::vector<int> simple_diference(std::vector<double> samples, std::size_t sampleCount);
    
    // Diferece between samples in different channels
    std::vector<int> inter_diference(std::vector<double> samplesL,std::vector<double> samplesR, std::size_t sampleCount);
    
    
    void encode(); //Main encode function, branch into lossless or lossy


    void encode_lossless(); // Main lossless encode function, branch into mono or stereo
    void encode_mono_lossless(); // Encode mono audio lossless
    void encode_stereo_with_inter_channel_lossless(); // Encode stereo audio lossless
    void decode_lossless(); // Decode lossless audio
    
    void encode_lossy(); // Main lossy encode function, branch into mono or stereo
    int quantize_sample(double sample, int step); // Quantize a sample
    double dequantize_sample(int quantized_sample, int step); // Dequantize a sample
    void decode_lossy(); // Decode lossy audio

    void decode(); // Main decode function, branch into lossless or lossy
    
    
};