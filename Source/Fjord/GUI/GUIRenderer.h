#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Font.h" 
#include "Fjord/Graphics/Shader.h" 
#include "Fjord/Graphics/SpriteBatch.h" 
#include "Fjord/Graphics/Texture2D.h" 

namespace Fjord 
{

    class GUIRenderer : public RefCounted 
    {
        FJ_NO_COPY(GUIRenderer) 
    public: 
        static const unsigned MaxRectangleCount = 1000; 

        GUIRenderer(); 
        ~GUIRenderer(); 

        void Begin(); 
        void End(); 

        void SetFont(Font* font); 
        void SetColor(Color color); 
        void FillRect(float x, float y, float w, float h); 
        void DrawImage(Texture2D* img, float x, float y, float w, float h); 
        void DrawImage(Texture2D* img, float x, float y, float w, float h, float tx, float ty, float tw, float th); 
        void DrawString(const String& str, float x, float y); 

        // TODO remove 
        void SetZOrder(int z) {}  
        void Flush(); 

    private: 
        static void CreateDefaultResourcesIfNeeded(); 

        Font* GetRenderFont();  

        static Ref<Shader> DefaultTextShader_; 
        static Ref<Font> DefaultFont_; 
        static Ref<Texture2D> WhiteTexture_; 

        Ref<SpriteBatch> SpriteBatch_; 
        Ref<Font> CurFont_; 
        Color CurColor_; 
        int CurZOrder_; 
    };

}