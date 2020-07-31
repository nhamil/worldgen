#pragma once 

#include "Fjord/Common.h" 

#define FJ_BIT(x) (1 << (x)) 

namespace Fjord { namespace UI 
{

    enum WindowFlag 
    {
        WindowFlagAutoResize = FJ_BIT(1) 
    };

    void StartFrame(); 
    void FinishFrame(); 
    void Render(); 

    bool HasActiveWidget(); 

    void BeginWindow(const String& windowTitle, int startX, int startY, int startWidth = 300, int startHeight = 300); 
    void EndWindow(); 

    void SameLine(); 

    bool Button(const String& text); 

}}