#include "WorldGen/LOD/FaceNodeQueue.h" 

#include <algorithm> 

void FaceNodeQueue::Add(FaceNode* node) 
{
    MeshRequests_.push_back({node}); 
}

void FaceNodeQueue::Remove(FaceNode* node) 
{
    for (unsigned i = 0; i < MeshRequests_.size(); i++) 
    {
        if (MeshRequests_[i].Node == node) 
        {
            MeshRequests_.erase(MeshRequests_.begin() + i);
            return;  
        }
    }
}

void FaceNodeQueue::Sort(Vector3 pos) 
{
    std::sort(MeshRequests_.begin(), MeshRequests_.end(), [&](FaceNodeMeshRequest& a, FaceNodeMeshRequest& b) 
    {
        if (a.Node->GetLOD() == b.Node->GetLOD()) 
        {
            // return Length2(pos - a.Node->GetCenter()) < Length2(pos - b.Node->GetCenter()); 
            return Angle(pos, a.Node->GetCenter()) < Angle(pos, b.Node->GetCenter()); 
        }
        else 
        {
            return a.Node->GetLOD() < b.Node->GetLOD(); 
        }
    }); 
}

FaceNode* FaceNodeQueue::Peek() 
{
    if (MeshRequests_.size()) 
    {
        return MeshRequests_[0].Node; 
    }
    return nullptr; 
}

FaceNode* FaceNodeQueue::Poll() 
{
    if (MeshRequests_.size()) 
    {
        FaceNode* node = MeshRequests_[0].Node; 
        MeshRequests_.erase(MeshRequests_.begin()); 
        return node; 
    }
    return nullptr; 
}