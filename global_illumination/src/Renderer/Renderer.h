#pragma once
#include "Global_illumination/GITechnique.h"
#include "Voxelization/Voxelizer.h"
#include "Graphics/VertexArray.h"

#include "Window/Window.h"

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
		//Light light;
		//u32 model;

		const Window& window;
	public:
		Renderer(u32 width, u32 height, const Window& window);

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		~Renderer();
	public:
		void Init(Scene& scene);
		void Render(u32 width, u32 height, Scene& scene);
	public:
		RendererParameters parameters;
	private:
		void UpdateWindowSize(u32 width, u32 height);
	private:
		void ShadingPass(const Camera& camera, Scene& scene);
		void PostProcess();
		void DebugImGui(Scene& scene);
	private:
		std::vector<std::unique_ptr<DrawStrategy>> m_strategies;
		Voxelizer m_voxelizer;

		u32 m_shading_shader;
		FrameBuffer m_shading_buffer;

		u32 m_post_process_shader;
		u32 m_screen_filled_quad;

		// GI
		std::unique_ptr<GITechnique> m_gi_technique;
	};


}