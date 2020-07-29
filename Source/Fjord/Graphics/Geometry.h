#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{

    class Geometry : public GPUObject 
    {
    public: 
        static const unsigned MaxVertexBufferCount = 16; 

        Geometry(); 
        virtual ~Geometry(); 

        void Update(); 

        unsigned GetVertexBufferCount() const; 
        bool HasUsableVertexBuffer() const; 

        IndexBuffer* GetIndexBuffer() const; 
        VertexBuffer* GetVertexBuffer(unsigned index) const; 

        void SetIndexBuffer(IndexBuffer* ib); 
        void SetVertexBufferCount(unsigned numBuffers); 
        void SetVertexBuffer(unsigned index, VertexBuffer* vb); 

        void SetVertexBuffers(Vector<VertexBuffer*> buffers) 
        {
            SetVertexBufferCount(buffers.size()); 
            for (unsigned i = 0; i < buffers.size(); i++)  
            {
                SetVertexBuffer(i, buffers[i]); 
            }
        }

    private: 
        Ref<IndexBuffer> IndexBuffer_; 
        Ref<VertexBuffer> VertexBuffers_[MaxVertexBufferCount]; 
        unsigned NumVertexBuffers_ = 0; 
        bool Dirty_ = true; 
    };

}