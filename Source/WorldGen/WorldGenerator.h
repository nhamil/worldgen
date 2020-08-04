#pragma once 

#include "WorldGen/World.h" 

struct FluidSim; 

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

class ClimateSimulationRule : public WorldGenRule 
{
public: 
    static const int SimWidth; 
    static const int SimHeight; 
    static const float InvSimWidth; 
    static const float InvSimHeight; 

    ClimateSimulationRule(int air, int ocean); 
    virtual ~ClimateSimulationRule() = default; 

    virtual void Apply(World& world) override; 

private: 
    void SetBounds(FluidSim& air, FluidSim& ocean, World& world); 
    void AddPressureBands(FluidSim& air); 
    void InitWind(FluidSim& air); 
    void InitOcean(FluidSim& ocean, FluidSim& air); 
    void UpdateCellsAir(FluidSim& air, World& world); 
    void UpdateCellsOcean(FluidSim& air, World& world); 
    float GradX(FluidSim& sim, float* a, int x, int y); 
    float GradY(FluidSim& sim, float* a, int x, int y); 

    int AirIterations_; 
    int OceanIterations_; 
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