#include "Scene.h"
#include "imgui/imgui.h"

namespace GL
{
	Scene::Scene()
		:
		root(std::make_unique<NodeBehavior>())
	{

	}

	void Scene::Update(f32 dt)
	{
		ImGui::Begin("Scene");
		root.Update(dt);
		ImGui::End();
	}

	void Scene::Draw(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view)
	{
		root.Draw(strategy, proj, view);
	}

	void Scene::AddChild(Node&& node)
	{
		root.AddChild(std::move(node));
	}
}


