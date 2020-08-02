#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Core/Application.h" 
#include "Fjord/Core/Engine.h" 
#include "SDL2/SDL.h"
#define ENGINE_MAIN_CLASS(className) \
    int main(int argc, char* argv[]) \
    { \
        (void) argc; \
        (void) argv; \
        className* app = new className(); \
        int status = Fjord::Start(app); \
        delete app; \
        return status; \
    } 
