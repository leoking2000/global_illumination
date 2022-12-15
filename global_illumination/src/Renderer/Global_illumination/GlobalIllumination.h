#pragma once
#include "Renderer/Voxelization/Voxelizer.h"

namespace GL
{
	struct GlobalIlluminationParameters
	{
		VoxelizerParameters voxelizer_params;
		const u32 rsm_resoulution = 1024;
		u32 bounces = 1;
	};

	class GlobalIllumination
	{
	public:
		GlobalIllumination(const GlobalIlluminationParameters& params);
		void Init(Scene& scene);
		void Draw(Scene& scene);

		const Voxelizer& GetVoxelizer() const;
		const FrameBuffer& GetRSMBuffer() const;
	private:
		void CachingStage(Scene& scene);
	private:
		GlobalIlluminationParameters m_params;
		Voxelizer m_voxelizer;
		std::unique_ptr<DrawStrategy> m_rsm_stratagy;

		FrameBuffer m_caching_buffer;
	};
}