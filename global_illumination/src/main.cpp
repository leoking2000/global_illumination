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
        SetUpSceneFactory();
        //SetUpSceneGITest1();
    }

    void SetUpSceneFactory()
    {
        //scene.light = GL::Light(glm::vec3(0.0f, 6.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), GL::LightType::SPOTLIGHT);

        //u32 main = GL::AssetManagement::LoadFromObjFile("\\level1\\level1.obj");
        u32 main = GL::AssetManagement::LoadFromObjFile("\\factory\\factory.obj");

        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));

        scene.AddChild(std::move(node));
    }

    void SetUpSceneGITest1()
    {
        scene.camera.pos = glm::vec3(0.0f, 0.0f, -17.0f);
        scene.camera.dir = glm::vec3(0.0f, 0.0f, 1.0f);

        scene.light = GL::Light(glm::vec3(0.0f, 6.0f, 5.0f), glm::vec3(0.0028f, -1.0f, 0.0f), GL::LightType::SPOTLIGHT);

        // wall mesh
        u32 wall_mesh = GL::AssetManagement::CreateMesh(GL::DefaultShape::CUBE);

        GL::Material mat;

        // white wall model
        mat.Albedo = glm::vec3(1.0f);
        u32 white_wall_model = GL::AssetManagement::CreateModel(wall_mesh, mat);

        // red wall model
        mat.Albedo = glm::vec3(1.0f, 0.0f, 0.0f);
        u32 red_wall_model = GL::AssetManagement::CreateModel(wall_mesh, mat);

        // green wall model
        mat.Albedo = glm::vec3(0.0f, 1.0f, 0.0f);
        u32 green_wall_model = GL::AssetManagement::CreateModel(wall_mesh, mat);

        // blue wall model
        mat.Albedo = glm::vec3(0.0f, 0.0f, 1.0f);
        u32 blue_wall_model = GL::AssetManagement::CreateModel(wall_mesh, mat);

        GL::Transform transform;

        // floor Wall
        transform.pos = glm::vec3(0.0f, -3.0f, 5.0f);
        transform.scale = glm::vec3(30.0f, 1.0f, 30.0f);
        GL::Node floor(std::make_unique<GL::GeometryNodeBehavior>(white_wall_model), transform);


        scene.AddChild(std::move(floor));
    }


    void GameSetUp() override
    {        
        LOGINFO("Starting Test Game!!!");
    }

    void GameUpdate(f32 dt) override
    {
        if (window.KeyIsPressAsButton(KEY_F))
        {
            LOGDEBUG("Delta Time: %f", dt);
        }

        if (window.MouseButtonIsPress(MOUSE_BUTTON_LEFT))
        {
            glm::vec2 pos = window.MousePos();
            window.SetWindowTitle("(" + std::to_string(pos.x) + " , " + std::to_string(pos.y) + ")");
        }
        else
        {
            window.SetWindowTitle("FPS: " + std::to_string((int)glm::round(1 / dt)));
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

    p.background_color = glm::vec3(0.0f);
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

