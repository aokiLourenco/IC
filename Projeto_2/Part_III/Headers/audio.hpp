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

        // Basic information about the audio file
        const sf::Int16* samples; 
        double first_sample;
        std::size_t sampleCount;
        unsigned int sampleRate;
        unsigned int channelCount;
        float duration;

    public:
        EncoderGolomb encoder;
        DecoderGolomb decoder;
        audio_codec(std::string file, std::string output_file);
        ~audio_codec();
        void encode();
        std::vector<int>  simple_diference(std::vector<double> samples, std::size_t sampleCount);
        void encode_mono();
        void encode_stereo();
        void decode(bool mono);
};