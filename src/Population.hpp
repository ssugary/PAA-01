#ifndef POPULATION_HPP
#define POPULATION_HPP

#include <vector>
#include <random>

struct Individual 
{
    std::vector<int> path;
    double score;
    double dist;
    double fitness;
    int vis;
};

class Population 
{
    private:
 
        std::size_t cityQnt;
        
        const int startNode{};
        const std::vector<std::vector<double>> paths{};
        const std::vector<double> profits{};
        std::mt19937 rng{std::random_device{}()};
        
        void calcScore(Individual& ind) const;   
        
    public:
        
        const double TMax{};
        const int endNode{};

        Population(std::vector<std::vector<double>>, std::vector<double>, std::vector<int>, double, int, int);

        /*Heuristicas de Melhoria*/
        Individual opt2(const Individual&);
        Individual ruin(const Individual&);
        Individual recreate(const Individual&);
        
        /*Heuristicas Construtivas*/
        Individual generateGrasp();

        std::vector<Individual> pop;
        const std::vector<int> types{};
        
};


#endif //< POPULATION_HPP