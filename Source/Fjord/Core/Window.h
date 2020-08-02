#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Core/Input.h" 
#include "Fjord/Util/Thread.h" 

#include <SDL2/SDL.h> 

namespace Fjord 
{

    class WindowEventList; 

    class Window 
    {
    public: 
        Window(const char* title, int width = 800, int height = 600); 
        ~Window(); 

        void StartPollingThread(); 

        void SwapBuffers(); 
        void Poll(); 

        int GetWidth() const { return Width_; } 
        int GetHeight() const { return Height_; } 

        Input* GetInput() 
        {
            return &Input_; 
        }

    private: 
        Mutex EventMutex_; 
        int Width_; 
        int Height_; 
        Input Input_; 
        SDL_Window* Window_; 
        SDL_GLContext GLContext_; 
        WindowEventList* EventList_; 
    };

}