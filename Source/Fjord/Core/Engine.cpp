#include "Fjord/Core/Engine.h" 

#include "Fjord/Core/Application.h" 
#include "Fjord/Core/Input.h" 
#include "Fjord/Core/UI.h" 
#include "Fjord/Core/Window.h" 
#include "Fjord/Graphics/Graphics.h" 
// #include "Fjord/GUI/GUIEnvironment.h" 
// #include "Fjord/GUI/GUIRenderer.h" 
#include "Fjord/Util/Time.h" 
#include "Fjord/Util/Thread.h" 

#include <iostream> 

using namespace std; 

namespace Fjord 
{

    static Window* g_Window = nullptr; 
    static Input* g_Input = nullptr; 
    static Graphics* g_Graphics = nullptr; 
    static Logger* g_Logger = nullptr; 
    static Logger* g_EngineLogger = nullptr; 
    // static GUIEnvironment* g_GUI = nullptr; 

    static bool g_Running = false; 

    static float g_FPS = 0; 
    static float g_UPS = 0; 

    int Run(Application* app) 
    {
        FJ_EASSERT_RET_MSG(!g_Running, 1, "Application is already running"); 

        g_Running = true; 

        g_Window = new Window("WorldGen", 900*4/3, 900); 
        g_Input = g_Window->GetInput(); 
        g_Graphics = new Graphics(); 
        // g_GUI = new GUIEnvironment(g_Window->GetWidth(), g_Window->GetHeight()); 

        app->Init(); 

        double time = GetTimeSeconds(); 

        int ups = 0; 
        double updates = time; 
        double skipUpdates = 1.0 / 60.0; 

        int fps = 0; 
        double frames = time; 
        double skipFrames = 1.0 / 60.0; 

        // GUIRenderer guiRenderer; 

        while (g_Running) 
        {
            int loop = 0; 
            while (updates < GetTimeSeconds() && loop++ < 10) 
            {
                updates += skipUpdates; 
                ups++; 

                g_Window->Poll(); 
                // g_GUI->HandleUpdate(skipUpdates); 
                UI::StartFrame(); 
                app->UpdateGUI(skipUpdates); 
                bool mouseDown = GetInput()->GetButton(1); 
                // TODO quick hack to stop game from using mouse when UI is using it 
                if (UI::HasActiveWidget()) GetInput()->OnMouseUp(1); 
                app->Update(skipUpdates); 
                if (mouseDown) GetInput()->OnMouseDown(1); 
                UI::FinishFrame(); 
            }

            // if (frames < GetTimeSeconds()) 
            {
                frames += skipFrames; 
                fps++; 

                g_Graphics->ResetViewport(); 
                g_Graphics->ResetClip(); 

                app->Render(); 
                g_Graphics->Clear(false, true); 
                UI::Render(); 
                // g_GUI->HandleRender(guiRenderer); 
                // guiRenderer.Flush(); 

                g_Graphics->ResetViewport(); 
                g_Graphics->ResetClip(); 
                
                g_Window->SwapBuffers(); 
            }

            if (GetTimeSeconds() - time >= 1.0) 
            {
                FJ_ELOG(Debug, "Frames: %d, Updates: %d", fps, ups); 
                g_FPS = fps; 
                g_UPS = ups; 
                ups = fps = 0; 
                time = GetTimeSeconds(); 
            }

            // double delay = (updates - TimeSeconds()) * 1000; 
            // Sleep(1); //std::max(0.0, std::min(1.0, delay)));
            // Sleep(1); 
        }

        app->Stop();  

        return 0; 
    }

    int Start(Application* app) 
    {
        return Run(app); 
    }

    void Stop(bool imm) 
    {
        if (imm) 
        {
            exit(1); 
        }
        else 
        {
            g_Running = false; 
        }
    }

    float GetFPS() 
    {
        return g_FPS; 
    }

    float GetUPS() 
    {
        return g_UPS; 
    }

    Window* GetWindow() 
    {
        return g_Window; 
    }

    Input* GetInput() 
    {
        return g_Input; 
    }

    Graphics* GetGraphics() 
    {
        return g_Graphics; 
    }

    Logger* GetLogger() 
    {
        #ifdef DEBUG
        if (!g_Logger) g_Logger = new Logger(LogLevel::Debug, "[APP]"); 
        #else 
        if (!g_Logger) g_Logger = new Logger(LogLevel::Debug, "[APP]"); 
        #endif 
        return g_Logger; 
    }

    Logger* GetEngineLogger() 
    {
        #ifdef DEBUG
        if (!g_EngineLogger) g_EngineLogger = new Logger(LogLevel::Debug, "[ENG]"); 
        #else 
        if (!g_EngineLogger) g_EngineLogger = new Logger(LogLevel::Debug, "[ENG]"); 
        #endif 
        return g_EngineLogger; 
    }

    // GUIEnvironment* GetGUI() 
    // {
    //     return g_GUI; 
    // }

}
