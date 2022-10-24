#pragma once
#include "Nodes/Node.h"
#include "Light.h"
#include "Camera.h"
#include <memory>
#include <vector>

namespace GL
{
	class Scene : public Node
	{
	public:
		Scene(); // Default Scene
		//Scene(const std::string scene_file);

	protected:
		void UpdateSelf(f32 dt) override;
		void DrawSelf(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view) override;
	public:
		Light light;
		Camera camera;
	};
}