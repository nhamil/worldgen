#include "WorldGen/WorldGenerator.h" 

WorldGenerator::~WorldGenerator() 
{
    for (auto* rule : Rules_) delete rule; 
}

void WorldGenerator::AddRule(WorldGenRule* rule) 
{
    Rules_.push_back(rule); 
}

World* WorldGenerator::Generate() 
{
    World* world = new World(); 
    for (auto ruleRef : Rules_) 
    {
        ruleRef->Apply(*world); 
    }
    return world; 
}