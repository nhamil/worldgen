#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/GUI/GUIElement.h" 

namespace Fjord 
{

    class Container : public GUIElement 
    {
    public: 
        Container(int x, int y, int w, int h); 
        virtual ~Container(); 

        unsigned GetChildCount() const { return Children_.size(); } 
        GUIElement* GetChild(unsigned index) const 
        {
            FJ_EASSERT(index < GetChildCount()); 
            return Children_[index]; 
        }

        void AddChild(GUIElement* child); 
        bool RemoveChild(GUIElement* child); 

        virtual void Update(float dt) override; 
        virtual void Render(GUIRenderer* r) override; 

        virtual void HandleKeyEvent(GUIKeyEvent& e) override;  
        virtual void HandleMouseEvent(GUIMouseEvent& e) override;  
        virtual void HandleMouseMoveEvent(GUIMouseMoveEvent& e) override;  
        virtual void HandleMouseWheelEvent(GUIMouseWheelEvent& e) override;  

        virtual void OnRender(GUIRenderer* r) override; 

    private: 
        Vector<Ref<GUIElement>> Children_; 
    };

}