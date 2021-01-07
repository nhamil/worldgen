#include "WorldGen/Systems/WorldGenSystem.h" 

#include "Fjord/Graphics/PostProcess.h" 
#include "Fjord/Graphics/Renderer.h" 

#include "WorldGen/Components.h" 
#include "WorldGen/World.h" 
#include "WorldGen/WorldGenThread.h" 

WorldGenSystem::WorldGenSystem() 
{
    IncludeComponent<Planet>(); 
    IncludeComponent<Transform>(); 
}

static void CreateSphereEntity(Scene* scene, Planet& planet) 
{
    Entity e = scene->CreateEntity(); 
    auto& mc = scene->AddComponent<MeshContainer>(e); 

    Entity e2 = scene->CreateEntity(); 
    auto& mc2 = scene->AddComponent<MeshContainer>(e2); 

    static const int W = 128; 
    static const int H = W / 2; 

    static const int IW = 1024; 
    static const int IH = IW / 2; 

    Vector<Vector4> colors; 
    Vector<Vector3> verts; 
    Vector<Vector2> tex; 
    Vector<uint32> inds; 
    Vector<uint8> img; img.resize(4 * IW * IH); 
    
    for (int y = 0; y < H; y++) 
    {
        for (int x = 0; x < W; x++) 
        {
            float xf = (float) x / (W - 1); 
            float yf = (float) y / (H - 1); 

            Vector3 pos = GetPositionFromGeoCoords(GeoCoords{
                yf*180-90, xf*360+180 
            }) * 1.00; 

            tex.push_back({xf, 1.0f-yf}); 
            verts.push_back(pos); 
            colors.push_back(Color::White); 
        }
    }

    FJ_FDEBUG("Start"); 

    planet.World->UpdateSpatialGeometry(); 

    // for (int y = 0; y < IH; y++) 
    ParallelFor(0, IH-1, [&](int y) 
    {
        // FJ_FDEBUG("here %d", y); 
        float yf = (float) y / (IH - 1); 
        for (int x = 0; x < IW; x++) 
        {
            float xf = (float) x / (IW - 1); 

            Vector3 pos = GetPositionFromGeoCoords(GeoCoords{
                yf*180-90, xf*360+180 
            }); 
// FJ_FDEBUG("here %d", y); 
            CellId c = planet.World->GetCellIdPyPosition(pos); 
            Terrain t = planet.World->GetTerrain(c); 
// FJ_FDEBUG("here %d", y); 
            Color col = GetTerrainColor(t); 
            int i = (x+y*IW)*4; 
            img[i++] = ((uint8) Map<float>(col.R, 0.0f, 1.0f, 0.0f, 255.0f)); 
            img[i++] = ((uint8) Map<float>(col.G, 0.0f, 1.0f, 0.0f, 255.0f)); 
            img[i++] = ((uint8) Map<float>(col.B, 0.0f, 1.0f, 0.0f, 255.0f)); 
            img[i] = 127; 
        }
    }); 
    FJ_FDEBUG("Stop"); 

    Texture2D* texture = new Texture2D(IW, IH, TextureFormat::RGBA8); 
    texture->SetData(&img[0]); 
    texture->SetAddressMode(AddressMode::Repeat); 
    texture->SetFilter(TextureFilter::Nearest); 
    texture->Update(); 

    for (int y = 0; y < H-1; y++) 
    {
        for (int x = 0; x < W-1; x++) 
        {
            uint32 v00 = (x + 0) + (y + 0) * W; 
            uint32 v01 = (x + 0) + (y + 1) * W; 
            uint32 v10 = (x + 1) + (y + 0) * W; 
            uint32 v11 = (x + 1) + (y + 1) * W; 

            inds.push_back(v00); 
            inds.push_back(v10); 
            inds.push_back(v11); 
            inds.push_back(v00); 
            inds.push_back(v11); 
            inds.push_back(v01); 
        }
    }

    Mesh* mesh = new Mesh(); 
    mesh->SetVertices(verts); 
    mesh->SetColors(colors); 
    mesh->SetTexCoords(tex); 
    mesh->SetIndices(inds); 
    mesh->CalculateNormals(); 
    mesh->Update(); 
    mc.SetMesh(mesh); 

    Material* mat = new Material(); 
    mat->SetShader(Shader::Load("Terrain")); 
    mat->SetTexture("fj_MainTex", texture); 
    // mat->SetRenderQueue(RenderQueue::Translucent); 
    mc.SetMaterial(mat); 
}

static bool s_Outline = false; 

void WorldGenSystem::UpdateGUI() 
{
    auto* scene = GetScene(); 

    bool gen = false; 
    // UI::BeginWindow("WorldGen", UI::WindowFlag::WindowFlagAutoResize); 
    if (UI::Button("Generate")) gen = true; 
    UI::Checkbox("Draw Outline", &s_Outline); 
    // UI::EndWindow(); 

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

                planet.OutlineMesh = new Mesh(); 
                planet.GenThread->EdgeMeshData->Apply(planet.OutlineMesh); 
                planet.OutlineMesh->SetPrimitive(Primitive::Lines); 

                planet.GenThread = nullptr; 

                // CreateSphereEntity(scene, planet); 
            }

            if (gen || planet.Empty) //UI::Button("Generate")) 
            {
                planet.Empty = false; 
                planet.GenThread = new WorldGenThread(7); 
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

    Quaternion rot = Quaternion::AxisAngle(Vector3::Up, 0.0 * dt); 

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
        // tfm.SetScale(Vector3(10)); 

        if (planet.CellMesh) 
        {
            renderer->DrawMesh(
                planet.CellMesh, 
                planet.MainMaterial, 
                tfm.GetMatrix() 
            ); 
        }

        if (s_Outline && planet.OutlineMesh) 
        {
            renderer->DrawMesh(
                planet.OutlineMesh, 
                planet.MainMaterial, 
                tfm.GetMatrix() 
            ); 
        }
    }
}