#include "WorldGen/LOD/SphereQuadTree.h" 

#include "WorldGen/Noise.h" 

SphereQuadTree::SphereQuadTree(const Vector<float>& lodRanges) 
{
    LODRanges_ = lodRanges; 

    Vector3 corners[4]; 
    Vector3& topLeft = corners[0]; 
    Vector3& topRight = corners[1]; 
    Vector3& bottomLeft = corners[2]; 
    Vector3& bottomRight = corners[3]; 

    for (unsigned i = 0; i < SphereFaceCount; i++) 
    {
        SphereFace face = (SphereFace) i; 
        switch (face) 
        {
            case SphereFace::Front: 
                topLeft     = {-1, +1, +1}; 
                topRight    = {+1, +1, +1}; 
                bottomLeft  = {-1, -1, +1}; 
                bottomRight = {+1, -1, +1}; 
                break; 
            case SphereFace::Back: 
                topLeft     = {+1, +1, -1}; 
                topRight    = {-1, +1, -1}; 
                bottomLeft  = {+1, -1, -1}; 
                bottomRight = {-1, -1, -1}; 
                break; 
            case SphereFace::Left: 
                topLeft     = {-1, +1, -1}; 
                topRight    = {-1, +1, +1}; 
                bottomLeft  = {-1, -1, -1}; 
                bottomRight = {-1, -1, +1}; 
                break; 
            case SphereFace::Right: 
                topLeft     = {+1, +1, +1}; 
                topRight    = {+1, +1, -1}; 
                bottomLeft  = {+1, -1, +1}; 
                bottomRight = {+1, -1, -1}; 
                break; 
            case SphereFace::Top: 
                topLeft     = {-1, +1, -1}; 
                topRight    = {+1, +1, -1}; 
                bottomLeft  = {-1, +1, +1}; 
                bottomRight = {+1, +1, +1}; 
                break; 
            case SphereFace::Bottom: 
                topLeft     = {-1, -1, +1}; 
                topRight    = {+1, -1, +1}; 
                bottomLeft  = {-1, -1, -1}; 
                bottomRight = {+1, -1, -1}; 
                break; 
            default: 
                break; 
        }

        Faces_[i] = new SphereQuadTreeNode(this, face, 0, topLeft, topRight, bottomLeft, bottomRight);
    }
}

unsigned SphereQuadTree::GetMaxLOD() const 
{
    return LODRanges_.size(); 
}

float SphereQuadTree::GetLODRange(unsigned lod) const 
{
    return lod == 0 ? INFINITY : LODRanges_[lod - 1]; 
}

void SphereQuadTree::SetLODRange(unsigned lod, float range) 
{
    FJ_ASSERT(lod > 0); 
    LODRanges_[lod - 1] = range; 
}

void SphereQuadTree::Update(const Vector3& relCamPos) 
{
    for (unsigned i = 0; i < SphereFaceCount; i++) 
    {
        Faces_[i]->Update(relCamPos); 
    }
    // Faces_[(int) SphereFace::Front]->Update(relCamPos); 
}

void SphereQuadTree::Render(const Matrix4& transform) 
{
    for (unsigned i = 0; i < SphereFaceCount; i++) 
    {
        Faces_[i]->Render(transform); 
    }
    // Faces_[(int) SphereFace::Front]->Render(transform); 
}

Ref<Mesh> SphereQuadTreeNode::FaceMesh_; 
Ref<Shader> SphereQuadTreeNode::FaceShader_; 

SphereQuadTreeNode::SphereQuadTreeNode(SphereQuadTree* tree, SphereFace face, unsigned lod, Vector3 tl, Vector3 tr, Vector3 bl, Vector3 br) 
{
    Corners_[(int) FaceCorner::TopLeft] = tl; 
    Corners_[(int) FaceCorner::TopRight] = tr; 
    Corners_[(int) FaceCorner::BottomLeft] = bl; 
    Corners_[(int) FaceCorner::BottomRight] = br; 
    
    if (!FaceMesh_) 
    {
        Vector<Vector3> verts; 
        Vector<Vector2> texCoords; 
        Vector<uint32> inds; 
        const int Resolution = 32; 

        for (int y = 0; y < Resolution; y++) 
        {
            for (int x = 0; x < Resolution; x++) 
            {
                float inv = 0;//1.0f / (Resolution - 1); 
                float yf = Lerp<float>(0.0f, 1.0f, (float) (y-0) / (Resolution - 1)); 
                float xf = Lerp<float>(0.0f, 1.0f, (float) (x-0) / (Resolution - 1)); 
                float ytf = Lerp(1.0f-inv, 0.0f+inv, (float) (y-0) / (Resolution - 1)); 
                float xtf = Lerp(0.0f+inv, 1.0f-inv, (float) (x-0) / (Resolution - 1)); 

                // FJ_DEBUG("%f", xtf); 

                verts.push_back({xf, yf, 0}); 
                texCoords.push_back({xtf, ytf}); 
            }
        }

        for (int y = 0; y < Resolution - 1; y++) 
        {
            for (int x = 0; x < Resolution - 1; x++) 
            {
                int v00 = (x + 0) + (y + 0) * (Resolution-0); 
                int v01 = (x + 0) + (y + 1) * (Resolution-0); 
                int v10 = (x + 1) + (y + 0) * (Resolution-0); 
                int v11 = (x + 1) + (y + 1) * (Resolution-0); 

                inds.push_back(v00); 
                inds.push_back(v01); 
                inds.push_back(v11); 
                inds.push_back(v00); 
                inds.push_back(v11); 
                inds.push_back(v10); 
            }
        }

        FaceMesh_ = new Mesh(); 
        FaceMesh_->SetVertices(verts); 
        FaceMesh_->SetTexCoords(texCoords); 
        FaceMesh_->SetIndices(inds); 
        FaceMesh_->Update(); 
    }

    if (!FaceShader_) 
    {
        FaceShader_ = Shader::Load("Sphere"); 
    }

    Material_ = new Material(); 
    Material_->SetShader(FaceShader_); 
    Material_->SetVector3("u_TopLeft", Corners_[0]); 
    Material_->SetVector3("u_TopRight", Corners_[1]); 
    Material_->SetVector3("u_BottomLeft", Corners_[2]); 
    Material_->SetVector3("u_BottomRight", Corners_[3]); 

    Center_ = Vector3(0); 
    for (unsigned i = 0; i < FaceCornerCount; i++) 
    {
        Center_ += Corners_[i]; 
    }
    Center_ *= 1.0 / FaceCornerCount; 
    Center_ = Normalized(Center_); 

    QuadTree_ = tree; 
    Face_ = face; 
    LOD_ = lod; 

    const int Resolution = 32; 
    Vector<float> imgData(Resolution * Resolution * 4); 
    for (int y = 0; y < Resolution; y++) 
    {
        float inv = 0;//1.0f / (Resolution - 1); 
        float yf = Lerp<float>(-inv, 1+inv, (float) (y-0) / (Resolution - 1)); 
        for (int x = 0; x < Resolution; x++) 
        {
            float xf = Lerp<float>(-inv, 1+inv, (float) (x-0) / (Resolution - 1)); 
            Vector3 pos = Normalized(
                Lerp(
                    Lerp(tl, tr, xf), 
                    Lerp(bl, br, xf), 
                    yf
                )
            );

            Vector3 offset = {
                FractalNoise(pos, 2.0, 0.65, 5, 0), 
                FractalNoise(pos, 2.0, 0.65, 5, 1), 
                FractalNoise(pos, 2.0, 0.65, 5, 2)
            };

            float height = FractalNoise(pos + offset*0.4, 2.0, 0.65, 5, 4); 
            if (height < 0) height = 0; 
            imgData[(x + y * Resolution)*4] = Map<float>(
                height, 
                0, 1, 
                0, 1
            );
        }
    }

    Heightmap_ = new Texture2D(Resolution, Resolution, TextureFormat::RGBA32F); 
    Heightmap_->SetData(&imgData[0]); 
    Heightmap_->Update(); 
}

Mesh* SphereQuadTreeNode::GetMesh() const 
{
    return FaceMesh_; 
}

Material* SphereQuadTreeNode::GetMaterial() const 
{
    return Material_; 
}

void SphereQuadTreeNode::Update(const Vector3& camPos) 
{
    float range2 = QuadTree_->GetLODRange(LOD_); 
    range2 *= range2; 

    float dist2 = Length2(camPos - Center_); 

    for (unsigned i = 0; i < FaceCornerCount; i++) 
    {
        if (Children_[i]) Children_[i]->Update(camPos); 
    }

    Close_ = dist2 < range2; 

    if (LOD_ < QuadTree_->GetMaxLOD()-1 && dist2 < range2) 
    {
        // FJ_DEBUG("close: %u %d %f %f", LOD_, Face_, Length(camPos - Center_), QuadTree_->GetLODRange(LOD_)); 
        if (!Children_[0]) 
        {
            Vector3 topLeft = GetCorner(FaceCorner::TopLeft); 
            Vector3 topRight = GetCorner(FaceCorner::TopRight); 
            Vector3 bottomLeft = GetCorner(FaceCorner::BottomLeft); 
            Vector3 bottomRight = GetCorner(FaceCorner::BottomRight); 
            Vector3 center = (topLeft + topRight + bottomLeft + bottomRight) * 0.25; 

            Vector3 top = (topLeft + topRight) * 0.5; 
            Vector3 bottom = (bottomLeft + bottomRight) * 0.5; 
            Vector3 left = (topLeft + bottomLeft) * 0.5; 
            Vector3 right = (topRight + bottomRight) * 0.5; 
            
            Children_[(int) FaceCorner::TopLeft] = new SphereQuadTreeNode(QuadTree_, Face_, LOD_ + 1, topLeft, top, left, center); 
            Children_[(int) FaceCorner::TopRight] = new SphereQuadTreeNode(QuadTree_, Face_, LOD_ + 1, top, topRight, center, right); 
            Children_[(int) FaceCorner::BottomLeft] = new SphereQuadTreeNode(QuadTree_, Face_, LOD_ + 1, left, center, bottomLeft, bottom); 
            Children_[(int) FaceCorner::BottomRight] = new SphereQuadTreeNode(QuadTree_, Face_, LOD_ + 1, center, right, bottom, bottomRight); 
        } 
    }
    else if (dist2 > 4 * range2) 
    {
        if (Children_[0]) 
        {
            for (unsigned i = 0; i < FaceCornerCount; i++) 
            {
                Children_[i] = nullptr; 
            }
        } 
    }
}

void SphereQuadTreeNode::Render(const Matrix4& transform) 
{
    bool childClose = false; 
    for (unsigned i = 0; i < FaceCornerCount; i++) 
    {
        if (Children_[i]) 
        {
            childClose |= Children_[i]->IsClose(); 
        }
        else 
        {
            childClose = false; 
            break; 
        }
    }

    // FJ_DEBUG("%u %d", LOD_, childClose); 
    if (childClose) 
    {
        for (unsigned i = 0; i < FaceCornerCount; i++) 
        {
            Children_[i]->Render(transform); 
        }
    }
    else 
    {
        Material* mat = GetMaterial(); 
        mat->SetFloat("u_MinHeight", QuadTree_->MinHeight_); 
        mat->SetFloat("u_MaxHeight", QuadTree_->MaxHeight_); 
        mat->SetTexture("u_Heightmap", Heightmap_); 

        GetRenderer()->DrawMesh(
            GetMesh(), 
            mat, 
            transform
        );
    }
}