#ifndef _ATG_MESH_H_
#define _ATG_MESH_H_

class atgVertexDecl;
class atgMaterial;

class atgMesh
{
public:
    atgMesh(void);
    virtual ~atgMesh(void);

    virtual uint32          GetNumberOfVertexs() const;
    virtual void*           GetVertexs();
    virtual atgVertexDecl*  GetVertexDeclaration() const;
    virtual bool            IsVertexsDataDirty() const;
    inline void             SetVertexsDataDirty();


    virtual uint32          GetNumberOfIndices() const;
    virtual void*           GetIndices();
    virtual uint32          GetIndexFormat() const;
    virtual bool            IsIndicesDataDirty() const;
    inline void             SetIndicesDataDirty();

    virtual void            SetMaterial(atgMaterial* material);
    virtual atgMaterial*    GetMaterial();

    virtual void            Render();
protected:
    class atgVertexBuffer*  _vertexBuffer;
    class atgIndexBuffer* _indexBuffer;
    class atgMaterial*  _material;

    bool _vertexsDataDirty;
    bool _indicesDataDirty;
};

inline void atgMesh::SetVertexsDataDirty()
{
    _vertexsDataDirty = true;
}

inline void atgMesh::SetIndicesDataDirty()
{
    _indicesDataDirty = true;
}

#endif // _ATG_MESH_H_
