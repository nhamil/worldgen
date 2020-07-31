#include "Fjord/GUI/GUIRenderer.h" 

#include "Fjord/Core/Window.h" 
#include "Fjord/Graphics/Graphics.h"
#include "Fjord/GUI/GUIElement.h" 
#include "Fjord/Math/MathUtil.h" 

namespace Fjord 
{

    GUIRenderer::GUIRenderer() 
    {
        SpriteBatch_ = new SpriteBatch(); 
        auto* window = GetWindow(); 

        ElemState s; 
        s.ClipW = window->GetWidth(); 
        s.ClipH = window->GetHeight(); 
        ElemStack_.push_back(s); 
    }

    // GUIRenderer GUIRenderer::Create(const Vector2& translate) 
    // {
    //     // ref counted spritebatch will be handled automatically 
    //     GUIRenderer r = *this; 
    //     r.Translation_ += translate; 
    //     return r; 
    // }

    void GUIRenderer::Push(int clipX, int clipY, int clipW, int clipH, bool clip) 
    {
        ElemState& curState = GetElemState(); 
        ElemState state = GetElemState(); 
        state.Translation += Vector2(clipX, clipY); 
        
        int x = Max(curState.ClipX, (int) state.Translation.X); 
        int y = Max(curState.ClipY, (int) state.Translation.Y); 
        int x2 = Max(Min(curState.ClipX + curState.ClipW, (int) state.Translation.X + clipW), x); 
        int y2 = Max(Min(curState.ClipY + curState.ClipH, (int) state.Translation.Y + clipH), y); 

        state.ClipX = x; 
        state.ClipY = y; 
        state.ClipW = x2 - x; 
        state.ClipH = y2 - y; 
        state.Clip = clip; 

        Flush(); 

        if (clip) 
        {
            SetClip(state.ClipX, state.ClipY, state.ClipW, state.ClipH); 
        }
        else 
        {
            FullClip(); 
        }

        Translation_ = state.Translation; 
        ElemStack_.push_back(state); 
    }
    
    void GUIRenderer::Pop() 
    {
        Flush(); 

        ElemStack_.pop_back(); 
        ElemState& state = GetElemState(); 
        Translation_ = state.Translation; 
        if (state.Clip) 
        {
            SetClip(state.ClipX, state.ClipY, state.ClipW, state.ClipH); 
        }
        else 
        {
            FullClip(); 
        }
    }  

    void GUIRenderer::SetFont(Font* font) 
    {
        Font_ = font; 
    }

    void GUIRenderer::SetColor(Color color) 
    {
        Color_ = color; 
    }

    void GUIRenderer::FillRect(float x, float y, float w, float h)  
    {
        x += Translation_.X; 
        y += Translation_.Y; 
        SpriteBatch_->Draw(Color_, x, y, w, h); 
    }

    void GUIRenderer::DrawString(const String& str, float x, float y) 
    {
        x += Translation_.X; 
        y += Translation_.Y; 
        SpriteBatch_->DrawString(Font_, Color_, str.c_str(), x, y); 
    }

    void GUIRenderer::DrawString(const String& str, float x, float y, float w, float h, Alignment a) 
    {
        if (!Font_) 
        {
            FJ_EFWARN("Font should not need to be non-null!"); 
            return; 
        }
        StringMetrics sm = Font_->GetMetrics(str); 

        if (IsAlignmentLeft(a)) 
        {
            // do nothing 
        }
        else if (IsAlignmentRight(a)) 
        {
            x = w - sm.PxWidth; 
        }
        else // center 
        {
            x = (w - sm.PxWidth) * 0.5f; 
        }

        if (IsAlignmentTop(a)) 
        {
            y = sm.PxHeight; 
        }
        else if (IsAlignmentBottom(a)) 
        {
            y = h; 
        }
        else // center 
        {
            y = (h + sm.PxHeight) * 0.5f; 
        }

        DrawString(str, x, y); 
    }

    void GUIRenderer::SetClip(int x, int y, int w, int h) 
    {
        auto* g = GetGraphics(); 
        g->SetClip(x, y, w, h); 
    }

    void GUIRenderer::FullClip() 
    {
        auto* g = GetGraphics(); 
        g->ResetClip(); 
    }

    void GUIRenderer::Flush() 
    {
        SpriteBatch_->Flush(); 
    }

}