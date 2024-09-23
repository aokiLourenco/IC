#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main() {
    // Declare a file stream object
    std::ifstream file("ep-01-01-15.txt");

    // Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return 1;
    }

    // Declare a container to hold the file's content
    std::vector<std::string> fileContent;
    std::string line;

    // Read the file line by line
    while (std::getline(file, line)) {
        fileContent.push_back(line);
    }

    // Close the file
    file.close();

    // Print the content to check if it's read correctly
    std::cout << "File content: " << std::endl;
    for (const auto& line : fileContent) {
        std::cout << line << std::endl;
    }

    return 0;
}
