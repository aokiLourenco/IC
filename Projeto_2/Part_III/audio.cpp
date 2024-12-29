#include <string>
#include <iostream>
#include <SFML/Audio.hpp>

#include "./Headers/audio.hpp"


/**
 * Constructor for the audio_codec class
 * Open the file, load the samples and calculate the basic information about the audio file
 * Set the M value for the encoder and decoder
 * @param file: path to the audio file
 * @param output_file: path to the output file
 * @return audio_codec object
 */
audio_codec::audio_codec(std::string file, std::string output_file) : encoder(output_file, EncodingMode::SIGN_MAGNITUDE), decoder(output_file, EncodingMode::SIGN_MAGNITUDE)
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
    first_sample = static_cast<double>(samples[0]);
    sampleCount = buffer.getSampleCount();
    sampleRate = buffer.getSampleRate();
    channelCount = buffer.getChannelCount();
    duration = static_cast<float>(sampleCount) / sampleRate / channelCount;
    encoder.set_M(128);
    decoder.set_M(128);
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
 * Encode the audio file when it's mono
 * Calculate the residual values for the samples and store it in a file
 */
void audio_codec::encode_mono()
{
    // Calculate the residual values for the samples
    std::vector<double> Channel(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        Channel[i] = static_cast<double>(samples[i]);
    }
    std::vector<int> residual = simple_diference(Channel, sampleCount);

    // Encode the residual values and store it in a file
    for (int number : residual)
    {
        encoder.encode(number);
    }
}

/**
 * Encode the audio file when it's stereo
 * Calculate the residual values for the samples of each channel and store it in a file
 */
void audio_codec::encode_stereo_with_inter_channel()
{
    std::vector<int> residual;
    // Calculate the residual values for the samples
    for(std::size_t i = 0; i<sampleCount; i++)
    {
        // Left / Right / Left / Right
        // I / I+1 - I / I+2 - I / I + 3 - I + 1 / I + 4 - I +2
        if(i == 0){
            residual.push_back(samples[i]);
        }
        else if(i%2 != 0){
            residual.push_back(samples[i] - samples[i-1]);
        }else{
            residual.push_back(samples[i] - samples[i-2]);
        }
    }

    for (int number : residual)
    {
        // printf("Number : %d\n", number);
        encoder.encode(number);
    }

}

void audio_codec::encode()
{
    
    printf("Channel Count : %d\n", channelCount);
    // Subdivide into mono or stereo
    if (channelCount == 1)
    {
        // If mono, calculate the residual values for the samples
        encode_mono();
    }
    else
    {
        // If stereo, calculate the residual values for the samples of each channel
        encode_stereo_with_inter_channel();
    }
}

void audio_codec::decode(bool mono)
{
    // Decode the residual values from the file
    std::vector<int> residual_decoded;

    while (!decoder.getBitStream()->isEndOfStream())
    {   
        int decodedNumber = decoder.decode();
        residual_decoded.push_back(decodedNumber);
    }

    // Calculate the original samples using the residual values
    std::vector<double> samples_decoded(sampleCount);
    samples_decoded[0] = residual_decoded[0];
    for (std::size_t i = 1; i < sampleCount; ++i)
    {
        if(i%2 != 0){
            samples_decoded[i] = samples_decoded[i - 1] + residual_decoded[i];
        }else{
            samples_decoded[i] = samples_decoded[i - 2] + residual_decoded[i];
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
    sf::Sound sound_decoded;
    sound_decoded.setBuffer(buffer_decoded);
    sound_decoded.play();
    while (sound_decoded.getStatus() == sf::Sound::Playing)
    {
        // Wait until the sound is over
    }
    
}

int main()
{

    audio_codec audio("../Data/test.wav", "../Data/output.bin");
    printf("Encoding\n");
    audio.encode();
    printf("Decoding\n");
    audio.decode(true);

    return 0;
}