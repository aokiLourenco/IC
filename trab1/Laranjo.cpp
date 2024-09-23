#include <iostream>
#include <fstream>
#include <cctype>
#include <map>
using namespace std;

// Function to read and print the contents of "t1.txt"
int T1() {
    string line;
    std::ifstream file("t1.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            cout << line << '\n';
        }
        file.close();
    } else {
        cout << "Unable to open file";
    }
    return 0;
}

// Function to read "t1.txt" and perform operations based on the option provided
int T2(int Option) {
    string line;
    std::ifstream file("t1.txt");
    if (file.is_open()) {
        if(Option == 1) {
            // Convert all characters to lowercase and print
            while (getline(file, line)) {
                string newline;
                for (char c : line) {
                    newline += tolower(static_cast<unsigned char>(c)); 
                }
                cout << newline << '\n'; 
            }
            file.close();
        } else if(Option == 2) {
            // Remove all punctuation and print
            while (getline(file, line)) {
                string newline;
                for (char c : line) {
                    if (!ispunct(c)) {
                        newline += c;
                    }
                }
                cout << newline << '\n'; 
            }
            file.close();
        } else {
            cout << "Invalid option" << endl;
        }
    } else {
        cout << "Unable to open file";
    }
    return 0;
}

// Function to read "t1.txt" and count the frequency of each character
int T3() {
    string line;
    std::map <char, int> freq;
    std::ifstream file("t1.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            for (char &c : line) {
                freq[c]++;
            }
        }
        file.close();
    } else {
        cout << "Unable to open file";
    }
    cout << "-----------------------------------";
    for (auto &c : freq) {
        cout << c.first << " " << c.second << endl;
    }
    cout << "-----------------------------------";
    return 0;
}

/* Although not mandatory, you can use a plotting library to visualize the frequencies.

    IDK how to do this in C++ but I can do it in Python. surely
*/

// Function to read "t1.txt" and count the frequency of each word
int T4() {
    string line;
    std::map <string, int> freq;
    std::ifstream file("t1.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            string word;
            for (char &c : line) {
                if (c == ' ') {
                    freq[word]++;
                    word = "";
                } else {
                    word += c;
                }
            }
        }
        file.close();
    } else {
        cout << "Unable to open file";
    }
    cout << "-----------------------------------";
    for (auto &c : freq) {
        cout << c.first << ": " << c.second << endl;
    }
    cout << "-----------------------------------";
    return 0;
}

// Main function to choose and run one of the above functions
int main() {

    int choice;
    cout << "Choose a function to run: " << endl;
    cout << "1. T1" << endl;
    cout << "2. T2" << endl;
    cout << "3. T3" << endl;
    cout << "4. T4" << endl;

    std::cin >> choice;

    switch(choice) {
        case 1:
            T1();
            break;
        case 2:
            int option; 
            cout << "Choose an option: " << endl;
            cout << "1. Convert to lowercase" << endl;
            cout << "2. Remove punctuation" << endl;
            std::cin >> option;
            T2(option);
            break;
        case 3:
            T3();
            break;
        case 4:
            T4();
            break;
        default:
            cout << "Invalid choice" << endl;
    }
    return 0;
}