///
/// This code is modified from source code developed by David Gasquez (https://github.com/davidgasquez/qap)
///

#ifndef MEMETIC_H
#define MEMETIC_H

#include "input.h"
#include "common.h"
#include "localSearch.h"

using namespace std;
using namespace std::chrono;

// Struct Chromosome
typedef struct Chromosome
{
    vector<int> solution;
    float cost;
    bool operator<(const Chromosome &n) const
    {
        return cost < n.cost;
    }
} Chromosome;


class Memetic
{
public:
    Memetic(const Input &input, int maxEvaluations,  int populationSize, double probabilityLS, bool initRandom = true);

    // Execute algorythm
    void execute();

    // Simple swap
    void swap(vector<int> &solution, int r, int s);

    // Random solution
    vector<int> randomSolution();

    // Calculate cost using objetive function
    float calculateCost(vector<int> &solution);

    // Initialize population on oldPopulation
    void createPopulation();

    // Reproduces the population and adds the children to it
    void breed(vector <Chromosome> &oldPopulation);

    // Cross 2 chromosomes based on position cross operator
    Chromosome cross(Chromosome father, Chromosome mother);

    // Mutate the population based on expected values from mutation probability
    void mutate(vector <Chromosome> &oldPopulation);

    // Evaluate the population with objetive function
    void evaluate(vector <Chromosome> &oldPopulation);

    // Getters
    vector<int> getSolution();
    float getCost();
    float getCostWithOriginalOrder();
    double getTime();

private:
    // Input data
    int dimension_;
    vector< vector<float> > distances_;
    vector< vector<float> > flow_;

    // Old and new population
    vector<Chromosome> oldPopulation_;
    vector<Chromosome> newPopulation_;

    // Vector wich contains true if we must to compute the cost of the element i
    vector<bool> touched_;

    // Evaluations of the objetive function
    int evaluations_;

    // Parameters
    int maxEvaluations_;
    int populationSize_;
    int crossovers_;
    double probabilityLS_;
    bool onlyBest_;

    // Solution data
    vector<int> solution_;
    float cost_;
    float costWithOriginalOrder;
    double time_;
};

#endif // MEMETIC_H
