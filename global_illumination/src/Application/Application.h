#pragma once
#include "Global/Logger.h"
#include "Window/Window.h"
#include "Renderer/Renderer.h"
#include "SceneGraph/Scene.h"
#include <memory>

namespace GL
{
	struct ApplicationParameters
	{
		WindowParameters windows_params;
		RendererParameters renderer_params;

		// scene
		std::string asset_dir = "";

		// camera
		glm::vec3 cam_pos = glm::vec3(0.0f);
		glm::vec3 cam_dir = glm::vec3(0.0f, 0.0f, -1.0f);
	};

	class Application
	{
	public:
		Application(const ApplicationParameters& params);

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		virtual ~Application();
	public:
		void Run();
	protected:
		virtual void UpdateCamera(f32 dt);
		virtual void GameSetUp() = 0;
		virtual void GameUpdate(f32 dt) = 0;
	protected:
		ApplicationParameters params;
		Window window;
		Scene scene;
		std::unique_ptr<Renderer> pRenderer;
	};
}