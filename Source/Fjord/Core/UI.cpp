#include "Fjord/Core/UI.h" 

#include "Fjord/Common.h" 
#include "Fjord/Core/Input.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Font.h" 
#include "Fjord/Graphics/SpriteBatch.h" 
#include "Fjord/Math/MathUtil.h" 
#include "Fjord/Util/Memory.h" 

#include <algorithm> 

using namespace Fjord; 

#define PRIME_OFF 0xCBF29CE484222325ULL
#define PRIME_MUL 0x100000001B3ULL 

namespace Fjord { namespace UI 
{

    using Id = uint64; 
    static const Id NullId = 0; 

    static uint64 NextZOrder = 0; 

    static uint64 GetNextZOrder() 
    {
        return NextZOrder++; 
    }

    struct Config 
    {
        struct 
        {
            // start 
            int StartX = 20; 
            int StartY = 20; 
            int StartWidth = 300; 
            int StartHeight = 300; 
            // title 
            int TitleHeight = 20; 
            // border 
            int BorderLeft = 2; 
            int BorderTop = 2; 
            int BorderRight = 2; 
            int BorderBottom = 2; 
            // padding 
            int PaddingX = 10; 
            int PaddingY = 10; 
        } Window; 
        struct 
        {
            int PaddingX = 10; 
            int PaddingY = 10; 
        } Widget;
    };

    static const Config DefaultConfig; 
    static Config CurConfig; 

    struct Window 
    {
        UI::Id Id = NullId; 
        String Title; 
        WindowFlag Flags = WindowFlagAutoResize; 
        uint64 ZOrder = GetNextZOrder(); 
        bool DragMode = false; 
        int X, Y, Width, Height; 
        int CursorX, CursorY; 
        int SameLineX, SameLineY; 
        int MouseX = 0, MouseY = 0; 

        void ResetForFrame() 
        {
            CursorX = X + CurConfig.Window.PaddingX; 
            CursorY = Y + CurConfig.Window.PaddingY + CurConfig.Window.TitleHeight; 
            SameLineX = CursorX; 
            SameLineY = CursorY; 
        }

        bool IsAutoResizeEnabled() 
        {
            return Flags & WindowFlagAutoResize; 
        }

        void AddItem(int& x, int& y, int w, int h) 
        {
            x = CursorX; 
            y = CursorY; 

            // FJ_EFDEBUG("%d", Flags); 
            if (IsAutoResizeEnabled()) 
            {
                Width = Max(Width, CursorX - X + w + CurConfig.Window.PaddingX); 
                Height = Max(Height, CursorY - Y + h + CurConfig.Window.PaddingY); 
                // FJ_EFDEBUG("auto resize to %d %d", Width, Height); 
            }

            SameLineX = CursorX + w + CurConfig.Window.PaddingX; 
            SameLineY = CursorY; 

            // by default, next widget is on next line 
            CursorX = X + CurConfig.Window.PaddingX; 
            CursorY += h + CurConfig.Widget.PaddingY; 
        }

        void SetSameLine() 
        {
            CursorX = SameLineX; 
            CursorY = SameLineY; 
        }

        Window() 
        {
            X = CurConfig.Window.StartX; 
            Y = CurConfig.Window.StartY; 
            Width = CurConfig.Window.StartWidth; 
            Height = CurConfig.Window.StartHeight; 
            ResetForFrame(); 
        }

        Window(const Window& other) = default; 
    };

    struct Bounds 
    {
        bool Enabled = false; 
        int X = 0; 
        int Y = 0; 
        int W = INT_MAX; 
        int H = INT_MAX; 

        Bounds& Apply(int x, int y, int w, int h) 
        {
            if (!Enabled) 
            {
                X = x; 
                Y = y; 
                W = w; 
                H = h; 
            }
            else 
            {
                int x1 = Max(X, x); 
                int y1 = Max(Y, y); 
                int x2 = Min(X + W, x + w); 
                int y2 = Min(Y + H, y + h); 

                X = x1; 
                Y = y1; 
                W = x2 - x1; 
                H = y2 - y1; 
            }
            Enabled = true; 
            return *this; 
        }

        Bounds& Apply(Window& w) 
        {
            return Apply(w.X, w.Y, w.Width, w.Height); 
        }

        Bounds& ApplyBorder(int left, int top, int right, int bottom) 
        {
            X += left; 
            Y += top; 
            W -= left + right; 
            H -= top + bottom; 
            return *this; 
        }

        Bounds& ApplyBorder(int amt) 
        {
            return ApplyBorder(amt, amt, amt, amt); 
        }
    };

    enum class DrawCallType 
    {
        None, 
        Rect, 
        Text 
    };

    struct RectDrawCall 
    {
        int BoundX, BoundY, BoundW, BoundH; 
        bool Bound; 
        int X, Y, W, H; 
        Fjord::Color Color; 
    }; 

    struct TextDrawCall 
    {
        int BoundX, BoundY, BoundW, BoundH; 
        bool Bound; 
        int X, Y; 
        String Text; 
        Ref<FontFace> Font; 
        Fjord::Color Color; 
    };

    struct State 
    {
        HashMap<Id, Window> Windows; 
        Id Hot = NullId; 
        Id Active = NullId; 
        bool MouseDown = false;
        int MouseX = 0, MouseY = 0;

        void Clear() 
        {
            Windows.clear(); 
            Hot = NullId; 
        }
    };

    static Id CurWindow = NullId; 
    static State CurState, PrevState; 

    static Ref<SpriteBatch> Batch; 
    static Vector<Ref<FontFace>> FontStack; 
    static Vector<DrawCallType> DrawCallList; 
    static Vector<RectDrawCall> RectDrawCallList; 
    static Vector<TextDrawCall> TextDrawCallList; 

    static Vector<int> IdStack; 
    static uint64 IdOffset = 0; 

    static Id GetId(const String& name, Id parent = 0) 
    {
        // mix of FNV hash and xorshift 
        const char* data = name.c_str(); 
        unsigned len = strlen(data); 
        Id hash = PRIME_OFF; 
        for (unsigned i = 0; i < len; i++) 
        {
            hash ^= data[i]; 
            hash *= PRIME_MUL; 
        }
        hash ^= hash << 13; 
        hash ^= hash >> 7; 
        hash ^= hash << 17; 
        return hash + parent + IdOffset; 
    } 

    static Id GetId(int i) 
    {
        char c[20]; 
        snprintf(c, 20, "%d", i); 
        return GetId(c); 
    }

    void PushId(int i) 
    {
        IdStack.push_back(i); 
        IdOffset += GetId(i); 
    }

    void PopId() 
    {
        int i = IdStack.back(); 
        IdStack.pop_back(); 
        IdOffset -= GetId(i); 
    }

    static void AddRect(const Color& color, int x, int y, int w, int h, Bounds bounds) 
    {
        RectDrawCall call; 
        call.Color = color; 
        call.X = x; 
        call.Y = y; 
        call.W = w; 
        call.H = h; 
        call.Bound = bounds.Enabled; 
        call.BoundX = bounds.X; 
        call.BoundY = bounds.Y;  
        call.BoundW = bounds.W;  
        call.BoundH = bounds.H;  
        DrawCallList.push_back(DrawCallType::Rect); 
        RectDrawCallList.push_back(call); 
    }

    static FontFace* GetFontFace() 
    {
        return FontStack.back(); 
    }

    static void AddText(const Color& color, int x, int y, int w, int h, const String& str, Bounds bounds) 
    {
        StringMetrics sm = GetFontFace()->GetMetrics(str); 

        TextDrawCall call; 
        call.Color = color; 
        call.Font = GetFontFace(); 
        call.Text = str; 
        call.X = x + w*0.5 - sm.PxWidth*0.5; 
        call.Y = y + h*0.5 + sm.PxHeight*0.5; 
        call.Bound = bounds.Enabled; 
        call.BoundX = bounds.X; 
        call.BoundY = bounds.Y;  
        call.BoundW = bounds.W;  
        call.BoundH = bounds.H;  
        DrawCallList.push_back(DrawCallType::Text); 
        TextDrawCallList.push_back(call); 
    }

    static Window& GetWindow(Id id, WindowFlag flags = WindowFlagNone) 
    {
        auto it = CurState.Windows.find(id); 

        if (it != CurState.Windows.end()) 
        {
            return it->second; 
        }
        else 
        {
            auto itPrev = PrevState.Windows.find(id); 
            if (itPrev != PrevState.Windows.end()) 
            {
                // bring old window state to current context 
                CurState.Windows[id] = itPrev->second; 
                Window& w = CurState.Windows[id]; 
                w.ResetForFrame(); 
                return w; 
            }
            else 
            {
                // window does not exist, make it 
                FJ_EFDEBUG("Creating new window for ID %llu", id); 
                Window& w = CurState.Windows[id]; 
                w.Id = id; 
                w.Flags = flags; 
                return w; 
            }
        }
    }

    static bool IsMouseInBounds(int x, int y, int w, int h, Bounds b = {}) 
    {
        if (CurState.MouseX < x || CurState.MouseX >= x + w) return false; 
        if (CurState.MouseY < y || CurState.MouseY >= y + h) return false; 
        if (b.Enabled) 
        {
            if (CurState.MouseX < b.X || CurState.MouseX >= b.X + b.W) return false; 
            if (CurState.MouseY < b.Y || CurState.MouseY >= b.Y + b.H) return false; 
        }
        return true; 
    }

    static bool IsMouseJustDown() 
    {
        return CurState.MouseDown && !PrevState.MouseDown; 
    }

    static bool IsMouseJustUp() 
    {
        return !CurState.MouseDown && PrevState.MouseDown; 
    }

    static Id GetActive() 
    {
        return CurState.Active; 
    }

    static Id GetHot() 
    {
        return CurState.Hot; 
    }

    static bool IsActive(Id id) 
    {
        return CurState.Active == id; 
    }

    static bool IsHot(Id id) 
    {
        return CurState.Hot == id; 
    }

    static void SetActive(Id id, bool setActive) 
    {
        if (setActive) 
        {
            if (!IsActive(id) && GetActive() != NullId) 
            {
                FJ_EFWARN("Something else is active: %llu", GetActive()); 
            }
            CurState.Active = id; 
        }
        else 
        {
            if (IsActive(id)) CurState.Active = NullId; 
        }
    }

    static void SetHot(Id id, bool setHot) 
    {
        if (setHot) 
        {
            if (!IsHot(id) && GetHot() != NullId) 
            {
                FJ_EFWARN("Something else is hot: %llu", GetHot()); 
            }
            CurState.Hot = id; 
        }
        else 
        {
            if (IsHot(id)) CurState.Hot = NullId; 
        }
    }

    void PushFont(Font* font, int size) 
    {
        font = font ? font : Font::GetDefaultFont(); 
        if (size == 0) 
        {
            size = Font::DefaultSize; 
        }
        FontStack.push_back(font->GetFace(size)); 
    }

    void PopFont() 
    {
        FontStack.pop_back(); 
    }

    void StartFrame() 
    {
        auto* input = GetInput(); 

        FontStack.clear(); 
        PushFont(nullptr); 

        CurState.MouseX = (int) input->GetMouse().X; 
        CurState.MouseY = (int) input->GetMouse().Y; 
        CurState.MouseDown = input->GetButton(1); 
        CurWindow = NullId; 

        DrawCallList.clear(); 
        RectDrawCallList.clear(); 
        TextDrawCallList.clear(); 

        IdStack.clear(); 
        IdOffset = 0; 
    }

    void FinishFrame() 
    {
        FJ_EASSERT(CurWindow == NullId); 

        PrevState = CurState; 
        CurConfig = DefaultConfig; 
        CurState.Clear(); 
    }

    bool HasActiveWidget() 
    {
        return GetActive() != NullId; 
    }

    void Render() 
    {
        if (!Batch) 
        {
            Batch = new SpriteBatch(); 
        }

        Batch->Begin(); 

        unsigned iRect = 0; 
        unsigned iText = 0; 

        Batch->ClearBounds(); 
        Batch->DrawString(Color::Black, "UI Draw Calls: " + ToString(DrawCallList.size()), 10, 55); 

        for (unsigned i = 0; i < DrawCallList.size(); i++) 
        {
            DrawCallType type = DrawCallList[i]; 

            if (type == DrawCallType::Rect) 
            {
                RectDrawCall call = RectDrawCallList[iRect++]; 
                if (call.Bound) 
                {
                    Batch->SetBounds(call.BoundX, call.BoundY, call.BoundW, call.BoundH); 
                } 
                else 
                {
                    Batch->ClearBounds(); 
                }
                Batch->Draw(
                    call.Color, 
                    call.X, 
                    call.Y, 
                    call.W, 
                    call.H 
                );
            }
            else if (type == DrawCallType::Text) 
            {
                TextDrawCall call = TextDrawCallList[iText++]; 
                if (call.Bound) 
                {
                    Batch->SetBounds(call.BoundX, call.BoundY, call.BoundW, call.BoundH); 
                } 
                else 
                {
                    Batch->ClearBounds(); 
                }
                Batch->DrawString(
                    call.Font, 
                    call.Color, 
                    call.Font->GetSize(), 
                    call.Text.c_str(), 
                    call.X, 
                    call.Y 
                );
            }
        }

        Batch->End(); 
    }

    void SetNextWindowPosition(int x, int y) 
    {
        CurConfig.Window.StartX = x; 
        CurConfig.Window.StartY = y; 
    }

    void SetNextWindowSize(int w, int h) 
    {
        CurConfig.Window.StartWidth = w; 
        CurConfig.Window.StartHeight = h; 
    }

    void BeginWindow(const String& title, WindowFlag flags) 
    {
        FJ_EASSERT(CurWindow == NullId); 

        Id id = GetId(title); 
        Window& w = GetWindow(id, flags); 
        w.Title = title; 
        
        CurWindow = id; 
        // FJ_EFDEBUG("%s: %d %d %d %d", title.c_str(), w.X, w.Y, w.Width, w.Height); 

        if (w.DragMode) 
        {
            w.X = CurState.MouseX - w.MouseX; 
            w.Y = CurState.MouseY - w.MouseY; 
        }
        w.ResetForFrame(); 

        AddRect({0.9f, 0.9f, 0.9f}, w.X, w.Y, w.Width, w.Height, Bounds().Apply(w)); 
        AddRect({0.7f, 0.7f, 0.7f}, w.X, w.Y, w.Width, CurConfig.Window.TitleHeight, Bounds().Apply(w)); 
        AddText(Color::Black, w.X, w.Y, w.Width, CurConfig.Window.TitleHeight, w.Title, Bounds().Apply(w)); 

        if (w.IsAutoResizeEnabled()) 
        {
            w.Width = w.Height = 0; 
        }
    }

    void EndWindow() 
    {
        FJ_EASSERT(CurWindow != NullId); 

        Id id = CurWindow; 
        Window& w = GetWindow(id); 

        SetHot(id, GetHot() == NullId && IsMouseInBounds(w.X, w.Y, w.Width, CurConfig.Window.TitleHeight)); 

        if (GetActive() == NullId && IsMouseJustDown() && IsMouseInBounds(w.X, w.Y, w.Width, w.Height))  
        {
            if (IsMouseInBounds(w.X, w.Y, w.Width, CurConfig.Window.TitleHeight)) 
            {
                w.DragMode = true; 
            }
            SetActive(id, true); 
            w.MouseX = CurState.MouseX - w.X; 
            w.MouseY = CurState.MouseY - w.Y; 
        }

        if (IsMouseJustUp()) 
        {
            SetActive(id, false); 
            w.DragMode = false; 
        }

        CurWindow = NullId; 
    }

    void SameLine() 
    {
        FJ_EASSERT(CurWindow != NullId); 
        Window& w = GetWindow(CurWindow); 
        w.SetSameLine(); 
    }

    static void RenderButton(Window& window, Id id, const String& text, int x, int y, int w, int h, Bounds b) 
    {
        if (IsActive(id)) 
        {
            AddRect({0.3f, 0.3f, 0.3f}, x, y, w, h, b); 
        }
        else if (IsHot(id)) 
        {
            AddRect({0.35f, 0.3f, 1.0f}, x, y, w, h, b); 
        }
        else 
        {
            AddRect({0.5f, 0.5f, 1.0f}, x, y, w, h, b); 
        }
        b.Apply(x, y, w, h); 
        AddText(Color::Black, x, y, w, h, text, b); 
    }

    bool Button(const String& text) 
    {
        FJ_EASSERT(CurWindow != NullId); 
        Window& w = GetWindow(CurWindow); 

        Id id = GetId(text, CurWindow); 
        int x, y;  
        int width = 100; 
        int height = 20; 
        w.AddItem(x, y, width, height); 

        // FJ_EFDEBUG("%d %d %d %d", x, y, width, height); 

        Bounds b; 
        b.Apply(x, y, width, height); 
        b.Apply(w); 

        if (IsMouseInBounds(x, y, width, height, b)) 
        {
            SetHot(id, true); 

            if (IsActive(id)) 
            {
                if (IsMouseJustUp()) 
                {
                    SetActive(id, false); 
                    RenderButton(w, id, text, x, y, width, height, b); 
                    return true; 
                }
            }
            else if (IsMouseJustDown()) 
            {
                SetActive(id, true); 
            }
        }
        else 
        {
            SetHot(id, false); 
        }

        if (IsMouseJustUp()) 
        {
            SetActive(id, false); 
        }

        RenderButton(w, id, text, x, y, width, height, b); 
        return false; 
    }

}}