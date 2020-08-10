#include "WorldGen/WorldGenSystem.h" 

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
    FJ_DEBUG("Generating world..."); 
    World = Generator_.Generate(); 
}

void WorldGenThread::GenMeshData() 
{
    FJ_DEBUG("Generating geometry..."); 

    unsigned neighborCount = 0; 
    Vector<unsigned> neighborIndex; neighborIndex.reserve(World.GetConnectionCount()); 
    for (CellId i = 0; i < World.GetCellCount(); i++) 
    {
        neighborIndex.push_back(neighborCount); 
        neighborCount += World.GetNeighborCount(i); 
    }

    // cells 
    {
        CellMeshData = new MeshData(); 
        Vector<Vector3>& verts = CellMeshData->Vertices; 
        Vector<Vector3>& normals = CellMeshData->Normals; 
        Vector<Vector4>& colors = CellMeshData->Colors; 
        Vector<uint32>& inds = CellMeshData->Indices; 
        // verts.reserve(World.GetCellCount() * 6); 
        // colors.reserve(World.GetCellCount() * 6); 
        // normals.reserve(World.GetCellCount() * 6); 
        // inds.reserve(World.GetCellCount() * 6 * 3); 
        verts.resize(neighborCount); 
        normals.resize(neighborCount); 
        colors.resize(neighborCount); 
        inds.resize((neighborCount - World.GetCellCount()*2) * 3); 
        // for (CellId i = 0; i < World.GetCellCount(); i++) 
        ParallelFor(CellId(0), CellId(World.GetCellCount()), [&](CellId i)
        {
            Vector3 normal = World.GetPosition(i); 
            Vector<Vector3> bounds = World.GetBounds(i); 
            unsigned start = neighborIndex[i]; //verts.size(); 
            unsigned index = start; 
            for (Vector3& v : bounds) 
            {
                verts[index] = v; 
                normals[index] = normal; 
                colors[index++] = GetTerrainColor(World.GetTerrain(i)); 
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

WorldGenSystem::WorldGenSystem() 
{
    IncludeComponent<Planet>(); 
    IncludeComponent<Transform>(); 
}

void WorldGenSystem::UpdateGUI() 
{
    auto* scene = GetScene(); 

    bool gen = false; 
    UI::BeginWindow("WorldGen", UI::WindowFlag::WindowFlagAutoResize); 
    if (UI::Button("Generate")) gen = true; 
    if (UI::Button("Quit")) Fjord::Stop(); 
    UI::EndWindow(); 

    int i = 0; 
    for (Entity e : GetEntities()) 
    {
        Planet& planet = scene->GetComponent<Planet>(e); 
        

        // UI::PushId(i++); 
        
        if (planet.GenThread && planet.GenThread->IsRunning()) 
        {
            // UI::Button("Generating..."); 
        }
        else 
        {
            if (planet.GenThread) 
            {
                planet.World = planet.GenThread->World; 
                
                Material* mat = new Material(); 
                mat->SetShader(Shader::Load("Basic")); 
                planet.MainMaterial = mat; 

                planet.CellMesh = new Mesh(); 
                planet.GenThread->CellMeshData->Apply(planet.CellMesh); 

                planet.GenThread = nullptr; 
            }

            if (gen || planet.Empty) //UI::Button("Generate")) 
            {
                planet.Empty = false; 
                planet.GenThread = new WorldGenThread(5); 
                planet.GenThread->Start(); 
            }
        }
        
        // UI::PopId(); 
        
    }
    // UI::EndWindow(); 
}

void WorldGenSystem::Update(float dt) 
{
    auto* scene = GetScene(); 

    Quaternion rot = Quaternion::AxisAngle(Vector3::Up, 0.01 * dt); 

    for (Entity e : GetEntities()) 
    {
        auto& tfm = scene->GetComponent<Transform>(e); 
        tfm.SetRotation(tfm.GetRotation() * rot); 
    }
}

void WorldGenSystem::Render() 
{
    auto* scene = GetScene(); 
    auto* renderer = GetRenderer(); 

    for (Entity e : GetEntities()) 
    {
        auto& planet = scene->GetComponent<Planet>(e); 
        auto& tfm = scene->GetComponent<Transform>(e); 
        tfm.SetScale(Vector3(10)); 

        if (planet.CellMesh) 
        {
            renderer->DrawMesh(
                planet.CellMesh, 
                planet.MainMaterial, 
                tfm.GetMatrix() 
            ); 
        }
    }
}