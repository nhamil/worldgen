#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    class Application
    {
    public: 
        Application(); 
        virtual ~Application();  

        virtual void Init() {} 

        virtual void Start() {} 

        virtual void PreUpdateGUI() {}

        virtual void PostUpdateGUI() {}

        virtual void PreUpdate(float dt) { (void) dt; } 

        virtual void PostUpdate(float dt) { (void) dt; } 

        virtual void PreRender() {} 

        virtual void PostRender() {} 

        virtual void Stop() {} 
    };

}