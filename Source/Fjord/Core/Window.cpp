#include "Fjord/Core/Window.h" 

#include "Fjord/Core/Input.h" 
#include "Fjord/Graphics/OpenGL.h" 

#include <iostream> 

#define KEY_MAP_SIZE 4096

namespace Fjord 
{

    static bool s_SDLInitiated = false; 

    static Key s_SDLKeyToKey[KEY_MAP_SIZE]; 
    
    static Key SDLKeyToKey(SDL_Keycode code) 
    {
        if (code >= KEY_MAP_SIZE) 
        {
            FJ_EFLOG(Warn, "Key out of bounds: %d", code); 
            return KeyUnknown; 
        }

        return s_SDLKeyToKey[code]; 
    }

    void InitSDL() 
    {
        for (unsigned k = KeyA; k <= KeyZ; k++) 
        {
            s_SDLKeyToKey[k] = (Key) k; 
        }

        if (!s_SDLInitiated) 
        {
            s_SDLInitiated = true; 
            SDL_Init(SDL_INIT_VIDEO); 
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        }
    }

    Window::Window(const char* title, int width, int height) 
        : Width_(width) 
        , Height_(height) 
    {
        InitSDL(); 

        Window_ = SDL_CreateWindow(
            title, 
            SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED, 
            width, 
            height, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        ); 

        GLContext_ = SDL_GL_CreateContext(Window_); 
        glewExperimental = GL_TRUE; 
        glewInit(); 

        SDL_GL_SetSwapInterval(0); 
    }

    Window::~Window() 
    {
        SDL_GL_DeleteContext(GLContext_); 
        SDL_DestroyWindow(Window_); 
    }

    void Window::SwapBuffers() 
    {
        SDL_GL_SwapWindow(Window_); 
    }

    void Window::Poll() 
    {
        SDL_Event e; 
        while (SDL_PollEvent(&e)) 
        {
            switch (e.type) 
            {
                case SDL_QUIT: 
                    Fjord::Stop(); 
                    break; 
                case SDL_KEYDOWN: 
                {
                    Key key = SDLKeyToKey(e.key.keysym.sym); 
                    Input_.OnKeyDown(key); 
                    break; 
                }
                case SDL_KEYUP: 
                {
                    Key key = SDLKeyToKey(e.key.keysym.sym); 
                    Input_.OnKeyUp(key); 
                    break; 
                }     
                case SDL_MOUSEMOTION: 
                {
                    float x = e.motion.x; 
                    float y = e.motion.y; 
                    float dx = e.motion.xrel; 
                    float dy = e.motion.yrel; 
                    Input_.OnMouseMove({x, y}, {dx, dy}); 
                    break; 
                }
                case SDL_MOUSEBUTTONDOWN: 
                {
                    int button = e.button.button; 
                    Input_.OnMouseDown(button); 
                    break; 
                }
                case SDL_MOUSEBUTTONUP: 
                {
                    int button = e.button.button; 
                    Input_.OnMouseUp(button); 
                    break; 
                }
                case SDL_MOUSEWHEEL: 
                {
                    int amount = e.wheel.y; 
                    Input_.OnMouseWheel(amount); 
                    break; 
                }
            }
        }
        Input_.OnUpdate(); 
    }

}