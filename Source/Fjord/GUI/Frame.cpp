#include "Fjord/GUI/Frame.h" 

namespace Fjord 
{

    Frame::Frame() 
        : GUIElement(DefaultWidth, DefaultHeight)
    {
        ContentPane_ = new Panel(); 
        GUIElement::AddChild(ContentPane_); 
        ContentPane_->SetPosition(4, 34); 
        ContentPane_->SetSize(GetWidth() - 8, GetHeight() - 38); 
        ContentPane_->SetColor({0.5f, 0.5f, 0.5f, 0.75f}); 
    }

    void Frame::SetSize(int w, int h) 
    {
        GUIElement::SetSize(w, h); 
        ContentPane_->SetPosition(4, 34); 
        ContentPane_->SetSize(GetWidth() - 8, GetHeight() - 38); 
    }

    void Frame::AddChild(GUIElement* c) 
    {
        ContentPane_->AddChild(c); 
    }

    bool Frame::RemoveChild(GUIElement* c) 
    {
        return ContentPane_->RemoveChild(c); 
    }

    unsigned Frame::GetChildCount() const 
    {
        return ContentPane_->GetChildCount(); 
    }

    GUIElement* Frame::GetChild(unsigned index) const 
    {
        return ContentPane_->GetChild(index); 
    }

    void Frame::OnMouseEvent(GUIMouseEvent& e) 
    {
        if (e.Down) 
        {
            if (e.Button == 1 && !DragMode_) 
            {
                if (InBounds(e.X, e.Y)) 
                {
                    DragMode_ = true; 
                    XOff_ = -e.X; 
                    YOff_ = -e.Y; 
                    e.Consume(); 
                }
            }
        }
        else 
        {
            DragMode_ = false; 
        }
    }

    void Frame::OnMouseMoveEvent(GUIMouseMoveEvent& e) 
    {
        if (DragMode_) 
        {
            SetPosition(
                GetX() + e.X + XOff_, 
                GetY() + e.Y + YOff_ 
            );
            e.Consume(); 
        }
    }

}