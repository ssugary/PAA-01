#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <functional>
#include <format>

#include "Population.hpp"
#include "Parser.hpp"

const int FUNCTIONS_QNT = 8;
const int POPULATION_QNT = 100;

std::unordered_map<int, std::string> m = 
{
    {0, "Grasp                               "},
    {1, "Grasp + Ruin + Recreate             "},
    {2, "Grasp + 2-OPT-BIBI + Ruin + Recreate"},
    {4, "Grasp + 4-OPT-BIBI + Ruin + Recreate"},
    {3, "Grasp + 3-OPT-BIBI + Ruin + Recreate"},
    {5, "Grasp + 4-OPT-FIFI + Ruin + Recreate"},
    {6, "Grasp + 4-OPT-FIBI + Ruin + Recreate"},
    {7, "Grasp + 4-OPT-BIFI + Ruin + Recreate"},
};


std::vector<std::vector<Result>> runTests(Population* pop, int qnt, std::vector<double>* averageTimes)
{

    std::vector<std::vector<Result>> results(FUNCTIONS_QNT);
    averageTimes->assign(FUNCTIONS_QNT, 0.);

    std::unordered_map<int, std::function<Individual(const Individual&)>> funcs
    {
        {0, [](const Individual& ind) 
        { 
            return ind; 
        }},
        {1, [pop](const Individual& ind)
        { 
            return pop->recreate(pop->ruin(ind)); 
        }},
        {2, [pop](const Individual& ind) 
        { 
            return pop->opt2(pop->recreate(pop->ruin(pop->opt2(ind)))); 
        }},
        {3, [pop](const Individual& ind) 
        { 
            return pop->opt3(pop->recreate(pop->ruin(pop->opt3(ind)))); 
        }},
        {4, [pop](const Individual& ind) 
        { 
            return pop->opt4BI(pop->recreate(pop->ruin(pop->opt4BI(ind)))); 
        }},
        {5, [pop](const Individual& ind) 
        { 
            return pop->opt4FI(pop->recreate(pop->ruin(pop->opt4FI(ind)))); 
        }},
        {6, [pop](const Individual& ind) 
        { 
            return pop->opt4BI(pop->recreate(pop->ruin(pop->opt4FI(ind)))); 
        }},
        {7, [pop](const Individual& ind) 
        { 
            return pop->opt4FI(pop->recreate(pop->ruin(pop->opt4BI(ind)))); 
        }}
    };

    for(int i{0}; i < qnt; ++i)
    {
        Individual ind = pop->generateGrasp();
        
        for(int j{0}; j < FUNCTIONS_QNT; ++j)
        {
            std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
            Individual res = funcs[j](ind);
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            
            double indTime = std::chrono::duration<double, std::milli>(end - begin).count();

            results[j].push_back({res, indTime});
            (*averageTimes)[j] += indTime;
        }

        
    }

    for(auto& v : results)
        std::sort(v.begin(), v.end(),[](const Result& a, const Result& b) -> bool {return a.ind.fitness < b.ind.fitness;});
    

    for(auto& tt : (*averageTimes))
        tt /= qnt;

    return results;
}


signed main(int argc, char* argv[])
{
    
    Population gaPop = Parser::parse(argc, argv);
        
    std::vector<double> averageTimes;

    auto results = runTests(&gaPop, POPULATION_QNT, &averageTimes);

    Parser::printResults (results, averageTimes, m);
    Parser::exportResults(results, averageTimes, m);

    return 0;
}