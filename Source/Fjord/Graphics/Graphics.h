#pragma once 

#include "Fjord/Graphics/Geometry.h" 
#include "Fjord/Graphics/RenderTarget.h" 
#include "Fjord/Graphics/Shader.h" 
#include "Fjord/Graphics/Texture.h" 
#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{

    class GraphicsAPI; 

    class Graphics
    {
    public: 
        static const unsigned MaxTextureCount = 16;

        Graphics(); 
        ~Graphics(); 

        void BeginFrame(); 
        void EndFrame(); 

        GraphicsState GetState(); 
        void SetState(GraphicsState state); 

        void ClearTextures(); 
        void SetTexture(unsigned index, Texture* texture); 
        void SetShader(Shader* shader); 
        void SetGeometry(Geometry* geom); 
        void SetRenderTarget(RenderTarget* target); 

        void SetDepthTest(bool enabled); 

        unsigned GetWidth(); 
        unsigned GetHeight(); 
        unsigned GetScreenWidth(); 
        unsigned GetScreenHeight(); 
        float GetAspectRatio(); 

        void ResetViewport(); 
        void SetViewport(int x, int y, int w, int h); 

        void ResetClip(); 
        void SetClip(int x, int y, int w, int h); 

        void SetBlendMode(BlendMode source, BlendMode dest); 

        void SetPointSize(float size); 
        void SetLineWidth(float width); 

        void SetClearColor(const Color& color); 
        void SetClearDepth(float depth); 
        void Clear(bool color, bool depth); 

        void Draw(Primitive prim, unsigned startInd, unsigned numInds); 
        void DrawIndexed(Primitive prim, unsigned startInd, unsigned numInds); 

        GraphicsAPI* GetAPI() { return API_; } 

    private: 
        void PrepareRenderTarget(); 
        void PrepareDraw(); 

        GraphicsAPI* API_; 

        Ref<Shader> CurShader_; 
        Ref<Geometry> CurGeom_; 
        Ref<Texture> CurTextures_[MaxTextureCount];
        Ref<RenderTarget> CurRenderTarget_; 
    };

}