#include "Application/Application.h"
#include "AssetManagement/AssetManagement.h"
#include <memory>

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

    void GameSetUp() override
    {        
        SetUpSceneFactory();
        //SetUpSponza();

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

