#include "WorldGen/Systems/LODSystem.h" 

#include "WorldGen/Components.h" 

static Ref<Material> s_AtmosMaterial; 
static Ref<Mesh> s_AtmosMesh; 

static bool s_Render = true; 

LODSystem::LODSystem() 
{
    IncludeComponent<Transform>(); 
    IncludeComponent<Planet>(); 
    IncludeComponent<LODSphere>(); 

    if (!s_AtmosMaterial) 
    {
        s_AtmosMaterial = new Material(); 
        s_AtmosMaterial->SetRenderQueue(RenderQueue::Translucent); 
        s_AtmosMaterial->SetShader(Shader::Load("Atmosphere")); 
        s_AtmosMaterial->SetTexture("fj_MainTex", Texture2D::Load("TextImage")); 
        s_AtmosMaterial->SetFloat("u_Radius", 1.06); 
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
                verts[x + y*Resolution] = Normalized(GetPositionFromGeoCoords(GeoCoords{
                    (float) y / (Resolution-1) * 180 - 90, 
                    (float) x / (Resolution-1) * 360 - 180, 
                }));
                colors[x + y*Resolution] = Color{ 1.0, 1.0, 1.0, 0.5 }; 
            }
        }

        for (unsigned y = 0; y < Resolution-1; y++) 
        {
            for (unsigned x = 0; x < Resolution-1; x++) 
            {
                unsigned i = (x + y * (Resolution - 1)) * 6; 

                inds[i + 0] = (x + 0) + (y + 0) * (Resolution); 
                inds[i + 1] = (x + 1) + (y + 0) * (Resolution); 
                inds[i + 2] = (x + 1) + (y + 1) * (Resolution); 
                inds[i + 3] = (x + 0) + (y + 0) * (Resolution); 
                inds[i + 4] = (x + 1) + (y + 1) * (Resolution); 
                inds[i + 5] = (x + 0) + (y + 1) * (Resolution); 

                // inds[i + 0] = (x + 0) + (y + 0) * (Resolution); 
                // inds[i + 1] = (x + 0) + (y + 1) * (Resolution); 
                // inds[i + 2] = (x + 1) + (y + 1) * (Resolution); 
                // inds[i + 3] = (x + 0) + (y + 0) * (Resolution); 
                // inds[i + 4] = (x + 1) + (y + 1) * (Resolution); 
                // inds[i + 5] = (x + 1) + (y + 0) * (Resolution); 
            }
        }

        s_AtmosMesh = new Mesh(); 
        s_AtmosMesh->SetVertices(verts); 
        s_AtmosMesh->SetIndices(inds); 
        s_AtmosMesh->CalculateNormals(); 
        s_AtmosMesh->Update(); 
    }
}

static unsigned MaxLOD = 5;
// static float s_LOD[MaxLOD];
// {
//     2.0, 
//     1.0, 
//     0.5, 
//     0.25, 
//     0.125, 
//     0.0625, 
//     0.03125, 
//     0.015125, 
//     0.0 
// }; 

static float minHeight = 0.98; 
static float maxHeight = 1.02; 

void LODSystem::UpdateGUI() 
{
    UI::SliderFloat("MinHeight", &minHeight, 0.5, 1.0); 
    UI::SliderFloat("MaxHeight", &maxHeight, 1.0, 2.0); 
    for (unsigned i = 0; i < MaxLOD; i++) 
    {
        // UI::SliderFloat("LOD " + ToString(i+1) + " Range", &s_LOD[i], 0, 1); 
    }
}

void LODSystem::Update(float dt) 
{
    auto* scene = GetScene(); 

    Ref<Group> cam = scene->GetGroup(EntityFilter().Include<Transform, Camera>()); 
    Entity camEntity = cam->GetEntities()[0]; 

    Vector3 camPos = scene->GetComponent<Transform>(camEntity).GetPosition(); 

    int meshesAllowed = 2; 

    Vector<float> ranges; 
    for (unsigned i = 0; i < MaxLOD; i++) 
    {
        // ranges.push_back(s_LOD[i]); 
        ranges.push_back(
            4.0 / std::pow(2.0, i)
        );
    }

    for (Entity e : GetEntities()) 
    {
        auto& tfm = scene->GetComponent<Transform>(e); 
        auto& planet = scene->GetComponent<Planet>(e); 
        auto& lodSphere = scene->GetComponent<LODSphere>(e); 

        // if (planet.World) 
        {
            // planet.World->UpdateSpatialGeometry(); 

            if (!lodSphere.QuadTree) 
            {
                lodSphere.QuadTree = new SphereQuadTree(ranges); 
            }
            else 
            {
                for (unsigned i = 0; i < MaxLOD; i++) 
                {
                    lodSphere.QuadTree->SetLODRange(i+1, ranges[i]); 
                }
            }

            // lodSphere.QuadTree->Update(Normalized(camPos - tfm.GetPosition())); 
        }
    }
}

void LODSystem::Render() 
{
    if (!s_Render) return; 

    auto* scene = GetScene(); 
    auto* renderer = GetRenderer(); 

    for (Entity e : GetEntities()) 
    {
        auto& tfm = scene->GetComponent<Transform>(e); 
        auto& planet = scene->GetComponent<Planet>(e); 
        auto& lodSphere = scene->GetComponent<LODSphere>(e); 

        renderer->DrawMesh(
            s_AtmosMesh, 
            s_AtmosMaterial, 
            tfm.GetMatrix()
        );

        // if (lodSphere.QuadTree) 
        // {
        //     lodSphere.QuadTree->MinHeight_ = minHeight; 
        //     lodSphere.QuadTree->MaxHeight_ = maxHeight; 
        //     lodSphere.QuadTree->Render(tfm.GetMatrix()); 
        // }
    }
}