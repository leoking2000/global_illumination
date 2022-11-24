#include "Application.h"
#include "AssetManagement/AssetManagement.h"
#include "SceneGraph/NodeBehavior.h"

namespace GL
{

	Application::Application(const ApplicationParameters& params)
		:
		window(params.windows_params),
		renderer(params.windows_params.width, params.windows_params.height, window)
	{
		renderer.parameters.background_color = params.background_color;

		AssetManagement::SetAssetDir(params.asset_dir);

		scene.camera.pos = params.cam_pos;
		scene.camera.dir = params.cam_dir;

		scene.light = Light(glm::vec3(0.0f, 6.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), LightType::SPOTLIGHT);

		//u32 main = GL::AssetManagement::LoadFromObjFile("\\level1\\level1.obj");
		u32 main = GL::AssetManagement::LoadFromObjFile("\\factory\\factory.obj");

		GL::Node node(std::make_unique<GL::GeometryNodeBehavior>(main));

		scene.AddChild(std::move(node));

		LOGINFO("Application Created");
	}

	Application::~Application()
	{
		LOGINFO("Application Destroyed");
	}

	void Application::Run()
	{
		renderer.Init(scene);

		LOGINFO("Application Run");

		GameSetUp();

		window.RunMainLoop([&](f32 dt) {

			glm::vec2 win_size = window.WindowSize();

			GameUpdate(dt);
			UpdateCamera(dt);
			scene.Update(dt);

			renderer.Render((u32)win_size.x, (u32)win_size.y, scene);
		});
	}

	void Application::UpdateCamera(f32 dt)
	{
		float move_speed = 10.0f;

		if (window.KeyIsPress(KEY_W))
		{
			scene.camera.pos = scene.camera.pos + scene.camera.dir * move_speed * dt;
		}
		if (window.KeyIsPress(KEY_S))
		{
			scene.camera.pos = scene.camera.pos - scene.camera.dir * move_speed * dt;
		}

		if (window.KeyIsPress(KEY_D))
		{
			scene.camera.pos += glm::normalize(glm::cross(scene.camera.dir, glm::vec3(0.0f, 1.0f, 0.0f))) * move_speed * dt;
		}
		if (window.KeyIsPress(KEY_A))
		{
			scene.camera.pos -= glm::normalize(glm::cross(scene.camera.dir, glm::vec3(0.0f, 1.0f, 0.0f))) * move_speed * dt;
		}

		glm::vec2 mousePos = window.MousePos();

		static bool firstMouse = true;
		static double lastX;
		static double lastY;

		if (firstMouse)
		{
			lastX = mousePos.x;
			lastY = mousePos.y;
			firstMouse = false;
		}

		double xoffset = mousePos.x - lastX;
		double yoffset = lastY - mousePos.y;
		lastX = mousePos.x;
		lastY = mousePos.y;


		if (window.MouseButtonIsPress(MOUSE_BUTTON_RIGHT))
		{
			double sensitivity = 0.016;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			scene.camera.yaw += (float)xoffset;
			scene.camera.pitch += (float)yoffset;

			if (scene.camera.pitch > PI / 2.0f)
				scene.camera.pitch = PI / 2.0f;
			if (scene.camera.pitch < -PI / 2.0f)
				scene.camera.pitch = -PI / 2.0f;

			glm::vec3 direction;
			direction.x = glm::cos(scene.camera.yaw) * glm::cos(scene.camera.pitch);
			direction.y = glm::sin(scene.camera.pitch);
			direction.z = glm::sin(scene.camera.yaw) * glm::cos(scene.camera.pitch);
			scene.camera.dir = glm::normalize(direction);
		}

	}
}

