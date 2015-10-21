#include "StdAfx.h"
#include "FxbModel.h"
#include "FxbMesh.h"
#include "atgMaterial.h"
#include "atgRenderer.h"
#include "atgMisc.h"
#include "atgShaderLibrary.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <assimp/Exporter.hpp>

struct assimp_load_struct
{
	Assimp::Importer importer;
	const aiScene* scene;
	std::string fileName;

	assimp_load_struct():scene(NULL)
	{
	}
};

typedef assimp_load_struct ALS;

#define ToALS(x) reinterpret_cast<ALS*>(x)


FxbModel::FxbModel( const char* pFile )
{
	m_LoaderData = new assimp_load_struct;

	ALS* pALS = ToALS(m_LoaderData);
	pALS->fileName.clear();
	pALS->fileName.assign(pFile);

	Paser();
}

FxbModel::~FxbModel(void)
{
	for (unsigned int i = 0; i < m_Meshs.size(); ++i)
	{
		atgVertexDecl* pVDecl = m_Meshs[i]->GetVertexDeclaration();
		SAFE_DELETE(pVDecl);
		m_Meshs[i]->SetVertexDeclaration(NULL);

	}
	m_Meshs.clear();
}

bool FxbModel::Paser()
{
	ALS* pALS = ToALS(m_LoaderData);

	pALS->scene = pALS->importer.ReadFile(pALS->fileName.c_str(),
		aiProcess_CalcTangentSpace|
		aiProcess_Triangulate|
		aiProcess_JoinIdenticalVertices|
		aiProcess_SortByPType);

	if (!pALS->scene)
	{
		LOG("loading fxb file fail!\n");
		return false;
	}

    Assimp::Exporter exporter;
    size_t t = exporter.GetExportFormatCount();
    for (size_t i = 0; i < t; ++i)
    {
        const aiExportFormatDesc* pDesc = exporter.GetExportFormatDescription(i);
        LOG("=============>", pDesc->id);
        LOG("%s\n", pDesc->id);
        LOG("%s\n", pDesc->description);
        LOG("%s\n", pDesc->fileExtension);
    }
    //exporter.Export(pALS->scene, "obj", "F:\\atGame\\Debug");


	size_t pos = pALS->fileName.rfind("\\"); 
	std::string name;
	if (pos != std::string::npos)
	{
		name = pALS->fileName.substr(pos);
	}


	LOG("modle(%s) has %d Meterial.\n", name.c_str(), pALS->scene->mNumMaterials);
	m_Materials.resize(pALS->scene->mNumMaterials);

    
	aiString path;
    aiString matName;
    aiString texName("model/Tx_Building_House1.jpg");
	for (unsigned int mj = 0; mj < pALS->scene->mNumMaterials; ++mj)
	{
		atgMaterial* pMaterial = new atgMaterial();
        aiMaterial* pAIMaterial = pALS->scene->mMaterials[mj];
		uint8 NumOfTexture = pAIMaterial->GetTextureCount(aiTextureType_DIFFUSE);
		for (int ti = 0; ti < NumOfTexture; ++ti)
		{
			pAIMaterial->GetTexture(aiTextureType_DIFFUSE,
													ti,
													&path);
		}


        for (int pi = 0, numProp = pAIMaterial->mNumProperties; pi < numProp; ++pi )
        {
            aiMaterialProperty* pProp = pAIMaterial->mProperties[pi];
            LOG("pAIMaterial[%s] Properties[%d][%s]\n", name.c_str(), pi, pProp->mKey.C_Str());
        }

        pAIMaterial->Get(AI_MATKEY_NAME, matName);
        LOG("pAIMaterial[%s] AI_MATKEY_NAME=[%s] \n", name.c_str(), matName.C_Str());

        aiShadingMode ShadingModel;
        pAIMaterial->Get(AI_MATKEY_SHADING_MODEL, (int&)ShadingModel);


        int isTwoSide;
        pAIMaterial->Get(AI_MATKEY_TWOSIDED, isTwoSide);


        aiColor4D colorDiff;
        pAIMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, colorDiff);
        //pMaterial->SetDiffuseColor(Vec3(colorDiff.r, colorDiff.g, colorDiff.b));

        aiColor4D ambient;
        pAIMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        //pMaterial->SetAmbientColor(Vec3(ambient.r, ambient.g, ambient.b));

		atgPass* pass = atgShaderLibFactory::FindOrCreatePass(LIGHT_TEXTURE_PASS_IDENTITY);
		pMaterial->SetPass(pass);
        pMaterial->SetShininess(128.0f);
        pMaterial->SetSpecularColor(Vec3(0.1f, 0.1f, 0.1f));

		JPEG_Image image;
		ColorOrder co = IsOpenGLGraph() ? CO_RGBA : CO_ARGB;

        atgTexture* texture = g_Renderer->FindCacheTexture(texName.C_Str());
        if (!texture)
        {
            JPEG_Loader::Load(&image, texName.C_Str(), true, co);
            texture = atgTexture::Create(image.width, image.height, image.bpp, image.imageData); 
            JPEG_Loader::Release(&image);
            g_Renderer->InsertCacheTexture(texName.C_Str(), texture);
        }

        if (texture)
        {
            pMaterial->AddTexture(texture);
        }
        m_Materials[mj] = pMaterial;
	}

	LOG("modle(%s) has %d Mesh.\n", name.c_str(), pALS->scene->mNumMeshes);
	m_Meshs.resize(pALS->scene->mNumMeshes);
	atgVertexDecl VDecl;
	VDecl.AppendElement(0, atgVertexDecl::VA_Pos3);
	VDecl.AppendElement(0, atgVertexDecl::VA_Normal);
	VDecl.AppendElement(0, atgVertexDecl::VA_Texture0);

	for (unsigned int mi = 0; mi < pALS->scene->mNumMeshes; ++mi)
	{
		aiMesh* pMesh = pALS->scene->mMeshes[mi];
		LOG("	Mesh Name=%s.\n", pMesh->mName.C_Str());
		m_Meshs[mi] = new FxbMesh(this);
		m_Meshs[mi]->SetData(pMesh);
		m_Meshs[mi]->SetNumberOfVertexs(pMesh->mNumVertices);
		m_Meshs[mi]->SetNumberOfIndices(pMesh->mNumFaces * 3);
		atgVertexDecl* NewVDecl = new atgVertexDecl(VDecl);
		m_Meshs[mi]->SetVertexDeclaration(NewVDecl);
		m_Meshs[mi]->SetMaterial(m_Materials[pMesh->mMaterialIndex]);
	}

	return true;
}

void FxbModel::Release()
{
	if (m_LoaderData)
	{
		delete m_LoaderData; // 析构的时候会释放 scene 资源。
		m_LoaderData = NULL;
	}
}

void FxbModel::Render()
{
	for (unsigned int i = 0; i < m_Meshs.size(); ++i)
	{
		m_Meshs[i]->Render();
	}
}
