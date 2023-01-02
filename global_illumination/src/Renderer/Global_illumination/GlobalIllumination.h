#pragma once
#include "Renderer/Voxelization/Voxelizer.h"

namespace GL
{
	struct GlobalIlluminationParameters
	{
		VoxelizerParameters voxelizer_params;
		const u32 rsm_resoulution = u32(1024);
		u32 bounces = 3;
	};

	class GlobalIllumination
	{
	public:
		GlobalIllumination(const GlobalIlluminationParameters& params);

		GlobalIllumination(const GlobalIllumination&) = delete;
		GlobalIllumination& operator=(const GlobalIllumination&) = delete;
	public:
		void Init(Scene& scene);
		void PreDraw(Scene& scene);
		void Draw(Scene& scene, const FrameBuffer& shading_buffer, const FrameBuffer& geometryBuffer,
			const glm::mat4& proj, const glm::mat4& view, const glm::vec3& background_color);

		const Voxelizer& GetVoxelizer() const;
		const FrameBuffer& GetRSMBuffer() const;
	private:
		void CachingStep(Scene& scene);
	private:
		GlobalIlluminationParameters m_params;
		std::unique_ptr<DrawStrategy> m_rsm_stratagy;
		Voxelizer m_voxelizer;

		// Caching
		FrameBuffer m_cachingBuffer;
		u32 m_caching_shader;

		// bounces
		u32 m_bounces;

		// Reconstruction
		u32 m_reconstruction_shader;

	};
}