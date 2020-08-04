#include "Fjord/Core/Window.h" 

#include "Fjord/Core/Input.h" 
#include "Fjord/Graphics/OpenGL.h" 
#include "Fjord/Util/Thread.h" 

#include <iostream> 
#include <queue> 

#define KEY_MAP_SIZE 4096

namespace Fjord 
{

    void DoGameLoop(); 

    int WindowEventFilter(void* self, const SDL_Event* event) 
    {
        if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_MOVED)
        {
            DoGameLoop(); 
        }

        return 1;
    }

    static bool s_SDLInitiated = false; 

    static Key s_SDLKeyToKey[KEY_MAP_SIZE]; 
    
    static Key SDLKeyToKey(SDL_Keycode code) 
    {
        if (code >= KEY_MAP_SIZE) 
        {
            // FJ_EFLOG(Warn, "Key out of bounds: %d", code); 
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
            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        }
    }

    Window::Window(const char* title, int width, int height) 
        : VSync_(true)
        , Mode_(WindowMode::Windowed)
        , Width_(width) 
        , Height_(height) 
    {
        InitSDL(); 

        Window_ = SDL_CreateWindow(
            title, 
            SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED, 
            width, 
            height, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE 
        ); 
        SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1"); 
        // SDL_CaptureMouse(SDL_TRUE); 

        GLContext_ = SDL_GL_CreateContext(Window_); 
        glewExperimental = GL_TRUE; 
        glewInit(); 

        SDL_GL_SetSwapInterval(1); 
        
        SDL_SetEventFilter((SDL_EventFilter) &WindowEventFilter, this); 

        SetMode(WindowMode::Windowed); 
    }

    Window::~Window() 
    {
        SDL_GL_DeleteContext(GLContext_); 
        SDL_DestroyWindow(Window_); 
    }

    WindowMode Window::GetMode() const 
    {
        return Mode_; 
    }

    void Window::SetVSync(bool enabled) 
    {
        if (VSync_ == enabled) return; 
        VSync_ = enabled; 
    }

    bool Window::IsVSyncEnabled() const 
    {
        return VSync_; 
    }

    void Window::SetMode(WindowMode mode) 
    {
        if (Mode_ != mode) 
        {
            if (Mode_ == WindowMode::Windowed) 
            {
                Width_ = GetWidth(); 
                Height_ = GetHeight(); 
                SDL_GetWindowPosition(Window_, &X_, &Y_); 
            }
            Mode_ = mode; 
            SDL_Rect b; 
            switch (mode) 
            {
                case WindowMode::Windowed: 
                    SDL_SetWindowBordered(Window_, SDL_TRUE); 
                    SDL_SetWindowFullscreen(Window_, 0); 
                    SDL_SetWindowSize(Window_, Width_, Height_); 
                    SDL_SetWindowPosition(Window_, X_, Y_); 
                    break; 
                case WindowMode::Fullscreen: 
                    SDL_SetWindowFullscreen(Window_, SDL_WINDOW_FULLSCREEN_DESKTOP); 
                    break; 
                case WindowMode::Borderless: 
                    SDL_GetDisplayBounds(SDL_GetWindowDisplayIndex(Window_), &b); 
                    SDL_SetWindowFullscreen(Window_, 0); 
                    SDL_SetWindowBordered(Window_, SDL_FALSE); 
                    SDL_SetWindowPosition(Window_, b.x, b.y); 
                    SDL_SetWindowSize(Window_, b.w, b.h); 
                    break; 
            }
        }
    }

    int Window::GetWidth() const 
    {
        int w; 
        SDL_GetWindowSize(Window_, &w, nullptr); 
        return w; 
    }

    int Window::GetHeight() const 
    {
        int h; 
        SDL_GetWindowSize(Window_, nullptr, &h); 
        return h; 
    }

    void Window::SwapBuffers() 
    {
        SDL_GL_SwapWindow(Window_); 
        SDL_GL_SetSwapInterval(VSync_ ? 1 : 0); 
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
                    // if (fabs(dx) < 1 || fabs(dy) < 1) break; 
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