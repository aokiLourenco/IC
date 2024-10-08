#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

// fucntion to uniform quantization T4
std::vector<int> uniformQuantization(const std::vector<int>& samples, int numLevels) {
    std::vector<int> quantizedSamples;
    int minSample = *std::min_element(samples.begin(), samples.end());
    int maxSample = *std::max_element(samples.begin(), samples.end());
    int range = maxSample - minSample;
    int stepSize = range / numLevels;

    for (int sample : samples) {
        int qunaitzedSamples = ((sample - minSample) /stepSize) * stepSize + minSample;
        quantizedSamples.push_back(qunaitzedSamples);
    }

    return quantizedSamples;
}

// create Histogram
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

// function to calculate Mean Square Error (MSE)
double calculateMSE(const std::vector<int>& original, const std::vector<int>& quantized) {
    double mse = 0.0;
    for (std::size_t i = 0; i < original.size(); ++i) {
        double diff = original[i] - quantized[i];
        mse += diff * diff;
    }
    return mse / original.size();
}

// function to calculate Signal-to-Noise Ratio (SNR)
double calculateSNR(const std::vector<int>& original, const std::vector<int>& quantized) {
    double signalPower = 0.0;
    double noisePower = 0.0;
    for (std::size_t i = 0; i < original.size(); ++i) {
        signalPower += original[i] * original[i];
        double noise = original[i] - quantized[i];
        noisePower += noise * noise;
    }
    return 10 * std::log10(signalPower / noisePower);
}

int main() {
    // load an audio file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("../samples/sample.wav")) {
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

    std::cout << "\nSample Rate: " << sampleRate << " Hz" << std::endl;
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

    // Quantize the audio samples
    int numLevels = 16; // Adjust the number of levels as needed
    std::vector<int> quantizedLeftChannel = uniformQuantization(leftChannel, numLevels);
    std::vector<int> quantizedRightChannel = uniformQuantization(rightChannel, numLevels);
    std::vector<int> quantizedMidChannel = uniformQuantization(midChannel, numLevels);
    std::vector<int> quantizedSideChannel = uniformQuantization(sideChannel, numLevels);

    // Save the quantized samples to a file
    std::cout << "\nCreating quantized_audio_sample.txt" << std::endl;
    std::ofstream quantizedOutFile("samples/quantized_audio_sample.txt");
    for (std::size_t i = 0; i < quantizedLeftChannel.size(); ++i) {
        quantizedOutFile << quantizedLeftChannel[i] << " " << quantizedRightChannel[i] << " "
                         << quantizedMidChannel[i] << " " << quantizedSideChannel[i] << std::endl;
    }
    quantizedOutFile.close();

        // Calculate MSE and SNR
    double mseLeft = calculateMSE(leftChannel, quantizedLeftChannel);
    double snrLeft = calculateSNR(leftChannel, quantizedLeftChannel);
    double mseRight = calculateMSE(rightChannel, quantizedRightChannel);
    double snrRight = calculateSNR(rightChannel, quantizedRightChannel);

    std::cout << "\nMSE (Left Channel): " << mseLeft << std::endl;
    std::cout << "SNR (Left Channel): " << snrLeft << " dB" << std::endl;
    std::cout << "MSE (Right Channel): " << mseRight << std::endl;
    std::cout << "SNR (Right Channel): " << snrRight << " dB" << std::endl;

    return 0;
}
