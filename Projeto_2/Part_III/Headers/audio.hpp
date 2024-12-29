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

        // Basic information about the audio file
        const sf::Int16* samples; 
        double first_sample;
        std::size_t sampleCount;
        unsigned int sampleRate;
        unsigned int channelCount;
        float duration;

        // // Dinamic change of M value
        // int block_size = 1024; // Block size that will be used to calculate the optimal M value
        
        // int calculate_optimal_m(std::vector<int> residual){

        //     int m_optimal = 0;
        //     printf("Residual Size : %d\n", residual.size());
        //     for(size_t i = 0; i<residual.size(); i++)
        //     {
        //         // printf("Residual (%d) : %d\n",i, residual[i]);
        //         m_optimal += abs(residual[i]);
        //     }

        //     m_optimal = ((m_optimal / residual.size())/2)*2;
        //     int power_of_two = 1;
        //     while (power_of_two < m_optimal && power_of_two < 128)
        //     {
        //         power_of_two <<= 1;
        //     }
        //     printf("M Optimal : %d\n", power_of_two);
        //     return power_of_two;

        // }



    public:
        EncoderGolomb encoder;
        DecoderGolomb decoder;
        audio_codec(std::string file, std::string output_file);
        ~audio_codec();
        void encode();
        std::vector<int>  simple_diference(std::vector<double> samples, std::size_t sampleCount);
        void encode_mono();
        void encode_stereo_with_inter_channel();
        void decode(bool mono);
};