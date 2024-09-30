#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

void createHistogram(const std::vector<int>& data, const std::string& filename, int binSize) {
    std::map<int, int> histogram;
    for (int value : data) {
        int bin = (value / binSize) * binSize;
        histogram[bin]++;
    }

    std::ofstream outFile(filename);
    for (const auto& pair : histogram) {
        outFile << pair.first << " " << pair.second << std::endl;
    }
    outFile.close();
}

int main() {
    // load an audio file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("samples/sample.wav")) {
        std::cerr << "Error loading audio file" << std::endl;
        return -1;
    }

    // extract raw audio samples
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();

    // Get basic information about the audio file
    unsigned int sampleRate = buffer.getSampleRate();
    unsigned int channelCount = buffer.getChannelCount();
    float duration = static_cast<float>(sampleCount) / sampleRate / channelCount;

    std::cout << "Sample Rate: " << sampleRate << " Hz" << std::endl;
    std::cout << "Channels: " << channelCount << std::endl;
    std::cout << "Duration: " << duration << " seconds" << std::endl;

    std::cout << "Creating audio_sample.txt" << std::endl;
    std::ofstream outFile("samples/audio_sample.txt");
    outFile << sampleRate << " " << channelCount << " " << sampleCount << std::endl;
    for (std::size_t i = 0; i < sampleCount; ++i) {
        outFile << samples[i] << std::endl;
    }
    outFile.close();

    std::vector<int> leftChannel, rightChannel, midChannel, sideChannel;
    for (std::size_t i = 0; i < sampleCount; i += channelCount) {
        int left = samples[i];
        int right = (channelCount == 2) ? samples[i + 1] : 0;
        leftChannel.push_back(left);
        rightChannel.push_back(right);
        midChannel.push_back((left + right) / 2);
        sideChannel.push_back((left - right) / 2);
    }

    int binSize = 256; // Adjust bin size as needed
    createHistogram(leftChannel, "samples/left_channel_histogram.txt", binSize);
    createHistogram(rightChannel, "samples/right_channel_histogram.txt", binSize);
    createHistogram(midChannel, "samples/mid_channel_histogram.txt", binSize);
    createHistogram(sideChannel, "samples/side_channel_histogram.txt", binSize);

    return 0;
}