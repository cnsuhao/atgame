#pragma once

#include "atgMesh.h"

class FxbModel;

class FxbMesh : public atgMesh
{
public:
	FxbMesh(FxbModel* parent);
	~FxbMesh(void);

	virtual uint32          GetNumberOfVertexs() const;
	virtual void*           GetVertexs();
	virtual atgVertexDecl*  GetVertexDeclaration() const;


	virtual uint32          GetNumberOfIndices() const;
	virtual void*           GetIndices();
	virtual uint32          GetIndexFormat() const;

public:


	inline void SetNumberOfVertexs(uint32 Number) { m_NumberOfVertexs = Number; }
	inline void SetNumberOfIndices(uint32 Number) { m_NumberOfIndices = Number; }

	inline void SetVertexDeclaration(atgVertexDecl* VDecl) { m_VertexDecl = VDecl; m_ChangeVDecl = true; }

	inline void SetData(void* Data) { m_Data = Data; }
protected:
	uint32	m_NumberOfVertexs;
	uint32	m_NumberOfIndices;


	void*	m_VertexStream;
	void*   m_IndicesStream;
	atgVertexDecl* m_VertexDecl;
	bool    m_ChangeVDecl;

	FxbModel*	m_Parent;
	void*		m_Data;
};

