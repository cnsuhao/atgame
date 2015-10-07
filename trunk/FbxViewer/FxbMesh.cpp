#include "StdAfx.h"
#include "FxbMesh.h"
#include "atgRenderer.h"

#include <assimp/mesh.h>


FxbMesh::FxbMesh( FxbModel* parent ):m_VertexStream(NULL),m_VertexDecl(NULL),m_ChangeVDecl(false),m_Parent(parent)
{
	
}


FxbMesh::~FxbMesh(void)
{
	if (m_VertexStream)
	{
		delete m_VertexStream;
		m_VertexStream = NULL;
	}
}

uint32 FxbMesh::GetNumberOfVertexs() const
{
	return m_NumberOfVertexs;
}

void* FxbMesh::GetVertexs()
{
	if (!m_ChangeVDecl && m_VertexStream)
	{
		return m_VertexStream;
	}
	else
	{
		if (m_VertexStream)
		{
			delete m_VertexStream;
			m_VertexStream = NULL;
		}

		aiMesh* pAIMesh = reinterpret_cast<aiMesh*>(m_Data);

		uint8 stride = m_VertexDecl->GetElementsStride();
		m_VertexStream = new float[pAIMesh->mNumVertices * stride];

		float* pBegin = reinterpret_cast<float*>(m_VertexStream) - 1;
		for (unsigned int i = 0; i < pAIMesh->mNumVertices; ++i)
		{
			// vertexs
			*(++pBegin) = pAIMesh->mVertices[i].x;
			*(++pBegin) = pAIMesh->mVertices[i].y;
			*(++pBegin) = pAIMesh->mVertices[i].z;

			// normals 
			*(++pBegin) = pAIMesh->mNormals[i].x;
			*(++pBegin) = pAIMesh->mNormals[i].y;
			*(++pBegin) = pAIMesh->mNormals[i].z;

			// texture coords
			*(++pBegin) = pAIMesh->mTextureCoords[0][i].x;
			*(++pBegin) = pAIMesh->mTextureCoords[0][i].y;
		}
		m_ChangeVDecl = false;

		return m_VertexStream;
	}
}

atgVertexDecl* FxbMesh::GetVertexDeclaration() const
{
	return m_VertexDecl;
}

uint32 FxbMesh::GetNumberOfIndices() const
{
	return m_NumberOfIndices;
}

void* FxbMesh::GetIndices()
{
	if (m_IndicesStream)
	{
		aiMesh* pAIMesh = reinterpret_cast<aiMesh*>(m_Data);

		m_IndicesStream = new uint16[pAIMesh->mNumFaces * 3];
		uint16* pBegin = static_cast<uint16*>(m_IndicesStream) - 1;
		for (unsigned int fi = 0; fi < pAIMesh->mNumFaces; ++fi)
		{
			*(++pBegin) = pAIMesh->mFaces[fi].mIndices[0];
			*(++pBegin) = pAIMesh->mFaces[fi].mIndices[1];
			*(++pBegin) = pAIMesh->mFaces[fi].mIndices[2];
		}
	}
	return m_IndicesStream;
}

uint32 FxbMesh::GetIndexFormat() const
{
	return atgIndexBuffer::IFB_Index16;
}
