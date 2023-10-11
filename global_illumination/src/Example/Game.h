#pragma once
#include "Application/Application.h"
#include "AssetManagement/AssetManagement.h"
#include "imgui/imgui.h"
#include <memory>

class Shutter : public GL::GeometryNodeBehavior
{
public:
    Shutter(u32 model_id, f32 speed, u32 id, GL::Window* win)
        :
        GL::GeometryNodeBehavior(model_id),
        m_speed(speed),
        m_id(id),
        m_window(win)
    {
    }

    void Update(f32 dt, GL::Transform& transform) override
    {
        if (m_window)
        {
            if (m_window->KeyIsPress(KEY_K) && m_key_is_pressed == false)
            {
                m_move = !m_move;
                m_key_is_pressed = true;
            }
            else if (!m_window->KeyIsPress(KEY_K))
            {
                m_key_is_pressed = false;
            }
        }

        ImGui(transform);

        if (m_move == false) return;

        transform.eulerRot.z += m_direction * m_speed * dt;

        if (transform.eulerRot.z > 5) {
            m_direction = -1;
            transform.eulerRot.z = 5.0f;
        }

        if (transform.eulerRot.z < -100.0f) {
            m_direction = 1;
            transform.eulerRot.z = -100.0f;
        }
    };

    void ImGui(GL::Transform& transform)
    {
        if (ImGui::CollapsingHeader(("Shutter" + std::to_string(m_id)).c_str()))
        {
            ImGui::PushID(m_id);
            ImGui::DragFloat("Rot", &transform.eulerRot.z);
            ImGui::PopID();
        }
    }

private:
    GL::Window* m_window = nullptr;
    bool m_key_is_pressed = false;
    bool m_move = false;
    u32 m_id;
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
        m_params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(0.0f, 5.0f, 0.0f);
        m_params.renderer_params.gi_params.voxelizer_params.size = glm::vec3(22.0f, 11.0f, 14.0f);
        m_params.renderer_params.gi_params.voxelizer_params.resolution = 32;

        m_scene.light = GL::Light(glm::vec3(-5.592f, 5.662f, -1.027f), glm::vec3(0.0f, 0.0f, -1.0f), GL::LightType::SPOTLIGHT);
        m_scene.light.m_radiance = glm::vec3(50);

        m_scene.camera.pos = glm::vec3(7.5f, 5.5f, 0.4f);
        m_scene.camera.dir = glm::vec3(-1.0f, 0.0f, 0.0f);

        u32 main = GL::AssetManagement::LoadFromObjFile("\\factory\\factory.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        m_scene.AddChild(std::move(node));
    }

    void SetUpSponza()
    {
        m_params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(0.0f, 8.0f, 0.0f);
        m_params.renderer_params.gi_params.voxelizer_params.size = glm::vec3(18.0f, 16.0f, 32.0f);
        m_params.renderer_params.gi_params.voxelizer_params.resolution = 128;

        m_scene.light = GL::Light(glm::vec3(-2.0f, 6.5f, -1.7f), glm::vec3(1.0f, 0.0f, 0.0f), GL::LightType::SPOTLIGHT);
        //m_scene.light = GL::Light(glm::vec3(30.0f, 100.0f, 0.0f), glm::vec3(-30.0f, -70.0f, 0.0f), GL::LightType::SPOTLIGHT);
        m_scene.light.m_radiance = glm::vec3(50);
        m_scene.light.m_shadow_bias = 0.00001f;

        m_scene.camera.pos = glm::vec3(0.0f, 6.5f, 7.333f);
        m_scene.camera.dir = glm::vec3(0.0f, 0.0f, -1.0f);

        u32 main = GL::AssetManagement::LoadFromObjFile("\\sponza\\sponza_dualcolor.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        m_scene.AddChild(std::move(node));
    }

    void SetUpGITestBox()
    {
        m_params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(0.0f, 0.0f, 6.0f);
        m_params.renderer_params.gi_params.voxelizer_params.size = glm::vec3(11.0f);
        m_params.renderer_params.gi_params.voxelizer_params.resolution = 32;

        m_scene.light = GL::Light(glm::vec3(0.0f, 5.0f, 8.0f), glm::vec3(0.0f, -1.0f, 0.0f), GL::LightType::SPOTLIGHT);
        m_scene.light.up = glm::vec3(1.0f, 0.0f, 0.0f);
        m_scene.light.m_radiance = glm::vec3(50.0f);

        m_scene.camera.pos = glm::vec3(-1.0f, 0.0f, 3.0f);
        m_scene.camera.dir = glm::vec3(0.0f, 0.0f, 1.0f);

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
        m_scene.AddChild(std::move(back_wall_node));

        // celing wall
        for (u32 i = 0; i < 3; i++)
        {
            for (u32 j = 0; j < 3; j++)
            {
                if (i == 1 && j == 1) continue;
                t.pos = glm::vec3(-1.0f + j, 2.0f, 9.0f - i);
                t.scale = glm::vec3(1.0f, 1.0f, 1.0f);
                GL::Node celing_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_white), t);
                m_scene.AddChild(std::move(celing_wall_node));
            }
        }

        // floor wall
        t.pos = glm::vec3(0.0f, -2.0f, 8.0f);
        t.scale = glm::vec3(3.0f, 1.0f, 3.0f);
        GL::Node floor_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_white), t);
        m_scene.AddChild(std::move(floor_wall_node));

        // left wall
        t.pos = glm::vec3(2.0f, 0.0f, 8.0f);
        t.scale = glm::vec3(1.0f, 3.0f, 3.0f);
        GL::Node left_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_red), t);
        m_scene.AddChild(std::move(left_wall_node));

        // right wall
        t.pos = glm::vec3(-2.0f, 0.0f, 8.0f);
        t.scale = glm::vec3(1.0f, 3.0f, 3.0f);
        GL::Node right_wall_node(std::make_unique<GL::GeometryNodeBehavior>(cube_blue), t);
        m_scene.AddChild(std::move(right_wall_node));

        // cube 1
        t.pos = glm::vec3(0.5f, -1.25f, 9.0f);
        t.eulerRot = glm::vec3(0.0f, 45.0f, 0.0f);
        t.scale = glm::vec3(0.75f, 2.0f, 0.75f);
        GL::Node cube1_node(std::make_unique<GL::GeometryNodeBehavior>(cube_white), t);
        m_scene.AddChild(std::move(cube1_node));

    }

    void SetUpSceneTripodroom()
    {
        m_params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(-4.8f, 3.0f, 0.0f);
        m_params.renderer_params.gi_params.voxelizer_params.size = glm::vec3(30.0f);
        m_params.renderer_params.gi_params.voxelizer_params.resolution = 128;

        m_scene.light = GL::Light(glm::vec3(-2.408f, 1.303f, -2.680f), glm::vec3(0.0f, 0.0f, 1.0f), GL::LightType::SPOTLIGHT);
        m_scene.light.m_radiance = glm::vec3(50);
        m_scene.light.m_cutOffAngle = 0.02f;
        m_scene.light.m_outercutOffAngle = 0.075f;
        m_scene.light.up = glm::vec3(1.0f, 0.0f, 0.0f);

        m_scene.camera.pos = glm::vec3(-2.0f, 1.2f, -2.4f);
        m_scene.camera.dir = glm::vec3(0.0f, 0.0f, 1.0f);

        GL::Transform t;

        u32 main = GL::AssetManagement::LoadFromObjFile("\\tripodroom\\garage.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        m_scene.AddChild(std::move(node));
    }

    void SetUpRoom()
    {
        m_params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(0.0f, 2.5f, 1.6f);
        m_params.renderer_params.gi_params.voxelizer_params.size = glm::vec3(8.0f, 7.0f, 12.5f);
        //params.renderer_params.gi_params.voxelizer_params.size = glm::vec3(15.0f, 15.0f, 15.0f);
        m_params.renderer_params.gi_params.voxelizer_params.resolution = 64;

        m_scene.light = GL::Light(glm::vec3(0.5f, 4.6f, -0.5f), glm::vec3(-0.84277f, -0.53905f, 0.0f), GL::LightType::SPOTLIGHT);
        m_scene.light.m_radiance = glm::vec3(50);
        m_scene.light.up = glm::vec3(1.0f, 0.0f, 0.0f);

        m_scene.camera.pos = glm::vec3(0.0f, 1.0f, 7.0f);
        m_scene.camera.dir = glm::vec3(0.0f, 0.0f, -1.0f);

        u32 main = GL::AssetManagement::LoadFromObjFile("\\room\\room.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        m_scene.AddChild(std::move(node));

        GL::Transform t;

        u32 shutter = GL::AssetManagement::LoadFromObjFile("\\room\\shutter.obj");

        t.pos = glm::vec3(0.5f, 2.05f, -2.0f);
        GL::Node shutter1_node(std::make_unique<Shutter>(shutter, 20.0f, 1, &m_window), t);
        m_scene.AddChild(std::move(shutter1_node));

        t.pos = glm::vec3(0.5f, 2.05f, 0.0f);
        GL::Node shutter2_node(std::make_unique<Shutter>(shutter, 20.0f, 2, &m_window), t);
        m_scene.AddChild(std::move(shutter2_node));

        t.pos = glm::vec3(0.5f, 2.05f, 2.0f);
        GL::Node shutter3_node(std::make_unique<Shutter>(shutter, 20.0f, 3, &m_window), t);
        m_scene.AddChild(std::move(shutter3_node));
    }

    void SetUpbreakfast_room()
    {
        m_params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(-0.5f, 3.5f, 3.0f);
        m_params.renderer_params.gi_params.voxelizer_params.size = glm::vec3(13.0f, 12.0f, 16.0f);
        m_params.renderer_params.gi_params.voxelizer_params.resolution = 64;

        m_scene.light = GL::Light(glm::vec3(20.3f, 4.7f, -0.9f), glm::vec3(-1.0f, 0.0f, 0.0f), GL::LightType::SPOTLIGHT);
        m_scene.light.m_radiance = glm::vec3(500);
        m_scene.light.m_shadow_bias = 0.00003f;

        m_scene.camera.pos = glm::vec3(0.0f, 0.0f, 0.0f);
        m_scene.camera.dir = glm::vec3(1.0f, 0.0f, 0.0f);

        u32 main = GL::AssetManagement::LoadFromObjFile("\\breakfast_room\\breakfast_room.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        m_scene.AddChild(std::move(node));
    }

    void SetUpLostEmpire()
    {
        m_params.renderer_params.gi_params.voxelizer_params.center = glm::vec3(0.0f, 0.0f, 0.0f);
        m_params.renderer_params.gi_params.voxelizer_params.size = glm::vec3(100.0f, 100.0f, 100.0f);
        m_params.renderer_params.gi_params.voxelizer_params.resolution = 128;

        m_scene.light = GL::Light(glm::vec3(20.3f, 4.7f, -0.9f), glm::vec3(-1.0f, 0.0f, 0.0f), GL::LightType::SPOTLIGHT);
        m_scene.light.m_radiance = glm::vec3(500);
        m_scene.light.m_shadow_bias = 0.00003f;

        m_scene.camera.pos = glm::vec3(0.0f, 0.0f, 0.0f);
        m_scene.camera.dir = glm::vec3(1.0f, 0.0f, 0.0f);

        u32 main = GL::AssetManagement::LoadFromObjFile("\\lostempire\\lost_empire.obj");
        GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));
        m_scene.AddChild(std::move(node));
    }

    void SetUp() override
    {
        SetUpSceneFactory();
        //SetUpSponza();
        //SetUpGITestBox();
        //SetUpSceneTripodroom();
        //SetUpRoom();
        //SetUpbreakfast_room();
        //SetUpLostEmpire();
    }

    void Update(f32 dt) override
    {
        if (m_window.KeyIsPressAsButton(KEY_F))
        {
            LOGDEBUG("Delta Time: %f", dt);
        }

        m_window.SetWindowTitle("FPS: " + std::to_string((int)glm::round(1 / dt)));

        if (m_window.KeyIsPress(KEY_E))
        {
            m_scene.light.m_pos = m_scene.camera.pos;
            m_scene.light.m_dir = m_scene.camera.dir;
        }

        if (m_window.KeyIsPressAsButton(KEY_R)) {
            GL::AssetManagement::ReloadShaders();
        }

        if (m_window.KeyIsPress(KEY_ESCAPE))
        {
            m_window.CloseWindow();
        }
    }
};