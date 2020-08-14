#pragma once 

#include <Fjord/Fjord.h> 
#include <Fjord/Scene/EntitySystem.h> 

using namespace Fjord; 

class WorldGenSystem : public EntitySystem 
{
public: 
    WorldGenSystem(); 

    virtual void UpdateGUI() override; 
    virtual void Update(float dt) override; 
    virtual void Render() override; 
};
