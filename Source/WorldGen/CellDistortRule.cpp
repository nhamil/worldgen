#include "WorldGen/WorldGenerator.h" 

CellDistortRule::CellDistortRule() 
    : MinEdges_(5) 
    , MaxEdges_(7) 
    , Ratio_(0.5) 
    , Random_() 
{

}

void CellDistortRule::Apply(World& world) 
{
    FJ_DEBUG("Distorting cells..."); 

    unsigned distortions = (unsigned) (world.GetCellCount() * Ratio_); 

    for (CellId i = 0; i < distortions; i++) 
    {
        Distort(world, Random_.NextULongLong() % world.GetCellCount());  
    }
}

static float DotAngle(Vector3 a, Vector3 b, Vector3 c) 
{
    return Dot(b - a, c - a); 
}

static bool IsObtuse(Vector3 a, Vector3 b, Vector3 c) 
{
    if (DotAngle(a, b, c) < 0) return true; 
    if (DotAngle(b, a, c) < 0) return true; 
    if (DotAngle(c, a, b) < 0) return true; 
    return false; 
}

void CellDistortRule::Distort(World& world, CellId cell1) 
{
    if (world.GetNeighborCount(cell1) <= MinEdges_) return; 

    CellId cell2 = world.GetNeighbor(cell1, 
        Random_.NextULongLong() % world.GetNeighborCount(cell1)
    ); 

    if (world.GetNeighborCount(cell2) <= MinEdges_) return; 

    Vector3 pos1 = world.GetPosition(cell1); 
    Vector3 pos2 = world.GetPosition(cell2); 

    CellId cell3 = -1, cell4 = -1; 

    for (unsigned i = 0; i < world.GetNeighborCount(cell1); i++) 
    {
        CellId c = world.GetNeighbor(cell1, i); 

        if (!world.HasConnection(cell2, c)) continue; 

        if (cell3 == -1) 
        {
            cell3 = c; 
        }
        else 
        {
            cell4 = c; 
        }
    }

    if (cell3 == -1 || cell4 == -1) return; 

    if (world.GetNeighborCount(cell3) >= MaxEdges_) return; 
    if (world.GetNeighborCount(cell4) >= MaxEdges_) return; 

    Vector3 pos3 = world.GetPosition(cell3); 
    Vector3 pos4 = world.GetPosition(cell4); 

    if (IsObtuse(pos1, pos2, pos3) || IsObtuse(pos1, pos2, pos4)) return; 

    world.RemoveConnection(cell1, cell2); 
    world.AddConnection(cell3, cell4); 
}