#pragma once 

#include <Fjord/Fjord.h> 

using namespace Fjord; 

enum class SphereFace  
{
    Top, 
    Bottom, 
    Left, 
    Right, 
    Front, 
    Back, 

    count 
};
static const unsigned SphereFaceCount = (unsigned) SphereFace::count; 

enum class FaceCorner 
{
    TopLeft, 
    TopRight, 
    BottomLeft, 
    BottomRight, 

    count
};
static const unsigned FaceCornerCount = (unsigned) FaceCorner::count; 

class SphereQuadTree; 

class SphereQuadTreeNode : public RefCounted 
{
public: 
    unsigned GetLOD() const { return LOD_; } 

    Texture2D* GetHeightmap() const { return Heightmap_; } 
    bool HasHeightmap() const { return Heightmap_ != nullptr; } 

    Mesh* GetMesh() const; 
    Material* GetMaterial() const; 

    const Vector3& GetCenter() const 
    { 
        return Center_; 
    }

    SphereFace GetFace() const 
    {
        return Face_; 
    }

    const Vector3& GetCorner(FaceCorner corner) const 
    {
        return Corners_[(unsigned) corner]; 
    }

    SphereQuadTreeNode* GetChild(FaceCorner corner) 
    {
        return Children_[(unsigned) corner]; 
    }

    bool IsClose() const 
    { 
        return Close_; 
    }

    void Update(const Vector3& relCamPos); 
    void Render(const Matrix4& transform); 

private: 
    friend class SphereQuadTree; 

    static Ref<Mesh> FaceMesh_; 
    static Ref<Shader> FaceShader_; 

    SphereQuadTreeNode(SphereQuadTree* tree, SphereFace face, unsigned lod, Vector3 tl, Vector3 tr, Vector3 bl, Vector3 br); 

    WeakRef<SphereQuadTree> QuadTree_; 
    Ref<SphereQuadTreeNode> Children_[FaceCornerCount]; 
    Vector3 Corners_[FaceCornerCount]; 
    Vector3 Center_; 
    SphereFace Face_; 
    Ref<Texture2D> Heightmap_; 
    Ref<Material> Material_; 
    unsigned LOD_; 
    bool Close_ = false; 
};

class SphereQuadTree : public RefCounted 
{
public: 
    SphereQuadTree(const Vector<float>& lodRanges); 

    unsigned GetMaxLOD() const; 
    float GetLODRange(unsigned lod) const; 
    void SetLODRange(unsigned lod, float range); 

    SphereQuadTreeNode* GetFace(SphereFace face) const 
    {
        return Faces_[(unsigned) face]; 
    }

    void Update(const Vector3& relativeCamPos); 
    void Render(const Matrix4& transform); 

    float MinHeight_ = 1.0; 
    float MaxHeight_ = 1.5; 

private: 
    Ref<SphereQuadTreeNode> Faces_[SphereFaceCount]; 
    Vector<float> LODRanges_; 
};