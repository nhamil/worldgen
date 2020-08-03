#include "WorldGen/WorldGenerator.h" 

static const float T = (1.0 + std::sqrt(5.0)) * 0.5; 

struct Context 
{
    class World& World; 
    HashMap<Pair<CellId, CellId>, CellId, PairHash> IdMap; 

    explicit Context(class World& world) : World(world) {} 

    unsigned GetOrCreateCell(CellId a, CellId b) 
    {
        auto abId = MakeSortedPair(a, b); 
        auto id = IdMap.find(abId); 

        if (id == IdMap.end()) 
        {
            Vector3 aPos = World.GetPosition(a); 
            Vector3 bPos = World.GetPosition(b); 
            Vector3 pos = (aPos + bPos) * 0.5; 
            CellId newId = World.AddCell(pos); 
            IdMap[abId] = newId; 
            return newId; 
        }
        else 
        {
            return id->second; 
        }
    }

    void GenerateTriangles(CellId a, CellId b, CellId c, int iter) 
    {
        if (iter > 0) 
        {
            CellId ab = GetOrCreateCell(a, b); 
            CellId bc = GetOrCreateCell(b, c); 
            CellId ac = GetOrCreateCell(a, c); 

            GenerateTriangles(a, ab, ac, iter - 1); 
            GenerateTriangles(ab, b, bc, iter - 1); 
            GenerateTriangles(ac, bc, c, iter - 1); 
            GenerateTriangles(ab, bc, ac, iter - 1); 
        }
        else 
        {
            World.AddConnection(a, b); 
            World.AddConnection(b, c); 
            World.AddConnection(a, c); 
        }
    }
};

SubdivideCellGenRule::SubdivideCellGenRule(int iterations) 
    : Iterations(iterations) {} 

void SubdivideCellGenRule::Apply(World& world) 
{
    FJ_ASSERT(world.GetCellCount() == 0); 
    FJ_DEBUG("Subdividing cells..."); 

    world.AddCell({ 0, 1, T}); // 0
    world.AddCell({ 0, 1,-T}); // 1 
    world.AddCell({ 0,-1, T}); // 2
    world.AddCell({ 0,-1,-T}); // 3

    world.AddCell({ 1, T, 0}); // 4
    world.AddCell({ 1,-T, 0}); // 5
    world.AddCell({-1, T, 0}); // 6
    world.AddCell({-1,-T, 0}); // 7

    world.AddCell({ T, 0, 1}); // 8
    world.AddCell({-T, 0, 1}); // 9
    world.AddCell({ T, 0,-1}); // 10
    world.AddCell({-T, 0,-1}); // 11

    Context c(world); 

    c.GenerateTriangles(0, 4, 8, Iterations); 
    c.GenerateTriangles(4, 8, 10, Iterations); 
    c.GenerateTriangles(0, 2, 8, Iterations); 
    c.GenerateTriangles(5, 8, 10, Iterations); 
    c.GenerateTriangles(2, 5, 8, Iterations); 

    c.GenerateTriangles(0, 2, 9, Iterations); 
    c.GenerateTriangles(0, 6, 9, Iterations); 
    c.GenerateTriangles(2, 7, 9, Iterations); 
    c.GenerateTriangles(7, 9, 11, Iterations); 
    c.GenerateTriangles(6, 9, 11, Iterations); 

    c.GenerateTriangles(1, 4, 10, Iterations); 
    c.GenerateTriangles(1, 4, 6, Iterations); 
    c.GenerateTriangles(1, 6, 11, Iterations); 
    c.GenerateTriangles(1, 3, 10, Iterations); 
    c.GenerateTriangles(1, 3, 11, Iterations); 

    c.GenerateTriangles(0, 4, 6, Iterations); 
    c.GenerateTriangles(2, 5, 7, Iterations); 
    c.GenerateTriangles(3, 5, 7, Iterations); 
    c.GenerateTriangles(3, 5, 10, Iterations); 
    c.GenerateTriangles(3, 7, 11, Iterations); 
}
