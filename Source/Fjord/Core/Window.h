#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Core/Input.h" 

#include <SDL2/SDL.h> 

namespace Fjord 
{

    class WindowEventList; 

    enum class WindowMode 
    {
        Windowed, 
        Fullscreen, 
        Borderless 
    };

    class Window 
    {
    public: 
        Window(const char* title, int width = 800, int height = 600); 
        ~Window(); 

        void SwapBuffers(); 
        void Poll(); 

        int GetWidth() const; 
        int GetHeight() const; 

        void SetSize(int w, int h); 

        void SetVSync(bool enabled); 
        bool IsVSyncEnabled() const; 

        void SetMode(WindowMode mode); 

        WindowMode GetMode() const; 

        Input* GetInput() 
        {
            return &Input_; 
        }

    private: 
        bool VSync_; 
        WindowMode Mode_; 
        int X_, Y_; 
        int Width_, Height_; 
        Input Input_; 
        SDL_Window* Window_; 
        SDL_GLContext GLContext_; 
    };

}