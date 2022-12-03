#include "AssetManagement.h"
#include "Factories/TextureFactory.h"
#include "Factories/ShaderFactory.h"
#include "Factories/MeshFactory.h"


namespace GL
{
	AssetManagement GL::AssetManagement::assetManager;


	std::string AssetManagement::GetAssetDir()
	{
		return Get().asset_dir;
	}

	void AssetManagement::SetAssetDir(const std::string& path)
	{
		Get().asset_dir = path;
	}

	void AssetManagement::CreateTexture2D(const std::string& file_path)
	{
		Get().m_textures.insert({file_path, TextureFactory::Create(file_path) });
	}

	Texture* AssetManagement::GetTexture(const std::string& file_path)
	{
		return &Get().m_textures.find(file_path)->second;
	}

	u32 AssetManagement::CreateShader(const std::string& name)
	{
		ShaderProgram shader = ShaderFactory::Create(name);
		Get().m_shaders.push_back(std::move(shader));
		Get().m_shader_names.push_back(name);

		return (u32)Get().m_shaders.size() - 1;
	}

	ShaderProgram* AssetManagement::GetShader(u32 id)
	{
		return GetFrom<ShaderProgram>(Get().m_shaders, id);
	}

	void AssetManagement::ReloadShaders()
	{
		for (u32 i = 0; i < Get().m_shaders.size(); i++)
		{
			ShaderFactory::Reload(Get().m_shaders[i], Get().m_shader_names[i]);
		}
	}

	u32 AssetManagement::CreateMesh(DefaultShape shape)
	{
		switch (shape)
		{
		case DefaultShape::CUBE:
			{
				Mesh mesh = MeshFactory::GenarateCube();
				Get().m_meshs.push_back(std::move(mesh));
			}
			break;
		case DefaultShape::SPHERE:
			{
				Mesh mesh = MeshFactory::GenarateSphere();
				Get().m_meshs.push_back(std::move(mesh));
			}
			break;
		case DefaultShape::QUARD:
			{
				Mesh mesh = MeshFactory::GenarateQuard();
				Get().m_meshs.push_back(std::move(mesh));
			}
			break;
		case DefaultShape::SCRERN_FILLED_QUARD:
			{
				Mesh mesh = MeshFactory::GenarateScreenFilledQuard();
				Get().m_meshs.push_back(std::move(mesh));
			}
			break;
		}

		return (u32)Get().m_meshs.size() - 1;
	}

	u32 AssetManagement::CreateMesh(const CPUMesh& cpu_mesh)
	{
		Mesh mesh = MeshFactory::CreateFromCPU(cpu_mesh);
		Get().m_meshs.push_back(std::move(mesh));

		return (u32)Get().m_meshs.size() - 1;
	}

	Mesh* AssetManagement::GetMesh(u32 id)
	{
		return GetFrom<Mesh>(Get().m_meshs, id);
	}

	u32 AssetManagement::CreateModel(u32 mesh)
	{
		std::vector<u32> meshs;
		std::vector<Material> material;

		meshs.emplace_back(mesh);
		material.emplace_back();

		Get().m_models.emplace_back(meshs, material);

		return (u32)Get().m_models.size() - 1;
	}

	u32 AssetManagement::CreateModel(u32 mesh, Material mat)
	{
		Get().m_models.emplace_back(mesh, mat);

		return (u32)Get().m_models.size() - 1;
	}

	u32 AssetManagement::LoadFromObjFile(const std::string& file_path)
	{
		std::vector<u32> meshs;
		std::vector<Material> material;
		std::vector<CPUMesh> cpu_meshs = ObjLoader::Load((GetAssetDir() + file_path).c_str());

		for (CPUMesh& m : cpu_meshs)
		{
			meshs.push_back(CreateMesh(m));
			if(!m.material.AlbedoMap.empty())
				AssetManagement::CreateTexture2D(m.material.AlbedoMap);

			if (!m.material.NormalMap.empty())
				AssetManagement::CreateTexture2D(m.material.NormalMap);

			if (!m.material.RoughnessMap.empty())
				AssetManagement::CreateTexture2D(m.material.RoughnessMap);

			if (!m.material.MetallicMap.empty())
				AssetManagement::CreateTexture2D(m.material.MetallicMap);

			material.emplace_back(m.material);
		}

		Get().m_models.emplace_back(meshs, material);

		return (u32)Get().m_models.size() - 1;
	}

	Model* AssetManagement::GetModel(u32 id)
	{
		return GetFrom<Model>(Get().m_models, id);
	}

	void AssetManagement::Clear()
	{
		Get().m_shader_names.clear();
		Get().m_shaders.clear();
		Get().m_textures.clear();
		Get().m_meshs.clear();
		Get().m_models.clear();
	}

	AssetManagement& AssetManagement::Get()
	{
		return assetManager;
	}
}


