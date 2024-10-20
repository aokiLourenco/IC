#include <iostream>
#include <opencv2/opencv.hpp>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

void T1(const std::string& imagePath) {
    // Load an image from a file
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
    
    // Check if the image was loaded successfully
    if (image.empty()) {
        std::cerr << "Error: Could not open or find the image." << std::endl;
        return;
    }

    // Display the image
    cv::imshow("Display window", image);

    // Wait for a keystroke in the window
    cv::waitKey(0);
    cv::destroyAllWindows();
}

void T2(const std::string& imagePath) {
    // Load the image from the file
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);

    // Check if the image was loaded successfully
    if (image.empty()) {
        std::cerr << "Error: Could not open or find the image." << std::endl;
        return;
    }

    // Split the image into its three channels: Blue, Green, and Red
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    cv::Mat blueChannel = channels[0];
    cv::Mat greenChannel = channels[1];
    cv::Mat redChannel = channels[2];

    // Create images to display each channel in color
    cv::Mat blueImage, greenImage, redImage;
    std::vector<cv::Mat> blueChannels = {blueChannel, cv::Mat::zeros(blueChannel.size(), CV_8UC1), cv::Mat::zeros(blueChannel.size(), CV_8UC1)};
    std::vector<cv::Mat> greenChannels = {cv::Mat::zeros(greenChannel.size(), CV_8UC1), greenChannel, cv::Mat::zeros(greenChannel.size(), CV_8UC1)};
    std::vector<cv::Mat> redChannels = {cv::Mat::zeros(redChannel.size(), CV_8UC1), cv::Mat::zeros(redChannel.size(), CV_8UC1), redChannel};
    cv::merge(blueChannels, blueImage);
    cv::merge(greenChannels, greenImage);
    cv::merge(redChannels, redImage);

    // Convert the image from RGB to grayscale
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Display the original image and its channels
    cv::imshow("Original Image", image);
    cv::imshow("Blue Channel", blueImage);
    cv::imshow("Green Channel", greenImage);
    cv::imshow("Red Channel", redImage);
    cv::imshow("Grayscale Image", grayImage);

    // Wait for a key press and close all windows
    cv::waitKey(0);
    cv::destroyAllWindows();
}

void T3(const std::string& imagePath) {
    // Load the image from the file
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);

    // Check if the image was loaded successfully
    if (image.empty()) {
        std::cerr << "Error: Could not open or find the image." << std::endl;
        return;
    }    
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    cv::imshow("Grayscale Image", grayImage);

    // Calculate the histogram
    cv::Mat hist;
    int histSize = 256; // Number of bins
    float range[] = {0, 256}; // Range of pixel values
    const float* histRange = {range};
    cv::calcHist(&grayImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    // Convert histogram to a vector for matplotlibcpp
    std::vector<float> histData(histSize);
    for (int i = 0; i < histSize; ++i) {
        histData[i] = hist.at<float>(i);
    }

    // Plot the histogram using matplotlibcpp
    plt::figure_size(800, 600);
    plt::bar(histData);
    plt::title("Grayscale Histogram");
    plt::xlabel("Pixel Intensity");
    plt::ylabel("Frequency");
    plt::show();


    cv::destroyAllWindows();
}

void T4(const std::string& imagePath) {
    // Load the image from the file
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);

    // Check if the image was loaded successfully
    if (image.empty()) {
        std::cerr << "Error: Could not open or find the image." << std::endl;
        return;
    }

    // Apply Gaussian blur filter with different kernel sizes
    cv::Mat blurredImage1, blurredImage2, blurredImage3;
    cv::GaussianBlur(image, blurredImage1, cv::Size(5, 5), 0);
    cv::GaussianBlur(image, blurredImage2, cv::Size(9, 9), 0);
    cv::GaussianBlur(image, blurredImage3, cv::Size(15, 15), 0);

    // Display the original and blurred images
    cv::imshow("Original Image", image);
    cv::imshow("Blurred Image (Kernel Size: 5x5)", blurredImage1);
    cv::imshow("Blurred Image (Kernel Size: 9x9)", blurredImage2);
    cv::imshow("Blurred Image (Kernel Size: 15x15)", blurredImage3);

    // Wait for a key press and close all windows
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// Calculating the difference between two images.
// - Implement a function to calculate the absolute difference between two images.
// - Compute and display the Mean Squared Error (MSE) and Peak Signal-to-Noise Ratio (PSNR) 
// between the two images.
// - Display the difference image to visually observe where the differences are.
void T5(const std::string& imagePath1, const std::string& imagePath2) {
    // Load the images from the files
    cv::Mat image1 = cv::imread(imagePath1, cv::IMREAD_COLOR);
    cv::Mat image2 = cv::imread(imagePath2, cv::IMREAD_COLOR);

    // Check if the images were loaded successfully
    if (image1.empty() || image2.empty()) {
        std::cerr << "Error: Could not open or find the images." << std::endl;
        return;
    }

    // Check if the images have the same dimensions
    if (image1.size() != image2.size()) {
        std::cerr << "Error: Images must have the same dimensions." << std::endl;
        return;
    }

    // Calculate the absolute difference between the two images
    cv::Mat diffImage;
    cv::absdiff(image1, image2, diffImage);

    // Calculate the Mean Squared Error (MSE)
    cv::Mat squaredError;
    cv::pow(diffImage, 2, squaredError);
    double mse = cv::mean(squaredError)[0];

    // Calculate the Peak Signal-to-Noise Ratio (PSNR)
    double psnr = 10.0 * std::log10((255 * 255) / mse);

    // Display the difference image
    cv::imshow("Absolute Diff", diffImage);
    std::cout << "Mean Squared Error (MSE): " << mse << std::endl;
    std::cout << "Peak Signal-to-Noise Ratio (PSNR): " << psnr << " dB" << std::endl;
    
    // Wait for a key press and close all windows
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// - Image quantization. To reduce the number of bits used to represent each image (i.e., to perform 
// uniform scalar quantization).
// - Implement the quantization function on grayscale images.
// - Experiment with different numbers of quantization levels.
// - Compare the original image with the quantized one using MSE and PSNR to evaluate the qualit
void T6 (const std::string& imagePath, int numBits) {
    // Load the image from the file
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);

    // Check if the image was loaded successfully
    if (image.empty()) {
        std::cerr << "Error: Could not open or find the image." << std::endl;
        return;
    }

    // Prepare vectors for original and quantized samples
    std::vector<double> originalSamples;
    std::vector<double> quantizedSamples;

    // Calculate the maximum value for the given number of bits
    int maxVal = (1 << numBits) - 1;

    // Quantization step
    int numLevels = (1 << numBits) - 1;

    // Perform uniform quantization on the image
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            // Store the original sample
            originalSamples.push_back(static_cast<double>(image.at<uchar>(i, j)));

            // Uniform quantization: scale to [0, numLevels], then map back to [0, 255]
            quantizedSamples.push_back(round((image.at<uchar>(i, j) / 255.0) * numLevels) * (255.0 / numLevels));
        }
    }

    // Calculate the Mean Squared Error (MSE) and Peak Signal-to-Noise Ratio (PSNR)
    double mse = 0.0;
    double signalPower = 0.0;
    double noisePower = 0.0;

    for (std::size_t i = 0; i < originalSamples.size(); ++i) {
        mse += std::pow(originalSamples[i] - quantizedSamples[i], 2);
        signalPower += std::pow(originalSamples[i], 2);
        noisePower += std::pow(originalSamples[i] - quantizedSamples[i], 2);
    }

    mse /= originalSamples.size();
    double psnr = 10 * std::log10(signalPower / noisePower);

    std::cout << "Quantization Level: " << numBits << " bits" << std::endl;
    std::cout << "Mean Squared Error (MSE): " << mse << std::endl;
    std::cout << "Peak Signal-to-Noise Ratio (PSNR): " << psnr << " dB" << std::endl;
}

int main() {
    std::string image = "To.png"; // Replace with your image path
    std::string image2 = "kaitou.png";
    // T1(image);
    // T2(image);
    // T3(image);
    // T4(image);
    // T5(image, image2);
    T6 (image, 5);
    return 0;
}