#include <iostream>
#include <fstream>


string txtFile;

// Read
ifstream MyReadFile("filename.txt");

while (getline (MyReadFile, txtFile)) {
	
	ofstream MyFile("writeFile.txt");
	MyFile << txtFile		
	cout << txtFile;
}

close MyReadFile;
