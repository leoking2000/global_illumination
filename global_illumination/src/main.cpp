#include "Application/Application.h"
#include "AssetManagement/AssetManagement.h"
#include <memory>

class Shutter : public GL::GeometryNodeBehavior
{
public:
    Shutter(u32 model_id, f32 speed)
        :
        GL::GeometryNodeBehavior(model_id),
        m_speed(speed)
    {
    }

    void Update(f32 dt, GL::Transform& transform) override
    {
        transform.eulerRot.z += m_direction * m_speed * dt;

        if (transform.eulerRot.z > 0) {
            m_direction = -1;
            transform.eulerRot.z = 0.0f;
        }

        if (transform.eulerRot.z < -90.0f) {
            m_direction = 1;
            transform.eulerRot.z = -90.0f;
        }
    };
private:
    f32 m_speed;
    f32 m_direction = -1;
};

class Game : public GL::Application
{
public:
    Game(const GL::ApplicationParameters& params)
        :
        GL::Application(params)
    {

    }

    void SetUpSceneFactory()
    {
        scene.light = GL::Light(glm::vec3(0.0f, 6.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), GL::LightType::SPOTLIGHT);
        u32 main = GL::AssetManagement::LoadFromObjFile("\\factory\\factory.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        scene.AddChild(std::move(node));
    }

    void SetUpSponza()
    {
        scene.light = GL::Light(glm::vec3(-2.0f, 6.5f, -1.7f), glm::vec3(1.0f, 0.0f, 0.0f), GL::LightType::SPOTLIGHT);
        u32 main = GL::AssetManagement::LoadFromObjFile("\\sponza\\sponza_dualcolor.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        scene.AddChild(std::move(node));
    }

    void SetUpGITestBox()
    {
        params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(0.0f, 0.0f, 6.0f);
        params.renderer_params.gi_params.voxelizer_params.size = 10.0f;

        scene.light = GL::Light(glm::vec3(0.0f, 5.0f, 8.0f), glm::vec3(0.0f, -1.0f, 0.0f), GL::LightType::SPOTLIGHT);
        scene.light.up = glm::vec3(1.0f, 0.0f, 0.0f);

        scene.camera.pos = glm::vec3(-1.0f, 0.0f, 3.0f);
        scene.camera.dir = glm::vec3(0.0f, 0.0f, 1.0f);

        GL::Transform t;
        u32 cube = GL::AssetManagement::CreateMesh(GL::DefaultShape::CUBE);

        u32 cube_white = GL::AssetManagement::CreateModel(cube);
        GL::AssetManagement::GetModel(cube_white)->m_materials[0].Albedo = glm::vec3(1.0f);

        u32 cube_red = GL::AssetManagement::CreateModel(cube);
        GL::AssetManagement::GetModel(cube_red)->m_materials[0].Albedo = glm::vec3(1.0f, 0.0f, 0.0f);

        u32 cube_blue = GL::AssetManagement::CreateModel(cube);
        GL::AssetManagement::GetModel(cube_blue)->m_materials[0].Albedo = glm::vec3(0.0f, 0.0f, 1.0f);


        // back wall
        t.pos = glm::vec3(0.0f, 0.0f, 10.0f);
        t.scale = glm::vec3(3.0f, 3.0f, 1.0f);
        GL::Node back_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_white), t);
        scene.AddChild(std::move(back_wall_node));

        // celing wall
        for (u32 i = 0; i < 3; i++)
        {
            for (u32 j = 0; j < 3; j++)
            {
                if (i == 1 && j == 1) continue;
                t.pos = glm::vec3(-1.0f + j, 2.0f, 9.0f - i);
                t.scale = glm::vec3(1.0f, 1.0f, 1.0f);
                GL::Node celing_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_white), t);
                scene.AddChild(std::move(celing_wall_node));
            }
        }

        // floor wall
        t.pos = glm::vec3(0.0f, -2.0f, 8.0f);
        t.scale = glm::vec3(3.0f, 1.0f, 3.0f);
        GL::Node floor_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_white), t);
        scene.AddChild(std::move(floor_wall_node));

        // left wall
        t.pos = glm::vec3(2.0f, 0.0f, 8.0f);
        t.scale = glm::vec3(1.0f, 3.0f, 3.0f);
        GL::Node left_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_red), t);
        scene.AddChild(std::move(left_wall_node));

        // right wall
        t.pos = glm::vec3(-2.0f, 0.0f, 8.0f);
        t.scale = glm::vec3(1.0f, 3.0f, 3.0f);
        GL::Node right_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_blue), t);
        scene.AddChild(std::move(right_wall_node));

        // cube 1
        t.pos = glm::vec3(0.5f, -1.25f, 9.0f);
        t.eulerRot = glm::vec3(0.0f, 45.0f, 0.0f);
        t.scale = glm::vec3(0.75f, 2.0f, 0.75f);
        GL::Node cube1_node(std::make_unique<GL::GeometryNodeBehavior>(cube_white), t);
        scene.AddChild(std::move(cube1_node));
        
    }

    void SetUpSceneTripodroom()
    {
        params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(-4.8f, 3.0f, 0.0f);
        params.renderer_params.gi_params.voxelizer_params.size = 11.0f;

        scene.light = GL::Light(glm::vec3(0.0f, 5.0f, 8.0f), glm::vec3(0.0f, -1.0f, 0.0f), GL::LightType::SPOTLIGHT);
        scene.light.up = glm::vec3(1.0f, 0.0f, 0.0f);

        scene.camera.pos = glm::vec3(0.0f, 0.0f, 0.0f);
        scene.camera.dir = glm::vec3(1.0f, 0.0f, 1.0f);

        GL::Transform t;

        u32 main = GL::AssetManagement::LoadFromObjFile("\\tripodroom\\garage.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        scene.AddChild(std::move(node));
    }

    void SetUpRoom()
    {
        params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(0.0f, 3.0f, 1.6f);
        params.renderer_params.gi_params.voxelizer_params.size = 22.0f;

        scene.light = GL::Light(glm::vec3(0.5f, 4.6f, -0.5f), glm::vec3(-0.84277f, -0.53905f, 0.0f), GL::LightType::SPOTLIGHT);
        scene.light.up = glm::vec3(1.0f, 0.0f, 0.0f);

        scene.camera.pos = glm::vec3(0.0f, 1.0f, 7.0f);
        scene.camera.dir = glm::vec3(0.0f, 0.0f, -1.0f);

        u32 main = GL::AssetManagement::LoadFromObjFile("\\room\\room.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        scene.AddChild(std::move(node));

        GL::Transform t;

        u32 shutter = GL::AssetManagement::LoadFromObjFile("\\room\\shutter.obj");

        t.pos = glm::vec3(0.5f, 2.05f, -2.0f);
        GL::Node shutter1_node(std::make_unique<Shutter>(shutter, 50.0f), t);
        scene.AddChild(std::move(shutter1_node));

        t.pos = glm::vec3(0.5f, 2.05f, 0.0f);
        GL::Node shutter2_node(std::make_unique<Shutter>(shutter, 50.0f), t);
        scene.AddChild(std::move(shutter2_node));

        t.pos = glm::vec3(0.5f, 2.05f, 2.0f);
        GL::Node shutter3_node(std::make_unique<Shutter>(shutter, 50.0f), t);
        scene.AddChild(std::move(shutter3_node));
    }

    void GameSetUp() override
    {        
        //SetUpSceneFactory();
        //SetUpSponza();
        //SetUpGITestBox();
        //SetUpSceneTripodroom();
        SetUpRoom();

        LOGINFO("Starting Test Game!!!");
    }

    void GameUpdate(f32 dt) override
    {
        if (window.KeyIsPressAsButton(KEY_F))
        {
            LOGDEBUG("Delta Time: %f", dt);
        }

        window.SetWindowTitle("FPS: " + std::to_string((int)glm::round(1 / dt)));

        if (window.KeyIsPress(KEY_E))
        {
            scene.light.m_pos = scene.camera.pos;
            scene.light.m_dir = scene.camera.dir;
        }

        if (window.KeyIsPressAsButton(KEY_R)) {
            GL::AssetManagement::ReloadShaders();
        }

        if (window.KeyIsPress(KEY_ESCAPE))
        {
            window.CloseWindow();
        }
    }

};


int main()
{
    GL::Logger::SetLoggingLevel(GL::LoggingLevel::DEBUG);

    GL::ApplicationParameters p;
    p.windows_params.width = 1600;
    p.windows_params.height = 900;
    p.windows_params.title = "TestGame";
    p.windows_params.isResizable = true;
    p.windows_params.vsync = false;
    p.asset_dir = "C:\\Users\\User\\Desktop\\dev\\computer_graphics\\ptixiaki\\EngineData";

    try {
        Game game(p);
        game.Run();
    }
    catch (const std::runtime_error& e)
    {
        LOGERROR(e.what());
        return -1;
    }

    return 0;
}

