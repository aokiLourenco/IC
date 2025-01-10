#include <string>
#include <iostream>
#include <SFML/Audio.hpp>
#include <memory>
#include <chrono>

// Include snd
#include <sndfile.hh>

#include "./Headers/audio.hpp"

/**
 * Constructor for the audio_codec class
 * Open the file, load the samples and calculate the basic information about the audio file
 * Set the M value for the encoder and decoder
 * @param file: path to the audio file
 * @param output_file: path to the output file
 * @return audio_codec object
 */
audio_codec::audio_codec(std::string file, std::string output_file, int M, int target_bits, int encode_mode) : encoder(output_file, EncodingMode::SIGN_MAGNITUDE), decoder(output_file, EncodingMode::SIGN_MAGNITUDE)
{
    // Open file and extract basic information
    filename = file;
    output = output_file;
    if (!buffer.loadFromFile(filename))
    {
        std::cerr << "Error loading audio file: " << filename << std::endl;
        return;
    }
    samples = buffer.getSamples();
    sampleCount = buffer.getSampleCount();
    sampleRate = buffer.getSampleRate();
    channelCount = buffer.getChannelCount();
    duration = static_cast<float>(sampleCount) / sampleRate / channelCount;
    dynamic_M = true;
    if (M != 0)
    {
        dynamic_M = false;
        encoder.set_M(M);
        decoder.set_M(M);
    }
    mode = encode_mode;
    target_bitrate = target_bits;
}

audio_codec::~audio_codec()
{
    std::cout << "Destroying audio_codec object" << std::endl;
}

/**
 *  Calculate the residual signal using the simple formula: residual[i] = samples[i] - samples[i - 1]
 * @param samples: vector with the samples of the audio file
 * @param sampleCount: number of samples in the audio file
 * @return vector with the residual values
 *
 */
std::vector<int> audio_codec::simple_diference(std::vector<double> samples, std::size_t sampleCount)
{
    // Calculate the residual signal using the simple formula: residual[i] = samples[i] - samples[i - 1]
    std::vector<int> residual;
    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        residual.push_back(static_cast<int>(samples[i] - samples[i - 1]));
    }
    return residual;
}

/**
 * Calculate the residual signal using the simple formula: residual[i] = samplesL[i] - samplesR[i]
 * @param samplesL: vector with the samples of the left channel
 * @param samplesR: vector with the samples of the right channel
 * @param sampleCount: number of samples in the audio file
 * @return vector with the residual values
 */

std::vector<int> audio_codec::inter_diference(std::vector<double> samplesL, std::vector<double> samplesR, std::size_t sampleCount)
{
    std::vector<int> residual;
    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        residual.push_back(static_cast<int>(samplesL[i] - samplesR[i]));
    }
    return residual;
}

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                         Lossless Encoding                                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/**
 * Encode the audio file when it's mono
 * Calculate the residual values for the samples and store it in a file
 * Lossless way
 */
void audio_codec::encode_mono_lossless()
{
    // Calculate the residual values for the samples
    std::vector<double> Channel(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        Channel[i] = static_cast<double>(samples[i]);
    }
    std::vector<int> residual = simple_diference(Channel, sampleCount);

    std::vector<int> residuals;

    // std::cout << "Total encoded residuals: " << residual.size() << std::endl;
    for (int i = 0; i < residual.size(); i++)
    {

        if (dynamic_M && i % 512 == 0)
        {
            int m = calculate_optimal_m(residuals);
            residuals.clear();
            encoder.set_M(m);
        }

        encoder.encode(residual[i]);
        residuals.push_back(residual[i]);
    }

    std::ofstream file;
    file.open("../Data/samples", std::ios::app);
    for (int i = 0; i < sampleCount; i++)
    {
        file << samples[i] << std::endl;
    }
    file.close();
    encoder.encode(13102003); // End of file
    // Encode the things refering to the audio
    encoder.encode(channelCount);
    encoder.encode(sampleRate);
    encoder.encode(sampleCount);
    encoder.finishEncoding();
}

/**
 * Encode the audio file when it's stereo
 * Calculate the residual values for the samples of each channel and store it in a file
 * Lossless way
 */
void audio_codec::encode_stereo_with_inter_channel_lossless()
{

    std::vector<double> Channel1(sampleCount / 2);
    std::vector<double> Channel2(sampleCount / 2);

    for (std::size_t i = 0; i < sampleCount / 2; ++i)
    {
        Channel1[i] = static_cast<double>(samples[2 * i]);     // Left channel sample
        Channel2[i] = static_cast<double>(samples[2 * i + 1]); // Right channel sample
    }

    std::vector<int> residual1 = simple_diference(Channel1, Channel1.size());
    std::vector<int> residual2 = inter_diference(Channel1, Channel2, Channel1.size());

    std::vector<int> residuals;

    for (int i = 0; i < residual1.size(); i++)
    {

        if (dynamic_M && i % 256 == 0)
        {
            // printf("Calculating optimal M\n");
            int m = calculate_optimal_m(residuals);
            residuals.clear();
            encoder.set_M(m);
        }

        encoder.encode(residual1[i]);
        residuals.push_back(residual1[i]);
        encoder.encode(residual2[i]);
        residuals.push_back(residual2[i]);
    }

    encoder.encode(13102003); // End of file

    // Encode the things refering to the audio
    encoder.encode(channelCount);
    encoder.encode(sampleRate);
    encoder.encode(sampleCount);
    encoder.finishEncoding();
}

/**
 * Main encode function, branch into lossless or lossy
 */
void audio_codec::encode_lossless()
{

    // Subdivide into mono or stereo
    if (channelCount == 1)
    {
        // If mono, calculate the residual values for the samples
        encode_mono_lossless();
    }
    else
    {
        // If stereo, calculate the residual values for the samples of each channel
        encode_stereo_with_inter_channel_lossless();
    }
}

void audio_codec::decode_lossless()
{
    // Decode the residual values from the file
    std::vector<int> residual_decoded;
    std::vector<int> residuals;

    int cen = 0;
    while (!decoder.getBitStream()->isEndOfStream())
    {

        if (dynamic_M && cen % 512 == 0)
        {
            int m = calculate_optimal_m(residuals);
            residuals.clear();
            decoder.set_M(m);
        }

        int decodedNumber = decoder.decode();
        if (decodedNumber == 13102003)
        {
            channelCount = decoder.decode();
            sampleRate = decoder.decode();
            sampleCount = decoder.decode();
            break;
        }
        residual_decoded.push_back(decodedNumber);
        residuals.push_back(decodedNumber);
        cen++;
    }

    // Calculate the original samples using the residual values
    std::vector<double> samples_decoded(sampleCount);
    samples_decoded[0] = residual_decoded[0];

    if (channelCount == 1)
    {
        for (std::size_t i = 1; i < sampleCount; ++i)
        {
            samples_decoded[i] = samples_decoded[i - 1] + residual_decoded[i];
        }
    }
    else
    {

        for (std::size_t i = 1; i < sampleCount; ++i)
        {
            if (i % 2 != 0)
            {
                samples_decoded[i] = samples_decoded[i - 1] - residual_decoded[i];
            }
            else
            {
                samples_decoded[i] = samples_decoded[i - 2] + residual_decoded[i];
            }
        }
    }
    // Create a new audio buffer with the decoded samples
    sf::SoundBuffer buffer_decoded;

    // Transform samples_decoded to Int16
    std::vector<sf::Int16> samples_decoded_int16(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        samples_decoded_int16[i] = static_cast<sf::Int16>(samples_decoded[i]);
    }

    buffer_decoded.loadFromSamples(&samples_decoded_int16[0], sampleCount, channelCount, sampleRate);

    // Save the decoded samples to a new audio file
    buffer_decoded.saveToFile("../Data/output_decoded.wav");
    std::cout << "Decoded audio file saved in /Data/output_decoded.wav" << std::endl;
}

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           Lossy Encoding                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/**
 * Quantize a sample to reduce its precision based on the quantization step size.
 * @param sample: the original sample value.
 * @param step: the quantization step size.
 * @return quantized value.
 */
int audio_codec::quantize_sample(double sample, int step)
{
    double halfStep = step / 2.0;
    if (sample >= 0)
    {
        return static_cast<int>((sample + halfStep) / step);
    }
    else
    {
        return static_cast<int>((sample - halfStep) / step);
    }
}

/**
 * Dequantize a sample to approximate the original value.
 * @param quantized_sample: the quantized sample value.
 * @param step: the quantization step size.
 * @return dequantized value.
 */
double audio_codec::dequantize_sample(int quantized_sample, int step)
{
    return static_cast<double>(quantized_sample * step);
}

/**
 * Encode the audio file with lossy compression.
 * Implements quantization to achieve bitrate control.
 */
void audio_codec::encode_lossy()
{

    int max_value = 32767;  // Max value for 16-bit PCM audio
    int min_value = -32768; // Min value for 16-bit PCM audio

    if (target_bitrate >= 16)
    {
        std::cerr << "Warning: Target bitrate too high, limiting to 15 bits" << std::endl;
        target_bitrate = 15;
    }

    int quantization_levels = pow(2, target_bitrate);
    int step_size = (max_value - min_value) / quantization_levels;

    std::vector<int> quantized_samples(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        quantized_samples[i] = quantize_sample(samples[i], step_size);
    }

    std::vector<int> temp;

    for (int i = 0; i < quantized_samples.size(); i++)
    {

        if (i % 512 == 0)
        {
            int m = calculate_optimal_m(temp);
            temp.clear();
            encoder.set_M(m);
        }
        encoder.encode(quantized_samples[i]);
        temp.push_back(quantized_samples[i]);
    }

    encoder.encode(13102003); // End of file

    encoder.encode(channelCount);
    encoder.encode(sampleRate);
    encoder.encode(sampleCount);
    encoder.finishEncoding();
}

/**
 * Decode the lossy compressed audio file.
 * Reconstructs the audio file using the quantized values and step size.
 */
void audio_codec::decode_lossy()
{
    int max_value = 32767;  // Max value for 16-bit PCM audio
    int min_value = -32768; // Min value for 16-bit PCM audio
    int quantization_levels = pow(2, target_bitrate);
    int step_size = (max_value - min_value) / quantization_levels;

    std::vector<int> quantized_samples;
    std::vector<int> temp;
    int cen = 0;
    while (!decoder.getBitStream()->isEndOfStream())
    {

        if (cen % 512 == 0)
        {
            int m = calculate_optimal_m(temp);
            temp.clear();
            decoder.set_M(m);
        }

        int decoded_value = decoder.decode();

        if (decoded_value == 13102003)
        {
            channelCount = decoder.decode();
            sampleRate = decoder.decode();
            sampleCount = decoder.decode();
            break;
        }
        quantized_samples.push_back(decoded_value);
        temp.push_back(decoded_value);
        cen++;
    }

    // Reconstruct the original samples
    std::vector<sf::Int16> reconstructed_samples(sampleCount);
    // double mse = 0.0;
    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        reconstructed_samples[i] = static_cast<sf::Int16>(dequantize_sample(quantized_samples[i], step_size));
        // double error = static_cast<double>(samples[i]) - reconstructed_samples[i];
        // mse += error * error;
    }

    // mse /= sampleCount;

    // std::cout << "Mean Squared Error (MSE): " << mse << std::endl;

    // Create a new audio buffer with the reconstructed samples
    sf::SoundBuffer buffer_decoded;
    buffer_decoded.loadFromSamples(&reconstructed_samples[0], sampleCount, channelCount, sampleRate);

    // Save the decoded samples to a new audio file
    buffer_decoded.saveToFile("../Data/output_lossy_decoded.wav");
    std::cout << "Lossy decoded audio file saved in /Data/output_lossy_decoded.wav" << std::endl;
}

/**
 * Main encode function, branch into lossless or lossy
 */

void audio_codec::encode()
{

    if (mode == 1)
    {
        encode_lossless();
    }
    else
    {
        encode_lossy();
    }
}

/**
 * Main decode function, branch into lossless or lossy
 */
void audio_codec::decode()
{
    if (mode == 1)
    {
        decode_lossless();
    }
    else
    {
        decode_lossy();
    }
}

int main(int argc, char *argv[])
{

    // Args : input file, output file, M

    if (argc < 3)
    {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    int mode;

    while (mode != 1 && mode != 2)
    {
        std::cout << "Mode: 1 - Lossless, 2 - Lossy" << std::endl;
        std::cin >> mode;
    }

    std::unique_ptr<audio_codec> audio;

    // Lossless
    if (mode == 1)
    {
        int M = 0;
        std::cout << "M (0 for dynamic): ";
        std::cin >> M;
        if (M != 0)
        {
            int M = pow(2, ceil(log2(M)));
        }
        audio = std::make_unique<audio_codec>(input_file, output_file, M, 0, 1);
    }
    // Lossy
    else
    {
        int target_bitrate;
        while (1)
        {
            std::cout << "Enter target bitrate (bits per sample): ";
            std::cin >> target_bitrate;
            if (target_bitrate > 0)
            {
                break;
            }
            std::cout << "Invalid bitrate. Please enter a positive integer." << std::endl;
        }
        audio = std::make_unique<audio_codec>(input_file, output_file, 0, target_bitrate, 2);
    }

    /* Decoment to measure time */

    printf("Encoding\n");
    // auto start_encode = std::chrono::high_resolution_clock::now();
    audio->encode();
    // auto end_encode = std::chrono::high_resolution_clock::now();

    // std::chrono::duration<double> encode_time = end_encode - start_encode;
    // std::cout << "Encoding time: " << encode_time.count() << " seconds" << std::endl;

    printf("Decoding\n");
    // auto start_decode = std::chrono::high_resolution_clock::now();
    audio->decode();
    // auto end_decode = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> decode_time = end_decode - start_decode;
    // std::cout << "Decoding time: " << decode_time.count() << " seconds" << std::endl;

    return 0;
}