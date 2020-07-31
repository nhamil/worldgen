#include "Fjord/Common.h" 

#include "Fjord/Core/Input.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Font.h" 
#include "Fjord/Graphics/SpriteBatch.h" 
#include "Fjord/Util/Memory.h" 

using namespace Fjord; 

#define PRIME_OFF 0xCBF29CE484222325ULL
#define PRIME_MUL 0x100000001B3ULL 

namespace Fjord { namespace UI 
{

    using Id = uint64; 
    static const Id NullId = 0; 

    static int WindowPaddingX = 10, WindowPaddingY = 10; 
    static int PaddingX = 10, PaddingY = 10; 

    static int WindowBorder = 2; 
    static int WindowTitleHeight = 20; 

    struct Window 
    {
        UI::Id Id; 
        int X, Y, Width, Height; 
        int CurX, CurY; 
        int SameX, SameY; 
        int MouseX = 0, MouseY = 0; 

        void AddItem(int w, int h) 
        {
            SameX = CurX + w + PaddingX; 
            SameY = CurY; 

            // by default, next widget is on next line 
            CurX = X + WindowPaddingX; 
            CurY += h + PaddingY; 
        }

        void SetSameLine() 
        {
            CurX = SameX; 
            CurY = SameY; 
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
    static Ref<Font> CurFont; 
    static Vector<DrawCallType> DrawCallList; 
    static Vector<RectDrawCall> RectDrawCallList; 
    static Vector<TextDrawCall> TextDrawCallList; 

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
        return hash + parent; 
    } 

    static void AddRect(Window* window, const Color& color, int x, int y, int w, int h) 
    {
        RectDrawCall call; 
        call.Color = color; 
        call.X = x; 
        call.Y = y; 
        call.W = w; 
        call.H = h; 
        call.Bound = window != nullptr; 
        call.BoundX = window ? window->X + WindowBorder : -1; 
        call.BoundY = window ? window->Y + WindowBorder + WindowTitleHeight : -1;  
        call.BoundW = window ? window->Width - 2*WindowBorder : -1;  
        call.BoundH = window ? window->Height - 2*WindowBorder : -1;  
        DrawCallList.push_back(DrawCallType::Rect); 
        RectDrawCallList.push_back(call); 
    }

    static void AddText(Window* window, const Color& color, int x, int y, int w, int h, const String& str) 
    {
        StringMetrics sm = CurFont->GetMetrics(str); 

        TextDrawCall call; 
        call.Color = color; 
        call.Font = CurFont->GetFace(); 
        call.Text = str; 
        call.X = x + w*0.5 - sm.PxWidth*0.5; 
        call.Y = y + h*0.5 + sm.PxHeight*0.5; 
        call.Bound = window != nullptr; 
        call.BoundX = window ? window->X + WindowBorder : -1; 
        call.BoundY = window ? window->Y + WindowBorder + WindowTitleHeight : -1;  
        call.BoundW = window ? window->Width - 2*WindowBorder : -1;  
        call.BoundH = window ? window->Height - 2*WindowBorder : -1;  
        DrawCallList.push_back(DrawCallType::Text); 
        TextDrawCallList.push_back(call); 
    }

    static Window& GetWindow(Id id, int sx = 0, int sy = 0, int sw = 300, int sh = 300) 
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
                w.CurX = w.X + WindowPaddingX; 
                w.CurY = w.Y + WindowPaddingY + WindowTitleHeight; 
                return w; 
            }
            else 
            {
                // window does not exist, make it 
                FJ_EFDEBUG("Creating new window for ID %llu", id); 
                Window w; 
                w.Id = id; 
                w.X = sx; 
                w.Y = sy; 
                w.CurX = w.X + WindowPaddingX; 
                w.CurY = w.Y + WindowPaddingY + WindowTitleHeight; 
                w.Width = sw; 
                w.Height = sh; 
                CurState.Windows[id] = w; 
                return CurState.Windows[id]; 
            }
        }
    }

    static bool IsMouseInBounds(Window* window, int x, int y, int w, int h) 
    {
        if (CurState.MouseX < x || CurState.MouseX >= x + w) return false; 
        if (CurState.MouseY < y || CurState.MouseY >= y + h) return false; 
        if (window) 
        {
            if (CurState.MouseX < window->X + WindowBorder || CurState.MouseX >= window->X + window->Width - WindowBorder) return false; 
            if (CurState.MouseY < window->Y + WindowBorder + WindowTitleHeight || CurState.MouseY >= window->Y + window->Height - WindowBorder) return false; 
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

    void StartFrame() 
    {
        auto* input = GetInput(); 

        if (!CurFont) 
        {
            CurFont = Font::GetDefaultFont(); 
        }

        CurState.MouseX = (int) input->GetMouse().X; 
        CurState.MouseY = (int) input->GetMouse().Y; 
        CurState.MouseDown = input->GetButton(1); 
        CurWindow = NullId; 

        DrawCallList.clear(); 
        RectDrawCallList.clear(); 
        TextDrawCallList.clear(); 
    }

    void FinishFrame() 
    {
        FJ_EASSERT(CurWindow == NullId); 

        // if (CurState.Active != NullId) 
        // {
        //     FJ_EFDEBUG("Active : %llu", CurState.Active); 
        // }

        // if (CurState.Hot != NullId) 
        // {
        //     FJ_EFDEBUG("Hot    : %llu", CurState.Hot); 
        // }

        PrevState = CurState; 
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

        Batch->ClearBounds(); 
        Batch->DrawString(Color::Black, "UI Draw Calls: " + ToString(DrawCallList.size()), 10, 55); 

        Batch->End(); 
    }

    void BeginWindow(const String& title, int startX, int startY, int sWidth, int sHeight) 
    {
        FJ_EASSERT(CurWindow == NullId); 

        Id id = GetId(title); 
        Window& w = GetWindow(id, startX, startY, sWidth, sHeight); 
        
        CurWindow = id; 
        // if (IsMouseInBounds(w.X, w.Y, w.Width, w.Height)) 
        // {
        //     Hot = id; 
        // }

        AddRect(nullptr, {0.9f, 0.9f, 0.9f}, w.X, w.Y, w.Width, w.Height); 
        AddRect(nullptr, {0.7f, 0.7f, 0.7f}, w.X, w.Y, w.Width, WindowTitleHeight); 
        AddText(nullptr, Color::Black, w.X, w.Y, w.Width, WindowTitleHeight, title); 
    }

    void EndWindow() 
    {
        FJ_EASSERT(CurWindow != NullId); 

        Id id = CurWindow; 
        Window& w = GetWindow(id); 

        SetHot(id, GetHot() == NullId && IsMouseInBounds(nullptr, w.X, w.Y, w.Width, WindowTitleHeight)); 

        if (GetActive() == NullId && IsMouseJustDown() && IsMouseInBounds(nullptr, w.X, w.Y, w.Width, WindowTitleHeight))  
        {
            FJ_EFDEBUG("active"); 
            SetActive(id, true); 
            w.MouseX = CurState.MouseX - w.X; 
            w.MouseY = CurState.MouseY - w.Y; 
        }
        if (IsMouseJustUp()) 
        {
            SetActive(id, false); 
        }

        if (IsActive(id)) 
        {
            w.X = CurState.MouseX - w.MouseX; 
            w.Y = CurState.MouseY - w.MouseY; 
        }

        CurWindow = NullId; 
    }

    void SameLine() 
    {
        FJ_EASSERT(CurWindow != NullId); 
        Window& w = GetWindow(CurWindow); 
        w.SetSameLine(); 
    }

    static void RenderButton(Window& window, Id id, const String& text, int x, int y, int w, int h) 
    {
        if (IsActive(id)) 
        {
            AddRect(&window, {0.3f, 0.3f, 0.3f}, x, y, w, h); 
        }
        else if (IsHot(id)) 
        {
            AddRect(&window, {0.35f, 0.3f, 1.0f}, x, y, w, h); 
        }
        else 
        {
            AddRect(&window, {0.5f, 0.5f, 1.0f}, x, y, w, h); 
        }
        AddText(&window, Color::Black, x, y, w, h, text); 
    }

    bool Button(const String& text) 
    {
        FJ_EASSERT(CurWindow != NullId); 
        Window& w = GetWindow(CurWindow); 

        Id id = GetId(text, CurWindow); 
        int x = w.CurX; 
        int y = w.CurY; 
        int width = 100; 
        int height = 20; 
        w.AddItem(width, height); 

        if (IsMouseInBounds(&w, x, y, width, height)) 
        {
            SetHot(id, true); 

            if (IsActive(id)) 
            {
                if (IsMouseJustUp()) 
                {
                    SetActive(id, false); 
                    RenderButton(w, id, text, x, y, width, height); 
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

        RenderButton(w, id, text, x, y, width, height); 
        return false; 
    }

}}