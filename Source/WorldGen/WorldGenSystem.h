#pragma once 

#include <Fjord/Fjord.h> 
#include <Fjord/Graphics/Renderer.h> 

#include "WorldGen/WorldGenerator.h" 

class WorldGenThread : public Thread 
{
public: 
    WorldGenThread(unsigned worldSize); 

    virtual void Run() override; 

    class World World; 
    Ref<MeshData> CellMeshData; 

private: 
    void GenWorld(); 
    void GenMeshData(); 

    WorldGenerator Generator_; 
};

class Planet : public Component 
{
public: 
    bool Empty = true; 
    class World World; 
    Ref<WorldGenThread> GenThread; 
    Ref<Material> MainMaterial; 
    Ref<Mesh> CellMesh, OutlineMesh; 
};

class WorldGenSystem : public EntitySystem 
{
public: 
    WorldGenSystem(); 

    virtual void UpdateGUI() override; 
    virtual void Update(float dt) override; 
    virtual void Render() override; 
};