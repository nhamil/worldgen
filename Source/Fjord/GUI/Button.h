#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/GUI/GUIElement.h" 

namespace Fjord 
{

    class Button : public GUIElement 
    {
    public: 
        static const int DefaultWidth = 100; 
        static const int DefaultHeight = 40; 

        Button() : GUIElement(DefaultWidth, DefaultHeight) {} 
        Button(int w, int h) : GUIElement(w, h) {} 
        virtual ~Button() = default; 

        virtual void OnRender(GUIRenderer& r) override; 
        virtual void OnMouseEvent(GUIMouseEvent& e) override; 
        virtual void OnMouseMoveEvent(GUIMouseMoveEvent& e) override; 

    private: 
        bool Down_{false}; 
        bool Hover_{false}; 
    };

}