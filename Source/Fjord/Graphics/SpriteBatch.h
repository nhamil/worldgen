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
        static const unsigned MaxSpriteCount = 1000; 

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
        void Draw(
            Texture2D* tex, Color col, 
            float x, float y, float w, float h, 
            float tx, float ty, float tw, float th, 
            float rotX, float rotY, float angle
        ); 

        void DrawString(Color col, const char* str, float x, float y); 
        void DrawString(Color col, float size, const char* str, float x, float y); 

        void DrawString(Font* font, Color col, const char* str, float x, float y); 
        void DrawString(Font* font, Color col, float size, const char* str, float x, float y); 

        void SetShader(Shader* shader); 
        void SetTextShader(Shader* shader); 
        
    private: 
        static void CreateDefaultResourcesIfNeeded(); 
        
        static Ref<Shader> DefaultShader_, DefaultTextShader_; 
        static Ref<Texture2D> WhiteTexture_; 
        static Ref<Font> DefaultFont_; 

        Shader* GetDrawShader(); 
        Texture2D* GetDrawTexture(); 

        void SetTextMode(bool text); 

        void SetTexture(Texture2D* tex); 

        void AddRect(VertexPositionTextureColor* rect); 

        Ref<VertexBuffer> Vertices_; 
        Ref<IndexBuffer> Indices_; 
        Ref<Geometry> Geometry_; 
        Ref<Shader> Shader_, TextShader_; 
        Ref<Texture2D> Texture_; 
        unsigned CurRect_ = 0; 
        bool TextMode_ = false; 
    };

}