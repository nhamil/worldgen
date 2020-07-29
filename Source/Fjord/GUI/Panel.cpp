#include "Fjord/GUI/Panel.h" 

namespace Fjord 
{

    void DragPanel::OnMouseEvent(GUIMouseEvent& e) 
    {
        if (e.Button == 1 && e.Down) 
        {
            if (e.IsAvailable() && InBounds(e.X, e.Y)) 
            {
                DragMode_ = true; 
                e.Consume(); 
            }
        }
        else if (e.Button == 1 && !e.Down) 
        {
            DragMode_ = false; 
        }
    }

    void DragPanel::OnMouseMoveEvent(GUIMouseMoveEvent& e) 
    {
        if (DragMode_) 
        {
            if (e.IsAvailable()) 
            {
                SetPosition(
                    GetX() + e.DX * GetXMultiplier(GetAlignment()), 
                    GetY() + e.DY * GetYMultiplier(GetAlignment()) 
                );
                e.Consume(); 
            }
        }
    }

}