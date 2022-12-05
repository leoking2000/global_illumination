#pragma once
#include "SceneGraph/Scene.h"

namespace GL
{
	struct RendererParameters
	{
		u32 window_width;
		u32 window_height;

		glm::vec3 background_color = glm::vec3(0.0f);

		// projection settings
		f32 fov_angle = glm::radians(45.0f);
		f32 min_z = 0.1f;
		f32 max_z = 100.0f;
	};

	class Renderer
	{
	public:
		Renderer(const RendererParameters& params);

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		~Renderer();
	public:
		void Init(Scene& scene);
		void Render(u32 width, u32 height, Scene& scene);
		void DebugImGui(Scene& scene, f32 dt);
	private:
		void UpdateWindowSize(u32 width, u32 height);

		void ShadingPass(const Camera& camera, Scene& scene);
		void PostProcess();
	private:
		RendererParameters m_parameters;
		std::unique_ptr<DrawStrategy> m_shadow_stratagy;
		std::unique_ptr<DrawStrategy> m_geometry_stratagy;

		u32 m_shading_shader;
		FrameBuffer m_shading_buffer;

		u32 m_post_process_shader;
		u32 m_screen_filled_quad;
	};


}