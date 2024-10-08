#include "Graphics/OpenGL.h"
#include "GeometryDrawStratagy.h"
#include "AssetManagement/AssetManagement.h"

namespace GL
{
	GeometryDrawStratagy::GeometryDrawStratagy(FrameBuffer&& buffer, u32 shader, std::function<void(ShaderProgram&)> setExtraUniforms)
		:
		m_geometry_buffer(std::move(buffer)),
		m_geometry_shader(shader),
		m_setExtraUniforms(setExtraUniforms)
	{
	}

	GL::GeometryDrawStratagy::GeometryDrawStratagy(u32 width, u32 height)
		:
		m_geometry_buffer(width, height, 4, TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST),
		m_setExtraUniforms([](ShaderProgram&) {})
	{
		static bool shader_created = false;
		if (!shader_created) {
			m_geometry_shader = AssetManagement::CreateShader("Geometry/geometry_pass");
			shader_created = true;
		}
	}

	void GL::GeometryDrawStratagy::ClearFrameBuffer()
	{
		m_geometry_buffer.Bind();

		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_geometry_buffer.UnBind();
	}

	void GL::GeometryDrawStratagy::Draw(const Model& model, const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model_matrix) const
	{
		m_geometry_buffer.Bind();

		glCall(glViewport(0, 0, m_geometry_buffer.Width(), m_geometry_buffer.Height()));

		glCall(glEnable(GL_DEPTH_TEST));
		glCall(glDepthFunc(GL_LEQUAL));
		glCall(glClearDepth(1.0f));

		glCall(glEnable(GL_CULL_FACE));
		glCall(glCullFace(GL_BACK));
		glCall(glFrontFace(GL_CCW));

		ShaderProgram& geometry_shader = *AssetManagement::GetShader(m_geometry_shader);

		geometry_shader.SetUniform("u_model_matrix", model_matrix);
		geometry_shader.SetUniform("u_proj_view_model_matrix", proj * view * model_matrix);
		geometry_shader.SetUniform("u_normal_matrix", glm::transpose(glm::inverse(model_matrix)));

		m_setExtraUniforms(geometry_shader);

		for (u32 i = 0; i < model.m_meshs.size(); i++)
		{
			if (model.m_materials[i].AlbedoMap.empty())
			{
				geometry_shader.SetUniform("u_HasAlbedoMap", 0);
				geometry_shader.SetUniform("u_albedo", model.m_materials[i].Albedo);
			}
			else
			{
				AssetManagement::GetTexture(model.m_materials[i].AlbedoMap)->Bind(0);
				geometry_shader.SetUniform("u_albedoMap", 0);
				geometry_shader.SetUniform("u_HasAlbedoMap", 1);
			}

			if (model.m_materials[i].NormalMap.empty())
			{
				geometry_shader.SetUniform("u_HasNormalMap", 0);
			}
			else
			{
				geometry_shader.SetUniform("u_HasNormalMap", 1);
				AssetManagement::GetTexture(model.m_materials[i].NormalMap)->Bind(1);
				geometry_shader.SetUniform("u_normalMap", 1);
			}

			geometry_shader.SetUniform("u_roughness", model.m_materials[i].Roughness);
			geometry_shader.SetUniform("u_metallic", model.m_materials[i].Metallic);

			geometry_shader.Bind();
			AssetManagement::GetMesh(model.m_meshs[i])->Draw();
		}

		geometry_shader.UnBind();

		m_geometry_buffer.UnBind();
	}

	FrameBuffer& GL::GeometryDrawStratagy::GetFrameBuffer()
	{
		return m_geometry_buffer;
	}

	void GeometryDrawStratagy::SetExtraUniformsFunction(std::function<void(ShaderProgram&)> setExtraUniforms)
	{
		m_setExtraUniforms = setExtraUniforms;
	}
}


