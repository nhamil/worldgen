#include "Fjord/GUI/Button.h" 

#include "Fjord/Graphics/Graphics.h" 
#include "Fjord/GUI/GUIRenderer.h" 

namespace Fjord 
{

    void Button::OnMouseEvent(GUIMouseEvent& e) 
    {
        if (e.Button == 1) 
        {
            if (e.Down && InBounds(e.X, e.Y)) 
            {
                Down_ = true; 
                e.Consume(); 
            }
            else 
            {
                // do not consume 
                Down_ = false; 
            }
        }
    }

    void Button::OnMouseMoveEvent(GUIMouseMoveEvent& e) 
    {
        // do not consume 
        Hover_ = InBounds(e.X, e.Y); 
    }

    void Button::OnRender(GUIRenderer& r) 
    {
        Color color = GetColor(); 
        float alpha = color.A; 

        if (Down_) 
        {
            color = (Vector4) color * 0.15f; 
        }
        else if (Hover_) 
        {
            color = (Vector4) color * 0.5f; 
            // color.B *= 2.0f; 
        }

        color.A = 1;//alpha; 

        r.SetColor(color); 
        r.FillRect(0, 0, GetWidth(), GetHeight()); 

        Font* font = GetFont(); 
        if (!font) 
        {
            FJ_EFWARN("Handle null font!"); 
            return; 
        }

        String text = "BigboiButton"; 

        r.SetColor(Color::White); 
        r.DrawString(text, 0, 0, GetWidth(), GetHeight(), Alignment::Center); 
    }

}