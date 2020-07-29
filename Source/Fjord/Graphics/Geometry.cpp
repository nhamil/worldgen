#include "Fjord/Graphics/Geometry.h" 

#include "Fjord/Graphics/IndexBuffer.h" 
#include "Fjord/Graphics/OpenGL.h"
#include "Fjord/Graphics/VertexBuffer.h" 

namespace Fjord 
{

    Geometry::Geometry() 
    {
        // FJ_EFDEBUG("ctor"); 
        GLCALL(glGenVertexArrays(1, &Handle_)); 
    }

    Geometry::~Geometry() 
    {
        // FJ_EFDEBUG("dtor"); 
        if (Handle_ != 0) 
        {
            GLCALL(glDeleteVertexArrays(1, &Handle_)); 
            Handle_ = 0; 
        }
    }

    void Geometry::Update() 
    {
        if (IndexBuffer_) IndexBuffer_->Update(); 

        for (unsigned i = 0; i < GetVertexBufferCount(); i++) 
        {
            if (VertexBuffers_[i]) VertexBuffers_[i]->Update(); 
        }

        // vertex array update 

        if (!Dirty_) return; 

        GLCALL(glBindVertexArray(Handle_)); 

        GLuint ibo = 0; 
        if (IndexBuffer_) 
        {
            ibo = IndexBuffer_->GetHandle(); 
        }
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); 

        int attribs[(unsigned) Attribute::count]; 

        for (unsigned i = 0; i < (unsigned) Attribute::count; i++) attribs[i] = -1; 

        for (unsigned i = 0; i < GetVertexBufferCount(); i++) 
        {
            auto vb = VertexBuffers_[i].Get(); 

            if (!vb) continue; 

            auto& format = vb->GetFormat(); 

            for (unsigned j = 0; j < format.GetAttributeCount(); j++) 
            {
                Attribute attrib = format.GetAttribute(j); 

                if (attribs[(unsigned) attrib] == -1) attribs[(unsigned) attrib] = i; 
            }
        }

        for (unsigned i = 0; i < (unsigned) Attribute::count; i++) 
        {
            if (attribs[i] == -1) 
            {
                GLCALL(glDisableVertexAttribArray(i)); 
            }
            else 
            {
                GLCALL(glEnableVertexAttribArray(i)); 
                auto vb = VertexBuffers_[attribs[i]].Get(); 
                GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vb->GetHandle())); 
                GLCALL(glVertexAttribPointer(
                    i, 
                    AttributeSize[i], 
                    GL_FLOAT, 
                    GL_FALSE, 
                    vb->GetFormat().GetVertexByteSize(), 
                    (void*) ((size_t) (vb->GetFormat().GetByteOffset((Attribute) i)))
                )); 
            }
        }

        GLCALL(glBindVertexArray(0)); 

        Dirty_ = false; 
    }

    void Geometry::SetIndexBuffer(IndexBuffer* ib) 
    {
        IndexBuffer_ = ib; 
        Dirty_ = true; 
    }

    bool Geometry::HasUsableVertexBuffer() const 
    {
        for (unsigned i = 0; i < GetVertexBufferCount(); i++) 
        {
            if (VertexBuffers_[i]) return true; 
        }

        return false; 
    }

    unsigned Geometry::GetVertexBufferCount() const 
    {
        return NumVertexBuffers_; 
    }

    IndexBuffer* Geometry::GetIndexBuffer() const 
    {
        return IndexBuffer_; 
    }

    VertexBuffer* Geometry::GetVertexBuffer(unsigned index) const 
    {
        FJ_EASSERT(index < GetVertexBufferCount()); 
        return VertexBuffers_[index]; 
    }

    void Geometry::SetVertexBufferCount(unsigned count) 
    {
        FJ_EASSERT(count <= MaxVertexBufferCount); 

        for (unsigned i = count; i < MaxVertexBufferCount; i++) 
        {
            VertexBuffers_[i] = nullptr; 
        }

        NumVertexBuffers_ = count; 
    }

    void Geometry::SetVertexBuffer(unsigned index, VertexBuffer* vb) 
    {
        FJ_EASSERT(index < GetVertexBufferCount()); 

        VertexBuffers_[index] = vb; 
        Dirty_ = true; 
    }

}