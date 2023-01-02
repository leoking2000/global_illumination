#include "Graphics/OpenGL.h"
#include "Renderer.h"

#include "Global/Logger.h"

#include "AssetManagement/AssetManagement.h"
#include "imgui/imgui.h"

#include "DrawStratagies/ShadowMapDrawStrategy.h"
#include "DrawStratagies/GeometryDrawStratagy.h"

namespace GL
{
	Renderer::Renderer(const RendererParameters& params)
		:
		m_parameters(params),
		m_shading_buffer(params.window_width, params.window_height, 1, TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST),
		m_global_illumination(params.gi_params)
	{

	}

	Renderer::~Renderer()
	{
		AssetManagement::Clear();
	}

	void Renderer::Init(Scene& scene)
	{	
		// create Draw strategies
		m_geometry_stratagy = std::unique_ptr<DrawStrategy>(new GeometryDrawStratagy(m_parameters.window_width, m_parameters.window_height));

		// shaders
		m_shading_shader = AssetManagement::CreateShader("Lighting/shading_pass");
		m_post_process_shader = AssetManagement::CreateShader("PostProcess/post_process");

		m_screen_filled_quad = AssetManagement::CreateMesh(DefaultShape::SCRERN_FILLED_QUARD);

		m_global_illumination.Init(scene);

		LOGINFO("Renderer Init");
	}

	void Renderer::Render(u32 width, u32 height, Scene& scene)
	{
		UpdateWindowSize(width, height);

		glm::mat4 proj = glm::perspective(m_parameters.fov_angle,
			(f32)m_parameters.window_width / (f32)m_parameters.window_height,
			m_parameters.min_z, m_parameters.max_z);

		glm::mat4 view = scene.camera.GetCameraView();

		m_geometry_stratagy->ClearFrameBuffer();
		scene.Draw(*m_geometry_stratagy, proj, view);

		m_global_illumination.PreDraw(scene);
		PreviewPass_voxels(m_geometry_stratagy->GetFrameBuffer(), proj * view);
	
		m_global_illumination.Draw(scene, m_shading_buffer, m_geometry_stratagy->GetFrameBuffer(),
			proj, view, m_parameters.background_color);


		//ShadingPass(scene.camera, scene);

		PostProcess();
	}

	void Renderer::UpdateWindowSize(u32 width, u32 height)
	{
		if (width != m_parameters.window_width || height != m_parameters.window_height)
		{
			m_parameters.window_width = width;
			m_parameters.window_height = height;

			m_geometry_stratagy->GetFrameBuffer().Resize(width, height);
			m_shading_buffer.Resize(width, height);
		}

	}

	void Renderer::ShadingPass(const Camera& camera, Scene& scene)
	{
		m_shading_buffer.Bind();

		glCall(glViewport(0, 0, m_shading_buffer.Width(), m_shading_buffer.Height()));
		glCall(glClearColor(m_parameters.background_color.r, m_parameters.background_color.g, m_parameters.background_color.b, 1));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		glCall(glDisable(GL_DEPTH_TEST));

		m_geometry_stratagy->GetFrameBuffer().BindColorTexture(0, 0);
		m_geometry_stratagy->GetFrameBuffer().BindColorTexture(1, 1);
		m_geometry_stratagy->GetFrameBuffer().BindColorTexture(2, 2);
		m_geometry_stratagy->GetFrameBuffer().BindColorTexture(3, 3);
		m_geometry_stratagy->GetFrameBuffer().BindDepthTexture(5);

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

		m_global_illumination.GetRSMBuffer().BindDepthTexture(6);
		shading_shader.SetUniform("u_shadowMap", 6);

		shading_shader.Bind();

		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();

		shading_shader.UnBind();
		m_shading_buffer.UnBind();
	}

	void Renderer::PostProcess()
	{
		glCall(glViewport(0, 0, m_parameters.window_width, m_parameters.window_height));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glCall(glDisable(GL_DEPTH_TEST));

		ShaderProgram& post_process_shader = *AssetManagement::GetShader(m_post_process_shader);

		switch (m_active_preview)
		{
		case FINAL_OUTPUT:
			post_process_shader.SetUniform("u_depth", 0);
			m_shading_buffer.BindColorTexture(0, 10);
			break;


		case SHADOW_MAP_DEPTH:
			post_process_shader.SetUniform("u_depth", 1);
			m_global_illumination.GetRSMBuffer().BindDepthTexture(10);
			break;
		case SHADOW_MAP_FLUX:
			post_process_shader.SetUniform("u_depth", 0);
			m_global_illumination.GetRSMBuffer().BindColorTexture(0, 10);
			break;
		case SHADOW_MAP_POSITION:
			post_process_shader.SetUniform("u_depth", 0);
			m_global_illumination.GetRSMBuffer().BindColorTexture(1, 10);
			break;
		case SHADOW_MAP_NORMAL:
			post_process_shader.SetUniform("u_depth", 0);
			m_global_illumination.GetRSMBuffer().BindColorTexture(2, 10);
			break;


		case GEOMETRY_BUFFER_DEPTH:
			post_process_shader.SetUniform("u_depth", 1);
			m_geometry_stratagy->GetFrameBuffer().BindDepthTexture(10);
			break;
		case GEOMETRY_BUFFER_ALBEDO:
			post_process_shader.SetUniform("u_depth", 0);
			m_geometry_stratagy->GetFrameBuffer().BindColorTexture(0, 10);
			break;
		case GEOMETRY_BUFFER_POSITION:
			post_process_shader.SetUniform("u_depth", 0);
			m_geometry_stratagy->GetFrameBuffer().BindColorTexture(1, 10);
			break;
		case GEOMETRY_BUFFER_NORMAL:
			post_process_shader.SetUniform("u_depth", 0);
			m_geometry_stratagy->GetFrameBuffer().BindColorTexture(2, 10);
			break;
		case GEOMETRY_BUFFER_MASK:
			post_process_shader.SetUniform("u_depth", 0);
			m_geometry_stratagy->GetFrameBuffer().BindColorTexture(3, 10);
			break;


		default:
			post_process_shader.SetUniform("u_depth", 0);
			m_shading_buffer.BindColorTexture(0, 10);
			break;
		}

		post_process_shader.SetUniform("uniform_texture", 10);
		post_process_shader.Bind();

		AssetManagement::GetMesh(m_screen_filled_quad)->Draw();

		post_process_shader.UnBind();
	}

	void Renderer::PreviewPass_voxels(const FrameBuffer& geometryBuffer, const glm::mat4& proj_view)
	{
		if (m_show_voxels)
		{
			const FrameBuffer& voxels = m_global_illumination.GetVoxelizer().GetVoxels(m_musked);

			m_preview.DrawVoxels(geometryBuffer, proj_view,
				m_global_illumination.GetVoxelizer().GetData(), voxels, m_musked, m_show_all);
		}
	}

	void Renderer::DebugImGui(Scene& scene, f32 dt)
	{
		ImGui::Begin("Renderer");

		ImGui::Text("Camera Position  | X: %f | Y: %f | Z: %f", scene.camera.pos.x, scene.camera.pos.y, scene.camera.pos.z);
		ImGui::Text("Camera Direction | X: %f | Y: %f | Z: %f", scene.camera.dir.x, scene.camera.dir.y, scene.camera.dir.z);

		if (ImGui::CollapsingHeader("Renderer Parameters"))
		{
			ImGui::Text("Window Width: %i", m_parameters.window_width);
			ImGui::Text("Window Height: %i", m_parameters.window_height);
			//ImGui::ColorPicker3("Background Color", glm::value_ptr(m_parameters.background_color));
			ImGui::DragFloat("FOV angle (in radians)", &m_parameters.fov_angle, 0.01f, 0.0f, 2 * PI);
			ImGui::InputFloat("Min Z", &m_parameters.min_z);
			ImGui::InputFloat("Max Z", &m_parameters.max_z);
		}

		scene.light.ImGui();

		if (ImGui::CollapsingHeader("Preview"))
		{
			ImGui::ListBox("Previews", (int*)&m_active_preview, m_name_previews, NUMBER_OF_PREVIEWS, 5);
			ImGui::Checkbox("Musked", &m_musked);
			ImGui::Checkbox("Show Voxels", &m_show_voxels);
			ImGui::Checkbox("Show All Voxels", &m_show_all);
		}

		ImGui::End();

	}

}


