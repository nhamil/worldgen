#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    class GUIElement; 

    class Layout : public RefCounted 
    {
    public: 
        Layout() = default;  
        virtual ~Layout() = default;  

        virtual void Apply(GUIElement* element) = 0; 
    };

    class DefaultLayout : public Layout 
    {
    public: 
        DefaultLayout() = default; 
        virtual ~DefaultLayout() = default; 

        virtual void Apply(GUIElement* element) override; 

    private: 
        int PaddingX_{10}; 
        int PaddingY_{10}; 
        int ElemsPerRow_{1}; 
    };

}
