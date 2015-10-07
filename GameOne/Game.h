#ifndef __GAME_H_
#define __GAME_H_

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif // WIN32


#include "atgBase.h"
#include "atgSkinAnimation.h"
#include "atgMaterial.h"
#include "atgMesh.h"
#include "atgGame.h"

struct MdxNode;
class MdxModel;
class MyMdxNode;
class MyMdxMaterial;
class MyMdxMesh;

class MyMdxNode : public atgJoint , public atgJointAnimData
{
public:
    static void                 BuildSkeleton(MyMdxNode* root);
    static void                 Update(MyMdxNode* root, float elapsedTime, float startTime, float endTime);
    static MyMdxNode*           GetNodeById(MyMdxNode* root, int id);
    MyMdxNode(MdxModel* model);
    ~MyMdxNode(void);

    void                        SetPivot(float (*position)[3]);

    //////////////////////////////////////////////////////////////////////////
    // atgJointAnimData override
    virtual uint32              GetNumberTranslationKey() OVERRIDE;
    virtual uint32              GetTimeTranslationKey(int keyIndex) OVERRIDE;

    virtual uint32              GetNumberRotationKey() OVERRIDE;
    virtual uint32              GetTimeRotationKey(int keyIndex) OVERRIDE;

    virtual uint32              GetNumberScalingKey() OVERRIDE;
    virtual uint32              GetTimeScalingKey(int keyIndex) OVERRIDE;

    virtual uint32              GetKeyTranslationInterpolationType(int keyIndex) OVERRIDE;
    virtual uint32              GetKeyRotationInterpolationType(int keyIndex) OVERRIDE;
    virtual uint32              GetKeyScalingInterpolationType(int keyIndex) OVERRIDE;

    virtual void                GetKeyTranslation(float (*translation)[3], float (*control1)[3], float (*control2)[3],int keyIndex) OVERRIDE;
    virtual void                GetKeyRotation(float (*rotation)[4], float (*control1)[4], float (*control2)[4],int keyIndex) OVERRIDE;
    virtual void                GetKeyScaling(float (*scaling)[3],float (*control1)[3], float (*control2)[3], int keyIndex) OVERRIDE;

protected:
    void                        UpdataAninData(float elapsedTime, float startTime, float endTime);
    void                        BuildChild();

protected:
    MdxNode* _nodeData;
    MdxModel* _model;
};


class MyMdxMaterial : public atgMaterial
{
public:
    MyMdxMaterial(MyMdxMesh* mesh);
    ~MyMdxMaterial(){};

    virtual bool Load(const char* fileName) OVERRIDE;
protected:
    MyMdxMesh* _mesh;
};

class MyMdxMesh : public atgMesh
{
public:
    MyMdxMesh(MdxModel* model, int index);
    ~MyMdxMesh(){ if(_vertexsStreamData) { SAFE_DELETE(_vertexsStreamData); } }

    MdxModel*                   GetModel() { return _model; }
    MyMdxNode*                  GetRootNode() { return _rootNode; }
    int                         GetIndex() const { return _index; }
    //////////////////////////////////////////////////////////////////////////
    // atgMesh override
    virtual uint32              GetNumberOfVertexs() const OVERRIDE;
    virtual void*               GetVertexs() OVERRIDE;
    virtual atgVertexDecl*      GetVertexDeclaration() const OVERRIDE;

    virtual uint32              GetNumberOfIndices() const OVERRIDE;
    virtual void*               GetIndices() OVERRIDE;
    virtual uint32              GetIndexFormat() const OVERRIDE;

    virtual void                Render() OVERRIDE; 
    void                        DrawJoint();

    void                        BuildMaterial();
    void                        UpdataAnim(float elapsedTime, float startTime, float endTime);
protected:
    MdxModel* _model;
    MyMdxNode* _rootNode;
    float _rootPosition[3];
    int _index;
    float* _vertexsStreamData;
};

/*
**  mdx Animation public atgSkinAnimation..
*/
class MyMdxAnimation : public atgSkinAnimation
{
public:
    explicit MyMdxAnimation(MdxModel* model);
    //////////////////////////////////////////////////////////////////////////
    // atgSkinAnimation override
    virtual uint8              GetNumberAnimation() const OVERRIDE;
    virtual const char*        GetName(uint8 index) const OVERRIDE;
    virtual float              GetStartTime(uint8 index) const OVERRIDE;
    virtual float              GetEndTime(uint8 index) const OVERRIDE;

protected:
    MdxModel* _model;
};

class Game : public atgGame
{
public:
    Game(void);
    ~Game(void);

    virtual bool OnInitialize() OVERRIDE;

    virtual void OnShutdown() OVERRIDE;

    virtual void OnPause() OVERRIDE;

    virtual void OnResume() OVERRIDE;
protected:
    virtual void OnFrame() OVERRIDE;

    virtual void OnPointerDown(uint8 id, int16 x, int16 y) OVERRIDE;

    virtual void OnPointerMove(uint8 id, int16 x, int16 y) OVERRIDE;

    virtual void OnPointerUp(uint8 id, int16 x, int16 y) OVERRIDE;

    virtual void OnKeyScanDown(Key::Scan keyscan) OVERRIDE;

    void LoadConfig();

    void DrawAxis();
public:
	class atgCamera* GetCamera() { return _Camera; }
protected:
    uint16 _mx;
    uint16 _my;
    bool   _down;
    class atgCamera* _Camera;
	class ScriptObject* _mainScript;
};

extern Game* gGame;

#endif // __GAME_H_
