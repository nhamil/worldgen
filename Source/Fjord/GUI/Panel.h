#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/GUI/GUIElement.h" 

namespace Fjord
{
    
    class Panel : public GUIElement 
    {
    public: 
        static const int DefaultWidth = 300; 
        static const int DefaultHeight = 300; 

        Panel() : GUIElement(DefaultWidth, DefaultHeight) {} 
        Panel(int w, int h) : GUIElement(w, h) {}  
        virtual ~Panel() = default; 
    };

} 
