#include <Fjord/Core/Entry.h> 
#include <Fjord/Core/Input.h> 
#include <Fjord/Core/Object.h> 
#include <Fjord/Core/UI.h> 
#include <Fjord/Core/Window.h> 
#include <Fjord/Math/MathUtil.h> 
#include <Fjord/Math/Quaternion.h> 
#include <Fjord/Graphics/Font.h> 
#include <Fjord/Graphics/Geometry.h> 
#include <Fjord/Graphics/Graphics.h> 
#include <Fjord/Graphics/IndexBuffer.h> 
#include <Fjord/Graphics/Mesh.h> 
#include <Fjord/Graphics/Shader.h> 
#include <Fjord/Graphics/SpriteBatch.h> 
#include <Fjord/Graphics/Texture2D.h> 
#include <Fjord/Graphics/VertexBuffer.h> 
#include <Fjord/Util/ClassId.h> 
#include <Fjord/Util/FileUtil.h> 
#include <Fjord/Util/Random.h> 
#include <Fjord/Util/Thread.h> 
#include <Fjord/Util/Time.h> 

#include "WorldGen/FluidSim.h" 

#include <iostream> 

using namespace Fjord; 
using namespace std; 

class Main : public Application 
{
public: 
    static const int W = 128; 
    static const int H = 64; 

    virtual void Init() override; 
    virtual void UpdateGUI(float dt) override; 
    virtual void Update(float dt) override; 
    virtual void Render() override; 

    void NewSim(); 

    FluidSim* Sim = nullptr; 
    Ref<Texture2D> SimTex; 
    Ref<SpriteBatch> SB; 
    int Updates = 0; 
};

void Main::NewSim() 
{
    if (Sim) delete Sim; 
    Sim = new FluidSim(W, H); 
    Updates = 0; 

    Sim->Diff = 100.0; 
    Sim->Visc = 100.0; 

    Sim->Ps[Sim->ix(W/2,H/2)] = 100.0; 

    Random r; 

    for (int i = 0; i < 10; i++) 
    {
        int x = r.NextULongLong() % W; 
        int y = r.NextULongLong() % H; 
        FJ_DEBUG("%d %d", x, y); 
        Sim->Vxs[Sim->ix(x, y)] = r.NextFloat() * 1000 - 500; 
        Sim->Vys[Sim->ix(x, y)] = r.NextFloat() * 1000 - 500; 
    }

    // Sim->Vxs[Sim->ix(W*3/4,H/2)] = -1.0; 
    // Sim->Ps[Sim->ix(W/2,H/2)] = 1.0; 
}

void Main::Init() 
{
    NewSim(); 

    SimTex = new Texture2D(W, H, TextureFormat::RGB8); 
    SimTex->SetFilter(TextureFilter::Nearest); 

    SB = new SpriteBatch(); 
}

void Main::UpdateGUI(float dt) 
{
    (void) dt; 

    UI::BeginWindow("Sim", UI::WindowFlag::WindowFlagAutoResize); 
    if (UI::Button("Restart")) 
    {
        NewSim(); 
    }
    UI::EndWindow(); 
}

void Main::Update(float dt) 
{
    (void) dt; 

    Sim->Update(1.0 / 60); 

    uint8* img = new uint8[3 * W * H]; 
    for (int y = 0; y < H; y++) 
    {
        for (int x = 0; x < W; x++) 
        {
            int i = 3 * (x + y * W); 
            img[i+1] = (int) Clamp(Map<float>(Sim->P[Sim->ix(x,y)], 0, 1, 0, 255), 0, 255); 
            img[i+0] = (int) Clamp(Map<float>(Sim->Vx[Sim->ix(x,y)], -1, 1, 0, 255), 0, 255); 
            img[i+2] = (int) Clamp(Map<float>(Sim->Vy[Sim->ix(x,y)], -1, 1, 0, 255), 0, 255); 
        }
    }
    SimTex->SetData(img); 
    SimTex->Update(); 
    delete[] img; 
}

void Main::Render() 
{
    Graphics* g = GetGraphics(); 

    g->SetClearColor(Color::White); 
    g->Clear(true, true); 

    float xScale = (float) g->GetWidth() / W; 
    float yScale = (float) g->GetHeight() / H; 
    float scale = Min(xScale, yScale); 

    SB->Begin(); 
    SB->Draw(SimTex, 0, 0, W * scale, H * scale); 
    SB->DrawString(Color::Black, 
        "FPS: " + ToString((int) GetFPS()) + "\nSim Iteration: " + ToString(Updates), 
        10, SimTex->GetHeight() * scale + 20
    ); 
    SB->End(); 
}

ENGINE_MAIN_CLASS(Main) 
