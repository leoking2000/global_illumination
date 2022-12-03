#include "Graphics/OpenGL.h"
#include "Renderer.h"

#include "Global/Logger.h"

#include "AssetManagement/AssetManagement.h"
#include "imgui/imgui.h"

#include "Global_illumination/RSMGITechique.h"

#include "ShadowMapDrawStrategy.h"
#include "GeometryDrawStratagy.h"

#define SHADOW_STRATEGY (*m_strategies[0])
#define GEOMETRY_STRATEGY (*m_strategies[1])

namespace GL
{
	Renderer::Renderer(u32 width, u32 height, const Window& window)
		:
		m_shading_buffer(width, height, 1),
		window(window),
		m_voxelizer({})
	{
		parameters.window_width = width;
		parameters.window_height = height;
	}

	Renderer::~Renderer()
	{
		AssetManagement::Clear();
	}

	void Renderer::Init(Scene& scene)
	{	
		// create Draw strategies
		m_strategies.emplace_back(new ShadowMapDrawStrategy(2048));
		m_strategies.emplace_back(new GeometryDrawStratagy(parameters.window_width, parameters.window_height));

		// create technique
		m_gi_technique = std::make_unique<RSMGLTechique>(2048, scene);

		// shaders
		m_shading_shader = AssetManagement::CreateShader("Lighting/shading_pass");
		m_post_process_shader = AssetManagement::CreateShader("PostProcess/post_process");

		m_screen_filled_quad = AssetManagement::CreateMesh(DefaultShape::SCRERN_FILLED_QUARD);

		// init staff
		m_gi_technique->Init();
		m_voxelizer.Init();

		LOGINFO("Renderer Init");
	}

	void Renderer::Render(u32 width, u32 height, Scene& scene)
	{
		if (window.KeyIsPressAsButton(KEY_F12))
		{
			AssetManagement::ReloadShaders();
		}

		UpdateWindowSize(width, height);

		m_voxelizer.Voxelize(scene);

		m_gi_technique->Draw(scene, scene.light.LightProj(), scene.light.LightView());

		SHADOW_STRATEGY.ClearFrameBuffer();
		scene.Draw(SHADOW_STRATEGY, scene.light.LightProj(), scene.light.LightView());

		glm::mat4 proj = glm::perspective(parameters.fov_angle,
			(f32)parameters.window_width / (f32)parameters.window_height,
			parameters.min_z, parameters.max_z);
		GEOMETRY_STRATEGY.ClearFrameBuffer();
		scene.Draw(GEOMETRY_STRATEGY, proj, scene.camera.GetCameraView());

		static bool renderPreviewSpheres = false;
		if (window.KeyIsPressAsButton(KEY_V))
		{
			renderPreviewSpheres = !renderPreviewSpheres;
		}
		
		if (renderPreviewSpheres)
		{
			m_voxelizer.DrawPreviewSpheres(GEOMETRY_STRATEGY.GetFrameBuffer(), proj * scene.camera.GetCameraView());
		}
		
		ShadingPass(scene.camera, scene);
		PostProcess();
		DebugImGui(scene);
	}

	void Renderer::UpdateWindowSize(u32 width, u32 height)
	{
		if (width != parameters.window_width || height != parameters.window_height)
		{
			parameters.window_width = width;
			parameters.window_height = height;

			GEOMETRY_STRATEGY.GetFrameBuffer().Resize(width, height);
			m_shading_buffer.Resize(width, height);
		}

	}

	void Renderer::ShadingPass(const Camera& camera, Scene& scene)
	{
		m_shading_buffer.Bind();

		glCall(glViewport(0, 0, m_shading_buffer.Width(), m_shading_buffer.Height()));
		glCall(glClearColor(parameters.background_color.r, parameters.background_color.g, parameters.background_color.b, 1));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		glCall(glDisable(GL_DEPTH_TEST));

		GEOMETRY_STRATEGY.GetFrameBuffer().BindColorTexture(0, 0);
		GEOMETRY_STRATEGY.GetFrameBuffer().BindColorTexture(1, 1);
		GEOMETRY_STRATEGY.GetFrameBuffer().BindColorTexture(2, 2);
		GEOMETRY_STRATEGY.GetFrameBuffer().BindColorTexture(3, 3);
		GEOMETRY_STRATEGY.GetFrameBuffer().BindDepthTexture(5);

		ShaderProgram& shading_shader = *AssetManagement::GetShader(m_shading_shader);

		shading_shader.SetUniform("u_tex_albedo", 0);
		shading_shader.SetUniform("u_tex_pos", 1);
		shading_shader.SetUniform("u_tex_normal", 2);
		shading_shader.SetUniform("u_tex_mask", 3);
		shading_shader.SetUniform("u_tex_depth", 5);

		shading_shader.SetUniform("u_camera_pos", camera.pos);
		shading_shader.SetUniform("u_camera_dir", camera.dir);
		
		scene.light.SetUniforms(shading_shader);

		shading_shader.SetUniform("u_light_projection_view", scene.light.LightProj() * scene.light.LightView());

		SHADOW_STRATEGY.GetFrameBuffer().BindDepthTexture(6);
		shading_shader.SetUniform("u_shadowMap", 6);

		shading_shader.Bind();

		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();

		shading_shader.UnBind();
		m_shading_buffer.UnBind();
	}

	void Renderer::PostProcess()
	{
		glCall(glViewport(0, 0, parameters.window_width, parameters.window_height));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDisable(GL_DEPTH_TEST));

		static bool b = true;
		static int i = 0;

		if (window.KeyIsPressAsButton(KEY_M))
			b = !b;

		if (window.KeyIsPressAsButton(KEY_N))
			i = (i + 1) % m_gi_technique->GetFrameBuffer().NumberOfColorAttachments();

		ShaderProgram& post_process_shader = *AssetManagement::GetShader(m_post_process_shader);

		if (b)
		{
			post_process_shader.SetUniform("u_depth", 0);
			m_shading_buffer.BindColorTexture(0, 10);
		}
		else 
		{
			post_process_shader.SetUniform("u_depth", 0);
			//SHADOW_STRATEGY.GetFrameBuffer().BindDepthTexture(10);
			m_gi_technique->GetFrameBuffer().BindColorTexture(i, 10);
		}

		post_process_shader.SetUniform("uniform_texture", 10);
		post_process_shader.Bind();

		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();

		post_process_shader.UnBind();
	}

	void Renderer::DebugImGui(Scene& scene)
	{
		scene.light.ImGui();
	}

}


