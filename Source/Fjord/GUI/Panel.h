#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/GUI/Container.h" 
#include "Fjord/GUI/GUIRenderer.h" 

namespace Fjord 
{

    class Panel : public Container 
    {
    public: 
        Panel(int x, int y, int w, int h) 
            : Container(x, y, w, h) {} 
        virtual ~Panel() = default; 

        virtual void OnRender(GUIRenderer* r) override 
        {
            r->SetColor(GetColor()); 
            r->FillRect(GetGlobalX(), GetGlobalY(), GetWidth(), GetHeight()); 
        }            
    };

    class DragPanel : public Panel 
    {
    public: 
        DragPanel(int x, int y, int w, int h) 
            : Panel(x, y, w, h) {} 
        virtual ~DragPanel() = default; 

        virtual void OnMouseEvent(GUIMouseEvent& e) override; 
        virtual void OnMouseMoveEvent(GUIMouseMoveEvent& e) override; 

    private: 
        bool DragMode_ = false; 
    }; 

}