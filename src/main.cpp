#include <algorithm>
#include <chrono>
#include <iostream>
#include "Population.hpp"
#include "Parser.hpp"
#include "DungeonRasterizer.hpp" 

int main(int argc, char* argv[])
{
    Population gaPop = Parser::parse(argc, argv);

    std::vector<Individual> graspPop;
    std::vector<Individual> graspRRPop;

    for(int i = 0; i < 30; ++i) 
        graspPop.push_back(gaPop.generateGrasp());

    
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 30; ++i) 
        gaPop.pop.push_back(gaPop.opt2(gaPop.recreate(gaPop.ruin(gaPop.opt2(gaPop.generateGrasp())))));
    auto end = std::chrono::high_resolution_clock::now();

    // double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

    // double avgIterTimeMs = timeMs / 30.;

    // std::sort(graspPop.begin(), graspPop.end(), [](const Individual& a, const Individual& b) {
    //     return a.fitness < b.fitness;
    // });
    // std::sort(graspRRPop.begin(), graspRRPop.end(), [](const Individual& a, const Individual& b) {
    //     return a.fitness < b.fitness;
    // });

    // auto grasp = graspPop[graspPop.size() / 2];
    // auto median = graspRRPop[graspRRPop.size() / 2];
    // double gainPercent = ((median.fitness - grasp.fitness) / (grasp.fitness <= 0 ? 1.0 : grasp.fitness)) * 100.0;

    // std::ofstream ofs{"temp.txt"};

    // ofs << gaPop.TMax << ' ' << grasp.fitness << ' ' << median.fitness << ' ' << gainPercent << "% " << avgIterTimeMs << "ms\n" ;
    // ofs.close();

    DungeonRasterizer rasterizer(35, 35);
    
    rasterizer.rasterizeGoldenPath(gaPop.pop.back().path, gaPop.types, gaPop.endNode);
    rasterizer.attachSideRooms(gaPop.types, gaPop.types.size());
    
    rasterizer.exportJSON("dungeon_map.json");
    rasterizer.exportHTML("dungeon.html");



    return 0;
}