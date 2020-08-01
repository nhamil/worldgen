#include <Fjord/Core/Entry.h> 
#include <Fjord/Core/Input.h> 
#include <Fjord/Core/Object.h> 
#include <Fjord/Core/UI.h> 
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
// #include <Fjord/GUI/Button.h> 
// #include <Fjord/GUI/Frame.h> 
// #include <Fjord/GUI/Widget.h> 
// #include <Fjord/GUI/GUIEnvironment.h> 
// #include <Fjord/GUI/GUIRenderer.h> 
// #include <Fjord/GUI/Label.h> 
// #include <Fjord/GUI/Layout.h> 
// #include <Fjord/GUI/Panel.h> 
#include <Fjord/Util/ClassId.h> 
#include <Fjord/Util/FileUtil.h> 
#include <Fjord/Util/Random.h> 
#include <Fjord/Util/Time.h> 

#include "WorldGen/World.h" 
#include "WorldGen/WorldGenerator.h" 

#include <iostream> 

using namespace Fjord; 
using namespace std; 

class Main : public Application 
{
public: 
    virtual void Init() override
    {
        Random r; 

        FJ_ELOG_LEVEL(Info); 

        FJ_INFO("Application initializing..."); 

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

        BasicShader = Shader::Load("Basic");
        LightShader = Shader::Load("Shaded"); 
        TestTexture = Texture2D::Load("TestImage"); 

        MyFont = new Font("Default"); 

        GenWorld(); 

        FJ_INFO("Done!"); 
    }

    virtual void Stop() override
    {
        FJ_INFO("Finished successfully"); 
    }

    virtual void UpdateGUI(float dt) override
    {
        (void) dt; 

        UI::SetNextWindowPosition(20, 100); 
        UI::SetNextWindowSize(300, 200-10); 
        UI::BeginWindow("WorldGen Util", UI::WindowFlagAutoResize); 
        
        // settings 
        UI::Checkbox("Show Stars", &DrawStars); 
        UI::SameLine(); 
        UI::Checkbox("Show Outlines", &DrawOutlines); 

        UI::Checkbox("Show Links", &DrawConnections); 
        UI::SameLine(); 
        UI::Checkbox("Show Cells", &DrawCells); 

        UI::Checkbox("Lighting Enabled", &DrawShaded); 
        UI::SameLine(); 
        UI::Checkbox("Camera Light", &CameraLight); 

        UI::Separator(); 
        UI::SliderFloat("Light Dir X", &LightDirection[0], -1.0, 1.0, 0, "%0.3f"); //UI::SameLine(); 
        UI::SliderFloat("Light Dir Y", &LightDirection[1], -1.0, 1.0, 0, "%0.3f"); //UI::SameLine(); 
        UI::SliderFloat("Light Dir Z", &LightDirection[2], -1.0, 1.0, 0, "%0.3f"); 
        LightDirection = Normalized(LightDirection); 
        UI::Separator(); 

        float size = WorldSize; 
        UI::SliderFloat("Next World Size", &size, 0, 10, 10, "%0.0f"); 
        WorldSize = (int) size; 

        UI::SliderFloat("Zoom Sensitivity", &ZoomSens, 0.001, 2, 0, "%0.3f"); 
        if (UI::Button("Generate")) GenWorld(); 
        UI::SameLine(); 
        if (UI::Button("Reset Zoom")) Zoom = 1.0; 

        // gen world 
        // for (int i = 0; i <= 10; i++) 
        // {
        //     UI::PushId(i); 
        //     if (i % 2 == 0) UI::SameLine(); 
        //     if (UI::Button("Gen Size " + ToString(i))) 
        //     {
        //         WorldSize = i; 
        //         if (i >= 9) 
        //         {
        //             FJ_INFO("This will take a very long time"); 
        //         }
        //         else if (i >= 7) 
        //         {
        //             FJ_INFO("This will take a while"); 
        //         }
        //         GenWorld(); 
        //     }
        //     UI::PopId(); 
        // }
        UI::EndWindow(); 
    }

    virtual void Update(float dt) override 
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

        Zoom *= 1 - (0.1 * input->GetMouseWheel()) * ZoomSens; 

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

        Zoom = Clamp(Zoom, 0.0001, 5.0); 

        if (CameraLight) 
        {
            Matrix4 tfm = Matrix4::RotationY(90 * FJ_TO_RAD); 
            LightDirection = Normalized(Transform(tfm, -CamPos, true)); 
            LightDirection.Y *= -1; 

            Vector3 right = Normalized(Cross(Vector3::Up, LightDirection)); 
            Vector3 up = Normalized(Cross(right, LightDirection)); 
            LightDirection = Normalized(Quaternion::AxisAngle(up, 45 * FJ_TO_RAD) * LightDirection); 
        }
    }

    virtual void Render() override 
    {
        auto graphics = GetGraphics(); 

        graphics->SetDepthTest(true); 
        graphics->SetClearColor(Color::Black); 
        graphics->Clear(true, true); 

        graphics->SetPointSize(1); 
        graphics->SetLineWidth(1); 

        Matrix4 tfm = Matrix4::Perspective(15.0 * Zoom * FJ_TO_RAD, 1.333, 0.1, 1000.0); 
        tfm *= Matrix4::LookAt(CamPos, Vector3::Zero); 
        tfm *= Matrix4::RotationY(90 * FJ_TO_RAD); 

        graphics->ClearTextures(); 

        BasicShader->SetMatrix4("u_Model", tfm); 
        graphics->SetShader(BasicShader); 

        if (DrawStars) 
        {
            graphics->SetGeometry(StarMesh->GetGeometry()); 
            graphics->Draw(Primitive::Points, 0, StarMesh->GetVertexCount()); 
        }

        // if () 
        // {
        //     graphics->SetGeometry(PointMesh->GetGeometry()); 
        //     graphics->Draw(Primitive::Points, 0, PointMesh->GetVertexCount()); 
        // }

        if (DrawCells && !DrawShaded) 
        {
            graphics->SetGeometry(CellMesh->GetGeometry()); 
            graphics->DrawIndexed(Primitive::Triangles, 0, CellMesh->GetIndexCount()); 
        }

        if (DrawConnections) 
        {
            graphics->SetGeometry(ConnMesh->GetGeometry()); 
            graphics->DrawIndexed(Primitive::Lines, 0, ConnMesh->GetIndexCount()); 
        }

        if (DrawOutlines) 
        {
            graphics->SetGeometry(EdgeMesh->GetGeometry()); 
            graphics->DrawIndexed(Primitive::Lines, 0, EdgeMesh->GetIndexCount()); 
        }

        LightShader->SetMatrix4("u_Model", tfm); 
        LightShader->SetVector3("u_LightDirection", LightDirection); 
        graphics->SetShader(LightShader); 

        if (DrawCells && DrawShaded) 
        {
            graphics->SetGeometry(CellMesh->GetGeometry()); 
            graphics->DrawIndexed(Primitive::Triangles, 0, CellMesh->GetIndexCount()); 
        }

        Ref<SpriteBatch> sb = new SpriteBatch(); 
        sb->Begin(); 
        String info = ""; 
        info += "FPS: " + ToString((int) GetFPS()); 
        info += "\n"; 
        info += "UPS: " + ToString((int) GetUPS()); 
        sb->DrawString(Color::White, 16, info.c_str(), 10, 20); 
        // sb->Draw(TestTexture, 100, 200, 100, 100); 
        sb->End(); 
    }

    void GenWorld() 
    {
        WorldGenerator gen; 
        gen.AddRule(new SubdivideCellGenRule(WorldSize)); 
        if (WorldSize > 1) gen.AddRule(new CellDistortRule()); 
        gen.AddRule(new CellRelaxRule(200)); 
        gen.AddRule(new BasicTerrainGenRule()); 
        World = gen.Generate();

        FJ_LOG(Debug, 
            "Created world with %u cell and %u connections (%dkb, %0.1f seconds at 10MB/s)", 
            World.GetCellCount(), 
            World.GetConnectionCount(), 
            sizeof(Cell) * World.GetCellCount() / 1024, 
            sizeof(Cell) * World.GetCellCount() / 1024 / 1024 / 10.0
        ); 

        GenMeshes(); 
    }

    void GenMeshes() 
    {
        Random r; 

        // stars 
        {
            Vector<Vector3> verts; 
            Vector<Vector4> colors; 
            for (int i = 0; i < 100000; i++) 
            {
                verts.push_back(Normalized(Vector3(
                    r.NextFloat(), 
                    r.NextFloat(), 
                    r.NextFloat()
                )) * 900); 
                colors.push_back({1, 1, 1, 1}); 
            }
            StarMesh = new Mesh(); 
            StarMesh->SetVertices(verts); 
            StarMesh->SetColors(colors); 
            StarMesh->Update(); 
        }

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
                colors.push_back({0.1, 0.1, 0.1, 1}); 
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
            Vector<Vector3> normals; 
            Vector<Vector4> colors; 
            Vector<uint32> inds; 
            for (CellId i = 0; i < World.GetCellCount(); i++) 
            {
                Vector3 normal = World.GetPosition(i); 
                Vector<Vector3> bounds = World.GetBounds(i); 
                unsigned start = verts.size(); 
                unsigned index = start + 1; 
                for (Vector3& v : bounds) 
                {
                    verts.push_back(v); 
                    normals.push_back(normal); 
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
            CellMesh->SetNormals(normals); 
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
    Ref<Shader> BasicShader, LightShader; 
    Ref<Texture2D> TestTexture; 
    Ref<Mesh> CubeMesh; 
    bool DrawGrid = true; 
    bool DrawConnections = false; 
    bool DrawOutlines = true; 
    bool DrawCells = true; 
    bool DrawStars = true; 
    bool DrawShaded = true; 
    bool CameraLight = false; 
    Vector3 LightDirection = Vector3::Right; 
    // world gen 
    Ref<Mesh> PointMesh, ConnMesh, EdgeMesh, CellMesh, StarMesh, FluidMesh, FluidVelMesh; 
    class World World; 
    int WorldSize = 5; 
    // gui 
    Ref<Font> MyFont; 
    float ZoomSens = 1.0; 
    // Ref<Widget> UI, UI2; 
}; 

ENGINE_MAIN_CLASS(Main) 
