#pragma once
#include "Renderer/Voxelization/Voxelizer.h"

namespace GL
{
	struct GlobalIlluminationParameters
	{
		VoxelizerParameters voxelizer_params;
		const u32 rsm_resoulution = u32(512);
		u32 bounces = 1;
	};

	class GlobalIllumination
	{
	public:
		GlobalIllumination(const GlobalIlluminationParameters& params, u32 window_width, u32 window_height);

		GlobalIllumination(const GlobalIllumination&) = delete;
		GlobalIllumination& operator=(const GlobalIllumination&) = delete;
	public:
		void Init(Scene& scene);
		void Draw(Scene& scene,const FrameBuffer& shading_buffer, const FrameBuffer& geometryBuffer,
			const glm::mat4& proj, const glm::mat4& view);

		const Voxelizer& GetVoxelizer() const;
		void DrawRSM(Scene& scene);
		const FrameBuffer& GetRSMBuffer() const;

		void ImGui();
	private:
		void CachingStep(Scene& scene);
		void BounceStep();
		void ReconstructionStep(Scene& scene, const FrameBuffer& shading_buffer, const FrameBuffer& geometryBuffer,
			const glm::mat4& proj, const glm::mat4& view);
	private:
		GlobalIlluminationParameters m_params;
		std::unique_ptr<DrawStrategy> m_rsm_stratagy;
		Voxelizer m_voxelizer;

		// Caching
		FrameBuffer m_cachingBuffer;
		u32 m_caching_shader;
		i32 m_num_RSM_samples = 100;
		i32 m_num_occlusion_sample = 10;
		f32 m_spread = 1.0f;
		bool m_occlusion_enable = true;

		// bounces
		FrameBuffer m_cachingBuffer_copy;
		FrameBuffer* m_active_cachingBuffer;
		i32 m_bounces;
		u32 m_bounces_shader;

		// Reconstruction
		f32 m_factor = 1.0f;
		u32 m_reconstruction_shader;

	};
}