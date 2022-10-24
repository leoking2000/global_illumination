#include "Graphics/OpenGL.h"
#include "VoxelizerDrawStratagy.h"
#include "AssetManagement/AssetManagement.h"

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

namespace GL
{
	VoxelizerDrawStratagy::VoxelizerDrawStratagy(const VoxelizerData& data)
		:
		m_data(data),
		m_framebuffer(data.resolution, data.resolution, 3, TextureFormat::RGBA32F, true)
	{
		Init();
	}

	void VoxelizerDrawStratagy::ClearFrameBuffer()
	{
		m_framebuffer.Bind();

		glCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_framebuffer.UnBind();
	}

	void VoxelizerDrawStratagy::Draw(const Model& model, const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model_matrix) const
	{
		m_framebuffer.Bind();

		glCall(glViewportArrayv(0, 3, &m_viewport[0][0]));

		glCall(glDisable(GL_DEPTH_TEST));
		glCall(glDisable(GL_CULL_FACE));
		glCall(glDisable(GL_BLEND));
		glCall(glEnable(GL_COLOR_LOGIC_OP));
		glCall(glLogicOp(GL_OR));

		ShaderProgram& shader = *AssetManagement::GetShader(m_treeWayShader);
		
		shader.SetUniform("u_model_matrix", model_matrix);

		glm::uvec3 u_dimensions(m_data.dimensions);
		shader.SetUniform("u_dimensions", u_dimensions);

		for (u32 i = 0; i < 3; i++)
		{
			shader.SetUniform("u_proj_view[" + std::to_string(i) + "]", m_proj_view_axis[i]);
		}

		shader.Bind();

		for (u32 i = 0; i < model.m_meshs.size(); i++)
		{
			AssetManagement::GetMesh(model.m_meshs[i])->Draw();
		}

		shader.UnBind();

		glCall(glEnable(GL_DEPTH_TEST));
		glCall(glEnable(GL_CULL_FACE));
		glCall(glEnable(GL_BLEND));
		glCall(glDisable(GL_COLOR_LOGIC_OP));

		m_framebuffer.UnBind();
	}

	FrameBuffer& GL::VoxelizerDrawStratagy::GetFrameBuffer()
	{
		return m_framebuffer;
	}

	void VoxelizerDrawStratagy::Init()
	{
		m_treeWayShader = AssetManagement::CreateShader("Voxelization\\ThreeWayBinary");

		glm::vec3 size = m_data.voxelizationArea.GetSize();
		glm::vec3 half_size = size * 0.5f;

		const glm::vec3& center = m_data.voxelizationArea.GetCenter();
		const glm::vec3& bmax = m_data.voxelizationArea.GetMax();
		const glm::vec3& bmin = m_data.voxelizationArea.GetMin();

		glm::vec3 eye;
		glm::vec3 up;

		// X AXIS
		eye = center;
		eye.x = bmin.x;
		up = glm::vec3(0, 1, 0);
		m_view_axis[AXIS_X] = glm::lookAt(eye, center, up);
		m_proj_axis[AXIS_X] = glm::ortho(-half_size.z, half_size.z, -half_size.y, half_size.y, 0.0f, size.x);
		m_viewport[AXIS_X] = glm::vec4(0.0f, 0.0f, m_data.dimensions.z, m_data.dimensions.y);

		// Y AXIS
		eye = center;
		eye.y = bmax.y;
		up = glm::vec3(-1, 0, 0);
		m_view_axis[AXIS_Y] = glm::lookAt(eye, center, up);
		m_proj_axis[AXIS_Y] = glm::ortho(-half_size.z, half_size.z, -half_size.x, half_size.x, 0.0f, size.y);
		m_viewport[AXIS_Y] = glm::vec4(0.0f, 0.0f, m_data.dimensions.z, m_data.dimensions.x);

		// Z AXIS
		eye = center;
		eye.z = bmax.z;
		up = glm::vec3(0, 1, 0);
		m_view_axis[AXIS_Z] = glm::lookAt(eye, center, up);
		m_proj_axis[AXIS_Z] = glm::ortho(-half_size.x, half_size.x, -half_size.y, half_size.y, 0.0f, size.z);
		m_proj_axis[AXIS_Z] = glm::ortho(-half_size.x, half_size.x, -half_size.y, half_size.y, 0.0f, size.z);

		for (int i = 0; i < 3; ++i)
		{
			m_proj_view_axis[i] = m_proj_axis[i] * m_view_axis[i];
		}
	}
}


