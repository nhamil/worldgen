#pragma once 

#include <Fjord/Fjord.h> 

#include "WorldGen/World.h" 

using namespace Fjord; 

enum class FaceIndex 
{
    Top, 
    Bottom, 
    Left, 
    Right, 
    Front, 
    Back, 

    count 
};
static const unsigned FaceIndexCount = (unsigned) FaceIndex::count; 

class FaceNodeQueue; 

class FaceNode : public RefCounted 
{
public: 
    FaceNode(FaceNodeQueue* queue, World* world, FaceIndex face);  
    FaceNode(FaceNodeQueue* queue, World* world, FaceIndex face, unsigned lod, Vector3 tl, Vector3 tr, Vector3 bl, Vector3 br); 
    ~FaceNode(); 

    int Update(const Vector3& camPosition, bool buildMesh, Vector<FaceNode*>& childQueue); 

    void Render(Transform& tfm); 

    void Generate(); 

    World* GetWorld() const { return World_; } 

    Mesh* GetMesh() const { return Mesh_; } 
    Mesh* GetWaterMesh() const { return WaterMesh_; } 

    Material* GetMaterial() const { return Mat_; } 
    Material* GetWaterMaterial() const { return WaterMat_; } 

    bool CanRender() const; 

    bool HasChildren() const; 

    FaceNode* GetChild(unsigned index) const { return Children_[index]; } 

    const Vector3& GetCenter() const { return Center_; } 

    unsigned GetLOD() const { return LOD_; } 

private: 
    Ref<FaceNode> Children_[4]; 
    FaceNodeQueue* NodeQueue_; 
    FaceIndex Face_; 
    unsigned LOD_ = 0; 
    bool Requested_ = false; 
    Ref<World> World_; 
    Ref<Mesh> Mesh_, WaterMesh_; 
    Ref<Material> Mat_, WaterMat_; 
    Vector3 Center_; 
    Vector3 TopLeft_; 
    Vector3 TopRight_; 
    Vector3 BottomLeft_; 
    Vector3 BottomRight_; 
    bool CloseEnough_ = false; 
    Mutex Lock_; 
    Ref<MeshData> MeshData_, WaterMeshData_; 
    bool MeshDataReady_ = false; 
};