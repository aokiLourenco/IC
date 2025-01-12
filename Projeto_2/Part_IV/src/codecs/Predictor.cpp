#include <opencv2/opencv.hpp>
#include <vector>
#include <stdexcept>
#include <map>
#include <functional>
#include <iostream>
#include "../headers/Golomb.hpp"
#include "../headers/Predictor.hpp"

int Predictor_One(int x, int y, int z)
{
    return x;
}

int Predictor_Two(int x, int y, int z)
{
    return y;
}

int Predictor_Three(int x, int y, int z)
{
    return z;
}

int Predictor_Four(int x, int y, int z)
{
    return x + y - z;
}

int Predictor_Five(int x, int y, int z)
{
    return x + (y - z) / 2;
}

int Predictor_Six(int x, int y, int z)
{
    return y + (x - z) / 2;
}

int Predictor_Seven(int x, int y, int z)
{
    return (x + y) / 2;
}

int LS_Predictor(int x, int y, int z)
{
    int prediction = x + y - z;

    int minimum = min(x, y);
    int maximum = max(x, y);

    if (z >= maximum)
    {
        prediction = minimum;
    }
    if (z <= minimum)
        prediction = maximum;

    return prediction;
}

vector<function<int(int, int, int)>> GetPredictors()
{
    vector<function<int(int, int, int)>> predictorFunctions;

    predictorFunctions.push_back(Predictor_One);
    predictorFunctions.push_back(Predictor_Two);
    predictorFunctions.push_back(Predictor_Three);
    predictorFunctions.push_back(Predictor_Four);
    predictorFunctions.push_back(Predictor_Five);
    predictorFunctions.push_back(Predictor_Six);
    predictorFunctions.push_back(Predictor_Seven);
    predictorFunctions.push_back(LS_Predictor);

    return predictorFunctions;
}