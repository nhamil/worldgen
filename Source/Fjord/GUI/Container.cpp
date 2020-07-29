#include "Fjord/GUI/Container.h" 

namespace Fjord 
{

    Container::Container(int x, int y, int w, int h) 
        : GUIElement(x, y, w, h) 
    {

    }

    Container::~Container() 
    {

    }

    void Container::AddChild(GUIElement* child) 
    {
        FJ_EASSERT(child != nullptr); 
        // keep tmp strong ref
        Ref<GUIElement> c = child; 

        if (child->Parent_.IsValid()) 
        {
            ((Container*) child->Parent_.Get())->RemoveChild(child); 
        }

        child->Parent_ = this; 
        Children_.push_back(child); 
    }

    bool Container::RemoveChild(GUIElement* child) 
    {
        // keep tmp strong ref
        Ref<GUIElement> c = child; 
        bool removed = false; 
        for (auto it = Children_.begin(); it != Children_.end(); it++) 
        {
            if ((*it).Get() == child) 
            {
                child->Parent_ = nullptr; 
                removed = true; 
                it = Children_.erase(it); 
            }
        }
        return removed; 
    }

    void Container::Update(float dt) 
    {
        ComputeBounds(); 

        OnUpdate(dt); 

        // TODO what if child is added or removed 
        for (auto child : Children_) 
        {
            child->Update(dt); 
        }
    }

    void Container::Render(GUIRenderer* r) 
    {
        OnRender(r); 

        // TODO what if child is added or removed 
        for (auto child : Children_) 
        {
            child->Render(r); 
        }
    }

    void Container::OnRender(GUIRenderer* r) { (void) r; } 

    void Container::HandleMouseEvent(GUIMouseEvent& e)
    { 
        for (auto child : Children_) 
        {
            child->HandleMouseEvent(e); 
        }

        OnMouseEvent(e); 
    } 

    void Container::HandleMouseMoveEvent(GUIMouseMoveEvent& e)
    { 
        for (auto child : Children_) 
        {
            child->HandleMouseMoveEvent(e); 
        }

        OnMouseMoveEvent(e); 
    } 

    void Container::HandleMouseWheelEvent(GUIMouseWheelEvent& e)
    { 
        for (auto child : Children_) 
        {
            child->HandleMouseWheelEvent(e); 
        }

        OnMouseWheelEvent(e); 
    } 

    void Container::HandleKeyEvent(GUIKeyEvent& e)
    { 
        for (auto child : Children_) 
        {
            child->HandleKeyEvent(e); 
        }

        OnKeyEvent(e); 
    } 

}