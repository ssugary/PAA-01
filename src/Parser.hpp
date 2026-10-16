#ifndef PARSER_HPP
#define PARSER_HPP

#include "Population.hpp"
#include <unordered_map>

struct Result 
{
    Individual ind;
    double time;
};

const double LAMBDA{3.};

class Parser
{
    private:
        
        static std::string output;

    public:
        static Population parse(int argc, char* argv[]);
};
    


#endif //< PARSER_HPP