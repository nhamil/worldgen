#include <Fjord/Core/Entry.h> 
#include <Fjord/Core/Input.h> 
#include <Fjord/Core/Object.h> 
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
#include <Fjord/GUI/GUI.h> 
#include <Fjord/GUI/GUIElement.h> 
#include <Fjord/GUI/GUIEnvironment.h> 
#include <Fjord/GUI/GUIRenderer.h> 
#include <Fjord/GUI/Panel.h> 
#include <Fjord/Util/ClassId.h> 
#include <Fjord/Util/FileUtil.h> 
#include <Fjord/Util/Random.h> 
#include <Fjord/Util/Time.h> 

#include "WorldGen/World.h" 
#include "WorldGen/WorldGenerator.h" 

#include <iostream> 

using namespace Fjord; 
using namespace std; 

class Test
{
public: 
    Test() { cout << "ctor" << endl; } 
    ~Test()  {cout << "dtor" << endl; } 
};

class Main : public Application 
{
public: 
    virtual void Init() 
    {
        Random r; 

        FJ_FINFO("Application initializing..."); 

        FJ_FDEBUG("Init Zoom = %f", Zoom); 

        CubeMesh = new Mesh(); 

        Vector<uint32> indexData({
            0, 1, 3, 0, 3, 2, 
            5, 4, 6, 5, 6, 7,  

            3, 7, 6, 3, 6, 2, 
            0, 4, 5, 0, 5, 1,  
            1, 5, 7, 1, 7, 3, 
            4, 0, 2, 4, 2, 6 
        });

        Vector<Vector3> vertexData({
            {-1, -1, -1},
            {-1, -1,  1},
            {-1,  1, -1}, 
            {-1,  1,  1},

            { 1, -1, -1}, 
            { 1, -1,  1},
            { 1,  1, -1},
            { 1,  1,  1}, 
        });

        Vector<Vector2> texCoordData({
            {0, 1}, 
            {1, 1}, 
            {0, 0}, 
            {1, 0}, 

            {1, 1}, 
            {0, 1}, 
            {1, 0}, 
            {0, 0}
        });

        Vector<Vector4> colorData({
            {0, 0, 0, 1}, 
            {0, 0, 1, 1}, 
            {0, 1, 0, 1}, 
            {0, 1, 1, 1}, 

            {1, 0, 0, 1}, 
            {1, 0, 1, 1}, 
            {1, 1, 0, 1}, 
            {1, 1, 1, 1}, 
        });

        CubeMesh->SetVertices(vertexData); 
        CubeMesh->SetTexCoords(texCoordData); 
        CubeMesh->SetColors(colorData); 
        CubeMesh->SetIndices(indexData); 
        CubeMesh->Update(); 

        BasicShader = Shader::Load("../Assets/Shaders/Basic");
        TextShader = Shader::Load("../Assets/Shaders/Text"); 
        TestTexture = Texture2D::Load("../Assets/Textures/TestImage.png"); 

        MyFont = new Font("../Assets/Fonts/Monospace.ttf"); 

        auto gui = GetGUI(); 

        UI = new DragPanel(10, 10, 300, 200); 
        UI->SetColor(Color::White); 
        UI->SetAlignment(Alignment::BottomRight); 

        GUIElement* testUI = new GUIElement(0, 10, 100, 40); 
        testUI->SetColor(Color::Blue); 
        testUI->SetAlignment(Alignment::Bottom); 
        UI->AddChild(testUI); 

        UI2 = new DragPanel(10, 10, 230, 60); 
        UI2->SetAlignment(Alignment::BottomLeft); 
        UI2->SetColor(Color::White); 
        testUI = new GUIElement(10, 10, 100, 40); 
        testUI->SetColor(Color::Blue); 
        UI2->AddChild(testUI); 
        testUI = new GUIElement(120, 10, 100, 40); 
        testUI->SetColor(Color::Blue); 
        UI2->AddChild(testUI); 

        gui->AddChild(UI); 
        gui->AddChild(UI2); 

        GenWorld(); 

        FJ_FINFO("Done!"); 
    }

    virtual void Stop() 
    {
        FJ_FINFO("Finished successfully"); 
    }

    void Update(float dt) 
    {
        auto input = GetInput(); 

        float move = 5.0f * dt; 
        float mouseScale = 0.002 * Zoom * (1 + std::fabs(std::sin(Yaw))); 
        // float rot = 0.05f * FJ_TO_RAD; 

        if (input->GetKey(KeyI)) CamPos += move * Vector3::Forward; 
        if (input->GetKey(KeyK)) CamPos += move * Vector3::Backward; 
        if (input->GetKey(KeyJ)) CamPos += move * Vector3::Left; 
        if (input->GetKey(KeyL)) CamPos += move * Vector3::Right; 

        if (input->GetButton(1)) 
        {
            Vector2 dMouse = input->GetMouseMove(); 
            DYaw = dMouse.Y * mouseScale; 
            DPitch = -dMouse.X * mouseScale; 
        }

        if (input->GetKey(KeyG)) 
        {
            GenWorld(); 
        }

        Zoom *= 1 - 0.1 * input->GetMouseWheel(); 

        Yaw += DYaw; 
        Pitch += DPitch; 

        DYaw *= 0.9; 
        DPitch *= 0.9; 

        if (Yaw > FJ_PI_2 - 0.01) Yaw = FJ_PI_2 - 0.01; 
        if (Yaw < -FJ_PI_2 + 0.01) Yaw = -FJ_PI_2 + 0.01; 

        Quaternion rot = Quaternion::Identity; 
        rot = Quaternion::AxisAngle(Vector3::Up, Pitch); 
        rot = rot * Quaternion::AxisAngle(Vector3::Right, Yaw); 
        rot = Normalized(rot); 

        CamPos = rot * Vector3::Forward * 9; 

        // Vector2 dMouse = input->GetMouseMove(); 
        // dMouse *= rot; 

        // CamRot += dMouse; 
        // CamQuat = Quaternion::AxisAngle(Vector3::Left, -CamRot.Y) 
        //         * Quaternion::AxisAngle(Vector3::Up, CamRot.X); 
    }

    void Render() 
    {
        auto graphics = GetGraphics(); 

        graphics->SetDepthTest(true); 
        graphics->SetClearColor({0.6, 0.8, 1.0, 1.0}); 
        graphics->Clear(true, true); 

        Matrix4 tfm = Matrix4::Perspective(15.0 * Zoom * FJ_TO_RAD, 1.333, 0.1, 1000.0); 
        tfm *= Matrix4::LookAt(CamPos, Vector3::Zero); 
        tfm *= Matrix4::RotationY(90 * FJ_TO_RAD); 

        // graphics->SetTexture(0, TestTexture); 
        // graphics->SetTexture(0, MyFont->GetTexture()); 

        BasicShader->SetMatrix4("u_Model", tfm); 
        graphics->SetShader(BasicShader); 

        // graphics->SetGeometry(CubeMesh->GetGeometry()); 
        // graphics->DrawIndexed(Primitive::Triangles, 0, CubeMesh->GetIndexCount()); 

        graphics->ClearTextures(); 

        graphics->SetGeometry(EdgeMesh->GetGeometry()); 
        graphics->DrawIndexed(Primitive::Lines, 0, EdgeMesh->GetIndexCount()); 

        graphics->SetGeometry(CellMesh->GetGeometry()); 
        graphics->DrawIndexed(Primitive::Triangles, 0, CellMesh->GetIndexCount()); 

        Ref<SpriteBatch> sb = new SpriteBatch(); 
        sb->Begin(); 
        String info = ""; 
        info += "FPS: " + ToString((int) GetFPS()); 
        info += "\n"; 
        info += "UPS: " + ToString((int) GetUPS()); 
        sb->DrawString(Color::Black, 16, info.c_str(), 10, 20); 
        sb->Draw(TestTexture, 100, 200, 100, 100); 
        sb->End(); 
    }

    void GenWorld() 
    {
        Random r; 

        WorldGenerator gen; 
        gen.AddRule(new SubdivideCellGenRule(4)); 
        gen.AddRule(new CellDistortRule()); 
        gen.AddRule(new CellRelaxRule(200)); 
        gen.AddRule(new BasicTerrainGenRule()); 
        World = gen.Generate();

        // Sim = new FluidSim(FLUID_WIDTH, FLUID_HEIGHT); 
        // FluidMesh = nullptr; 
        // int done = 0; 
        // for (int y = 0; y < FLUID_HEIGHT; y++) 
        // {
        //     for (int x = 0; x < FLUID_WIDTH; x++) 
        //     {
        //         GeoCoords gc = {
        //             ((float) y / FLUID_HEIGHT) * -180 + 90, 
        //             ((float) x / FLUID_WIDTH) * -360 + 180
        //         };
        //         // FJ_LOG(Debug, "gc %0.1f %0.1f", gc.Latitude, gc.Longitude); 
        //         Vector3 pos = GetPositionFromGeoCoords(gc); 
        //         // FJ_LOG(Debug, "pos %0.3f %0.3f %0.3f", pos.X, pos.Y, pos.Z); 
        //         CellId cell = World.GetCellIdPyPosition(pos); 
        //         if (!IsTerrainWater(World.GetTerrain(cell))) 
        //         {
        //             Sim->SetWall(x, y, true); 
        //         }
        //     }
        //     // FJ_LOG(Debug, "%d", done++); 
        // }

        // for (int y = 0; y < Sim->GetHeight(); y++) 
        // {
        //     for (int x = 0; x < Sim->GetWidth(); x++) 
        //     {
        //         int index = x + Sim->GetWidth() * y; 
        //         if (Sim->IsWall(x, y)) 
        //         {
        //             Sim->SetVelocity(x, y, {0, 0}); 
        //         }
        //         else 
        //         {
        //             Sim->SetVelocity(x, y, Normalized(Vector2(r.NextFloat(), r.NextFloat()))); 
        //         }
        //     }
        // }


        FJ_LOG(Debug, 
            "Created world with %u cell and %u connections (%dkb, %0.1f seconds at 10MB/s)", 
            World.GetCellCount(), 
            World.GetConnectionCount(), 
            sizeof(Cell) * World.GetCellCount() / 1024, 
            sizeof(Cell) * World.GetCellCount() / 1024 / 1024 / 10.0
        ); 

        // stars 
        {
            Vector<Vector3> verts; 
            Vector<Vector4> colors; 
            for (int i = 0; i < 10000; i++) 
            {
                verts.push_back(Normalized(Vector3(
                    r.NextFloat(), 
                    r.NextFloat(), 
                    r.NextFloat()
                )) * 100); 
                colors.push_back({1, 1, 1, 1}); 
            }
            StarMesh = new Mesh(); 
            StarMesh->SetVertices(verts); 
            StarMesh->SetColors(colors); 
            StarMesh->Update(); 
        }

        GenMeshes(); 
    }

    void GenMeshes() 
    {
        // points 
        {
            Vector<Vector3> verts; 
            Vector<Vector4> colors; 
            for (CellId i = 0; i < World.GetCellCount(); i++) 
            {
                verts.push_back(World.GetPosition(i)); 
                colors.push_back({1, 1, 1, 1}); 
            }
            PointMesh = new Mesh(); 
            PointMesh->SetVertices(verts); 
            PointMesh->SetColors(colors); 
            PointMesh->Update(); 
        }

        // connections 
        {
            Vector<Vector3> verts; 
            Vector<Vector4> colors; 
            Vector<uint32> inds; 
            for (CellId i = 0; i < World.GetCellCount(); i++) 
            {
                verts.push_back(World.GetPosition(i)*1.01); 
                colors.push_back({0.2, 0.2, 0.2, 1}); 
                for (CellId ni = 0; ni < World.GetNeighborCount(i); ni++) 
                {
                    CellId n = World.GetNeighbor(i, ni); 
                    if (i < n) 
                    {
                        inds.push_back(i); 
                        inds.push_back(n); 
                    }
                }
            }
            ConnMesh = new Mesh(); 
            ConnMesh->SetVertices(verts); 
            ConnMesh->SetColors(colors); 
            ConnMesh->SetIndices(inds); 
            ConnMesh->Update(); 
        }

        // edges 
        {
            Vector<Vector3> verts; 
            Vector<Vector4> colors; 
            Vector<uint32> inds; 
            for (CellId i = 0; i < World.GetCellCount(); i++) 
            {
                Vector<Vector3> bounds = World.GetBounds(i); 
                unsigned start = verts.size(); 
                unsigned index = start; 
                for (Vector3& v : bounds) 
                {
                    verts.push_back(v*1.001); 
                    colors.push_back({0, 0, 0, 1}); 
                }
                for (unsigned v = 0; v < bounds.size() - 1; v++) 
                {
                    inds.push_back(index++); 
                    inds.push_back(index); 
                }
                inds.push_back(start); 
                inds.push_back(start+bounds.size()-1); 
            }
            EdgeMesh = new Mesh(); 
            EdgeMesh->SetVertices(verts); 
            EdgeMesh->SetColors(colors); 
            EdgeMesh->SetIndices(inds); 
            EdgeMesh->Update(); 
        }

        // cells 
        {
            Vector<Vector3> verts; 
            Vector<Vector4> colors; 
            Vector<uint32> inds; 
            for (CellId i = 0; i < World.GetCellCount(); i++) 
            {
                Vector<Vector3> bounds = World.GetBounds(i); 
                unsigned start = verts.size(); 
                unsigned index = start + 1; 
                for (Vector3& v : bounds) 
                {
                    verts.push_back(v); 
                    colors.push_back(GetTerrainColor(World.GetTerrain(i))); 
                }
                for (unsigned v = 0; v < bounds.size() - 2; v++) 
                {
                    inds.push_back(start); 
                    inds.push_back(index++); 
                    inds.push_back(index); 
                }
            }
            CellMesh = new Mesh(); 
            CellMesh->SetVertices(verts); 
            CellMesh->SetColors(colors); 
            CellMesh->SetIndices(inds); 
            CellMesh->Update(); 
        }
    }

    // camera
    Vector3 CamPos; 
    float Zoom = 1; 
    float Yaw = 0, Pitch = 0; 
    float DYaw = 0, DPitch = 0; 
    Quaternion CamQuat; 
    // graphics 
    Ref<Geometry> Geom; 
    Ref<IndexBuffer> IB; 
    Ref<VertexBuffer> VB; 
    Ref<Shader> BasicShader, TextShader; 
    Ref<Texture2D> TestTexture; 
    Ref<Mesh> CubeMesh; 
    // world gen 
    Ref<Mesh> PointMesh, ConnMesh, EdgeMesh, CellMesh, StarMesh, FluidMesh, FluidVelMesh; 
    class World World; 
    // gui 
    Ref<Font> MyFont; 
    Ref<Panel> UI, UI2; 
}; 

ENGINE_MAIN_CLASS(Main) 
