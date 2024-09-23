#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>

std::vector<std::string> split(std::string &s, char delim)
{

	// * Initialize the vector
	std::vector<std::string> result;

	// * Transform the string into a stream
	std::stringstream ss(s);

	// * Get the item from the stream
	std::string item;

	while (std::getline(ss, item, delim))
	{
		result.push_back(item);
	}

	return result;
}

std::map<std::string, int> count_words(std::string &line, std::map<std::string, int> result)
{

	// * Split the line into words

	std::vector<std::string> words = split(line, ' ');

	// * Count the words

	for (std::string &w : words)
	{
		result[w]++;
	}

	return result;
}

std::map<char, int> count_characters(std::string &s, std::map<char, int> result)
{

	// * Count the char, add to the map

	for (char &c : s)
	{
		result[c]++;
	}

	return result;
}

std::string normalize(std::string &s)
{
	std::string result;

	// * Convert to lower case and remove pontuation

	for (char &c : s)
	{
		if (std::ispunct(c))
		{
			continue;
		}
		result += std::tolower(c);
	}

	return result;
}

std::vector<std::string> read_file(std::ifstream &file)
{

	// * Read line by line of the file
	std::string line;

	// * Store the lines in a vector as strings
	std::vector<std::string> lines;
	while (std::getline(file, line))
	{
		lines.push_back(normalize(line));
	}

	return lines;
}

int main()
{

	// * Open the file
	std::ifstream file("../file.txt");

	// * Check if the file is open or exists
	if (!file.is_open())
	{
		std::cout << "Error opening file" << std::endl;
		return 1;
	}

	// * Read File
	std::vector<std::string> lines = read_file(file);
	int option = -1;

	while (option != 4)
	{
		std::cout << "-----------------------------------" << std::endl;

		std::cout << "Chose your option: \n";
		std::cout << "1 : Print File: \n";
		std::cout << "2 : Count Characters: \n";
		std::cout << "3 : Count Words: \n";
		std::cout << "4 : Exit: \n";
		std::cout << "Option: ";
		std::cin >> option;

		// * Count the characters appearence
		//* Init the map

		std::map<char, int> count_map;

		// * Count the characters

		for (std::string &l : lines)
		{
			count_map = count_characters(l, count_map);
		}

		// * Count the words appearence

		//* Init the map

		std::map<std::string, int> count_map_words;

		for (std::string &l : lines)
		{
			count_map_words = count_words(l, count_map_words);
		}

		switch (option)
		{
		case 1:
			// * Print the file
			for (auto &l : lines)
			{
				std::cout << l << std::endl;
			}
			break;
		case 2:

			// * Count the Characters
			for (auto &p : count_map)
			{
				std::cout << p.first << " : " << p.second << std::endl;
			}

			break;
		case 3:
			// * Count the words
			for (auto &p : count_map_words)
			{
				std::cout << p.first << " : " << p.second << std::endl;
			}
			break;
		case 4:
			// * Exit
			break;
		default:
			std::cout << "Invalid option" << std::endl;
			break;
		}
	}

	return 0;
}
