#include "Application.h"
#include "AssetManagement/AssetManagement.h"

namespace GL
{

	Application::Application(const ApplicationParameters& params)
		:
		m_window(params.windows_params),
		m_params(params)
	{
		AssetManagement::SetAssetDir(params.asset_dir);

		m_scene.camera.pos = params.cam_pos;
		m_scene.camera.dir = params.cam_dir;

		LOGINFO("Application Created");
	}

	Application::~Application()
	{
		LOGINFO("Application Destroyed");
	}

	void Application::Run()
	{
		SetUp();
		m_pRenderer = std::make_unique<Renderer>(m_params.windows_params.width, m_params.windows_params.height,
			m_params.renderer_params);

		m_pRenderer->Init(m_scene);

		LOGINFO("Application Run");

		m_window.RunMainLoop([&](f32 dt) {

			glm::vec2 win_size = m_window.WindowSize();

			Update(dt);
			UpdateCamera(dt);
			m_scene.Update(dt);

			m_pRenderer->Render((u32)win_size.x, (u32)win_size.y, m_scene);
			m_pRenderer->DebugImGui(m_scene, dt);
		});
	}

	void Application::UpdateCamera(f32 dt)
	{
		float move_speed = 10.0f;

		if (m_window.KeyIsPress(KEY_W))
		{
			m_scene.camera.pos = m_scene.camera.pos + m_scene.camera.dir * move_speed * dt;
		}
		if (m_window.KeyIsPress(KEY_S))
		{
			m_scene.camera.pos = m_scene.camera.pos - m_scene.camera.dir * move_speed * dt;
		}

		if (m_window.KeyIsPress(KEY_D))
		{
			m_scene.camera.pos += glm::normalize(glm::cross(m_scene.camera.dir, glm::vec3(0.0f, 1.0f, 0.0f))) * move_speed * dt;
		}
		if (m_window.KeyIsPress(KEY_A))
		{
			m_scene.camera.pos -= glm::normalize(glm::cross(m_scene.camera.dir, glm::vec3(0.0f, 1.0f, 0.0f))) * move_speed * dt;
		}

		glm::vec2 mousePos = m_window.MousePos();

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


		if (m_window.MouseButtonIsPress(MOUSE_BUTTON_RIGHT))
		{
			double sensitivity = 0.016;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			m_scene.camera.yaw += (float)xoffset;
			m_scene.camera.pitch += (float)yoffset;

			if (m_scene.camera.pitch > PI / 2.0f)
				m_scene.camera.pitch = PI / 2.0f;
			if (m_scene.camera.pitch < -PI / 2.0f)
				m_scene.camera.pitch = -PI / 2.0f;

			glm::vec3 direction;
			direction.x = glm::cos(m_scene.camera.yaw) * glm::cos(m_scene.camera.pitch);
			direction.y = glm::sin(m_scene.camera.pitch);
			direction.z = glm::sin(m_scene.camera.yaw) * glm::cos(m_scene.camera.pitch);
			m_scene.camera.dir = glm::normalize(direction);
		}

	}
}

