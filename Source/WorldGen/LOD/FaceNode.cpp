#include "WorldGen/LOD/FaceNode.h" 

#include "WorldGen/Noise.h" 
#include "WorldGen/LOD/FaceNodeQueue.h" 

static Ref<Material> s_Material; 

FaceNode::FaceNode(FaceNodeQueue* queue, World* world, FaceIndex face, unsigned lod, Vector3 tl, Vector3 tr, Vector3 bl, Vector3 br) 
    : NodeQueue_(queue)
    , Face_(face) 
    , LOD_(lod) 
    , World_(world) 
    , TopLeft_(tl) 
    , TopRight_(tr) 
    , BottomLeft_(bl) 
    , BottomRight_(br) 
{
    TopLeft_ = Normalized(TopLeft_); 
    TopRight_ = Normalized(TopRight_); 
    BottomLeft_ = Normalized(BottomLeft_); 
    BottomRight_ = Normalized(BottomRight_); 
    Center_ = Normalized(TopLeft_ + TopRight_ + BottomLeft_ + BottomRight_);// * 0.25; 

    // FJ_FDEBUG("Creating Child FaceNode (World:%p, Face:%d, LOD:%u)", world, face, lod); 
}

FaceNode::FaceNode(FaceNodeQueue* queue, World* world, FaceIndex face) 
    : NodeQueue_(queue)
    , Face_(face) 
    , World_(world) 
{
    // FJ_FDEBUG("Creating FaceNode (World:%p, Face:%d)", world, face); 

    switch (face) 
    {
        case FaceIndex::Front: 
            TopLeft_     = {-1, +1, +1}; 
            TopRight_    = {+1, +1, +1}; 
            BottomLeft_  = {-1, -1, +1}; 
            BottomRight_ = {+1, -1, +1}; 
            break; 
        case FaceIndex::Back: 
            TopLeft_     = {+1, +1, -1}; 
            TopRight_    = {-1, +1, -1}; 
            BottomLeft_  = {+1, -1, -1}; 
            BottomRight_ = {-1, -1, -1}; 
            break; 
        case FaceIndex::Left: 
            TopLeft_     = {-1, +1, -1}; 
            TopRight_    = {-1, +1, +1}; 
            BottomLeft_  = {-1, -1, -1}; 
            BottomRight_ = {-1, -1, +1}; 
            break; 
        case FaceIndex::Right: 
            TopLeft_     = {+1, +1, +1}; 
            TopRight_    = {+1, +1, -1}; 
            BottomLeft_  = {+1, -1, +1}; 
            BottomRight_ = {+1, -1, -1}; 
            break; 
        case FaceIndex::Top: 
            TopLeft_     = {-1, +1, -1}; 
            TopRight_    = {+1, +1, -1}; 
            BottomLeft_  = {-1, +1, +1}; 
            BottomRight_ = {+1, +1, +1}; 
            break; 
        case FaceIndex::Bottom: 
            TopLeft_     = {-1, -1, +1}; 
            TopRight_    = {+1, -1, +1}; 
            BottomLeft_  = {-1, -1, -1}; 
            BottomRight_ = {+1, -1, -1}; 
            break; 
        default: 
            FJ_FWARN("Unknown face index: %d", face); 
    }

    TopLeft_ = Normalized(TopLeft_); 
    TopRight_ = Normalized(TopRight_); 
    BottomLeft_ = Normalized(BottomLeft_); 
    BottomRight_ = Normalized(BottomRight_); 
    Center_ = Normalized(TopLeft_ + TopRight_ + BottomLeft_ + BottomRight_);// * 0.25; 

    for (unsigned i = 0; i < 4; i++) 
    {
        Children_[i] = nullptr; 
    }

    if (!s_Material) 
    {
        s_Material = new Material(); 
        s_Material->SetShader(Shader::Load("Basic")); 
    }

    Requested_ = true; 
    NodeQueue_->Add(this); 
}

FaceNode::~FaceNode() 
{
    NodeQueue_->Remove(this); 

    for (unsigned i = 0; i < 4; i++) 
    {
        Children_[i] = nullptr; 
    }
}

bool FaceNode::CanRender() const 
{
    return Mesh_ && Mat_; 
}

bool FaceNode::HasChildren() const 
{
    for (unsigned i = 0; i < 4; i++) 
    {
        if (Children_[i]) return true; 
    }
    return false; 
}

static float s_Radius[] = {
    INFINITY, 
    160.0, 
    80.0, 
    40.0, 
    20.0, 
    10.0, 
    5.0, 
    2.5, 
    1.25, 
    0.75, 
    0.325, 
    0.1625, 
    0.08125, 
    0.040625, 

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    4.0, 
    2.0, 
    1.0, 
    0.5, 
    0.25,
    0.125, 
    0.0625, 
    0.03125, 
    0.015625
};

int FaceNode::Update(const Vector3& camPosition, bool buildMesh, Vector<FaceNode*>& childQueue) 
{
    const float BaseDist = 180.0; 

    int meshesBuilt = 0; 

    if (buildMesh) 
    {
        if (Lock_.TryLock()) 
        {
            if (MeshDataReady_) 
            {
                Mat_ = new Material(); 
                Mat_->SetShader(s_Material->GetShader()); 

                WaterMat_ = new Material(); 
                WaterMat_->SetShader(s_Material->GetShader()); 

                Mesh_ = new Mesh(); 
                MeshData_->Apply(Mesh_); 
                Mesh_->CalculateNormals(); 

                WaterMesh_ = new Mesh(); 
                WaterMeshData_->Apply(WaterMesh_); 
                WaterMesh_->CalculateNormals(); 

                MeshData_ = nullptr; 
                WaterMeshData_ = nullptr; 
                MeshDataReady_ = false; 
                meshesBuilt++; 
            }
            Lock_.Unlock(); 
        }
    }

    float dist = Angle(camPosition, GetCenter()) * FJ_TO_DEG; 
    //Length2(camPosition - GetCenter()); 
    // float dist = Angle(camPosition, GetCenter()) * FJ_TO_DEG; 
    bool prevClose = CloseEnough_; 

    if (LOD_ > 0) 
    {
        // CloseEnough_ = dist < (BaseDist / (LOD_)); 
        CloseEnough_ = dist < (s_Radius[LOD_]); 
        // FJ_DEBUG("%f %f", dist, BaseDist / (LOD_ * LOD_)); 
    }

    // if (LOD_ > 0 && CloseEnough_) FJ_DEBUG("close: %d %f", LOD_, dist); 

    // if (LOD_ > 1 && dist > (4 * BaseDist / (LOD_))) 
    if (LOD_ > 1 && dist > (4 * s_Radius[LOD_])) 
    {
        for (unsigned i = 0; i < 4; i++) 
        {
            if (Children_[i]) 
            {
                Children_[i] = nullptr; 
            }
        }
    }

    if (prevClose != CloseEnough_) 
    {
        // FJ_FDEBUG("Close FaceNode (Face:%d, LOD:%u): %d", Face_, LOD_, CloseEnough_); 
    }

    for (unsigned i = 0; i < 4; i++) 
    {
        if (Children_[i]) 
        {
            // meshesAllowed -= Children_[i]->Update(camPosition, meshesAllowed); 
            childQueue.push_back(Children_[i]); 
            CloseEnough_ |= Children_[i]->CloseEnough_; 
        }
    }

    // if (CanRender()) 
    if ((LOD_ == 0 || dist < (s_Radius[LOD_ - 1])) && LOD_ < 16)
    {
        Vector3 top = (TopLeft_ + TopRight_) * 0.5; 
        Vector3 bottom = (BottomLeft_ + BottomRight_) * 0.5; 
        Vector3 left = (TopLeft_ + BottomLeft_) * 0.5; 
        Vector3 right = (TopRight_ + BottomRight_) * 0.5; 
        Vector3 center = (TopLeft_ + TopRight_ + BottomLeft_ + BottomRight_) * 0.25; 

        if (CloseEnough_) 
        {
            if (!Children_[0]) Children_[0] = new FaceNode(NodeQueue_, World_, Face_, LOD_+1, TopLeft_, top, left, center); 
            if (!Children_[1]) Children_[1] = new FaceNode(NodeQueue_, World_, Face_, LOD_+1, top, TopRight_, center, right); 
            if (!Children_[2]) Children_[2] = new FaceNode(NodeQueue_, World_, Face_, LOD_+1, left, center, BottomLeft_, bottom); 
            if (!Children_[3]) Children_[3] = new FaceNode(NodeQueue_, World_, Face_, LOD_+1, center, right, bottom, BottomRight_); 
        }
    }

    if (LOD_ == 0) 
    {
        CloseEnough_ = true; 
    }

    if (!Requested_)// && CloseEnough_) 
    {
        NodeQueue_->Add(this); 
        Requested_ = true; 
    }

    return meshesBuilt; 
}

void FaceNode::Render(Transform& tfm) 
{
    if (CanRender()) 
    {
        // GetRenderer()->DrawMesh(Mesh_, Mat_, tfm.GetMatrix()); 

        bool render = true; 

        if (HasChildren()) 
        {
            bool allCanRender = true; 
            for (unsigned i = 0; i < 4; i++) 
            {
                allCanRender = allCanRender && Children_[i] != nullptr && Children_[i]->CanRender(); 
            }
            if (allCanRender) 
            {
                bool anyChildInRange = false; 
                for (unsigned i = 0; i < 4; i++) 
                {
                    anyChildInRange = anyChildInRange || (Children_[i] != nullptr && Children_[i]->CloseEnough_); 
                }
                render = !anyChildInRange; 
            }
        }

        if (render) 
        {
            GetRenderer()->DrawMesh(Mesh_, Mat_, tfm.GetMatrix()); 
            GetRenderer()->DrawMesh(WaterMesh_, WaterMat_, tfm.GetMatrix()); 
        }
        else 
        {
            for (unsigned i = 0; i < 4; i++) 
            {
                Children_[i]->Render(tfm); 
            }
        }
    }
}

void FaceNode::Generate() 
{
    static const unsigned Resolution = 32; 
    static const float InvResolution = 1.0 / Resolution; 
    static const unsigned VertexSize = Resolution * Resolution; 
    static const unsigned IndexSize = (Resolution-1) * (Resolution-1) * 6; 

    MutexLock lock(Lock_); 

    MeshData_ = new MeshData(); 
    auto& verts = MeshData_->Vertices; 
    auto& normals = MeshData_->Normals; 
    auto& colors = MeshData_->Colors; 
    auto& texCoords = MeshData_->TexCoords; 
    auto& inds = MeshData_->Indices; 

    verts.resize(VertexSize); 
    normals.resize(VertexSize); 
    colors.resize(VertexSize); 
    texCoords.resize(VertexSize); 
    inds.resize(IndexSize); 

    WaterMeshData_ = new MeshData(); 
    auto& wverts = WaterMeshData_->Vertices; 
    auto& wnormals = WaterMeshData_->Normals; 
    auto& wcolors = WaterMeshData_->Colors; 
    auto& wtexCoords = WaterMeshData_->TexCoords; 
    auto& winds = WaterMeshData_->Indices; 

    wverts.resize(VertexSize); 
    wnormals.resize(VertexSize); 
    wcolors.resize(VertexSize); 
    wtexCoords.resize(VertexSize); 
    winds.resize(IndexSize); 

    // Vector<Vector3> verts(VertexSize); 
    // Vector<Vector4> colors(VertexSize); 
    // Vector<Vector2> texCoords(VertexSize); 
    // Vector<Vector3> normals(VertexSize); 
    // Vector<uint32> inds(IndexSize); 

    // Vector<Vector3> wverts(VertexSize); 
    // Vector<Vector4> wcolors(VertexSize); 
    // Vector<Vector2> wtexCoords(VertexSize); 
    // Vector<Vector3> wnormals(VertexSize); 
    // Vector<uint32> winds(IndexSize); 

    int anomalies = 0; 

    for (unsigned i = 0; i < VertexSize; i++) 
    {
        unsigned x = i % Resolution; 
        unsigned y = i / Resolution; 

        float fx = (float) (x - InvResolution) / (Resolution-3); 
        float fy = (float) (y - InvResolution) / (Resolution-3); 

        Vector3 pos = Normalized(Lerp(
            Lerp(TopLeft_, TopRight_, fx), 
            Lerp(BottomLeft_, BottomRight_, fx), 
            fy
        )); 

        float freq = 2.0; 
        float pers = 0.65; 
        int iterations = 9; 
        Vector3 offset = {
            FractalNoise(pos, freq, pers, iterations, 0), 
            FractalNoise(pos, freq, pers, iterations, 1), 
            FractalNoise(pos, freq, pers, iterations, 2)
        }; 
        offset *= 0.4; 

        Vector3 cellPos = Normalized(pos + offset); 

        float heightOffset = FractalNoise(cellPos, freq, pers, iterations+5, 3); 
        // float height = 1.0 + heightOffset * 0.01; 
        // Terrain terrain = height > 1.0 ? Terrain::Grassland : Terrain::DeepWater; 

        // FJ_DEBUG("%f %f %f", offset.X, offset.Y, offset.Z); 

        // CellId cCell = World_->GetCellIdPyPosition(pos + offset); 
        // CellId cell = cCell; 

        TriCellId tri = World_->GetTriCellIdPyPosition(pos + offset); 
        tri.Coords = Smooth(tri.Coords); 
        tri.Coords *= 1.0 / (tri.Coords.X + tri.Coords.Y + tri.Coords.Z); 
        // tri.Coords = Vector3(1.0, 0.0, 0.0); 
        CellId cell = tri.A; 

        // float height = 1.0 + Max(0.0f, World_->GetHeight(cell) * (1.0f + 1.0f * heightOffset) - 0.57f) * 0.1; 
        float height = tri.Interpolate<float>([&](CellId id) 
        { 
            return 1.0 + Max(-10.0f, World_->GetHeight(id) * (1.0f + heightOffset * 0.5f) - 0.57f) * 0.04; 
        }); 

        // height = 1.0 + (height - 0.57f) * 0.15; 

        Terrain terrain = World_->GetTerrain(cell); 

        // Color landTerrain = tri.Interpolate<Vector4>([&](CellId id) -> Vector4 {
        //     Terrain t = World_->GetTerrain(id); 
        //     Color colors[] = {
        //         GetTerrainColor(Terrain::Desert), 
        //         GetTerrainColor(t)
        //     };
        //     return GetTerrainColor(t);//colors[IsTerrainLand(t)]; 
        // });

        // Color waterTerrain = tri.Interpolate<Vector4>([&](CellId id) -> Vector4 {
        //     Terrain t = World_->GetTerrain(id); 
        //     Color colors[] = {
        //         GetTerrainColor(Terrain::ShallowWater), 
        //         GetTerrainColor(t)
        //     };
        //     return colors[IsTerrainWater(t)]; 
        // });

        // float height = 1.0; 
        Color landTerrain = GetTerrainColor(IsTerrainWater(terrain) ? Terrain::Desert : terrain); 
        Color waterTerrain = GetTerrainColor(terrain == Terrain::DeepWater ? Terrain::DeepWater : Terrain::ShallowWater); 

        verts[i] = pos * (height); 
        colors[i] = landTerrain; 
        normals[i] = pos; 
        texCoords[i] = {fx, fy}; 

        wverts[i] = pos * 1.0; 
        wcolors[i] = waterTerrain; 
        wnormals[i] = pos; 
        wtexCoords[i] = {fx, fy}; 
    }

    // FJ_FDEBUG("Anomalies: %d", anomalies); 

    for (unsigned y = 0; y < Resolution - 1; y++) 
    {
        for (unsigned x = 0; x < Resolution - 1; x++) 
        {
            unsigned v00 = (x + 0) + (y + 0) * Resolution; 
            unsigned v01 = (x + 0) + (y + 1) * Resolution; 
            unsigned v10 = (x + 1) + (y + 0) * Resolution; 
            unsigned v11 = (x + 1) + (y + 1) * Resolution; 

            unsigned i = (x + y * (Resolution-1)) * 6; 

            inds[i + 0] = v00; 
            inds[i + 1] = v01; 
            inds[i + 2] = v11; 
            inds[i + 3] = v00; 
            inds[i + 4] = v11; 
            inds[i + 5] = v10; 

            winds[i + 0] = v00; 
            winds[i + 1] = v01; 
            winds[i + 2] = v11; 
            winds[i + 3] = v00; 
            winds[i + 4] = v11; 
            winds[i + 5] = v10; 
        }
    }

    // WaterMat_ = new Material(); 
    // WaterMat_->SetShader(s_Material->GetShader()); 
    
    // Mesh_ = new Mesh(); 
    // Mesh_->SetVertices(verts); 
    // Mesh_->SetColors(colors); 
    // Mesh_->SetTexCoords(texCoords); 
    // Mesh_->SetIndices(inds); 
    // Mesh_->CalculateNormals(); 
    // Mesh_->Update(); 

    // WaterMesh_ = new Mesh(); 
    // WaterMesh_->SetVertices(wverts); 
    // WaterMesh_->SetNormals(wnormals); 
    // WaterMesh_->SetColors(wcolors); 
    // WaterMesh_->SetTexCoords(wtexCoords); 
    // WaterMesh_->SetIndices(winds); 
    // WaterMesh_->Update(); 

    MeshDataReady_ = true; 
}