#pragma once
#include "Renderer/Voxelization/Voxelizer.h"

namespace GL
{
	struct GlobalIlluminationParameters
	{
		VoxelizerParameters voxelizer_params;
		const u32 rsm_resoulution = u32(512);
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
		void BounceStep();
	private:
		GlobalIlluminationParameters m_params;
		std::unique_ptr<DrawStrategy> m_rsm_stratagy;
		Voxelizer m_voxelizer;

		// Caching
		FrameBuffer m_cachingBuffer;
		u32 m_caching_shader;

		// bounces
		FrameBuffer m_cachingBuffer_copy;
		FrameBuffer* m_active_cachingBuffer;
		u32 m_bounces;
		u32 m_bounces_shader;

		// Reconstruction
		u32 m_reconstruction_shader;

	};
}