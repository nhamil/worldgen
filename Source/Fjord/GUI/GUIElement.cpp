#include "Fjord/GUI/GUIElement.h"

#include "Fjord/Core/Window.h" 
#include "Fjord/GUI/GUIRenderer.h" 

#include "Fjord/Graphics/Graphics.h" 

namespace Fjord 
{

    GUIElement::GUIElement(int w, int h) 
        : Color_(Color::Fuchsia)
        , Width_(w) 
        , Height_(h) {}

    GUIElement::~GUIElement() {}

    void GUIElement::SetPosition(int x, int y) 
    {
        X_ = x; 
        Y_ = y; 
    }

    void GUIElement::SetSize(int w, int h) 
    {
        Width_ = w; 
        Height_ = h; 
    }

    void GUIElement::SetColor(const Color& c) 
    {
        Color_ = c; 
    }

    void GUIElement::SetFont(Font* font) 
    {
        Font_ = font; 
    }

    void GUIElement::SetLayout(Layout* layout) 
    {
        Layout_ = layout; 
    }

    void GUIElement::AddChild(GUIElement* child) 
    {
        FJ_EASSERT(child); 

        if (child->Parent_ == this) return; 

        if (child->Parent_) 
        {
            FJ_EFDEBUG("removing parent"); 
            child->Parent_->RemoveChild(child); 
        }

        child->Parent_ = this; 
        Children_.push_back(child); 
    }

    bool GUIElement::RemoveChild(GUIElement* child) 
    {
        FJ_EASSERT(child); 

        if (child->Parent_ != this) return false; 

        for (auto it = Children_.begin(); it != Children_.end(); it++) 
        {
            if (*it == child) 
            {
                FJ_EFDEBUG("found child"); 
                Children_.erase(it); 
                return true; 
            }
        }

        // if child's parent is this object, it should be in children list 
        FJ_EASSERT(false); 
        return false; 
    }

    void GUIElement::HandleUpdate(float dt) 
    {
        for (auto child : Children_) 
        {
            child->HandleUpdate(dt); 
        }

        UpdateLayout(); 

        OnUpdate(dt); 
    }

    void GUIElement::HandleRender(GUIRenderer& r) 
    {
        UpdateLayout(); 

        OnRender(r); 

        // reverse order to show elements at start of list on top 
        for (auto it = Children_.rbegin(); it != Children_.rend(); it++) 
        {
            Ref<GUIElement> child = *it; 
            r.Push(
                child->GetX(), 
                child->GetY(), 
                child->GetWidth(), 
                child->GetHeight(), 
                true
            ); 
            r.SetColor(child->GetColor()); 
            r.SetFont(child->GetFont()); 
            child->HandleRender(r); 

            r.Pop(); 
        }
    }

    void GUIElement::OnUpdate(float dt) 
    {
        (void) dt; 
    }

    void GUIElement::OnRender(GUIRenderer& r) 
    {
        r.SetColor(GetColor()); 
        r.FillRect(0, 0, GetWidth(), GetHeight()); 
    }

    bool GUIElement::InBounds(int gx, int gy) 
    {
        if (gx < 0 || gx > GetWidth()) return false; 
        if (gy < 0 || gy > GetHeight()) return false; 
        return true; 
    }

    void GUIElement::HandleMouseEvent(GUIMouseEvent& e)
    { 
        for (auto child : Children_) 
        {
            if (e.IsAvailable()) 
            {
                auto e2 = e.Create(-child->GetX(), -child->GetY()); 
                child->HandleMouseEvent(e2); 
                if (e2.IsConsumed()) e.Consume(); 
            }
            else 
            {
                break; 
            }
        }

        if (e.IsAvailable()) OnMouseEvent(e); 
    } 

    void GUIElement::HandleMouseMoveEvent(GUIMouseMoveEvent& e)
    { 
        for (auto child : Children_) 
        {
            if (e.IsAvailable()) 
            {
                auto e2 = e.Create(-child->GetX(), -child->GetY()); 
                child->HandleMouseMoveEvent(e2); 
                if (e2.IsConsumed()) e.Consume(); 
            }
            else 
            {
                break; 
            }
        }

        if (e.IsAvailable()) OnMouseMoveEvent(e); 
    } 

    void GUIElement::HandleMouseWheelEvent(GUIMouseWheelEvent& e)
    { 
        for (auto child : Children_) 
        {
            if (e.IsAvailable()) 
            {
                auto e2 = e.Create(-child->GetX(), -child->GetY()); 
                child->HandleMouseWheelEvent(e2); 
                if (e2.IsConsumed()) e.Consume(); 
            }
            else 
            {
                break; 
            }
        }

        if (e.IsAvailable()) OnMouseWheelEvent(e); 
    } 

    void GUIElement::HandleKeyEvent(GUIKeyEvent& e)
    { 
        for (auto child : Children_) 
        {
            if (e.IsAvailable()) 
            {
                child->HandleKeyEvent(e); 
            }
            else 
            {
                break; 
            }
        }

        if (e.IsAvailable()) OnKeyEvent(e); 
    } 

    void GUIElement::OnMouseEvent(GUIMouseEvent& e)
    { 
        if (e.IsAvailable() && InBounds(e.X, e.Y)) 
        {
            e.Consume(); 
        }
    } 

    void GUIElement::OnMouseMoveEvent(GUIMouseMoveEvent& e)
    { 
        (void) e; 
    } 

    void GUIElement::OnMouseWheelEvent(GUIMouseWheelEvent& e)
    { 
        if (InBounds(e.X, e.Y)) 
        {
            e.Consume(); 
        }
    } 

    void GUIElement::OnKeyEvent(GUIKeyEvent& e)
    { 
        (void) e; 
    } 

    void GUIElement::UpdateLayout() 
    {
        for (auto child : Children_) 
        {
            child->UpdateLayout(); 
        }

        if (Layout_) Layout_->Apply(this); 
    }

}