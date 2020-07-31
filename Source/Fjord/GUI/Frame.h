#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/GUI/GUIElement.h" 
#include "Fjord/GUI/Panel.h" 

namespace Fjord 
{

    /**
     * Draggable container 
     */ 
    class Frame : public GUIElement 
    {
    public: 
        static const int DefaultWidth = 300; 
        static const int DefaultHeight = 200; 

        Frame(); 
        virtual ~Frame() = default; 

        virtual void SetSize(int w, int h); 

        virtual void AddChild(GUIElement* child) override; 
        virtual bool RemoveChild(GUIElement* child) override; 
        virtual unsigned GetChildCount() const override; 
        virtual GUIElement* GetChild(unsigned index) const override; 

        virtual void OnMouseEvent(GUIMouseEvent& e) override; 
        virtual void OnMouseMoveEvent(GUIMouseMoveEvent& e) override; 

    private: 
        int XOff_{0}; 
        int YOff_{0}; 
        bool DragMode_{false}; 
        Ref<Panel> ContentPane_; 
    };

}