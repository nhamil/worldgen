#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Core/Key.h" 
#include "Fjord/Graphics/Color.h" 

#define FJ_BIT(x) (1 << (x)) 

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
        GUIKeyEvent(enum Key key, bool down) 
            : Key(key) 
            , Down(down) {}  
        virtual ~GUIKeyEvent() = default; 

        enum Key Key; 
        bool Down; 
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

        int X, Y; 
        unsigned Button; 
        bool Down; 
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

        int X, Y; 
        int DX, DY; 
    };

    class GUIMouseWheelEvent : public GUIEvent 
    {
    public: 
        GUIMouseWheelEvent(int x, int y, int amount) 
            : X(x) 
            , Y(y) 
            , Amount(amount) {}  
        virtual ~GUIMouseWheelEvent() = default; 

        int X, Y; 
        int Amount; 
    };

    enum class Alignment 
    {
        TopLeft = FJ_BIT(0) | FJ_BIT(2), 
        Top = FJ_BIT(0), 
        TopRight = FJ_BIT(0) | FJ_BIT(3), 
        Left = FJ_BIT(2), 
        Center = 0, 
        Right = FJ_BIT(3), 
        BottomLeft = FJ_BIT(1) | FJ_BIT(2), 
        Bottom = FJ_BIT(1), 
        BottomRight = FJ_BIT(1) | FJ_BIT(3)  
    };

    inline bool IsAlignmentTop(Alignment a) 
    {
        return (((unsigned) a) & FJ_BIT(0)) > 0; 
    }

    inline bool IsAlignmentBottom(Alignment a) 
    {
        return (((unsigned) a) & FJ_BIT(1)) > 0; 
    }

    inline bool IsAlignmentLeft(Alignment a) 
    {
        return (((unsigned) a) & FJ_BIT(2)) > 0; 
    }

    inline bool IsAlignmentRight(Alignment a) 
    {
        return (((unsigned) a) & FJ_BIT(3)) > 0; 
    }

    inline int GetXMultiplier(Alignment a) 
    {
        return IsAlignmentRight(a) ? -1 : 1; 
    }

    inline int GetYMultiplier(Alignment a) 
    {
        return IsAlignmentBottom(a) ? -1 : 1; 
    }

    class GUIElement : public RefCounted 
    {
        FJ_NO_COPY(GUIElement)
    public: 
        static const int DefaultZOrder = 1000; 

        GUIElement(int x, int y, int w, int h); 
        virtual ~GUIElement(); 

        int GetWidth() const { return Width_; } 
        int GetHeight() const { return Height_; } 
        int GetX() const { return X_; } 
        int GetY() const { return Y_; } 
        int GetGlobalX() const { return GlobalX_; } 
        int GetGlobalY() const { return GlobalY_; } 
        int GetZOrder() const { return ZOrder_; } 
        const Color& GetColor() const { return Color_; } 
        Alignment GetAlignment() const { return Alignment_; } 

        GUIElement* GetParent() const { return Parent_; } 

        bool InBounds(int gx, int gy); 

        void SetPosition(int x, int y); 
        void SetSize(int w, int h); 
        void SetZOrder(int z); 
        void SetColor(const Color& color); 
        void SetAlignment(Alignment a); 

        // most elements should not override these

        virtual void Update(float dt); 
        virtual void Render(GUIRenderer* r); 

        virtual void HandleKeyEvent(GUIKeyEvent& e); 
        virtual void HandleMouseEvent(GUIMouseEvent& e); 
        virtual void HandleMouseMoveEvent(GUIMouseMoveEvent& e); 
        virtual void HandleMouseWheelEvent(GUIMouseWheelEvent& e); 

        // elements can override these 

        virtual void OnUpdate(float dt); 
        virtual void OnRender(GUIRenderer* r); 
        virtual void OnKeyEvent(GUIKeyEvent& e); 
        virtual void OnMouseEvent(GUIMouseEvent& e); 
        virtual void OnMouseMoveEvent(GUIMouseMoveEvent& e); 
        virtual void OnMouseWheelEvent(GUIMouseWheelEvent& e); 

    private: 
        friend class Container; 

        void ComputeBounds(); 

        WeakRef<GUIElement> Parent_; 
        Alignment Alignment_; 
        Color Color_; 
        int ZOrder_; 
        int X_, Y_; 
        int Width_, Height_; 
        int GlobalX_, GlobalY_; 
    };

}
