#pragma once
#include "Renderer/Voxelization/Voxelizer.h"

#define USETIMER
#ifdef USETIMER
#include "Graphics/TimerGPU.h"
#endif // USETIMER

namespace GL
{
	class Texture;

	struct GlobalIlluminationParameters
	{
		VoxelizerParameters voxelizer_params;
		const u32 rsm_resoulution = u32(1024);
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
		void BlendStep();
		void ReconstructionStep(Scene& scene, const FrameBuffer& shading_buffer, const FrameBuffer& geometryBuffer,
			const glm::mat4& proj, const glm::mat4& view);
	private:
		// performance
#ifdef USETIMER
		TimerGPU m_voxelize_timer;
		TimerGPU m_caching_timer;
		TimerGPU m_bounce_timer;
		TimerGPU m_blend_timer;
		TimerGPU m_reconstruction_timer;
#endif // USETIMER
	private:
		GlobalIlluminationParameters m_params;
		std::unique_ptr<DrawStrategy> m_rsm_stratagy;
		Voxelizer m_voxelizer;

		// Caching
		u32 m_caching_shader;
		i32 m_num_RSM_samples = 100;
		i32 m_num_occlusion_sample = 20;
		f32 m_spread = 1.0f;
		bool m_occlusion_enable = true;

		// Bounces
		i32 m_bounces;
		u32 m_bounces_shader;
		f32 m_average_albedo = 0.8f;
		i32 m_num_bounces_samples = 80;

		const u32 m_random_size = 10000;
		Texture m_random_texture;

		// Blend
		u32 m_blend_shader;
		f32 m_blend_factor = 0.9f;

		// Reconstruction
		f32 m_factor = 1.0f;
		u32 m_reconstruction_shader;

		// light probe volume buffers
		FrameBuffer m_cachingBuffer_1;
		FrameBuffer m_cachingBuffer_2;
		FrameBuffer m_cachingBuffer_3;

		FrameBuffer* m_current_cachingBuffer;
		FrameBuffer* m_bounce_target;
		FrameBuffer* m_previous_cachingBuffer;
	};
}