#include "WorldGen/Systems/LODSystem.h" 

#include "WorldGen/Components.h" 

static Ref<Material> s_AtmosMaterial; 
static Ref<Mesh> s_AtmosMesh; 

LODSystem::LODSystem() 
{
    IncludeComponent<Transform>(); 
    IncludeComponent<Planet>(); 
    IncludeComponent<LODSphere>(); 

    NodeQueue_ = new FaceNodeQueue(); 
    LastGen_ = GetTimeSeconds(); 

    if (!s_AtmosMaterial) 
    {
        s_AtmosMaterial = new Material(); 
        s_AtmosMaterial->SetShader(Shader::Load("Basic")); 
        s_AtmosMaterial->SetTexture("fj_MainTex", Texture2D::Load("TextImage")); 
    }
    if (!s_AtmosMesh) 
    {
        const unsigned Resolution = 256; 

        Vector<Vector3> verts(Resolution * Resolution); 
        Vector<Vector4> colors(Resolution * Resolution); 
        Vector<uint32> inds((Resolution-1) * (Resolution-1) * 6); 

        for (unsigned y = 0; y < Resolution; y++) 
        {
            for (unsigned x = 0; x < Resolution; x++) 
            {
                verts[x + y*Resolution] = GetPositionFromGeoCoords(GeoCoords{
                    (float) y / (Resolution-1) * 180 - 90, 
                    (float) x / (Resolution-1) * 360 - 180, 
                }) * 1.1;
                colors[x + y*Resolution] = Color{ 1.0, 1.0, 1.0, 1.0 }; 
            }
        }

        for (unsigned y = 0; y < Resolution-1; y++) 
        {
            for (unsigned x = 0; x < Resolution-1; x++) 
            {
                unsigned i = (x + y * (Resolution - 1)) * 6; 

                inds[i + 0] = (x + 0) + (y + 0) * (Resolution); 
                inds[i + 1] = (x + 0) + (y + 1) * (Resolution); 
                inds[i + 2] = (x + 1) + (y + 1) * (Resolution); 
                inds[i + 3] = (x + 0) + (y + 0) * (Resolution); 
                inds[i + 4] = (x + 1) + (y + 1) * (Resolution); 
                inds[i + 5] = (x + 1) + (y + 0) * (Resolution); 
            }
        }

        s_AtmosMesh = new Mesh(); 
        s_AtmosMesh->SetVertices(verts); 
        s_AtmosMesh->SetIndices(inds); 
        s_AtmosMesh->CalculateNormals(); 
        s_AtmosMesh->Update(); 
    }
}

void LODSystem::Update(float dt) 
{
    auto* scene = GetScene(); 

    Ref<Group> cam = scene->GetGroup(EntityFilter().Include<Transform, Camera>()); 
    Entity camEntity = cam->GetEntities()[0]; 

    Vector3 camPos = scene->GetComponent<Transform>(camEntity).GetPosition(); 

    for (Entity e : GetEntities()) 
    {
        auto& planet = scene->GetComponent<Planet>(e); 
        auto& lodSphere = scene->GetComponent<LODSphere>(e); 

        for (unsigned face = 0; face < FaceIndexCount; face++) 
        {
            if (planet.World) 
            {
                if (!lodSphere.Nodes[face] || planet.World != lodSphere.Nodes[face]->GetWorld()) 
                {
                    lodSphere.Nodes[face] = new FaceNode(NodeQueue_, planet.World, (FaceIndex) face); 
                    // lodSphere.Nodes[face]->Generate(); 
                }
                else 
                {
                    lodSphere.Nodes[face]->Update(camPos); 
                }
            }
            else 
            {
                lodSphere.Nodes[face] = nullptr; 
            }
        }
    }

    if (GetTimeSeconds() - LastGen_ > 0.1) 
    {
        NodeQueue_->Sort(camPos); 
        FaceNode* node = NodeQueue_->Poll(); 
        if (node) 
        {
            // FJ_EFDEBUG("%u", node->GetLOD()); 
            node->Generate(); 
            LastGen_ = GetTimeSeconds(); 
        }
    }
}

void LODSystem::Render() 
{
    auto* scene = GetScene(); 
    auto* renderer = GetRenderer(); 

    for (Entity e : GetEntities()) 
    {
        auto& tfm = scene->GetComponent<Transform>(e); 
        auto& planet = scene->GetComponent<Planet>(e); 
        auto& lodSphere = scene->GetComponent<LODSphere>(e); 

        // renderer->DrawMesh(
        //     s_AtmosMesh, 
        //     s_AtmosMaterial, 
        //     tfm.GetMatrix()
        // );

        for (unsigned face = 0; face < FaceIndexCount; face++) 
        {
            if (planet.World) 
            {
                FaceNode* node = lodSphere.Nodes[face]; 
                if (node && planet.World == node->GetWorld()) // && node->CanRender()) 
                {
                    node->Render(tfm); 
                    // {
                    //     Mesh* mesh = lodSphere.Nodes[face]->GetMesh(); 
                    //     Material* mat = lodSphere.Nodes[face]->GetMaterial(); 
                    //     mat->SetShader(s_Material->GetShader()); 
                    //     renderer->DrawMesh(mesh, mat, tfm.GetMatrix()); 
                    // }
                }
            }
        }
    }
}