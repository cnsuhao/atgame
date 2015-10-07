#pragma once


class FxbMesh;
class atgMaterial;

class FxbModel
{
public:
	FxbModel(const char* pFile);
	~FxbModel(void);


	void					Render();

	inline	uint32			GetNumberOfMaterial() { return m_Materials.size(); }
	atgMaterial*			GetIndexOfMaterial(uint32 index) { return m_Materials[index]; }

protected:
	bool					Paser();
	void					Release();

protected:
	std::vector<FxbMesh*> m_Meshs;
	std::vector<atgMaterial*> m_Materials;

	void* m_LoaderData;
};

