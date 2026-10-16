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
    
    
    public:
        static std::string output;
        static Population parse(int argc, char* argv[]);
};
    


#endif //< PARSER_HPP