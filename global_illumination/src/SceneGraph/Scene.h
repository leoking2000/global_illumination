#pragma once
#include "Camera.h"
#include "Light.h"
#include "Node.h"
#include "NodeBehavior.h"

namespace GL
{
	class Scene
	{
	public:
		Scene(); // Default Scene
		//Scene(const std::string scene_file);
	public:
		void Update(f32 dt);
		void Draw(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view);
		void AddChild(Node&& node);
	public:
		Camera camera;
		Light light;
	private:
		Node root;
	};
}