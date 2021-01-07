#pragma once 

#include <Fjord/Fjord.h> 

using namespace Fjord; 

class LODSystem : public EntitySystem 
{
public: 
    LODSystem(); 

    virtual void UpdateGUI() override; 
    virtual void Update(float dt) override; 
    virtual void Render() override; 
private: 
};