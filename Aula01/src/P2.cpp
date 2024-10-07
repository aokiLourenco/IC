#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <sndfile.h>
#include <matplotlibcpp.h>
namespace plt = matplotlibcpp;

void histogram(std::vector<double> data, int i)
{
    plt::hist(data, 2^i);
    plt::show();
}

int main(int argc, char const *argv[])
{
    std::string filename = "../sample.wav";

    // *  Open the file
    SF_INFO sfinfo;
    SNDFILE *infile = sf_open(filename.c_str(), SFM_READ, &sfinfo);

    if (!infile)
    {
        std::cerr << "Error : Not able to open input file" << std::endl;
        return 1;
    }

    // *  Print some of the info
    std::cout << "Sample rate : " << sfinfo.samplerate << std::endl;
    std::cout << "Channels    : " << sfinfo.channels << std::endl;
    
    // *  Read the data
    std::vector<double> data(sfinfo.frames * sfinfo.channels);
    sf_read_double(infile, data.data(), data.size());
    printf("Data size: %d\n", data.size());

    // * Devide into channels
    std::vector<double> leftChannel(data.size() / 2);
    std::vector<double> rightChannel(data.size() / 2);
    std::vector<double> midChannel(data.size() / 2);
    std::vector<double> sideChannel(data.size() / 2);

    //* Split stereo samples into left and right channels, and calculate MID and SIDE channels
    for (std::size_t i = 0; i < data.size() / 2; ++i)
    {
        leftChannel[i] = static_cast<double>(data[2*i]);      // Left channel sample
        rightChannel[i] = static_cast<double>(data[2*i +1]); // Right channel sample
        midChannel[i] = (leftChannel[i] + rightChannel[i]) / 2;    // MID channel (L + R) / 2
        sideChannel[i] = (leftChannel[i] - rightChannel[i]) / 2;   // SIDE channel (L - R) / 2
    }


    histogram(leftChannel, 100);
    histogram(rightChannel,100);
    histogram(midChannel,100);
    histogram(sideChannel,100);
    

    // * Create histogram
    histogram(data,100);

    // *  Close the file
    sf_close(infile);

    return 0;
}