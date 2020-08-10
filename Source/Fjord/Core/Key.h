#pragma once 

#include "Fjord/Common.h" 

#include <SDL2/SDL_keycode.h> 

namespace Fjord 
{

    enum Key : uint32
    {
        KeyUnknown = 0, 

        KeyBackQuote = SDLK_BACKQUOTE, 
        KeyQuote = SDLK_QUOTE, 
        KeyMinus = SDLK_MINUS, 
        KeyEquals = SDLK_EQUALS, 
        KeyLBracket = SDLK_LEFTBRACKET, 
        KeyRBracket = SDLK_RIGHTBRACKET, 
        KeyBackslash = SDLK_BACKSLASH, 
        KeySlash = SDLK_SLASH, 
        KeyComma = SDLK_COMMA, 
        KeyPeriod = SDLK_PERIOD, 
        KeySemicolon = SDLK_SEMICOLON, 

        KeyA = SDLK_a, 
        KeyB, 
        KeyC, 
        KeyD, 
        KeyE, 
        KeyF, 
        KeyG, 
        KeyH, 
        KeyI, 
        KeyJ, 
        KeyK, 
        KeyL, 
        KeyM, 
        KeyN, 
        KeyO, 
        KeyP, 
        KeyQ, 
        KeyR, 
        KeyS, 
        KeyT, 
        KeyU, 
        KeyV, 
        KeyW, 
        KeyX, 
        KeyY, 
        KeyZ, 

        Key0 = SDLK_0, 
        Key1, 
        Key2, 
        Key3, 
        Key4, 
        Key5, 
        Key6, 
        Key7, 
        Key8, 
        Key9, 

        KeyF1, 
        KeyF2, 
        KeyF3, 
        KeyF4, 
        KeyF5, 
        KeyF6, 
        KeyF7, 
        KeyF8, 
        KeyF9, 
        KeyF10, 
        KeyF11, 
        KeyF12, 

        KeyEscape = SDLK_ESCAPE, 
        KeyEnter = SDLK_RETURN, 
        KeyBackspace = SDLK_BACKSPACE, 
        KeySpace = SDLK_SPACE, 
        KeyLShift = SDLK_LSHIFT, 
        KeyRShift = SDLK_RSHIFT, 
        KeyLAlt = SDLK_LALT, 
        KeyRAlt = SDLK_RALT, 
        KeyLControl = SDLK_LCTRL, 
        KeyRControl = SDLK_RCTRL
    }; 

}