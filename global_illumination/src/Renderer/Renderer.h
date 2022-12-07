#pragma once
#include "SceneGraph/Scene.h"
#include "Global_illumination/GlobalIllumination.h"

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

		GlobalIlluminationParameters gi_params;
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
		std::unique_ptr<DrawStrategy> m_geometry_stratagy;

		u32 m_shading_shader;
		FrameBuffer m_shading_buffer;

		u32 m_post_process_shader;
		u32 m_screen_filled_quad;

		GlobalIllumination m_global_illumination;
	private:
		// for previews
		enum Preview : int
		{
			FINAL_OUTPUT              = 0,
			SHADOW_MAP_DEPTH          = 1,
			SHADOW_MAP_FLUX           = 2,
			SHADOW_MAP_POSITION       = 3,
			SHADOW_MAP_NORMAL         = 4,
			GEOMETRY_BUFFER_DEPTH     = 5,
			GEOMETRY_BUFFER_ALBEDO    = 6,
			GEOMETRY_BUFFER_POSITION  = 7,
			GEOMETRY_BUFFER_NORMAL    = 8,
			GEOMETRY_BUFFER_MASK      = 9,
			NUMBER_OF_PREVIEWS        = 10
		};

		const char* m_name_previews[NUMBER_OF_PREVIEWS] = {
			"Final Output",
			"Shadow map Depth",
			"Shadow map Flux",
			"Shadow map Position",
			"Shadow map Normal",
			"Geometry buffer Depth",
			"Geometry buffer Albedo",
			"Geometry buffer Position",
			"Geometry buffer Normal",
			"Geometry buffer Mask"
		};

		Preview m_active_preview = FINAL_OUTPUT;
		bool m_show_voxels = false;

	};


}