#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include "matplotlibcpp.h" 
#include <cmath>

namespace plt = matplotlibcpp;
using namespace std;

// convert a string to lowercase
std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// remove punctuation from a string
std::string removePunctuation(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (!std::ispunct(c)) {
            result += c;
        }
    }
    return result;
}

// counting character frequency
std::map<char, int> countCharacterFrequencies(const std::vector<std::string>& content) {
    std::map<char, int> charFrequency;
    for (const auto& line : content) {
        for (char c : line) {
            if (!std::isspace(c)) { // ignore whitespace
                charFrequency[c]++;
            }
        }
    }

    // Prepare data for plotting
    std::vector<char> characters;
    std::vector<int> frequencies;
    for (const auto& pair : charFrequency) {
        characters.push_back(pair.first);
        frequencies.push_back(pair.second);
    }

    // Plotting
    plt::figure_size(1200, 800);
    plt::plot(characters, frequencies);
    plt::title("Character Frequency");
    plt::xlabel("Characters");
    plt::ylabel("Frequency");
    plt::grid(true);
    plt::show();

    return charFrequency;
}

// counting word frequency
std::map<std::string, int> countWordFrequencies(const std::vector<std::string>& content) {
    std::map<std::string, int> wordFrequency;
    for (const auto& line : content) {
        std::istringstream stream(line);
        std::string word;
        while (stream >> word) {
            wordFrequency[word]++;
        }
    }

    // Prepare data for plotting
    std::vector<int> indices;
    std::vector<int> frequencies;
    int index = 0;
    for (const auto& pair : wordFrequency) {
        indices.push_back(index++);
        frequencies.push_back(pair.second);
    }

    // Plotting
    plt::figure_size(1200, 800);
    plt::plot(indices, frequencies);
    plt::title("Word Frequency");
    plt::xlabel("Word Index");
    plt::ylabel("Frequency");
    plt::grid(true);    
    plt::show();

    return wordFrequency;
}

// calculate entropy
// ( H = -\sum p(x) \log_2 p(x) ), ( p(x) ) is the probability of character ( x ).
double calculateEntropy(const std::map<char, int>& charFrequency, int totalCharacters) {
    double entropy = 0.0;
    for (const auto& pair : charFrequency) {
        double probability = static_cast<double>(pair.second) / totalCharacters; // static_cast<double> ensures that the division is done in floating-point arithmetic rather than integer arithmetic.
        entropy -= probability * std::log2(probability);
    }
    return entropy;
}

int main(int argc, char* argv[]) {
    // Check if the file name is provided
    if (argc < 2) {
        std::cerr << "Error: No file name provided!" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <file_name>" << std::endl;
        return 1;
    }

    // Construct the file path
    std::string fileName = argv[1];
    std::string filePath = "./data/" + fileName + ".txt";

    // file stream object
    std::ifstream file(filePath);

    // check if file opened
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return 1;
    }

    // containers to hold the file's content and transformed content
    std::vector<std::string> fileContent;
    std::vector<std::string> transformedContent;
    std::string line;

    // read the file line by line
    while (std::getline(file, line)) {
        fileContent.push_back(line);
    }

    // close the file
    file.close();

    // apply transformations
    for (const auto& line : fileContent) {
        std::string lowerCaseLine = toLowerCase(line);
        std::string transformedLine = removePunctuation(lowerCaseLine);
        transformedContent.push_back(transformedLine);
    }

    // Print original content
    std::cout << "File content (T1): " << std::endl;
    for (const auto& line : fileContent) {
        std::cout << line << std::endl;
    }
    std::cout << "-------------------------------" << std::endl;
    // Print transformed content
    std::cout << "Transformed content: " << std::endl;
    for (const auto& line : transformedContent) {
        std::cout << line << std::endl;
    }
    std::cout << "-------------------------------" << std::endl;
    // Character frequencies
    std::map<char, int> charFrequency = countCharacterFrequencies(transformedContent);
    int totalChars = 0;
    
    std::cout << "Character frequencies (T3): " << std::endl;
    for (const auto& pair : charFrequency) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    std::cout << "-------------------------------" << std::endl;
    for (const auto& pair : charFrequency) {
        totalChars += pair.second;
    }
    std::cout << "-------------------------------" << std::endl;
    double entropy = calculateEntropy(charFrequency, totalChars);
    std::cout << "\nEntropy: " << entropy << std::endl;

    // Word frequencies
    std::map<std::string, int> wordFrequency = countWordFrequencies(transformedContent);
    std::cout << "Word frequencies (T4): " << std::endl;
    for (const auto& pair : wordFrequency) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    std::cout << "-------------------------------" << std::endl;

    return 0;
}