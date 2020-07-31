#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Font.h" 
#include "Fjord/Graphics/IndexBuffer.h" 
#include "Fjord/Graphics/Geometry.h" 
#include "Fjord/Graphics/Shader.h" 
#include "Fjord/Graphics/Texture2D.h" 
#include "Fjord/Graphics/VertexBuffer.h" 
#include "Fjord/Math/Vector4.h" 

namespace Fjord 
{

    class SpriteBatch : public RefCounted 
    {
    public: 
        static const unsigned MaxSpriteCount = 10; 

        static Font* GetDefaultFont() 
        {
            CreateDefaultResourcesIfNeeded(); 
            return DefaultFont_; 
        }

        SpriteBatch(); 
        ~SpriteBatch(); 

        void Begin(); 
        void End(); 
        void Flush(); 

        void Draw(Color col, float x, float y, float w, float h); 
        void Draw(Texture2D* tex, float x, float y, float w, float h); 
        void Draw(Texture2D* tex, Color col, float x, float y, float w, float h); 
        void Draw(
            Texture2D* tex, 
            float x, float y, float w, float h, 
            float tx, float ty, float tw, float th
        ); 
        void Draw(
            Texture2D* tex, Color col, 
            float x, float y, float w, float h, 
            float tx, float ty, float tw, float th
        ); 

        void DrawString(Color col, const char* str, float x, float y); 
        void DrawString(Color col, const String& str, float x, float y) { DrawString(col, str.c_str(), x, y); } 

        void DrawString(Color col, float size, const char* str, float x, float y); 

        void DrawString(Font* font, Color col, const char* str, float x, float y); 

        void DrawString(FontFace* font, Color col, float size, const char* str, float x, float y); 

        void SetDepth(float z); 
        void SetShader(Shader* shader); 
        void SetTextShader(Shader* shader); 

        void ClearBounds(); 
        void SetBounds(float x, float y, float w, float h); 
        
    private: 
        static void CreateDefaultResourcesIfNeeded(); 
        
        static Ref<Shader> DefaultShader_, DefaultTextShader_; 
        static Ref<Texture2D> WhiteTexture_; 
        static Ref<Font> DefaultFont_; 

        Shader* GetDrawShader(); 
        Texture2D* GetDrawTexture(); 

        void SetTextMode(bool text); 

        void SetTexture(Texture2D* tex); 

        bool Clip(float& x1, float& y1, float& x2, float& y2, float& xMix1, float& yMix1, float& xMix2, float& yMix2); 

        void AddRect(VertexPositionTextureColor* rect); 

        Ref<VertexBuffer> Vertices_; 
        Ref<IndexBuffer> Indices_; 
        Ref<Geometry> Geometry_; 
        Ref<Shader> Shader_, TextShader_; 
        Ref<Texture2D> Texture_; 
        unsigned CurRect_ = 0, CurTextRect_; 
        bool TextMode_ = false; 
        float Z_ = 0; 

        struct 
        {
            bool Enabled{false}; 
            float X{0}, Y{0}, W{0}, H{0}; 
        } Bounds_; 
    };

}