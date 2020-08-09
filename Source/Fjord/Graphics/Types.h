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

} 
