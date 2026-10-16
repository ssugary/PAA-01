#include "Population.hpp"

#include <algorithm>
#include <cstdint>


Individual Population::generateGrasp()
{
    Individual ind;
    ind.path.assign(cityQnt, SIZE_MAX); 
    ind.path[0] = startNode;

    std::vector<bool> visited(cityQnt, false);
    visited[startNode] = true;
    visited[endNode] = true;

    int currentNode = startNode;
    double currentDist{0.};
    std::size_t idx{1};

    while(idx < cityQnt) 
    {
        std::vector<std::pair<double, std::size_t>> candidates; 

        for(std::size_t i{0}; i < cityQnt; ++i) 
        {
            if(!visited[i]) 
            {
                int roomType = types[i];

                if (roomType == 4 || roomType == 6 || roomType == 7) 
                    continue; 
                

                double distToNext = (paths)[currentNode][i];
                double distToEnd = (paths)[i][endNode];

                if(currentDist + distToNext + distToEnd <= TMax) 
                {
                    double distSafe = (distToNext <= 1e-8) ? 1e-4 : distToNext; 
                    double ratio = (profits)[i] / distSafe; 
                    candidates.push_back({ratio, i});
                }
            }
        }

        if(candidates.empty()) 
            break; 

        std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {return a.first > b.first;});

        std::size_t rcl_size = std::min(std::size_t(3), candidates.size());
        
        std::uniform_int_distribution<std::size_t> dist_rand(0, rcl_size - 1);

        std::size_t chosenIdx = dist_rand(rng);
        int chosenCity = candidates[chosenIdx].second;

        ind.path[idx] = chosenCity;
        currentDist += (paths)[currentNode][chosenCity];
        currentNode = chosenCity;
        visited[chosenCity] = true;
        idx++;
    }

    calcScore(ind); 
    return ind;
}

Individual Population::opt2(const Individual& ind)
{
    Individual best = ind;
    std::vector<std::size_t> path;
    
    for(std::size_t n : best.path) 
        if(n != SIZE_MAX) 
            path.push_back(n);
    
    
    if(path.size() < 3) 
        return best;

    bool found{true};

    while(found)
    {
        found = false;
        std::size_t n = path.size();
        int bestI{-1}, bestJ{-1};
        double bestDelta{-1e-8};

        for(std::size_t i{1}; i < n - 1; ++i)
        {
            std::size_t A = path[i - 1];
            std::size_t B = path[i];

            for(std::size_t j{i + 1}; j <= n; ++j)
            {
                std::size_t C = path[j - 1];
                std::size_t D = (j == n) ? endNode : path[j];

                double currentDist = (paths)[A][B] + (paths)[C][D];
                double newDist     = (paths)[A][C] + (paths)[B][D];
                double delta       = newDist - currentDist;

                if(delta < bestDelta)
                {
                    bestDelta = delta;
                    bestI = i;
                    bestJ = j;
                }
            }

        }

        if(bestI != -1)
        {
            std::reverse(path.begin() + bestI, path.begin() + bestJ);
            found = true; 
        }

    }

    best.path.assign(cityQnt, SIZE_MAX);
    
    for(std::size_t i{0}; i < path.size(); ++i) 
        best.path[i] = path[i];
    
    calcScore(best);

    return best;
}


Individual Population::ruin(const Individual& ind)
{   
    Individual best = ind;
    std::vector<std::size_t> path;

    for(std::size_t n : best.path)
        if(n != SIZE_MAX)
            path.push_back(n);
    
    if(path.size() <= 2) 
        return best;

    std::size_t numToRemove = std::clamp(static_cast<std::size_t>(path.size() * 0.3), static_cast<std::size_t>(1), path.size() - 1);

    std::vector<std::size_t> idxPool(path.size() - 1);
    std::iota(idxPool.begin(), idxPool.end(), 1);
    std::shuffle(idxPool.begin(), idxPool.end(), rng);
    
    std::vector<std::size_t> toRemove(idxPool.begin(), idxPool.begin() + numToRemove);
    std::vector<bool> isRemoved(path.size(), false);

    for(std::size_t idx : toRemove)
        isRemoved[idx] = true;

    std::vector<std::size_t> newPath;
    newPath.reserve(path.size() - numToRemove);

    for(std::size_t i{0}; i < path.size(); ++i)
        if(!isRemoved[i])
            newPath.push_back(path[i]);
    

    best.path.assign(cityQnt, SIZE_MAX);
    
    for(std::size_t i{0}; i < newPath.size(); ++i) 
        best.path[i] = newPath[i];
    
    calcScore(best);

    return best;
}

Individual Population::recreate(const Individual& ind)
{
    Individual best = ind;
    std::vector<std::size_t> path;
    std::vector<bool> visited(cityQnt, false);
    
    for(size_t n : best.path) 
        if(n != SIZE_MAX) 
        {
            path.push_back(n);
            visited[n] = true;
        }
    
    visited[endNode] = true;
    bool inserted = true;

    while(inserted)
    {
        inserted = false;
        int bestPos{-1};
        int bestC {-1};
        double bestRatio{-1.};
        double bestAddedDist{0.};

        for(std::size_t i{0}; i < cityQnt; ++i) 
        {
            if(visited[i]) 
                continue;

            int roomType = types[i];

            if (roomType == 4 || roomType == 6 || roomType == 7) 
                continue;

            for(std::size_t p{1}; p <= path.size(); ++p) 
            {
                std::size_t prev = path[p - 1];
                std::size_t next = (p == path.size()) ? endNode : path[p];
                
                double addedDist = (paths)[prev][i] + (paths)[i][next] - (paths)[prev][next];
                
                if(best.dist + addedDist <= TMax) 
                {
                    double safeDist = (addedDist <= 1e-4) ? 1e-4 : addedDist;
                    double ratio = (profits)[i] / safeDist;
                    
                    if(ratio > bestRatio) 
                    {
                        bestRatio = ratio;
                        bestC = i;
                        bestPos = p;
                        bestAddedDist = addedDist;
                    }
                }
            }
        }

        if(bestC != -1) 
        {
            path.insert(path.begin() + bestPos, bestC);
            visited[bestC] = true;
            inserted = true;
            
            best.dist += bestAddedDist;
            best.score += (profits)[bestC];
            best.fitness = best.score;
            best.vis++;
        }

    }

    best.path.assign(cityQnt, SIZE_MAX);

    for(std::size_t i{0}; i < path.size(); ++i) 
        best.path[i] = path[i];

    calcScore(best);
    return best;
}

void Population::calcScore(Individual& ind) const
{
    ind.score = (profits)[ind.path[0]];
    ind.dist = 0;
    std::size_t prev = ind.path[0];
    ind.vis = 1;
    
    for(std::size_t i{1}; i < ind.path.size(); ++i) 
    {
        if(static_cast<std::size_t>(ind.path[i]) == SIZE_MAX) 
            continue;

        double edge = (paths)[prev][ind.path[i]];
        ind.dist += edge;
        ind.score += (profits)[ind.path[i]];
        prev = ind.path[i];
        ind.vis++;
    }

    ind.dist += (paths)[prev][endNode];

    ind.fitness = ind.score; 
}

Population::Population(std::vector<std::vector<double>> paths, std::vector<double> profits, std::vector<int> types, double TMax, int startNode, int endNode)
: cityQnt(paths.size()), startNode(startNode), endNode(endNode), TMax(TMax), paths(paths), profits(profits), types(types){}

