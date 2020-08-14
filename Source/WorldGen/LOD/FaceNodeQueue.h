#pragma once 

#include "WorldGen/LOD/FaceNode.h" 

struct FaceNodeMeshRequest 
{
    WeakRef<FaceNode> Node; 
};

class FaceNodeQueue : public RefCounted
{
public: 
    void Add(FaceNode* node); 
    void Remove(FaceNode* node); 

    void Sort(Vector3 pos); 

    FaceNode* Peek(); 
    FaceNode* Poll(); 

private: 
    Vector<FaceNodeMeshRequest> MeshRequests_; 
};
