#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    class GUIElement; 
    class GUIRenderer; 

    class GUI : public RefCounted 
    {
        FJ_NO_COPY(GUI) 
    public: 
        GUI(); 
        ~GUI(); 

        void Update(float dt); 
        void Render(GUIRenderer* r); 

        void AddElement(GUIElement* element); 

    private: 
        Vector<Ref<GUIElement>> Elements_; 
    };

}