#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Core/Key.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Font.h" 
#include "Fjord/GUI/Layout.h" 

namespace Fjord 
{

    class GUIRenderer; 

    class GUIEvent 
    {
    public: 
        virtual ~GUIEvent() = default; 

        bool IsConsumed() const { return Consumed_; } 
        bool IsAvailable() const { return !Consumed_; } 
        void Consume() { Consumed_ = true; } 

    private: 
        bool Consumed_ = false; 
    };

    class GUIKeyEvent : public GUIEvent 
    {
    public: 
        GUIKeyEvent(Fjord::Key key, bool down) 
            : Key(key) 
            , Down(down) {}  
        virtual ~GUIKeyEvent() = default; 

        const Fjord::Key Key; 
        const bool Down; 
    };

    class GUIMouseEvent : public GUIEvent 
    {
    public: 
        GUIMouseEvent(int x, int y, unsigned button, bool down) 
            : X(x) 
            , Y(y) 
            , Button(button) 
            , Down(down) {}  
        virtual ~GUIMouseEvent() = default; 

        GUIMouseEvent Create(int dx, int dy) 
        {
            return GUIMouseEvent(X+dx, Y+dy, Button, Down); 
        }

        const int X, Y; 
        const unsigned Button; 
        const bool Down; 
    };

    class GUIMouseMoveEvent : public GUIEvent 
    {
    public: 
        GUIMouseMoveEvent(int x, int y, int dx, int dy) 
            : X(x) 
            , Y(y) 
            , DX(dx) 
            , DY(dy) {}
        virtual ~GUIMouseMoveEvent() = default; 

        GUIMouseMoveEvent Create(int dx, int dy) 
        {
            return GUIMouseMoveEvent(X+dx, Y+dy, DX, DY); 
        }

        const int X, Y; 
        const int DX, DY; 
    };

    class GUIMouseWheelEvent : public GUIEvent 
    {
    public: 
        GUIMouseWheelEvent(int x, int y, int amount) 
            : X(x) 
            , Y(y) 
            , Amount(amount) {}  
        virtual ~GUIMouseWheelEvent() = default; 

        GUIMouseWheelEvent Create(int dx, int dy) 
        {
            return GUIMouseWheelEvent(X+dx, Y+dy, Amount); 
        }

        const int X, Y; 
        const int Amount; 
    };

    class GUIElement : public RefCounted 
    {
        FJ_NO_COPY(GUIElement)
    public: 
        GUIElement(int w, int h); 
        virtual ~GUIElement(); 

        int GetWidth() const { return Width_; } 
        int GetHeight() const { return Height_; } 
        int GetX() const { return X_; } 
        int GetY() const { return Y_; } 
        Color GetColor() const { return Color_; } 
        Font* GetFont() const { return Font_; } 
        Layout* GetLayout() const { return Layout_; } 

        GUIElement* GetParent() const { return Parent_; } 

        bool InBounds(int gx, int gy); 

        virtual void SetPosition(int x, int y); 
        virtual void SetSize(int w, int h); 
        virtual void SetColor(const Color& color); 
        virtual void SetFont(Font* font); 
        virtual void SetLayout(Layout* layout); 

        // container 

        virtual void AddChild(GUIElement* child); 
        virtual bool RemoveChild(GUIElement* child); 

        virtual unsigned GetChildCount() const { return Children_.size(); } 
        virtual GUIElement* GetChild(unsigned index) const 
        {
            FJ_EASSERT(index < GetChildCount()); 
            return Children_[index]; 
        }

        void HandleUpdate(float dt); 
        void HandleRender(GUIRenderer& r); 
        void HandleKeyEvent(GUIKeyEvent& e); 
        void HandleMouseEvent(GUIMouseEvent& e); 
        void HandleMouseMoveEvent(GUIMouseMoveEvent& e); 
        void HandleMouseWheelEvent(GUIMouseWheelEvent& e); 

        // elements can override these 

        virtual void OnUpdate(float dt); 
        virtual void OnRender(GUIRenderer& r); 
        virtual void OnKeyEvent(GUIKeyEvent& e); 
        virtual void OnMouseEvent(GUIMouseEvent& e); 
        virtual void OnMouseMoveEvent(GUIMouseMoveEvent& e); 
        virtual void OnMouseWheelEvent(GUIMouseWheelEvent& e); 

    private: 
        void UpdateLayout(); 

        Vector<Ref<GUIElement>> Children_{}; 
        WeakRef<GUIElement> Parent_{}; 
        Ref<Layout> Layout_{}; 
        Ref<Font> Font_{}; 
        Color Color_{Color::Black}; 
        int X_{0}, Y_{0}; 
        int Width_, Height_; 
    };

}
