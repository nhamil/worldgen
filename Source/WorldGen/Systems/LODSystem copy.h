#pragma once 

#include <Fjord/Fjord.h> 

#include "WorldGen/LOD/FaceNodeQueue.h" 

using namespace Fjord; 

class LODSystem : public EntitySystem 
{
public: 
    LODSystem(); 

    virtual void Update(float dt) override; 
    virtual void Render() override; 

private: 
    double LastGen_; 
    Ref<FaceNodeQueue> NodeQueue_; 
};