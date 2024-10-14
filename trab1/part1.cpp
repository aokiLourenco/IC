#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <cmath>

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

int main() {
    // file stream object
    std::ifstream file("../samples/pt/ep-00-01-17.txt");

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

    // display menu
    std::cout << "Choose what to print:" << std::endl;
    std::cout << "1. Original content" << std::endl;
    std::cout << "2. Transformed content" << std::endl;
    std::cout << "3. Both" << std::endl;
    std::cout << "4. Character frequencies" << std::endl;
    std::cout << "5. Word frequencies" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;

    // handle user's choice
    switch (choice) {
        case 1:
            std::cout << "File content: " << std::endl;
            for (const auto& line : fileContent) {
                std::cout << line << std::endl;
            }
            break;
        case 2:
            std::cout << "Transformed content: " << std::endl;
            for (const auto& line : transformedContent) {
                std::cout << line << std::endl;
            }
            break;
        case 3:
            std::cout << "File content: " << std::endl;
            for (const auto& line : fileContent) {
                std::cout << line << std::endl;
            }
            std::cout << "Transformed content: " << std::endl;
            for (const auto& line : transformedContent) {
                std::cout << line << std::endl;
            }
            break;
        case 4: {
            std::map<char, int> charFrequency = countCharacterFrequencies(transformedContent);
            int totalChars = 0;
            
            std::cout << "Character frequencies: " << std::endl;
            for (const auto& pair : charFrequency) {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }
            
            for (const auto& pair : charFrequency) {
                totalChars += pair.second;
            }
            double entropy = calculateEntropy(charFrequency, totalChars);
            std::cout << "\nEntropy: " << entropy << std::endl;
            
            break;
        }
        case 5: {
            std::map<std::string, int> wordFrequency = countWordFrequencies(transformedContent);
            std::cout << "Word frequencies: " << std::endl;
            for (const auto& pair : wordFrequency) {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }
            break;
        }
        case 6: {
            std::cout << "Exiting..." << std::endl;
            break;
        }
        default:
            std::cerr << "Invalid choice!" << std::endl;
            break;
    }

    return 0;
}