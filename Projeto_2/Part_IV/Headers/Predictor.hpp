#ifndef PREDICTOR_HPP
#define PREDICTOR_HPP

#include <vector>
#include <functional>

using namespace std;

int Predictor_One(int x, int y, int z);

int Predictor_Two(int x, int y, int z);

int Predictor_Three(int x, int y, int z);

int Predictor_Four(int x, int y, int z);

int Predictor_Five(int x, int y, int z);

int Predictor_Six(int x, int y, int z);

int Predictor_Seven(int x, int y, int z);

int LS_Predictor(int x, int y, int z);

vector<function<int(int, int, int)>> GetPredictors();

#endif // PREDICTOR_HPP