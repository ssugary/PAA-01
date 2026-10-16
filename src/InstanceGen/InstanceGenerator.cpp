#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

enum class RoomType 
{ 
    ENTRANCE, 
    BOSS, 
    COMBAT, 
    ELITE,
    TREASURE, 
    PUZZLE, 
    REST, 
    SECRET 
};

const int TYPE_QNT = 8;
const double LAMBDA{3.};

static const char* typeName(RoomType t)
{
    switch(t)
    {
        case RoomType::ENTRANCE: return "ENTRANCE";
        case RoomType::BOSS:     return "BOSS";
        case RoomType::COMBAT:   return "COMBAT";
        case RoomType::TREASURE: return "TREASURE";
        case RoomType::PUZZLE:   return "PUZZLE";
        case RoomType::REST:     return "REST";
        case RoomType::SECRET:   return "SECRET";
        case RoomType::ELITE:              break;
        
    }

    return "?";
}

struct Room
{
    int id;
    double difficulty;
    double profit;
    RoomType type;
};

struct Config
{
    std::size_t n{40};
    unsigned seed{1};      
    double budgetFactor{2.};
    double maxDifficulty{10.};  
};


static double penalty(RoomType a, RoomType b)
{   

    if(a == b && (a == RoomType::ENTRANCE || a == RoomType::BOSS || a == RoomType::REST))
        return 100.;

    if(a == RoomType::ENTRANCE && b == RoomType::BOSS)
        return 500.;

    if(a == RoomType::ENTRANCE && b == RoomType::ELITE)
        return 150.;

    if(a == RoomType::ELITE && b == RoomType::TREASURE)
        return -10.;

    return 0.;
}

static double transitionCost(const Room& a, const Room& b)
{
    if(a.id == b.id) 
        return 0.;

    double baseDiff = b.difficulty;
    double diffJump = std::abs(a.difficulty - b.difficulty);
    double flowPen  = penalty(a.type, b.type);

    double cost = baseDiff + (LAMBDA * diffJump) + flowPen;

    return std::max(0.1, cost);
}

static std::vector<Room> placeRooms(const Config& cfg, std::mt19937& rng)
{
    std::vector<Room> rooms;
    rooms.reserve(cfg.n);

    rooms.push_back({0, 0., 0., RoomType::ENTRANCE});

    rooms.push_back({1, cfg.maxDifficulty, 0., RoomType::BOSS});

    const std::vector<double> weights{0.35, 0.15, 0.20, 0.15, 0.10, 0.05};
    const RoomType types[] = 
    {
        RoomType::COMBAT, 
        RoomType::ELITE, 
        RoomType::PUZZLE, 
        RoomType::TREASURE, 
        RoomType::REST, 
        RoomType::SECRET
    };

    std::discrete_distribution<int> pickType(weights.begin(), weights.end());

    auto randRange = [&](double minV, double maxV) {
        return std::uniform_real_distribution<double>(minV, maxV)(rng);
    };

    for (std::size_t i{2}; i < cfg.n; ++i)
    {
        RoomType t = types[pickType(rng)];
        double diff{0.};
        double prof{0.};

        switch (t)
        {
            case RoomType::COMBAT:
                diff = randRange(3., 6.);
                prof = randRange(20., 40.);
                break;
            case RoomType::ELITE:
                diff = randRange(7., 9.5);
                prof = randRange(50., 85.);
                break;
            case RoomType::PUZZLE:
                diff = randRange(2., 5.);
                prof = randRange(15., 35.);
                break;
            case RoomType::TREASURE:
                diff = randRange(1., 3.);
                prof = randRange(40., 75.);
                break;
            case RoomType::REST:
                diff = randRange(1., 2.);
                prof = randRange(5., 15.);
                break;
            case RoomType::SECRET:
                diff = randRange(4., 7.);
                prof = randRange(60., 100.);
                break;
            default: break;
        }

        rooms.push_back({static_cast<int>(i), diff, prof, t});
    }

    
    for(std::size_t i{0}; i < rooms.size(); ++i) 
        rooms[i].id = i;
    
    return rooms;
}


static void writeInstance(const std::string& path, const std::vector<Room>& rooms, double tmax)
{
    std::ofstream ofs(path);
    
    if(!ofs) 
        throw std::runtime_error("Nao foi possivel abrir: " + path);

    ofs.setf(std::ios::fixed);
    ofs.precision(2);

    ofs << tmax << '\n';

    for(const Room& r : rooms)
        ofs << r.difficulty << " " << r.profit << " " << (int)r.type << '\n';

}

int main(int argc, char* argv[])
{
    if(argc < 5)
    {
        std::cerr << "Uso: " << argv[0]
                  << " <n_salas> <seed> <fator_orcamento> <saida.txt> [largura] [altura] [dist_min]\n";
        return 1;
    }

    Config cfg;
    cfg.n            = static_cast<std::size_t>(std::atoi(argv[1]));
    cfg.seed         = std::strtoul(argv[2], nullptr, 10);
    cfg.budgetFactor = std::atof(argv[3]);
    std::string out  = argv[4];

    if (argc > 5)
        cfg.maxDifficulty = std::atof(argv[5]);

    if (cfg.n < 4)                
    { 
        std::cerr << "n_salas deve ser >= 4\n"; 
        return 1; 
    }

    if (cfg.budgetFactor < 1.)   
    { 
        std::cerr << "fator_orcamento deve ser >= 1.0\n"; 
        return 1; 
    }

    std::mt19937 rng(cfg.seed);

    auto rooms = placeRooms(cfg, rng);

    double minValidPathCost = 1e9;
    for (std::size_t i = 2; i < rooms.size(); ++i) 
    {
        double cost = transitionCost(rooms[0], rooms[i]) + transitionCost(rooms[1], rooms[i]);
        minValidPathCost = std::min(minValidPathCost, cost);
    }
    double tmax = cfg.budgetFactor * minValidPathCost;

    writeInstance(out, rooms, tmax);

    double profitTotal{0.};
    
    for (const Room& r : rooms) 
        profitTotal += r.profit;

    std::cout << "Instancia gerada com sucesso em: " << out << "\n"
              << " | Salas: " << rooms.size()
              << " | Custo Direto: " << minValidPathCost
              << " | TMax : " << tmax
              << " | Profit Disponivel: " << profitTotal << "\n";

    return 0;
}