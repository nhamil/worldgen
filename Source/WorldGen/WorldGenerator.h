#pragma once 

#include "WorldGen/World.h" 

class WorldGenRule 
{
public: 
    virtual ~WorldGenRule() = default; 

    virtual void Apply(World& world) = 0; 
};

class SubdivideCellGenRule : public WorldGenRule 
{
public: 
    explicit SubdivideCellGenRule(int iterations); 
    virtual ~SubdivideCellGenRule() = default; 

    virtual void Apply(World& world) override; 

    const int Iterations; 
};


class BasicTerrainGenRule : public WorldGenRule 
{
public: 
    BasicTerrainGenRule(); 
    virtual ~BasicTerrainGenRule() = default; 

    virtual void Apply(World& world) override; 

private: 
    uint64 Seed_; 
};

class CellDistortRule : public WorldGenRule 
{
public: 
    CellDistortRule(); 
    virtual ~CellDistortRule() = default; 

    virtual void Apply(World& world) override; 

private: 
    void Distort(World& world, CellId cell); 

    int MinEdges_; 
    int MaxEdges_; 
    double Ratio_; 
    Random Random_; 
};

class CellRelaxRule : public WorldGenRule 
{
public: 
    CellRelaxRule(int iterations); 
    virtual ~CellRelaxRule() = default; 

    virtual void Apply(World& world) override; 

private: 
    void Relax(World& world, Vector<Vector3>& positions); 
    void SetPositions(World& world, Vector<Vector3>& positions); 

    int Iterations_; 
};

class WorldGenerator 
{
public: 
    WorldGenerator() = default; 
    ~WorldGenerator(); 

    void AddRule(WorldGenRule* rule); 

    World Generate(); 

private: 
    Vector<WorldGenRule*> Rules_; 
};