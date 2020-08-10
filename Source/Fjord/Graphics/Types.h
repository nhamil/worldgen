#pragma once 

#include "Fjord/Common.h" 

namespace Fjord
{
    
    class Color; 
    class VertexBuffer; 
    class IndexBuffer; 
    class Geometry; 
    class Shader; 
    class Texture2D; 
    class VertexFormat; 
    class GraphicsAPI; 
    
    using GPUHandle = unsigned; 

    class GPUObject : public RefCounted 
    {
        FJ_NO_COPY(GPUObject) 
    public: 
        GPUObject() = default; 
        virtual ~GPUObject() = default; 

        GPUHandle GetHandle() const { return Handle_; }
        
    protected: 
        GPUHandle Handle_ = 0; 
        GraphicsAPI* GetAPI(); 
    };

    enum class TextureFormat 
    {
        R8, 
        RG8, 
        RGB8, 
        RGBA8
    };

    inline unsigned GetTextureFormatByteCount(TextureFormat format) 
    {
        switch (format) 
        {
            case TextureFormat::RGBA8: return 4; 
            case TextureFormat::RGB8: return 3; 
            case TextureFormat::RG8: return 2; 
            case TextureFormat::R8: return 1; 
            default: 
                FJ_EFWARN("Unsupported texture format: %d", format); 
                return 1; 
        }
    }

    enum class AddressMode 
    {
        Clamp, 
        Repeat
    }; 

    enum class TextureAxis 
    {
        X, 
        Y
    };

    enum class TextureFilter 
    {
        Nearest, 
        Linear, 
        Bilinear, 
        Trilinear
    }; 

    enum class Primitive 
    {
        Points, 
        Lines, 
        Triangles
    };

    enum class BlendMode 
    {
        /**
         * All color 
         */ 
        One, 

        /** 
         * No color 
         */ 
        Zero, 

        /**
         * Color multiplied by the source alpha 
         */ 
        SourceAlpha, 

        /**
         * Color multiplied by (1 - source alpha) 
         */ 
        OneMinusSourceAlpha, 
    };

    enum class RenderQueue 
    {
        /**
         * No transparency, fastest
         */ 
        Opaque, 

        /**
         * Fully opaque or fully transparent 
         */ 
        Transparent, 

        /**
         * Partial transparency, avoid when possible as it is slower and less accurate
         */ 
        Translucent 
    };

    enum class FillMode 
    {
        /** 
         * Draw full triangles 
         */ 
        Full, 

        /** 
         * Wireframe 
         */ 
        Line, 

        /** 
         * Points 
         */ 
        Point 
    };

    inline unsigned GetPrimitiveElementCount(Primitive prim) 
    {
        switch (prim) 
        {
            case Primitive::Points: return 1; 
            case Primitive::Lines: return 2; 
            case Primitive::Triangles: return 3; 
            default: 
                FJ_EFLOG(Warn, "Unknown primitive type: %d", prim); 
                return 0; 
        }
    }

    enum class FrontFace 
    {
        /** 
         * No culling 
         */ 
        Both, 

        /**
         * Forward faces are in a clockwise order 
         */ 
        CW, 

        /** 
         * Forward faces are in a counter-clockwise order 
         */ 
        CCW 
    };

    enum class LightType 
    {
        Directional, 
        Point, 
        // Spot
    };

    enum class BufferUsage 
    {
        Static, 
        Dynamic, 
        Stream
    };

    enum class Attribute 
    {
        Position, /* 3f */ 
        Normal, /* 3f */ 
        Tangent, /* 3f */ 
        Texture0, /* 2f */ 
        Texture1, /* 2f */ 
        Color0, /* 4f */ 
        Color1, /* 4f */ 

        count
    };

    const unsigned AttributeSize[] = 
    {
        3, 3, 3, 
        2, 2, 
        4, 4 
    };

    struct GraphicsState 
    {
        BlendMode SourceBlendMode = BlendMode::One; 
        BlendMode DestBlendMode = BlendMode::Zero; 
        Fjord::FillMode FillMode = Fjord::FillMode::Full; 
        Fjord::FrontFace FrontFace = Fjord::FrontFace::CCW; 
        bool DepthWrite = true; 
        bool DepthTest = true; 
    };

} 
