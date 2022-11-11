#include "Graphics/OpenGL.h"
#include "Renderer.h"

#include "AssetManagement/AssetManagement.h"
#include "Factories/ShaderFactory.h"
#include "imgui/imgui.h"

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

	void Renderer::Init()
	{	
		// create Draw strategies
		m_strategies.emplace_back(new ShadowMapDrawStrategy(2048));
		m_strategies.emplace_back(new GeometryDrawStratagy(parameters.window_width, parameters.window_height));

		// shaders
		m_shading_shader = AssetManagement::CreateShader("Lighting/shading_pass");
		m_post_process_shader = AssetManagement::CreateShader("PostProcess/post_process");

		m_screen_filled_quad = AssetManagement::CreateMesh(DefaultShape::SCRERN_FILLED_QUARD);

		m_voxelizer.Init();
	}

	void Renderer::Render(u32 width, u32 height, Scene& scene)
	{
		UpdateWindowSize(width, height);

		m_voxelizer.Voxelize(scene);


		SHADOW_STRATEGY.ClearFrameBuffer();
		glm::mat4 light_view = glm::lookAt(light.m_light_pos, light.m_light_pos + light.m_light_dir, glm::vec3(0.0f, 1.0f, 0.0f));
		scene.Draw(SHADOW_STRATEGY, light.m_light_proj, light_view);


		glm::mat4 proj = glm::perspective(parameters.fov_angle,
			(f32)parameters.window_width / (f32)parameters.window_height,
			parameters.min_z, parameters.max_z);
		GEOMETRY_STRATEGY.ClearFrameBuffer();
		scene.Draw(GEOMETRY_STRATEGY, proj, scene.camera.GetCameraView());

		static bool renderPreviewSpheres = true;
		if (window.KeyIsPressAsButton(KEY_V))
		{
			renderPreviewSpheres = !renderPreviewSpheres;
		}
		
		if (renderPreviewSpheres)
		{
			m_voxelizer.DrawPreviewSpheres(GEOMETRY_STRATEGY.GetFrameBuffer(), proj * scene.camera.GetCameraView());
		}
		
		ShadingPass(scene.camera);
		PostProcess();
		DebugImGuiLight();
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

	void Renderer::ShadingPass(const Camera& camera)
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
		
		shading_shader.SetUniform("u_light_dir", light.m_light_dir);

		glm::mat4 light_view = glm::lookAt(light.m_light_pos, light.m_light_pos + light.m_light_dir, glm::vec3(0, 1, 0));
		shading_shader.SetUniform("u_light_projection_view", light.m_light_proj * light_view);

		SHADOW_STRATEGY.GetFrameBuffer().BindDepthTexture(6);
		shading_shader.SetUniform("u_shadowMap", 6);
		shading_shader.SetUniform("u_shadow_bias", light.m_shadow_bias);

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

		if (window.KeyIsPressAsButton(KEY_M))
			b = !b;

		ShaderProgram& post_process_shader = *AssetManagement::GetShader(m_post_process_shader);

		if (b)
		{
			post_process_shader.SetUniform("u_depth", 0);
			m_shading_buffer.BindColorTexture(0, 10);
		}
		else 
		{
			post_process_shader.SetUniform("u_depth", 1);
			SHADOW_STRATEGY.GetFrameBuffer().BindDepthTexture(10);
		}

		post_process_shader.SetUniform("uniform_texture", 10);
		post_process_shader.Bind();

		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();

		post_process_shader.UnBind();
	}

	void Renderer::DebugImGuiLight()
	{
		ImGui::Begin("Light");

		ImGui::DragFloat3("Pos", glm::value_ptr(light.m_light_pos));
		ImGui::DragFloat3("Dir", glm::value_ptr(light.m_light_dir), 0.1f, -1.0f, 1.0f);
		light.m_light_dir = glm::normalize(light.m_light_dir);
		ImGui::InputFloat("bias", &light.m_shadow_bias, 0.0001f, 0.0005f, "%f");

		ImGui::End();

		Model& m = *AssetManagement::GetModel(model);

		ImGui::Begin("Model Material");

		static int i = 0;

		ImGui::SliderInt("Mesh: ", &i, 0, (int)m.m_materials.size() - 1);
		ImGui::ColorEdit3("Color ", glm::value_ptr(m.m_materials[i].Albedo));
		ImGui::SliderFloat("Roughness", &m.m_materials[i].Roughness, 0.0f, 1.0f);
		ImGui::SliderFloat("Metallic", &m.m_materials[i].Metallic, 0.0f, 1.0f);
		

		ImGui::End();
	}

}


