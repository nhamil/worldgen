#pragma once 

#include <Fjord/Util/Thread.h> 

#include "WorldGen/World.h" 
#include "WorldGen/WorldGenerator.h" 

using namespace Fjord; 

class WorldGenThread : public Thread 
{
public: 
    WorldGenThread(unsigned worldSize); 

    virtual void Run() override; 

    Ref<World> World; 
    Ref<MeshData> CellMeshData; 

private: 
    void GenWorld(); 
    void GenMeshData(); 

    WorldGenerator Generator_; 
};