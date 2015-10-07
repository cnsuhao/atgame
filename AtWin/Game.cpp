#include "Game.h"
#include "atgMisc.h"
#include "atgCamera.h"
#include "atgShaderLibrary.h"
#include "atgProfile.h"
#include "atgUIBase.h"
#include "atgIntersection.h"

int width = 800;
int height = 600;

atgVertexDecl*      g_MdxVertexDecl;
typedef std::vector<class MyMdxMesh*> MdxMeshVector;
MdxMeshVector g_ModelMeshs;
class MyMdxAnimation* g_Animation;
uint8 g_palyIndex;

using namespace ELGUI;

//////////////////////////////////////////////////////////////////////////
// MyMdxNode
//////////////////////////////////////////////////////////////////////////
MyMdxNode::MyMdxNode(MdxModel* model):_model(model)
{
    _nodeData = NULL;
}

MyMdxNode::~MyMdxNode(void) 
{

}

void MyMdxNode::BuildSkeleton(MyMdxNode* root)
{
    MdxModel* model = root->_model;
    AASSERT (model);

    int number = model->GetNumberOfNodes();
    for (int i = 0; i < number; ++i)
    {
        MdxNode* node = model->GetNode(i);
        if (node && node->parentId == -1)  
        {
            MyMdxNode* childNode = new MyMdxNode(model);
			childNode->_id = node->objectId;
            childNode->_pivot = &model->GetPivot(node->objectId)->c;
            childNode->_nodeData = node;
            node->userData = childNode;
            root->AddChild(childNode);
            childNode->SetParent(root);
            childNode->BuildChild();
        }
    }
}

void MyMdxNode::Update(MyMdxNode* node, float elapsedTime, float startTime, float endTime)
{
    node->UpdataAninData(elapsedTime, startTime, endTime);
    node->UpdateJointTransform();
}

MyMdxNode* MyMdxNode::GetNodeById(MyMdxNode* root, int id)
{
    return static_cast<MyMdxNode*>(root->_model->GetNode(id)->userData);
}

void  MyMdxNode::SetPivot(float (*position)[3])
{
    _pivot = position;
}

void MyMdxNode::BuildChild()
{
    int number = _model->GetNumberOfNodes();
    for (int i = 0; i < number; i++)
    {
        MdxNode* node = _model->GetNode(i);
        if (node && node->parentId == _nodeData->objectId)
        {
            MyMdxNode* childNode = new MyMdxNode(_model);
			childNode->_id = node->objectId;
            childNode->_pivot = &_model->GetPivot(node->objectId)->c;
            childNode->_nodeData = node;
            node->userData = childNode;
            AddChild(childNode);
            childNode->SetParent(this);
            childNode->BuildChild();
        }
    }
}

void MyMdxNode::UpdataAninData(float elapsedTime, float startTime, float endTime)
{
	int time = static_cast<int>(elapsedTime);
    atgJointAnimData::UpdateAnim(this, this, time, (uint32)startTime, (uint32)endTime);

    int numberChild = GetNumberChild();
    for (int i = 0; i < numberChild; i++)
    {
        MyMdxNode* child = static_cast<MyMdxNode*>(GetChild(i));
        if (child)
        {
            child->UpdataAninData(elapsedTime, startTime, endTime);
        }
    }
}

uint32  MyMdxNode::GetNumberTranslationKey()
{
    return static_cast<uint32>(_nodeData->translateList.keyList.size());
}

uint32  MyMdxNode::GetTimeTranslationKey(int keyIndex)
{
    if (0 <= keyIndex && keyIndex < static_cast<int>(_nodeData->translateList.keyList.size()))
    {
        return _nodeData->translateList.keyList[keyIndex].time;
    }
    return 0;
}

uint32  MyMdxNode::GetNumberRotationKey()
{
    return static_cast<uint32>(_nodeData->rotationList.keyList.size());
}

uint32  MyMdxNode::GetTimeRotationKey(int keyIndex)
{
    if (0 <= keyIndex && keyIndex < static_cast<int>(_nodeData->rotationList.keyList.size()))
    {
        return _nodeData->rotationList.keyList[keyIndex].time;
    }
    return 0;
}

uint32 MyMdxNode::GetNumberScalingKey()
{
    return static_cast<uint32>(_nodeData->scalingList.keyList.size());
}

uint32 MyMdxNode::GetTimeScalingKey(int keyIndex)
{
    if (0 <= keyIndex && keyIndex < static_cast<int>(_nodeData->scalingList.keyList.size()))
    {
        return _nodeData->scalingList.keyList[keyIndex].time;
    }
    return 0;
}

uint32 MyMdxNode::GetKeyTranslationInterpolationType(int keyIndex)
{
    return _nodeData->translateList.interpolationType;
}

uint32 MyMdxNode::GetKeyRotationInterpolationType(int keyIndex)
{
    return _nodeData->rotationList.interpolationType;
}

uint32 MyMdxNode::GetKeyScalingInterpolationType(int keyIndex)
{
    return _nodeData->scalingList.interpolationType;
}

void MyMdxNode::GetKeyTranslation(float (*translation)[3], float (*control1)[3], float (*control2)[3],int keyIndex)
{
    const keyTranslate& translateKey = _nodeData->translateList.keyList[keyIndex];
    memcpy(*translation, translateKey.vector.c, sizeof(float[3]));
    if (_nodeData->translateList.interpolationType > INTERPOLATION_TYPE_LINEAR)
    {
		if(control1)
			memcpy(*control1, translateKey.inTran.c, sizeof(float[3]));
		if(control2)
			memcpy(*control2, translateKey.outTran.c, sizeof(float[3]));
    }
}

void MyMdxNode::GetKeyRotation(float (*rotation)[4], float (*control1)[4], float (*control2)[4],int keyIndex)
{
    const keyRotation& rotationKey = _nodeData->rotationList.keyList[keyIndex];
    memcpy(*rotation, rotationKey.rotation.c, sizeof(float[4]));
    if (_nodeData->rotationList.interpolationType > INTERPOLATION_TYPE_LINEAR)
    {
		if(control1)
			memcpy(*control1, rotationKey.inTran.c, sizeof(float[4]));
		if(control2)
			memcpy(*control2, rotationKey.outTran.c, sizeof(float[4]));
    }
}

void MyMdxNode::GetKeyScaling(float (*scaling)[3],float (*control1)[3], float (*control2)[3], int keyIndex)
{
    const keyScaling& scalingKey = _nodeData->scalingList.keyList[keyIndex];
    memcpy(*scaling, scalingKey.vector.c, sizeof(float[3]));
    if (_nodeData->scalingList.interpolationType > INTERPOLATION_TYPE_LINEAR)
    {
		if(control1)
			memcpy(*control1, scalingKey.inTran.c, sizeof(float[3]));
		if(control2)
			memcpy(*control2, scalingKey.outTran.c, sizeof(float[3]));
    }
}


//////////////////////////////////////////////////////////////////////////
// MyMdxMaterial
//////////////////////////////////////////////////////////////////////////
MyMdxMaterial::MyMdxMaterial(MyMdxMesh* mesh):_mesh(mesh)
{

}

bool MyMdxMaterial::Load(const char* fileName)
{
    int meshIndex = _mesh->GetIndex();
    MdxModel* model = _mesh->GetModel();

    MdxMaterial* material = model->GetMeshMaterial(meshIndex);
    // load textuers
    //TGA_Image image;
	JPEG_Image image;
    for (int i = 0 ; i < material->numberLayers; i++)
    {
        MdxTexture* texture = model->GetTexture((uint8)material->layers[i].textureId);
        std::string texturePath;
        //texturePath.assign("model/golemstatue.tga");
        //TGA_Loader::Load(&image, texturePath.c_str(), yReversed);

		texturePath.assign("model/golemstatue.jpg");
		bool rs = JPEG_Loader::Load(&image, texturePath.c_str(), false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);
        if(rs){
            LOG("image.width=%d, image.height=%d, image.bpp=%d", image.width, image.height, image.bpp);
        }
        atgTexture* pTex = atgTexture::Create(image.width, image.height, image.bpp, image.imageData); 
        if(pTex)
            _textures.push_back(pTex);
        //TGA_Loader::Release(&image);
		JPEG_Loader::Release(&image);
    }
    // create shader
	_pass = atgShaderLibFactory::FindOrCreatePass(NOT_LIGNTE_TEXTURE_PASS_IDENTITY);
    // set mesh material
    _mesh->SetMaterial(this);
    return true;
}

//////////////////////////////////////////////////////////////////////////
// MyMdxMesh
//////////////////////////////////////////////////////////////////////////
MyMdxMesh::MyMdxMesh(MdxModel* model, int index):_model(model)
{
    AASSERT(index < model->GetNumberOfMesh());
    _index = index;
    _vertexsStreamData = NULL;
    _rootPosition[0] = _rootPosition[1] = _rootPosition[2] = 0.0f;

    _rootNode = new MyMdxNode(model);
    _rootNode->SetPivot(&_rootPosition);
    MyMdxNode::BuildSkeleton(_rootNode);
}

uint32 MyMdxMesh::GetNumberOfVertexs() const
{
    return _model->GetNumberOfVertexs(_index);
}

void* MyMdxMesh::GetVertexs()
{
	ATG_PROFILE("MyMdxMesh::GetVertexs");
    int numberVertexs = _model->GetNumberOfVertexs(_index);
    MdxFloat3* vertexs = _model->GetVertexs(_index);
    MdxFloat3* normals = _model->GetNormalsOfVertexs(_index);
    MdxFloat2* texCoords = _model->GetTexturesOfVertexs(_index);
    int streamSize = numberVertexs * (sizeof(MdxFloat3) + sizeof(MdxFloat3) + sizeof(MdxFloat2));

    if (!_vertexsStreamData)
        _vertexsStreamData = new float[streamSize];

    float* pBegin = _vertexsStreamData;
    for (int i = 0; i < numberVertexs; i++)
    {
        float AddVec[3];
        float temp[3];
        VecZero(AddVec);
        VecZero(temp);
        const MdxMatrixList&  matrixList = _model->GetMatrixList(_index, i);

        for (uint8 ni = 0; ni < matrixList.numberOfNode; ni++)
        {
            MdxNode* node = _model->GetNode(matrixList.nodeIndices[ni]);
            VecTransform(vertexs[i].c, static_cast<MyMdxNode*>(node->userData)->GetMatrix().m, temp);
            VecAdd(AddVec, temp, AddVec);
        }
        if (matrixList.numberOfNode > 1)
        {
            VecScale(AddVec, 1.0f/matrixList.numberOfNode, AddVec);
        }

        // vertexs
        float* p = AddVec;
        *pBegin = *p;
        *(++pBegin) = *(++p);
        *(++pBegin) = *(++p);
        // normals
        p = normals[i].c;
        *(++pBegin) = *p;
        *(++pBegin) = *(++p);
        *(++pBegin) = *(++p);
        // texture coords
        p = texCoords[i].c;
        *(++pBegin) = *p;
        *(++pBegin) = *(++p);
        ++pBegin;
    }
    return _vertexsStreamData;
}

atgVertexDecl* MyMdxMesh::GetVertexDeclaration() const
{
    return g_MdxVertexDecl;
}

uint32 MyMdxMesh::GetNumberOfIndices() const 
{
    return _model->GetNumberOfFaces(_index) * 3;
}

void* MyMdxMesh::GetIndices()
{
    return _model->GetFace(_index);
}

uint32 MyMdxMesh::GetIndexFormat() const
{
    return atgIndexBuffer::IFB_Index16;
}

void MyMdxMesh::Render()
{
    atgMesh::Render();
    DrawJoint();
}

void MyMdxMesh::DrawJoint()
{
    int number = GetRootNode()->GetNumberChild();
    for (int i = 0; i < number; ++i)
    {
        MyMdxNode* node = static_cast<MyMdxNode*>(GetRootNode()->GetChild(i));
        if (node)
        {
            MyMdxNode::DrawJoint(node);
        }
    }
}

void MyMdxMesh::BuildMaterial()
{
    MyMdxMaterial* material = new MyMdxMaterial(this);
    material->Load(NULL);
}

void MyMdxMesh::UpdataAnim(float elapsedTime, float startTime, float endTime)
{
	ATG_PROFILE("MyMdxMesh::UpdataAnim");
    if (_rootNode)
    {
        int number = _rootNode->GetNumberChild();
        for (int i = 0; i < number; ++i)
        {
            MyMdxNode* node = static_cast<MyMdxNode*>(_rootNode->GetChild(i));
            if (node)
            {
                MyMdxNode::Update(node, elapsedTime, startTime, endTime);
                SetVertexsDataDirty();
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
MyMdxAnimation::MyMdxAnimation(MdxModel* model):_model(model)
{

}

//////////////////////////////////////////////////////////////////////////
// atgSkinAnimation override
uint8 MyMdxAnimation::GetNumberAnimation() const
{
    if (_model)
    {
        return _model->GetNumberAnimation();
    }
    return 0;
}

const char* MyMdxAnimation::GetName(uint8 index) const
{
    return _model->GetAnimation(index).name;
}

float MyMdxAnimation::GetStartTime(uint8 index) const
{
    return static_cast<float>(_model->GetAnimation(index).intervalStart);
}

float MyMdxAnimation::GetEndTime(uint8 index) const
{
    return static_cast<float>(_model->GetAnimation(index).intervalEnd);
}

Game::Game(void)
{
    _down = false;
    _Camera = NULL;
}

Game::~Game(void)
{

}

bool Game::OnInitialize()
{
    LOG("Game::OnInitialize!\n");
    SetRandomSeed((uint32)GetAbsoluteMsecTime());
    
    g_Renderer->SetViewport(0, 0, GetWindowWidth(), GetWindowHeight());
    g_Renderer->SetFaceCull(FCM_CW);

    Matrix world(MatrixIdentity);
    //world.RotationZXY(0.0f, -90.0f, -90.0f);
    g_Renderer->SetMatrix(MD_WORLD, world.m);

    _Camera = new atgCamera();
    if (_Camera)
    {
        _Camera->SetPosition(Vec3(0,120,250));
        g_Renderer->SetMatrix(MD_VIEW, _Camera->GetView().m);
        g_Renderer->SetMatrix(MD_PROJECTION, _Camera->GetProj().m);
    }


    MdxModel* model = new MdxModel("model/golemstatue.mdx");
    if (!model->IsLoadSuccess())
    {
        delete model;
        return false;
    }
    // mdx VertexDecl
    g_MdxVertexDecl = new atgVertexDecl();
    g_MdxVertexDecl->AppendElement(0, atgVertexDecl::VA_Pos3);
    g_MdxVertexDecl->AppendElement(0, atgVertexDecl::VA_Normal);
    g_MdxVertexDecl->AppendElement(0, atgVertexDecl::VA_Texture0);

    int numberMesh = model->GetNumberOfMesh();
    for (int mi = 0; mi < numberMesh; mi++)
    {
        MyMdxMesh* mesh = new MyMdxMesh(model, mi);
        mesh->BuildMaterial();
        g_ModelMeshs.push_back(mesh);
    }

    g_Animation = new MyMdxAnimation(model);
    SetRandomSeed((uint32)GetAbsoluteMsecTime());
    g_palyIndex = 6;//Random(0, g_Animation->GetNumberAnimation());
    g_Animation->Play(g_palyIndex);


    new System(new atgUIRenderer(this));

    WindowManager& winMgr = WindowManager::getSingleton();
    
    Texture* tex = (Texture*)System::getSingletonPtr()->getRenderer()->createTexture("ui/ui1.png");
    Imageset* imageset = ImagesetManager::getSingleton().createImageset("ui1", tex);
    Rect rect = Rect(Point(0.0f, 2.0f), Size(380.0f, 121.0f));
    imageset->defineImage("mainbar_left", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(407.0f, 60.0f), Size(40.0f, 83.0f));
    imageset->defineImage("mainbar_hp", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(448.0f, 60.0f), Size(40.0f, 83.0f));
    imageset->defineImage("mainbar_mp", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(245.0f, 163.0f), Size(24.0f, 22.0f));
    imageset->defineImage("mainbar_inventory", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(277.0f, 163.0f), Size(24.0f, 22.0f));
    imageset->defineImage("mainbar_char", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(307.0f, 163.0f), Size(24.0f, 22.0f));
    imageset->defineImage("mainbar_sprite", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(338.0f, 163.0f), Size(24.0f, 22.0f));
    imageset->defineImage("mainbar_quest", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(369.0f, 163.0f), Size(24.0f, 22.0f));
    imageset->defineImage("mainbar_pet", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(400.0f, 163.0f), Size(24.0f, 22.0f));
    imageset->defineImage("mainbar_log", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(432.0f, 163.0f), Size(24.0f, 22.0f));
    imageset->defineImage("mainbar_map", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(462.0f, 163.0f), Size(24.0f, 22.0f));
    imageset->defineImage("mainbar_settings", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(442.0f, 339.0f), Size(33.0f, 33.0f));
    imageset->defineImage("mainbar_pause", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(1.0f, 430.0f), Size(41.0f, 42.0f));
    imageset->defineImage("mainbar_resume_hover", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(43.0f, 430.0f), Size(41.0f, 42.0f));
    imageset->defineImage("mainbar_resume_normal", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(0.0f, 220.0f), Size(21.0f, 21.0f));
    imageset->defineImage("tooltip_TL", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(25.0f, 220.0f), Size(5.0f, 21.0f));
    imageset->defineImage("tooltip_TM", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(34.0f, 220.0f), Size(21.0f, 21.0f));
    imageset->defineImage("tooltip_TR", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(0.0f, 245.0f), Size(21.0f, 5.0f));
    imageset->defineImage("tooltip_ML", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(25.0f, 245.0f), Size(5.0f, 5.0f));
    imageset->defineImage("tooltip_MM", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(34.0f, 245.0f), Size(21.0f, 5.0f));
    imageset->defineImage("tooltip_MR", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(0.0f, 254.0f), Size(21.0f, 20.0f));
    imageset->defineImage("tooltip_BL", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(25.0f, 254.0f), Size(5.0f, 20.0f));
    imageset->defineImage("tooltip_BM", rect, Point(0.0f, 0.0f));
    rect = Rect(Point(34.0f, 254.0f), Size(21.0f, 20.0f));
    imageset->defineImage("tooltip_BR", rect, Point(0.0f, 0.0f));

    // set gui sheet
    Window* sheet = winMgr.createWindow("DefaultWindow", "Sheet");
    sheet->setSize(UVector2(elgui_reldim(1.0f), elgui_reldim(1.0f)));
    sheet->setPosition(UVector2(elgui_reldim(0.0f), elgui_reldim(0.0f)));
    System::getSingletonPtr()->setGUISheet(sheet);

    ImageButton* main_bar_resume = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Resume");
    main_bar_resume->setSize(UVector2(elgui_absdim(41.0f), elgui_absdim(42.0f)));
    main_bar_resume->setPosition(UVector2(UDim(0.0f, 0), UDim(0.0f, 0)));
    main_bar_resume->setNormalImage(Helper::stringToImage("set:ui1 image:mainbar_resume_normal"));
    main_bar_resume->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_resume_hover"));
    main_bar_resume->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_resume_hover"));
    main_bar_resume->addChildWindow(main_bar_resume);
    sheet->addChildWindow(main_bar_resume);
    
    return true;
}

void Game::OnShutdown()
{
    g_Renderer->Shutdown();
#ifdef USE_OPENGL
    //glDeleteVertexArrays(1, &VertexArrayID);
#endif
}

void Game::OnPause()
{

}

void Game::OnResume()
{

}

void Game::OnFrame()
{
    static int frame = 0;
    static float lastTime = GetAbsoluteMsecTime() / 1000.f;
    static float addTime = 0;
    ATG_PROFILE("Game::OnFrame");
    float time = GetAbsoluteMsecTime() / 1000.f;
    float elapsedTime = static_cast<float>(time - lastTime);
    lastTime = time;
    addTime += elapsedTime;
    if (addTime > 1.0f)
    {
        addTime -= 1.f;
        LOG("FPS = %d, %lf\n", frame, elapsedTime);
        frame = 0;
        //CProfileManager::dumpAll();
    }
    ++frame;

    if (_Camera)
    {
        g_Renderer->SetMatrix(MD_VIEW, _Camera->GetView().m);
        g_Renderer->SetMatrix(MD_PROJECTION, _Camera->GetProj().m);
    }

    //LOG("elapsedTime=%f\n", elapsedTime);
    //static float angle = -90.f;
    //if (angle > 270)
    //    angle = -90.f;
    //angle += elapsedTime * 0.1f;
    
    //Matrix world;
    //MatRotationZ(world.m, angle);
    //g_Renderer->SetMatrix(MD_WORLD, world.m);

    g_Renderer->Clear();

    g_Renderer->BeginFrame();

#if defined(OPENGL_FIXED_PIPELINE)
    glBegin(GL_QUADS);
    glVertex2f(-10,-10);
    glVertex2f(10,-10);
    glVertex2f(10,10);
    glVertex2f(-10,10);
    glEnd();
#elif defined(USE_PROGRAMABLE_PIPELINE)
    GLfloat q3[] = {
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
    };

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, q3);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisableClientState(GL_VERTEX_ARRAY);
#else

    g_Animation->Update(elapsedTime);
    MdxMeshVector::iterator it = g_ModelMeshs.begin();
    while(it != g_ModelMeshs.end())
    {
        MyMdxMesh* mesh = *it;
        mesh->UpdataAnim(g_Animation->GetAimationTime(),
                         g_Animation->GetStartTime(g_palyIndex), 
                         g_Animation->GetEndTime(g_palyIndex));
        mesh->Render();
        ++it;
    }

#endif
    DrawAxis();

	//DrawCurve();

    System::getSingleton().renderGUI();

    g_Renderer->EndFrame();

    g_Renderer->Present();

    CProfileManager::Increment_Frame_Counter();
}

void Game::OnPointerDown(uint8 id, int16 x, int16 y)
{
    if(id == 1)
    {
        // right button down
        _down = true;
    }
    else
    {
        ELGUI::System::getSingleton().injectMouseButtonDown(ELGUI::LeftButton);
    }
    
}

void Game::OnPointerMove(uint8 id, int16 x, int16 y)
{

    static int16 last_x = x;
    static int16 last_y = y;

    static float oYaw = DegreesToRadians(180.f);
    static float oPitch = 0;//DegreesToRadians(45.f);

    if (_down)
    {
        if (_Camera)
        {
            float dx = static_cast<float>(x - last_x);
            float dy = static_cast<float>(y - last_y);
            oYaw -= dx * 0.001f;
            oPitch += dy * 0.001f;
            _Camera->SetYaw(oYaw);
            _Camera->SetPitch(oPitch);
        }
    }

    if (id == MBID_MIDDLE)
    {
        float moveSpeed = 10.5f;
	    if (x > 0)
        {
            Vec3 forward = _Camera->GetForward();
            Vec3 pos = _Camera->GetPosition();
            VecScale(forward.m, moveSpeed, forward.m);
            VecAdd(pos.m, forward.m, pos.m);
            _Camera->SetPosition(pos.m);
        }
        else
        {
            Vec3 forward = _Camera->GetForward();
            Vec3 pos = _Camera->GetPosition();
            VecScale(forward.m, -moveSpeed, forward.m);
            VecAdd(pos.m, forward.m, pos.m);
            _Camera->SetPosition(pos.m);
        }
    }

    last_x = x;
    last_y = y;

    ELGUI::System::getSingleton().injectMousePosition((float)x, (float)y);
}

void Game::OnPointerUp(uint8 id, int16 x, int16 y)
{
    if(id == 1)
    {
        _down = false;
    }
    else
    {
        g_palyIndex = ++g_palyIndex % g_Animation->GetNumberAnimation();
        g_Animation->Play(g_palyIndex);
        ELGUI::System::getSingleton().injectMouseButtonUp(ELGUI::LeftButton);
    }
}

void Game::OnKeyScanDown(Key::Scan keyscan)
{
    float moveSpeed = 10.5f;

	switch (keyscan)
	{
	case Key::W:
		{
		    Vec3 forward = _Camera->GetForward();
			Vec3 pos = _Camera->GetPosition();
		    forward.Scale(moveSpeed);
			pos.Add(forward.m);
			_Camera->SetPosition(pos.m);

		}
		break;
	case Key::S:
		{
			Vec3 forward = _Camera->GetForward();
			Vec3 pos = _Camera->GetPosition();
			forward.Scale(-moveSpeed);
			pos.Add(forward.m);
			_Camera->SetPosition(pos.m);
		}
		break;
	case Key::A:
		{
			Vec3 right = _Camera->GetRight();
			Vec3 pos = _Camera->GetPosition();
			right.Scale(moveSpeed);
			pos.Add(right.m);
			_Camera->SetPosition(pos.m);
		}
		break;
	case Key::D:
		{
			Vec3 right = _Camera->GetRight();
			Vec3 pos = _Camera->GetPosition();
			right.Scale(-moveSpeed);
			pos.Add(right.m);
			_Camera->SetPosition(pos.m);
		}
		break;
	default:
		break;
	}

	atgGame::OnKeyScanDown(keyscan);
}

void Game::LoadConfig()
{

}

void Game::DrawAxis()
{
    ATG_PROFILE("Game::DrawAxis");
    uint32 oldVP[4];
    g_Renderer->GetViewPort(oldVP[0], oldVP[1], oldVP[2], oldVP[3]);

    uint32 newVP[4] = {0, IsOpenGLGraph() ? 0 : oldVP[3] - 100, 100, 100 };

    g_Renderer->SetViewport(newVP[0], newVP[1], newVP[2], newVP[3]);

    Matrix oldWorld;
    g_Renderer->GetMatrix(&oldWorld.m, MD_WORLD);
    Matrix newWorld(MatrixIdentity);
    g_Renderer->SetMatrix(MD_WORLD, newWorld.m);

    Matrix oldView;
    g_Renderer->GetMatrix(&oldView.m, MD_VIEW);
    Matrix newView(MatrixIdentity);
    Vec3 p(0.0f, 0.0f, 3.5f);
    if (_Camera)
    {
        newView.SetColumn3(0, _Camera->GetRight());
        newView.SetColumn3(1, _Camera->GetUp());
        newView.SetColumn3(2, _Camera->GetForward());
        newView.Transpose();
        newView.SetColumn3(3, p*-1.0f);
    }
    else
    {
        LookAt(p.m, Vec3Zero.m, Vec3Up.m, newView.m);
    }
    g_Renderer->SetMatrix(MD_VIEW, newView.m);

    Matrix oldProj;
    g_Renderer->GetMatrix(&oldProj.m, MD_PROJECTION);
    Matrix newProj;
    Perspective(37.5f,1,0.1f,500.0f,newProj.m);
    g_Renderer->SetMatrix(MD_PROJECTION, newProj.m);

    g_Renderer->DrawLine(VectorZero, VectorUp, VectorUp);
    g_Renderer->DrawLine(VectorZero, VectorRight, VectorRight);
    g_Renderer->DrawLine(VectorZero, VectorForward, VectorForward);

    g_Renderer->SetMatrix(MD_WORLD, oldWorld.m);
    g_Renderer->SetMatrix(MD_VIEW, oldView.m);
    g_Renderer->SetMatrix(MD_PROJECTION, oldProj.m);
    g_Renderer->SetViewport(oldVP[0], oldVP[1], oldVP[2], oldVP[3]);

    atgCamera camera;
    camera.SetClipNear(10);
    camera.SetClipFar(300);
    camera.SetPosition(Vec3(0,120,250));
    atgFrustum f;
    f.BuildFrustumPlanes(camera.GetView().m,camera.GetProj().m);
    f.DebugRender();

}

struct Point3
{
	float p[3];
	Point3(float n[3]){ p[0] = n[0]; p[1] = n[1]; p[2] = n[2]; }
};

void Game::DrawCurve()
{
    ATG_PROFILE("Game::DrawCurve");
	float curveData[][3] =
	{
	    {  0.0f,   0.f, -10.0f },
		{ 20.0f,  20.f, -10.0f },
		{ 20.0f,  20.f, -10.0f },
		{ 60.0f,   0.f, -10.0f }/*,
		{ 80.0f,  20.f, -10.0f },
		{100.0f, -20.f, -10.0f },
		{120.0f,   0.f, -10.0f },
		{140.0f,  20.f, -10.0f },*/
	};

	int dataSize = sizeof(curveData)/sizeof(curveData[0]);
	int step = 25;

	std::vector<Point3> pointSet;
	float curve[3];
	for (int ci = 0; ci < dataSize - 3; ++ci)
	{
		
		for (int i = 0; i < step; ++i)
		{
			InterpolationHermite(curveData[ci],
								 curveData[ci+3],
								 curveData[ci+2],
								 curveData[ci+1], 
								 float(i)/step,
								 curve);
            Point3 p(curve);
			pointSet.push_back(p);
		}
	}

	int count = pointSet.size();
	for (int i = 1; i < count; ++i)
	{
		g_Renderer->DrawLine(pointSet[i-1].p, pointSet[i].p, VectorUp);
	}
}
