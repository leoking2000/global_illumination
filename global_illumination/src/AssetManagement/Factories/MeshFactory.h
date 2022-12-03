#pragma once
#include "Graphics/Mesh.h"
#include "ObjLoader.h"

namespace GL
{
	class MeshFactory
	{
	public:
		static Mesh CreateFromCPU(const CPUMesh& cpu_mesh);
		static Mesh GenarateCube();
		static Mesh GenarateSphere(u32 prec = 48);
		static Mesh GenarateQuard(u32 repet = 1);
		static Mesh GenarateScreenFilledQuard();
	};


}