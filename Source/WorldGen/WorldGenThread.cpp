#include "WorldGen/WorldGenThread.h" 

WorldGenThread::WorldGenThread(unsigned worldSize) 
{
    WorldGenerator& gen = Generator_; 
    gen.AddRule(new SubdivideCellGenRule(worldSize)); 
    if (worldSize > 1) gen.AddRule(new CellDistortRule()); 
    gen.AddRule(new CellRelaxRule(50)); 
    gen.AddRule(new BasicTerrainGenRule()); 
    // gen.AddRule(new ClimateSimulationRule(70, 200)); 
}

void WorldGenThread::Run() 
{
    GenWorld(); 
    GenMeshData(); 
    FJ_DEBUG("Done!"); 
}

void WorldGenThread::GenWorld() 
{
    FJ_DEBUG("Generating World..."); 
    World = Generator_.Generate(); 
}

void WorldGenThread::GenMeshData() 
{
    FJ_DEBUG("Generating geometry..."); 

    unsigned neighborCount = 0; 
    Vector<unsigned> neighborIndex; neighborIndex.reserve(World->GetConnectionCount()); 
    for (CellId i = 0; i < World->GetCellCount(); i++) 
    {
        neighborIndex.push_back(neighborCount); 
        neighborCount += World->GetNeighborCount(i); 
    }

    // edges 
    {
        EdgeMeshData = new MeshData(); 
        Vector<Vector3>& verts = EdgeMeshData->Vertices; 
        Vector<Vector4>& colors = EdgeMeshData->Colors; 
        Vector<uint32>& inds = EdgeMeshData->Indices; 
        verts.resize(neighborCount); 
        colors.resize(neighborCount); 
        inds.resize(neighborCount * 2); 
        // for (CellId i = 0; i < World.GetCellCount(); i++) 
        ParallelFor(CellId(0), CellId(World->GetCellCount()), [&](CellId i)
        {
            Vector<Vector3> bounds = World->GetBounds(i); 
            unsigned start = neighborIndex[i]; //verts.size(); 
            unsigned index = start; 
            for (Vector3& v : bounds) 
            {
                verts[index] = v*1.0001; 
                colors[index++] = {0, 0, 0, 1}; 
            }

            index = start; 

            unsigned ind = neighborIndex[i] * 2; 
            for (unsigned v = 0; v < bounds.size() - 1; v++) 
            {
                inds[ind++] = (index++); 
                inds[ind++] = (index); 
            }
            inds[ind++] = (start); 
            inds[ind++] = (start+bounds.size()-1); 
        }); 
    }

    // cells 
    {
        CellMeshData = new MeshData(); 
        Vector<Vector3>& verts = CellMeshData->Vertices; 
        Vector<Vector3>& normals = CellMeshData->Normals; 
        Vector<Vector4>& colors = CellMeshData->Colors; 
        Vector<uint32>& inds = CellMeshData->Indices; 
        // Vector<Vector2>& tex = CellMeshData->TexCoords; 
        // verts.reserve(World->GetCellCount() * 6); 
        // colors.reserve(World->GetCellCount() * 6); 
        // normals.reserve(World->GetCellCount() * 6); 
        // inds.reserve(World->GetCellCount() * 6 * 3); 
        verts.resize(neighborCount); 
        normals.resize(neighborCount); 
        colors.resize(neighborCount); 
        // tex.resize(neighborCount); 
        inds.resize((neighborCount - World->GetCellCount()*2) * 3); 
        // for (CellId i = 0; i < World->GetCellCount(); i++) 
        ParallelFor(CellId(0), CellId(World->GetCellCount()), [&](CellId i)
        {
            Vector3 normal = World->GetPosition(i); 
            Vector<Vector3> bounds = World->GetBounds(i); 
            unsigned start = neighborIndex[i]; //verts.size(); 
            unsigned index = start; 
            for (Vector3& v : bounds) 
            {
                verts[index] = v; 
                normals[index] = Normalized(v); 
                colors[index++] =  GetTerrainColor(World->GetTerrain(i)); 
                GeoCoords gc = GetGeoCoordsFromPosition(v); 
                // tex[index] = {gc.Longitude / 360 + 0.5f, gc.Latitude / 180 + 0.5f}; 
                // FJ_DEBUG("%f %f", tex[index].X, tex[index].Y); 
            }

            index = start + 1; 
            
            unsigned ind = (neighborIndex[i] - i*2) * 3; 
            for (unsigned v = 0; v < bounds.size() - 2; v++) 
            {
                inds[ind++] = (start); 
                inds[ind++] = (index++); 
                inds[ind++] = (index); 
            }
        }); 
    }
}