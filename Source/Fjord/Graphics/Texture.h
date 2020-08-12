#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{

    class Texture : public GPUObject 
    {
    public: 
        virtual TextureType GetType() const = 0; 
    };

}