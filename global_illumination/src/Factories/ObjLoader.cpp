#include "ObjLoader.h"
#include "Global/Logger.h"
#include "FileUtilities.h"
#include <glm/gtx/intersect.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GL
{
	std::vector<CPUMesh> GL::ObjLoader::Load(const std::string& filename)
    {
		LOGINFO("Loading OBJ file: %s", filename.c_str());

		Assimp::Importer imp;

		const aiScene* model = imp.ReadFile(filename, 
			aiProcess_Triangulate | aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

		if (model == nullptr)
		{
			LOGERROR("Failed Loading OBJ file: %s", filename.c_str());
			return std::vector<CPUMesh>();
		}

		auto matirials = model->mMaterials;

		std::vector<CPUMesh> mesh_vector;

		for (u32 i = 0; i < model->mNumMeshes; i++)
		{
			const aiMesh* mesh = model->mMeshes[i];

			std::vector<Vertex> in_vertices;
			std::vector<u32> in_indices;

			for (u32 v = 0; v < mesh->mNumVertices; v++)
			{
				Vertex vert;

				vert.pos = glm::vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);

				if (mesh->mTextureCoords[0] != nullptr)
				{
					vert.texCord = glm::vec2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);

					vert.normal = glm::vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
					vert.tangent = glm::vec3(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
					vert.bitangent = glm::vec3(mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);
				}
				else
				{
					vert.texCord = glm::vec2(0.0f);
					vert.normal = glm::vec3(0.0f);
					vert.tangent = glm::vec3(0.0f);
					vert.bitangent = glm::vec3(0.0f);
				}

				in_vertices.push_back(vert);
			}

			for (uint32_t f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace& face = mesh->mFaces[f];
				assert(face.mNumIndices == 3);

				in_indices.push_back(face.mIndices[0]);
				in_indices.push_back(face.mIndices[1]);
				in_indices.push_back(face.mIndices[2]);
			}

			CPUMesh in_mesh;

			uint32_t matirialIndex = mesh->mMaterialIndex;

			if (matirialIndex >= 0)
			{
				auto& material = *matirials[matirialIndex];
				aiString texFileName;

				if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
				{
					in_mesh.material.AlbedoMap = DirNameOf(filename) + "\\" + texFileName.C_Str();
				}
				else
				{
					aiColor3D c(0.0f, 0.0f, 0.0f);
					material.Get(AI_MATKEY_COLOR_DIFFUSE, c);
					in_mesh.material.Albedo = glm::vec3(c.r, c.g, c.b);
				}

				if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
				{
					in_mesh.material.NormalMap = DirNameOf(filename) + "\\" + texFileName.C_Str();
				}

				// TODO: get Metallic and Roughness
			}

			in_mesh.vertices = in_vertices;
			in_mesh.indices = in_indices;

			mesh_vector.push_back(std::move(in_mesh));
		}

		LOGINFO("Has been Loaded!!! (sub_meshs: %i)", mesh_vector.size());

		return mesh_vector;
    }

    bool GL::CPUMesh::IntersectRayLocal(const glm::vec3 origin, const glm::vec3 dir, f32& distance)
    {
		// store the current minimum distance found.
		distance = std::numeric_limits<f32>::max();

		bool found_isect = false;

		for (u32 i = 0; i < indices.size(); i += 3)
		{
			// read the triangle
			u32 indice_a = indices[i];
			u32 indice_b = indices[i + 1];
			u32 indice_c = indices[i + 2];
			glm::vec3 a = vertices[indice_a].pos;
			glm::vec3 b = vertices[indice_b].pos;
			glm::vec3 c = vertices[indice_c].pos;

			glm::vec2 barycoord;
			float t;

			// check for intersection for that triangle
			if (glm::intersectRayTriangle(origin, dir, a, b, c, barycoord, t))
			{
				// find the position of the intersection
				const glm::vec3 temp_isect = origin + t * dir;

				// find the distance between the origin on the intersection
				f32 temp_distance = glm::distance(origin, temp_isect);

				if (temp_distance < distance)
				{
					distance = temp_distance;
					found_isect = true;
				}
			}
		}

		return found_isect;
    }
}


