#include "WorldGen/LOD/FaceNodeQueue.h" 

#include <algorithm> 

void FaceNodeQueue::Add(FaceNode* node) 
{
    MutexLock lock(Lock_); 
    MeshRequests_.push_back({node}); 
    // FJ_DEBUG("add to queue"); 
}

void FaceNodeQueue::Remove(FaceNode* node) 
{
    MutexLock lock(Lock_); 
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
    MutexLock lock(Lock_); 
    std::sort(MeshRequests_.begin(), MeshRequests_.end(), [&](FaceNodeMeshRequest& a, FaceNodeMeshRequest& b) 
    {
        if (a.Node->GetLOD() == b.Node->GetLOD()) 
        {
            return Length2(pos - a.Node->GetCenter()) < Length2(pos - b.Node->GetCenter()); 
            // return Angle(pos, a.Node->GetCenter()) < Angle(pos, b.Node->GetCenter()); 
        }
        else 
        {
            return a.Node->GetLOD() < b.Node->GetLOD(); 
        }
    }); 
    LastPos_ = pos; 
}

void FaceNodeQueue::Sort() 
{
    MutexLock lock(Lock_); 
    Vector3 pos = LastPos_; 
    std::sort(MeshRequests_.begin(), MeshRequests_.end(), [&](FaceNodeMeshRequest& a, FaceNodeMeshRequest& b) 
    {
        if (a.Node->GetLOD() == b.Node->GetLOD()) 
        {
            return Length2(pos - a.Node->GetCenter()) < Length2(pos - b.Node->GetCenter()); 
            // return Angle(pos, a.Node->GetCenter()) < Angle(pos, b.Node->GetCenter()); 
        }
        else 
        {
            return a.Node->GetLOD() < b.Node->GetLOD(); 
        }
    }); 
}

FaceNode* FaceNodeQueue::Peek() 
{
    MutexLock lock(Lock_); 
    if (MeshRequests_.size()) 
    {
        return MeshRequests_[0].Node; 
    }
    return nullptr; 
}

FaceNode* FaceNodeQueue::Poll() 
{
    MutexLock lock(Lock_); 
    if (MeshRequests_.size()) 
    {
        FaceNode* node = MeshRequests_[0].Node; 
        MeshRequests_.erase(MeshRequests_.begin()); 
        return node; 
    }
    return nullptr; 
}

FaceNodeGenThread::FaceNodeGenThread(FaceNodeQueue* queue) 
    : Queue_(queue) {} 

void FaceNodeGenThread::Run() 
{
    while (!IsInterrupted()) 
    {
        Queue_->Sort(); 
        FaceNode* node = Queue_->Poll(); 

        if (node) 
        {
            // FJ_FDEBUG("Generating node %d", node->GetLOD()); 
            node->Generate(); 
        }
        else 
        {
            // FJ_FDEBUG("Waiting for node"); 
        }

        Sleep(100); 
    }
}