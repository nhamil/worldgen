#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/GUI/GUIElement.h" 

namespace Fjord 
{

    class GUIEnvironment : public GUIElement 
    {
    public: 
        GUIEnvironment(int width, int height) 
            : GUIElement(width, height) {} 
        virtual ~GUIEnvironment() = default; 

        virtual void OnRender(GUIRenderer& r) override { (void) r; } 

        virtual void OnKeyEvent(GUIKeyEvent& e) { (void) e; } 
        virtual void OnMouseEvent(GUIMouseEvent& e) { (void) e; } 
        virtual void OnMouseMoveEvent(GUIMouseMoveEvent& e) { (void) e; } 
        virtual void OnMouseWheelEvent(GUIMouseWheelEvent& e) { (void) e; } 
    };

}