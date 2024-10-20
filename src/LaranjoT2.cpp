#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <limits>
#include "matplotlibcpp.h" 
#include <SFML/Audio.hpp>

namespace plt = matplotlibcpp;
using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames


// Function to load and process a .wav audio file
int T1(const std::string& filename) {
    // Load an audio file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sample.wav")) {
        std::cerr << "Error loading audio file" << std::endl;
        return -1;
    }

    // Extract raw audio samples
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();

    // Get basic information about the audio file
    unsigned int sampleRate = buffer.getSampleRate();
    unsigned int channelCount = buffer.getChannelCount();
    float duration = static_cast<float>(sampleCount) / sampleRate / channelCount;

    // Print the information
    std::cout << "Sample Rate: " << sampleRate << " Hz" << std::endl;
    std::cout << "Channels: " << channelCount << std::endl;
    std::cout << "Duration: " << duration << " seconds" << std::endl;

    return 0;
}

int T2(const std::string& filename) {
    sf::SoundBuffer buffer;

    // Load the audio file
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Error loading audio file: " << filename << std::endl;
        return -1;
    }

    // Get audio samples
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    unsigned int sampleRate = buffer.getSampleRate();

    // Prepare time vector
    std::vector<double> time(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i) {
        time[i] = static_cast<double>(i) / sampleRate;  // Time in seconds
    }

    // Prepare amplitude vector
    std::vector<double> amplitude(samples, samples + sampleCount); // Convert samples to double for plotting

    // Create the plot
    plt::figure_size(1200, 400);
    plt::plot(time, amplitude);
    plt::xlabel("Time (seconds)");
    plt::ylabel("Amplitude");
    plt::title("Waveform of " + filename);
    plt::grid(true);
    plt::show();  // Display the plot

    return 0; // Return success
}

int T3(const std::string& filename) {
    sf::SoundBuffer buffer;

    // Load the audio file
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Error loading audio file: " << filename << std::endl;
        return -1;
    }

    // Get audio samples
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    unsigned int channelCount = buffer.getChannelCount();
    unsigned int sampleRate = buffer.getSampleRate();

    if (channelCount != 2) {
        std::cerr << "This code only handles stereo (2-channel) audio." << std::endl;
        return -1;
    }

    // Prepare vectors for left and right channels, MID, and SIDE
    std::vector<double> leftChannel(sampleCount / 2);
    std::vector<double> rightChannel(sampleCount / 2);
    std::vector<double> midChannel(sampleCount / 2);
    std::vector<double> sideChannel(sampleCount / 2);

    // Split stereo samples into left and right channels, and calculate MID and SIDE channels
    for (std::size_t i = 0; i < sampleCount / 2; ++i) {
        leftChannel[i] = static_cast<double>(samples[2 * i]);      // Left channel sample
        rightChannel[i] = static_cast<double>(samples[2 * i + 1]); // Right channel sample
        midChannel[i] = (leftChannel[i] + rightChannel[i]) / 2;    // MID channel (L + R) / 2
        sideChannel[i] = (leftChannel[i] - rightChannel[i]) / 2;   // SIDE channel (L - R) / 2
    }

    int numBins = 256;

    // Left channel histogram
    plt::figure_size(1200, 800);
    plt::hist(leftChannel, numBins);
    plt::title("Left Channel Histogram");
    plt::show();

    // Right channel histogram
    plt::figure_size(1200, 800);
    plt::hist(rightChannel, numBins);
    plt::title("Right Channel Histogram");
    plt::show();

    // MID channel histogram
    plt::figure_size(1200, 800);
    plt::hist(midChannel, numBins);
    plt::title("MID Channel (L + R) / 2 Histogram");
    plt::show();

    // SIDE channel histogram
    plt::figure_size(1200, 800);
    plt::hist(sideChannel, numBins);
    plt::title("SIDE Channel (L - R) / 2 Histogram");
    plt::show();

    return 0;
}

int T4(const std::string& filename) {
    sf::SoundBuffer buffer;

    // Load the audio file
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Error loading audio file: " << filename << std::endl;
        return -1;
    }

    // Get audio samples
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    unsigned int sampleRate = buffer.getSampleRate();
    unsigned int channelCount = buffer.getChannelCount();
    unsigned int numBits = 2; // 16 is turning point
    // Prepare vectors for original and quantized samples
    std::vector<double> originalSamples(sampleCount);
    std::vector<double> quantizedSamples(sampleCount);

    int maxVal = (1 << 15) - 1; // Max value for 16-bit signed samples (SFML uses 16-bit audio)

    // Quantization step
    int numLevels = (1 << numBits) - 1; // Number of distinct levels based on numBits

    for (std::size_t i = 0; i < sampleCount; ++i) {
        // Store the original sample
        originalSamples[i] = static_cast<double>(samples[i]);

        //Uniform quantization: scale to [0, numLevels], then map back to [-maxVal, maxVal]
        quantizedSamples[i] = round((originalSamples[i] / maxVal) * numLevels) * (maxVal / numLevels);
    }

    // Time vector for plotting
    std::vector<double> time(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i) {
        time[i] = static_cast<double>(i) / sampleRate;  // Time in seconds
    }

    // Plot the original waveform
    plt::figure_size(1200, 400);
    plt::plot(time, originalSamples);
    plt::title("Original Waveform");
    plt::xlabel("Time (seconds)");
    plt::ylabel("Amplitude");
    plt::grid(true);        
    plt::show();

    // Plot the quantized waveform
    plt::figure_size(1200, 400);
    plt::plot(time, quantizedSamples);
    plt::title("Quantized Waveform with " + std::to_string(numBits) + " Bits");
    plt::xlabel("Time (seconds)");
    plt::ylabel("Amplitude");
    plt::grid(true);
    plt::show();

    return 0;
}

double calculateMSE(const std::vector<double>& original, const std::vector<double>& processed) {
    double mse = 0.0;
    std::size_t count = original.size();
    
    for (std::size_t i = 0; i < count; ++i) {
        mse += std::pow(original[i] - processed[i], 2);
    }
    
    return mse / count; // Return average
}

double calculateSNR(const std::vector<double>& original, const std::vector<double>& processed) {
    double signalPower = 0.0;
    double noisePower = 0.0;
    std::size_t count = original.size();
    
    for (std::size_t i = 0; i < count; ++i) {
        signalPower += std::pow(original[i], 2);
        noisePower += std::pow(original[i] - processed[i], 2);
    }
    
    // Avoid division by zero
    if (noisePower == 0) return std::numeric_limits<double>::infinity(); // Use numeric_limits here

    return 10 * std::log10(signalPower / noisePower); // Return SNR in dB
}

int T5(const std::string& filename, const std::string& filename2) {
    sf::SoundBuffer buffer;

    // Load the audio file
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Error loading audio file: " << filename << std::endl;
        return -1;
    }

    // Get audio samples
    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    unsigned int sampleRate = buffer.getSampleRate();
    unsigned int numBits = 10;
    // Prepare vectors for original and quantized samples
    std::vector<double> originalSamples(sampleCount);
    std::vector<double> quantizedSamples(sampleCount);

    int maxVal = (1 << 15) - 1; // Max value for 16-bit signed samples (SFML uses 16-bit audio)

    // Quantization step
    int numLevels = (1 << numBits) - 1; // Number of distinct levels based on numBits

    for (std::size_t i = 0; i < sampleCount; ++i) {
        // Store the original sample
        originalSamples[i] = static_cast<double>(samples[i]);

        // Uniform quantization: scale to [0, numLevels], then map back to [-maxVal, maxVal]
        quantizedSamples[i] = round((originalSamples[i] / maxVal) * numLevels) * (maxVal / numLevels);
    }

    // Calculate MSE and SNR
    double mse = calculateMSE(originalSamples, quantizedSamples);
    double snr = calculateSNR(originalSamples, quantizedSamples);

    std::cout << "Quantization Level: " << numBits << " bits" << std::endl;
    std::cout << "Mean Squared Error (MSE): " << mse << std::endl;
    std::cout << "Signal-to-Noise Ratio (SNR): " << snr << " dB" << std::endl;

    // Optionally plot the original and quantized waveforms
    // Time vector for plotting
    std::vector<double> time(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i) {
        time[i] = static_cast<double>(i) / sampleRate;  // Time in seconds
    }

    // Plot the original waveform
    plt::figure_size(1200, 400);
    plt::plot(time, originalSamples);
    plt::title("Original Waveform");
    plt::xlabel("Time (seconds)");
    plt::ylabel("Amplitude");
    plt::grid(true);
    plt::show();

    // Plot the quantized waveform
    plt::figure_size(1200, 400);
    plt::plot(time, quantizedSamples);
    plt::title("Quantized Waveform with " + std::to_string(numBits) + " Bits");
    plt::xlabel("Time (seconds)");
    plt::ylabel("Amplitude");
    plt::grid(true);
    plt::show();

    return 0;
}


int main() {
    int choice;
    cout << "Choose a function to run: " << endl;
    cout << "1. T1" << endl;
    cout << "2. T2" << endl;
    cout << "3. T3" << endl;
    cout << "4. T4" << endl;
    cout << "5. T5" << endl;

    std::cin >> choice;

    switch(choice) {
        case 1:
            T1("sample.wav");
            break;
        case 2:
            T2("sample.wav");
            break;
        case 3:
            T3("sample.wav");
            break;
        case 4:
            T4("sample.wav");
            break;
        case 5:
            T5("sample.wav", "sample_2.wav");
            break;
        default:
            cout << "Invalid choice" << endl;
    }
    return 0;
}