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
    void Sort(); 

    FaceNode* Peek(); 
    FaceNode* Poll(); 

private: 
    Mutex Lock_; 
    Vector<FaceNodeMeshRequest> MeshRequests_; 
    Vector3 LastPos_; 
};

class FaceNodeGenThread : public Thread 
{
public: 
    FaceNodeGenThread(FaceNodeQueue* queue); 

    virtual void Run() override; 

private: 
    FaceNodeQueue* Queue_; 
};