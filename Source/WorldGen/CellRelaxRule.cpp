#include "WorldGen/WorldGenerator.h" 

CellRelaxRule::CellRelaxRule(int iterations) 
    : Iterations_(iterations) 
{

}

void CellRelaxRule::Apply(World& world) 
{
    Vector<Vector3> positions; 

    for (int i = 0; i < Iterations_; i++) 
    {
        positions.clear(); 
        Relax(world, positions); 
        SetPositions(world, positions); 
    }
}

void CellRelaxRule::SetPositions(World& world, Vector<Vector3>& positions) 
{
    for (CellId i = 0; i < world.GetCellCount(); i++) 
    {
        world.SetPosition(i, positions[i]); 
    }
}

void CellRelaxRule::Relax(World& world, Vector<Vector3>& positions) 
{
    for (CellId c = 0; c < world.GetCellCount(); c++) 
    {
        Vector3 v = world.GetPosition(c); 
        Vector3 v_ = v; 
        int count = 1; 

        for (unsigned j = 0; j < world.GetNeighborCount(c); j++) 
        {
            count++; 
            v += world.GetPosition(world.GetNeighbor(c, j)) - v_; 
        }

        v *= 1.0 / count; 
        v += v_; 
        positions.push_back(v); 
    }
}