///
/// This code is modified from source code developed by David Gasquez (https://github.com/davidgasquez/qap)
///

#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "input.h"
#include "common.h"

using namespace std;
using namespace std::chrono;

class LocalSearch
{
public:
    LocalSearch(const Input &input);
    LocalSearch(const Input& input, bool noExecute);

    void execute();

    // Simple swap
    void swap(vector<int> &solution, int r, int s);
    // Random initial solution
    void randomStartSolution();
    // Factorization
    float moveCost(vector<int> &oldSolution, int r, int s);
    // Calculate cost using objetive function
    float calculateCost(vector<int> &solution);

    void setSolution(vector<int> solution);

    vector<int> getSolution();
    float getCost();
    double getTime();

private:
    // Input data
    int dimension_;
    vector< vector<float> > distances_;
    vector< vector<float> > flow_;

    // Don't look bits mask
    vector<bool> dlb_;

    // Solution data
    vector<int> solution_;
    float cost_;
    double time_;
};

#endif // LOCALSEARCH_H
