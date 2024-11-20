#include <string>
#include <iostream>
#include <SFML/Audio.hpp>

#include "./Headers/audio.hpp"

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
}

audio_codec::~audio_codec()
{
    std::cout << "Destroying audio_codec object" << std::endl;
}

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

void audio_codec::encode_stereo()
{
    //! To change: Could use values from the other channel to calculate one, that way it's more efficient

    std::vector<double> leftChannel(sampleCount / 2);
    std::vector<double> rightChannel(sampleCount / 2);
    for (std::size_t i = 0; i < sampleCount / 2; ++i)
    {
        leftChannel[i] = static_cast<double>(samples[2 * i]);      // Left channel sample
        rightChannel[i] = static_cast<double>(samples[2 * i + 1]); // Right channel sample
    }
    std::vector<int> residual_left = simple_diference(leftChannel, sampleCount / 2);
    std::vector<int> residual_right = simple_diference(rightChannel, sampleCount / 2);

    // Encode the residual values and store it in a file
    for(size_t i = 0; i<residual_left.size(); i++)
    {
        encoder.encode(residual_left[i]);
        encoder.encode(residual_right[i]);
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
        encode_stereo();
    }
}

void audio_codec::decode(bool mono)
{
    // Decode the residual values from the file
    std::vector<int> residual_decoded;
    if (mono)
    {
        while (!decoder.getBitStream()->isEndOfStream())
        {
            int decodedNumber = decoder.decode();
            residual_decoded.push_back(decodedNumber);
            // printf("Decoded Number : %d\n", decodedNumber);
        }
    }
    else
    {
        while (!decoder.getBitStream()->isEndOfStream())
        {
            int decodedNumber = decoder.decode();
            residual_decoded.push_back(decodedNumber);
            // printf("Decoded Number : %d\n", decodedNumber);
        }
    }

    // Calculate the original samples using the residual values
    std::vector<double> samples_decoded(sampleCount);
    samples_decoded[0] = first_sample;
    for (std::size_t i = 1; i < sampleCount; ++i)
    {
        samples_decoded[i] = samples_decoded[i - 1] + residual_decoded[i - 1];
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