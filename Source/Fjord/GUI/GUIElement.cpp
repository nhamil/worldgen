#include "Fjord/GUI/GUIElement.h"

#include "Fjord/Core/Window.h" 
#include "Fjord/GUI/GUIRenderer.h" 

namespace Fjord 
{

    GUIElement::GUIElement(int x, int y, int w, int h) 
        : Alignment_(Alignment::TopLeft)
        , Color_(Color::Fuchsia)
        , ZOrder_(DefaultZOrder)
        , X_(x) 
        , Y_(y) 
        , Width_(w) 
        , Height_(h)
        , GlobalX_(x) 
        , GlobalY_(y) {}

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

    void GUIElement::SetZOrder(int z) 
    {
        ZOrder_ = z; 
    }

    void GUIElement::SetColor(const Color& c) 
    {
        Color_ = c; 
    }

    void GUIElement::SetAlignment(Alignment a) 
    {
        Alignment_ = a; 
    }

    void GUIElement::Update(float dt) 
    {
        ComputeBounds(); 
        OnUpdate(dt); 
    }

    void GUIElement::Render(GUIRenderer* r) 
    {
        OnRender(r); 
    }

    void GUIElement::OnUpdate(float dt) 
    {
        (void) dt; 
    }

    void GUIElement::OnRender(GUIRenderer* r) 
    {
        r->SetColor(GetColor()); 
        r->FillRect(GetGlobalX(), GetGlobalY(), GetWidth(), GetHeight()); 
    }

    bool GUIElement::InBounds(int gx, int gy) 
    {
        if (gx < GetGlobalX() || gx > GetGlobalX() + GetWidth()) return false; 
        if (gy < GetGlobalY() || gy > GetGlobalY() + GetHeight()) return false; 
        return true; 
    }

    void GUIElement::HandleMouseEvent(GUIMouseEvent& e)
    { 
        OnMouseEvent(e); 
    } 

    void GUIElement::HandleMouseMoveEvent(GUIMouseMoveEvent& e)
    { 
        OnMouseMoveEvent(e); 
    } 

    void GUIElement::HandleMouseWheelEvent(GUIMouseWheelEvent& e)
    { 
        OnMouseWheelEvent(e); 
    } 

    void GUIElement::HandleKeyEvent(GUIKeyEvent& e)
    { 
        OnKeyEvent(e); 
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

    void GUIElement::ComputeBounds() 
    {
        GUIElement* p = GetParent(); 
        auto window = GetWindow(); 

        int gLeft = p ? p->GetGlobalX() : 0; 
        int gRight = p ? (gLeft + p->GetWidth()) : window->GetWidth(); 

        int gTop = p ? p->GetGlobalY() : 0; 
        int gBot = p ? (gTop + p->GetHeight()) : window->GetHeight(); 

        int x, y; 

        if (IsAlignmentTop(Alignment_)) 
        {
            y = gTop + GetY(); 
        }
        else if (IsAlignmentBottom(Alignment_)) 
        {
            y = gBot - GetY() - GetHeight(); 
        }
        else // center 
        {
            y = (gTop + gBot) / 2 + GetY() - GetHeight() / 2; 
        }

        if (IsAlignmentLeft(Alignment_)) 
        {
            x = gLeft + GetX(); 
        }
        else if (IsAlignmentRight(Alignment_)) 
        {
            x = gRight - GetX() - GetWidth(); 
        }
        else // center 
        {
            x = (gLeft + gRight) / 2 + GetX() - GetWidth() / 2; 
        }

        GlobalX_ = x; 
        GlobalY_ = y; 
    }

}