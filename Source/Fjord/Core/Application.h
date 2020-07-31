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

        virtual void UpdateGUI(float dt) { (void) dt; }

        virtual void Update(float dt) { (void) dt; } 

        virtual void Render() {} 

        virtual void Stop() {} 
    };

}