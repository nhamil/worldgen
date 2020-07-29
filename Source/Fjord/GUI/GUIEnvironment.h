#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/GUI/Container.h" 

namespace Fjord 
{

    class GUIEnvironment : public Container 
    {
    public: 
        GUIEnvironment(int width, int height) 
            : Container(0, 0, width, height) {} 
        virtual ~GUIEnvironment() = default; 

        virtual void OnRender(GUIRenderer* r) override { (void) r; } 

        virtual void OnKeyEvent(GUIKeyEvent& e) { (void) e; } 
        virtual void OnMouseEvent(GUIMouseEvent& e) { (void) e; } 
        virtual void OnMouseMoveEvent(GUIMouseMoveEvent& e) { (void) e; } 
        virtual void OnMouseWheelEvent(GUIMouseWheelEvent& e) { (void) e; } 
    };

}