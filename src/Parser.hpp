#ifndef PARSER_HPP
#define PARSER_HPP

#include "Population.hpp"
#include <unordered_map>

struct Result 
{
    Individual ind;
    double time;
};

class Parser
{
    private:
        static std::string output;
    public:
        static Population parse(int argc, char* argv[]);
        static void exportResults(const std::vector<std::vector<Result>>& res, const std::vector<double>& averageTimes, const std::unordered_map<int, std::string>& names);
        static void printResults (const std::vector<std::vector<Result>>& res, const std::vector<double>& averageTimes, const std::unordered_map<int, std::string>& names);
};
    

namespace calc
{
    double relativeFitness(double indFit, double graspFit);
    double relativeTime(double indTime, double targetTime);
    std::string formatRelativeTime(double indTime, double targetTime);
    std::string formatRelativeFitness(double indFit, double graspFit);
};

#endif //< PARSER_HPP