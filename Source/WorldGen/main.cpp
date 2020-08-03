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
#include <Fjord/Util/Thread.h> 
#include <Fjord/Util/Time.h> 

#include "WorldGen/World.h" 
#include "WorldGen/WorldGenerator.h" 

#include <iostream> 

using namespace Fjord; 
using namespace std; 

class Main; 

class TestThread : public Thread 
{
public: 
    TestThread(Main* main) : Main_(main) {} 

    virtual void Run() override; 

private: 
    Main* Main_; 
}; 

class Main : public Application 
{
public: 
    // TestThread thread; 

    virtual void Init() override
    {
        Random r; 

        FJ_ELOG_LEVEL(Info); 
        FJ_LOG_LEVEL(Debug); 
        
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

        GenThread = new TestThread(this); 
        GenThread->Start(); 

        StarMeshData = new MeshData(); 
        Vector<Vector3>& verts = StarMeshData->Vertices; 
        Vector<Vector4>& colors = StarMeshData->Colors; 
        verts.resize(100000); 
        colors.resize(100000); 
        // for (int i = 0; i < 100000; i++) 
        float theta, phi; 
        for (int i = 0; i < 100000; i++) 
        {
            // FJ_DEBUG("%f", r.NextFloat()); 
            theta = 2 * FJ_PI * r.NextFloat(); 
            phi = std::acos(2 * r.NextFloat() - 1.0f); 
            verts[i] = Normalized(Vector3(
                std::cos(theta) * std::sin(phi), 
                std::sin(theta) * std::sin(phi), 
                std::cos(phi) 
            )) * 900; 
            // verts[i] = Normalized(Quaternion::AxisAngle(Normalized(Vector3(
            //     r.NextFloat(), 
            //     r.NextFloat(), 
            //     r.NextFloat()
            // )), r.NextFloat() * FJ_2_PI) * Vector3::Right) * 900; 
            colors[i] = {1, 1, 1, 1}; 
        } 
        StarMesh = new Mesh(); 
        StarMeshData->Apply(StarMesh); 
        StarMeshData = nullptr; 

        FJ_INFO("Done!"); 
    }

    virtual void Stop() override
    {
        // FJ_DEBUG("Thread Join: %d", thread.Join()); 
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
        if (TicksToGenMeshes_ > 0) 
        {
            UI::Button("Finalizing..."); 
        }
        else if (RegenMeshes_) 
        {
            UI::Button("Generating..."); 
        }
        else if (UI::Button("Generate"))
        {
            RegenMeshes_ = true; 
            GenThread = new TestThread(this); 
            GenThread->Start(); 
        }
        UI::SameLine(); 
        if (UI::Button("Reset Zoom")) Zoom = 1.0; 

        WindowMode mode = GetWindow()->GetMode(); 

        UI::Separator(); 
        bool vsync = GetWindow()->IsVSyncEnabled(); 
        UI::Checkbox("VSync Enabled", &vsync); 
        if (UI::Button("Windowed")) mode = WindowMode::Windowed; 
        UI::SameLine(); 
        if (UI::Button("Fullscreen")) mode = WindowMode::Fullscreen; 
        if (UI::Button("Borderless")) mode = WindowMode::Borderless; 
        UI::SameLine(); 
        if (UI::Button("Quit")) Fjord::Stop(); 

        UI::EndWindow(); 

        GetWindow()->SetVSync(vsync); 
        GetWindow()->SetMode(mode); 
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

        if (TicksToGenMeshes_ > 0) TicksToGenMeshes_--; 

        if (TicksToGenMeshes_ == 0 && RegenMeshes_ && GenThread && !GenThread->IsRunning()) 
        {
            TicksToGenMeshes_ = -1; 
            RegenMeshes_ = false; 
            GenMeshes(); 
        }
        else if (TicksToGenMeshes_ == -1 && RegenMeshes_ && GenThread && !GenThread->IsRunning()) 
        {
            TicksToGenMeshes_ = 2; 
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

        Matrix4 tfm = Matrix4::Perspective(15.0 * Zoom * FJ_TO_RAD, graphics->GetAspectRatio(), 0.1, 1000.0); 
        tfm *= Matrix4::LookAt(CamPos, Vector3::Zero); 
        tfm *= Matrix4::RotationY(90 * FJ_TO_RAD); 
        // tfm *= Matrix4::RotationY(GetTimeSeconds()); 

        graphics->ClearTextures(); 

        BasicShader->SetMatrix4("u_Model", tfm); 
        graphics->SetShader(BasicShader); 

        if (DrawStars && StarMesh) 
        {
            graphics->SetGeometry(StarMesh->GetGeometry()); 
            graphics->Draw(Primitive::Points, 0, StarMesh->GetVertexCount()); 
        }

        // if () 
        // {
        //     graphics->SetGeometry(PointMesh->GetGeometry()); 
        //     graphics->Draw(Primitive::Points, 0, PointMesh->GetVertexCount()); 
        // }

        if (DrawCells && !DrawShaded && CellMesh) 
        {
            graphics->SetGeometry(CellMesh->GetGeometry()); 
            graphics->DrawIndexed(Primitive::Triangles, 0, CellMesh->GetIndexCount()); 
        }

        if (DrawConnections && ConnMesh) 
        {
            graphics->SetGeometry(ConnMesh->GetGeometry()); 
            graphics->DrawIndexed(Primitive::Lines, 0, ConnMesh->GetIndexCount()); 
        }

        if (DrawOutlines && EdgeMesh) 
        {
            graphics->SetGeometry(EdgeMesh->GetGeometry()); 
            graphics->DrawIndexed(Primitive::Lines, 0, EdgeMesh->GetIndexCount()); 
        }

        LightShader->SetMatrix4("u_Model", tfm); 
        LightShader->SetVector3("u_LightDirection", LightDirection); 
        graphics->SetShader(LightShader); 

        if (DrawCells && DrawShaded && CellMesh) 
        {
            graphics->SetGeometry(CellMesh->GetGeometry()); 
            graphics->DrawIndexed(Primitive::Triangles, 0, CellMesh->GetIndexCount()); 
        }

        Ref<SpriteBatch> sb = new SpriteBatch(); 
        sb->Begin(); 
        String info = ""; 
        info += "FPS: " + ToString((int) GetFPS()); 
        // info += "\n"; 
        // info += "UPS: " + ToString((int) GetUPS()); 
        sb->DrawString(Color::White, 16, info.c_str(), 10, 20); 
        // sb->Draw(TestTexture, 100, 200, 100, 100); 
        sb->End(); 
    }

    void GenWorld() 
    {
        GenTime = GetTimeSeconds(); 

        WorldGenerator gen; 
        gen.AddRule(new SubdivideCellGenRule(WorldSize)); 
        if (WorldSize > 1) gen.AddRule(new CellDistortRule()); 
        gen.AddRule(new CellRelaxRule(50)); 
        gen.AddRule(new BasicTerrainGenRule()); 
        World = gen.Generate();

        FJ_DEBUG("Generated world!"); 
        FJ_DEBUG("Generating meshes..."); 

        GenMeshData(); 
    }

    void GenMeshData() 
    {
        Random r; 

        // // points 
        // {
        //     FJ_DEBUG("Generating point mesh..."); 
        //     PointMeshData = new MeshData(); 
        //     Vector<Vector3>& verts = PointMeshData->Vertices; 
        //     Vector<Vector4>& colors = PointMeshData->Colors; 
        //     verts.resize(World.GetCellCount()); 
        //     colors.resize(World.GetCellCount()); 
        //     // for (CellId i = 0; i < World.GetCellCount(); i++) 
        //     ParallelFor(CellId(0), CellId(World.GetCellCount()), [&](CellId i)
        //     {
        //         verts[i] = World.GetPosition(i); 
        //         colors[i] = {1, 1, 1, 1}; 
        //     });
        // }

        FJ_DEBUG("Generating connection mesh..."); 

        unsigned neighborCount = 0; 
        Vector<unsigned> neighborIndex; neighborIndex.reserve(World.GetConnectionCount()); 
        for (CellId i = 0; i < World.GetCellCount(); i++) 
        {
            neighborIndex.push_back(neighborCount); 
            neighborCount += World.GetNeighborCount(i); 
        }

        // connections 
        {
            ConnMeshData = new MeshData(); 
            Vector<Vector3>& verts = ConnMeshData->Vertices; 
            Vector<Vector4>& colors = ConnMeshData->Colors; 
            Vector<uint32>& inds = ConnMeshData->Indices; 
            verts.resize(World.GetCellCount()); 
            colors.resize(World.GetCellCount()); 
            // TODO uses more memory than is needed
            inds.resize(neighborCount * 2); 
            // for (CellId i = 0; i < World.GetCellCount(); i++) 
            ParallelFor(CellId(0), CellId(World.GetCellCount()), [&](CellId i)
            {
                verts[i] = World.GetPosition(i)*1.01; 
                colors[i] = {0.1, 0.1, 0.1, 1}; 
                unsigned nOffset = neighborIndex[i] * 2; 
                for (CellId ni = 0; ni < World.GetNeighborCount(i); ni++) 
                {
                    CellId n = World.GetNeighbor(i, ni); 
                    if (i < n) 
                    {
                        inds[nOffset++] = i; 
                        inds[nOffset++] = n; 
                    }
                }
            }); 
        }

        FJ_DEBUG("Generating outline mesh..."); 

        // edges 
        {
            EdgeMeshData = new MeshData(); 
            Vector<Vector3>& verts = EdgeMeshData->Vertices; 
            Vector<Vector4>& colors = EdgeMeshData->Colors; 
            Vector<uint32>& inds = EdgeMeshData->Indices; 
            verts.resize(neighborCount); 
            colors.resize(neighborCount); 
            inds.resize(neighborCount * 2); 
            // for (CellId i = 0; i < World.GetCellCount(); i++) 
            ParallelFor(CellId(0), CellId(World.GetCellCount()), [&](CellId i)
            {
                Vector<Vector3> bounds = World.GetBounds(i); 
                unsigned start = neighborIndex[i]; //verts.size(); 
                unsigned index = start; 
                for (Vector3& v : bounds) 
                {
                    verts[index] = v*1.001; 
                    colors[index++] = {0, 0, 0, 1}; 
                }

                index = start; 

                unsigned ind = neighborIndex[i] * 2; 
                for (unsigned v = 0; v < bounds.size() - 1; v++) 
                {
                    inds[ind++] = (index++); 
                    inds[ind++] = (index); 
                }
                inds[ind++] = (start); 
                inds[ind++] = (start+bounds.size()-1); 
            }); 
        }

        FJ_DEBUG("Generating cell mesh..."); 

        // cells 
        {
            CellMeshData = new MeshData(); 
            Vector<Vector3>& verts = CellMeshData->Vertices; 
            Vector<Vector3>& normals = CellMeshData->Normals; 
            Vector<Vector4>& colors = CellMeshData->Colors; 
            Vector<uint32>& inds = CellMeshData->Indices; 
            // verts.reserve(World.GetCellCount() * 6); 
            // colors.reserve(World.GetCellCount() * 6); 
            // normals.reserve(World.GetCellCount() * 6); 
            // inds.reserve(World.GetCellCount() * 6 * 3); 
            verts.resize(neighborCount); 
            normals.resize(neighborCount); 
            colors.resize(neighborCount); 
            inds.resize((neighborCount - World.GetCellCount()*2) * 3); 
            // for (CellId i = 0; i < World.GetCellCount(); i++) 
            ParallelFor(CellId(0), CellId(World.GetCellCount()), [&](CellId i)
            {
                Vector3 normal = World.GetPosition(i); 
                Vector<Vector3> bounds = World.GetBounds(i); 
                unsigned start = neighborIndex[i]; //verts.size(); 
                unsigned index = start; 
                for (Vector3& v : bounds) 
                {
                    verts[index] = v; 
                    normals[index] = normal; 
                    colors[index++] = GetTerrainColor(World.GetTerrain(i)); 
                }

                index = start + 1; 
                
                unsigned ind = (neighborIndex[i] - i*2) * 3; 
                for (unsigned v = 0; v < bounds.size() - 2; v++) 
                {
                    inds[ind++] = (start); 
                    inds[ind++] = (index++); 
                    inds[ind++] = (index); 
                }
            }); 
        }
    }

    void GenMeshes() 
    {
        FJ_DEBUG("Finalizing..."); 

        // StarMesh = new Mesh(); 
        // StarMeshData->Apply(StarMesh); 
        // StarMeshData = nullptr; 

        // PointMesh = new Mesh(); 
        // PointMeshData->Apply(PointMesh); 
        // PointMeshData = nullptr; 

        ConnMesh = new Mesh(); 
        ConnMeshData->Apply(ConnMesh); 
        ConnMeshData = nullptr; 

        EdgeMesh = new Mesh(); 
        EdgeMeshData->Apply(EdgeMesh); 
        EdgeMeshData = nullptr; 

        CellMesh = new Mesh(); 
        CellMeshData->Apply(CellMesh); 
        CellMeshData = nullptr; 

        GenTime = GetTimeSeconds() - GenTime; 

        FJ_LOG(Debug, 
            "Done! Created world with %u cell and %u connections in %0.1f seconds (%dkb)", 
            World.GetCellCount(), 
            World.GetConnectionCount(), 
            GenTime, 
            sizeof(Cell) * World.GetCellCount() / 1024 
        ); 
    }

    // camera
    Vector3 CamPos; 
    float Zoom = 1; 
    float Yaw = 0, Pitch = 0; 
    float DYaw = 0, DPitch = 0; 
    Quaternion CamQuat; 
    // graphics 
    bool RegenMeshes_ = true; 
    int TicksToGenMeshes_ = -1; 
    double GenTime = 0; 
    Ref<Geometry> Geom; 
    Ref<IndexBuffer> IB; 
    Ref<VertexBuffer> VB; 
    Ref<Shader> BasicShader, LightShader; 
    Ref<Texture2D> TestTexture; 
    Ref<Mesh> CubeMesh; 
    bool DrawGrid = true; 
    bool DrawConnections = false; 
    bool DrawOutlines = false; 
    bool DrawCells = true; 
    bool DrawStars = true; 
    bool DrawShaded = true; 
    bool CameraLight = true; 
    Vector3 LightDirection = Vector3::Right; 
    // world gen 
    Ref<MeshData> PointMeshData, ConnMeshData, EdgeMeshData, CellMeshData, StarMeshData; 
    Ref<Mesh> PointMesh, ConnMesh, EdgeMesh, CellMesh, StarMesh, FluidMesh, FluidVelMesh; 
    class World World; 
    int WorldSize = 5; 
    Ref<Thread> GenThread; 
    // gui 
    Ref<Font> MyFont; 
    float ZoomSens = 1.0; 
    // Ref<Widget> UI, UI2; 
}; 

void TestThread::Run() 
{
    Main_->RegenMeshes_ = true; 
    Main_->GenWorld(); 
}

ENGINE_MAIN_CLASS(Main) 
