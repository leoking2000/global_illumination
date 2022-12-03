#pragma once
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Factories/ObjLoader.h"
#include "Model.h"
#include <string>
#include <vector>


namespace GL
{
	enum class DefaultShape
	{
		CUBE,
		SPHERE,
		QUARD,
		SCRERN_FILLED_QUARD
	};

	class AssetManagement
	{
	public:
		static std::string GetAssetDir();
		static void SetAssetDir(const std::string& path);
	public:
		static void CreateTexture2D(const std::string& file_path);
		static Texture* GetTexture(const std::string& file_path);

		static u32 CreateShader(const std::string& name);
		static ShaderProgram* GetShader(u32 id);
		static void ReloadShaders();

		static u32 CreateMesh(DefaultShape shape);
		static u32 CreateMesh(const CPUMesh& cpu_mesh);
		static Mesh* GetMesh(u32 id);

		static u32 CreateModel(u32 mesh);
		static u32 CreateModel(u32 mesh, Material mat);
		static u32 LoadFromObjFile(const std::string& file_path); // returns a Model id
		static Model* GetModel(u32 id);

		static void Clear();
	private:
		template<class T>
		static T* GetFrom(std::vector<T>& assets, u32 index)
		{
			if (assets.size() <= index)
			{
				return nullptr;
			}

			return &(assets[index]);
		}
	private:
		static AssetManagement& Get();
	private:
		AssetManagement() {}
		static AssetManagement assetManager;
	private:
		std::string asset_dir;

		std::vector<ShaderProgram> m_shaders;
		std::vector<std::string> m_shader_names;

		std::unordered_map<std::string, Texture> m_textures;

		std::vector<Mesh> m_meshs;

		std::vector<Model> m_models;
	};
}

