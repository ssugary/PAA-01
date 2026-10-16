#include "Parser.hpp"
#include <cmath>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

std::string Parser::output;


Population Parser::parse(int argc, char* argv[])
{
    std::string filename;
    if(argc < 2)
        filename = "tests/Instance1/intance1_100.txt";
    else  
        filename = std::string(argv[1]);

    if(argc < 3)
        output = "teste.txt";
    else  
        output = std::string(argv[2]);

    std::ifstream ifs{filename};
    if(!ifs.is_open()) 
        throw std::runtime_error("Error reading the file: " + filename);

    std::string line;
    if(!getline(ifs, line)) 
        throw std::runtime_error("Error: Empty file: " + filename);
    
    
    std::stringstream ss{line};
    std::string tmax;
    ss >> tmax;

    std::vector<std::tuple<double, double, int>> vertices;
    

    while(getline(ifs, line))
    {
        std::stringstream ssl{ line };

        std::string diff, profit, type;
        if(ssl >> diff >> profit >> type) 
            vertices.push_back(std::tuple(std::stod(diff), std::stod(profit), std::stoi(type)));
        
    }

    if(vertices.size() > 1) 
        std::swap(vertices[1], vertices.back());
    
    std::size_t N = vertices.size();

    std::vector<double> profits(N);
    std::vector<int> types(N);
    std::vector<std::vector<double>> dist(N, std::vector<double>(N, 0.0));

    for(std::size_t i{0}; i < N; ++i) 
    {
        profits[i] = std::get<1>(vertices[i]);
        types[i] = std::get<2>(vertices[i]);
    }

    for(std::size_t i{0}; i < N; ++i) 
        for(std::size_t j{0}; j < N; ++j) 
        {
            if (i == j) 
            {
                dist[i][j] = 0.;
                continue;
            }

            double baseDiff = std::get<0>(vertices[j]);
            double diffJump = std::abs(std::get<0>(vertices[j]) - std::get<0>(vertices[i]));
            
            double flowPen = 0.0;
            int typeA = std::get<2>(vertices[i]);
            int typeB = std::get<2>(vertices[j]);

            if(typeA == typeB && (typeA == 0 || typeA == 1 || typeA == 6)) flowPen = 100.0; 
            if(typeA == 0 && typeB == 1) flowPen = 500.0; 
            if(typeA == 0 && typeB == 3) flowPen = 150.0; 
            if(typeA == 3 && typeB == 5) flowPen = -10.0; 

            double cost = baseDiff + (LAMBDA * diffJump) + flowPen;
            dist[i][j] = std::max(0.1, cost);
        }
    

    return Population(dist, profits, types, std::stod(tmax), 0, N - 1);
}

