#pragma once
#include "Material.h"
#include <vector>

namespace GL
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 texCord;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;

		bool operator==(const Vertex& rhs)
		{
			return pos == rhs.pos &&
				texCord == rhs.texCord &&
				normal == rhs.normal &&
				tangent == rhs.tangent &&
				bitangent == rhs.bitangent;
		}
	};

	class CPUMesh
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<u32> indices;

		Material material;
	public:
		/// <summary>
		/// returns true if a ray is intersecting with at least one triangle in the mesh.
		/// </summary>
		/// <param name="origin">The rays origine</param>
		/// <param name="dir">The rays direction</param>
		/// <param name="distance">Sets the variable to the closest intersection distance</param>
		bool IntersectRayLocal(const glm::vec3 origin, const glm::vec3 dir, f32& distance);
	};


	/*
	* ObjLoader is a static class that loades a model from a file into CPU memory
	* a model is an array of CPUmeshs
	*/
	class ObjLoader
	{
	public:
		static std::vector<CPUMesh> Load(const std::string& filename);
	private:
		ObjLoader();
	};
}