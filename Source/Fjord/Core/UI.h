#pragma once 

#include "Fjord/Common.h" 

#define FJ_BIT(x) (1 << (x)) 

namespace Fjord 
{ 

    class Font; 

    namespace UI 
    {

        enum WindowFlag 
        {
            WindowFlagNone = 0, 
            WindowFlagAutoResize = FJ_BIT(0) 
        };

        void StartFrame(); 
        void FinishFrame(); 
        void Render(); 

        bool HasActiveWidget(); 

        void SetNextWindowPosition(int x, int y); 
        void SetNextWindowSize(int w, int h); 

        void PushId(int i); 
        void PopId(); 

        void PushFont(Font* font, int size = 0); 
        void PopFont(); 

        void BeginWindow(const String& title, WindowFlag flags = WindowFlagNone); 
        void EndWindow(); 

        void SameLine(); 

        void Separator(); 

        bool Button(const String& text); 
        bool Checkbox(const String& text, bool* value); 

    }
}