#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Font.h" 
#include "Fjord/Graphics/Shader.h" 
#include "Fjord/Graphics/SpriteBatch.h" 
#include "Fjord/Graphics/Texture2D.h" 
#include "Fjord/Math/Vector2.h" 

#define FJ_BIT(x) (1 << (x)) 

namespace Fjord 
{

    enum class Alignment 
    {
        TopLeft = FJ_BIT(0) | FJ_BIT(2), 
        Top = FJ_BIT(0), 
        TopRight = FJ_BIT(0) | FJ_BIT(3), 
        Left = FJ_BIT(2), 
        Center = 0, 
        Right = FJ_BIT(3), 
        BottomLeft = FJ_BIT(1) | FJ_BIT(2), 
        Bottom = FJ_BIT(1), 
        BottomRight = FJ_BIT(1) | FJ_BIT(3)  
    };

    inline bool IsAlignmentTop(Alignment a) 
    {
        return (((unsigned) a) & FJ_BIT(0)) > 0; 
    }

    inline bool IsAlignmentBottom(Alignment a) 
    {
        return (((unsigned) a) & FJ_BIT(1)) > 0; 
    }

    inline bool IsAlignmentLeft(Alignment a) 
    {
        return (((unsigned) a) & FJ_BIT(2)) > 0; 
    }

    inline bool IsAlignmentRight(Alignment a) 
    {
        return (((unsigned) a) & FJ_BIT(3)) > 0; 
    }

    inline int GetXMultiplier(Alignment a) 
    {
        return IsAlignmentRight(a) ? -1 : 1; 
    }

    inline int GetYMultiplier(Alignment a) 
    {
        return IsAlignmentBottom(a) ? -1 : 1; 
    }

    class GUIRenderer
    {
    public: 
        static const unsigned MaxRectangleCount = 1000; 

        GUIRenderer(); 
        ~GUIRenderer() = default;  

        GUIRenderer(const GUIRenderer& other) = delete; 
        GUIRenderer& operator=(const GUIRenderer& other) = delete; 

        void Push(int childX, int childY, int childW, int childH, bool clip);
        void Pop();  

        void SetFont(Font* font); 
        void SetColor(Color color); 
        void FillRect(float x, float y, float w, float h); 
        void DrawImage(Texture2D* img, float x, float y, float w, float h); 
        void DrawImage(Texture2D* img, float x, float y, float w, float h, float tx, float ty, float tw, float th); 
        void DrawString(const String& str, float x, float y); 
        void DrawString(const String& str, float x, float y, float w, float h, Alignment alignment); 

        void Flush(); 

    private: 
        struct ElemState 
        {
            ElemState() = default; 
            ElemState(const ElemState& other) = default; 

            Vector2 Translation{}; 
            int ClipX{0}, ClipY{0}, ClipW{0}, ClipH{0}; 
            bool Clip{false}; 
        };

        void SetClip(int x, int y, int w, int h); 
        void FullClip(); 

        ElemState& GetElemState() 
        {
            return ElemStack_.back(); 
        }

        Vector<ElemState> ElemStack_{}; 
        Vector2 Translation_{}; 
        Ref<SpriteBatch> SpriteBatch_{}; 
        Ref<Font> Font_{}; 
        Color Color_{Color::Black}; 
    };

}