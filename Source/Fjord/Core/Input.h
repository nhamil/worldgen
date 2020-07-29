#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Core/Engine.h" 
#include "Fjord/Core/Key.h" 
#include "Fjord/GUI/GUIEnvironment.h" 
#include "Fjord/Math/Vector2.h" 

#include <iostream> 
#include <unordered_set>  

namespace Fjord 
{

    class Application; 

    class Input
    {
    public: 
        Input() {} 
        ~Input() {} 

        bool GetKey(Key key) const { return DownKeys_.find(key) != DownKeys_.end(); }  

        bool GetButton(int button) const { return DownButtons_.find(button) != DownButtons_.end(); } 

        Vector2 GetMouse() const { return Mouse_; } 

        Vector2 GetMouseMove() const { return MouseMove_; } 

        int GetMouseWheel() const { return Wheel_; } 

    private: 
        friend class Window; 

        using IntSet = HashSet<int>; 

        void OnUpdate() 
        {
            MouseMove_ = CurMouseMove_; 
            CurMouseMove_ = {0, 0}; 
            Mouse_ = CurMouse_; 
            // CurMouse_ = {0, 0}; 
            Wheel_ = CurWheel_; 
            CurWheel_ = 0; 
        }

        void OnKeyDown(Key key) 
        {
            GUIKeyEvent e(key, true); 
            GetGUI()->OnKeyEvent(e); 

            if (e.IsAvailable()) 
            {
                DownKeys_.insert(key); 
            }
        } 

        void OnKeyUp(Key key) 
        {
            GUIKeyEvent e(key, false); 
            GetGUI()->OnKeyEvent(e); 

            if (e.IsAvailable()) 
            {
                DownKeys_.erase(key); 
            }
        } 

        void OnMouseMove(const Vector2& pos, const Vector2& vel) 
        {
            // TODO should this require the event to be available? 
            CurMouseMove_ += {vel.X, vel.Y}; 
            CurMouse_ = {pos.X, pos.Y}; 

            GUIMouseMoveEvent e(pos.X, pos.Y, vel.X, vel.Y); 
            GetGUI()->HandleMouseMoveEvent(e); 
        }

        void OnMouseDown(unsigned button) 
        {
            GUIMouseEvent e(Mouse_.X, Mouse_.Y, button, true); 
            GetGUI()->HandleMouseEvent(e); 

            if (e.IsAvailable()) 
            {
                DownButtons_.insert(button); 
            }
        }

        void OnMouseUp(unsigned button) 
        {
            GUIMouseEvent e(Mouse_.X, Mouse_.Y, button, false); 
            GetGUI()->HandleMouseEvent(e); 

            if (e.IsAvailable()) 
            {
                DownButtons_.erase(button); 
            }
        }

        void OnMouseWheel(int amount) 
        {
            GUIMouseWheelEvent e(Mouse_.X, Mouse_.Y, amount); 
            GetGUI()->HandleMouseWheelEvent(e); 

            if (e.IsAvailable()) 
            {
                CurWheel_ += amount; 
            }
        }

        // TODO keep prev update
        IntSet DownKeys_; 
        IntSet DownButtons_; 

        Vector2 MouseMove_; 
        Vector2 CurMouseMove_; 
        Vector2 Mouse_; 
        Vector2 CurMouse_; 
        int Wheel_ = 0; 
        int CurWheel_ = 0; 
    };

}