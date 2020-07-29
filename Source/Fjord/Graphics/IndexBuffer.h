#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{

    class IndexBuffer : public GPUObject
    {
    public: 
        IndexBuffer(unsigned numElements = 0); 
        virtual ~IndexBuffer(); 

        void Update(); 

        unsigned GetElementCount() const; 

        void SetElementCount(unsigned numElements); 
        void SetData(unsigned start, unsigned numElements, const uint32* in); 

        void SetData(const Vector<uint32>& in) 
        {
            SetElementCount(in.size()); 
            // FJ_EFDEBUG("%u %u", in.size(), GetElementCount()); 
            SetData(0, in.size(), &in[0]); 
        }

        void GetData(unsigned start, unsigned numElements, uint32* out) const; 

    private: 
        bool Dirty_ = true; 
        Vector<uint32> Data_; 
    };

}