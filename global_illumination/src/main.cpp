#include "Application/Application.h"


class Game : public GL::Application
{
public:
    Game(const GL::ApplicationParameters& params)
        :
        GL::Application(params)
    {

    }


    void GameSetUp() override
    {
        LOGINFO("Starting Test Game!!!");
    }

    void GameUpdate(f32 dt) override
    {
        ImGui::Begin("Camera");

        ImGui::Text("Position  | X: %f | Y: %f | Z: %f", scene.camera.pos.x, scene.camera.pos.y, scene.camera.pos.z);
        ImGui::Text("Direction | X: %f | Y: %f | Z: %f", scene.camera.dir.x, scene.camera.dir.y, scene.camera.dir.z);

        ImGui::End();


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

