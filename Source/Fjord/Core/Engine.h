#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Core/Ref.h" 

namespace Fjord 
{

    class Application; 
    class Graphics; 
    class Input; 
    class Logger; 
    class Renderer; 
    class Scene; 
    class Window; 

    int Start(Application* app); 

    void Stop(bool immediately = false); 

    float GetFPS(); 

    float GetUPS(); 

    Logger* GetLogger(); 

    Logger* GetEngineLogger(); 

    Window* GetWindow(); 

    Input* GetInput(); 

    Scene* GetScene(); 

    Graphics* GetGraphics(); 

    Renderer* GetRenderer(); 

}