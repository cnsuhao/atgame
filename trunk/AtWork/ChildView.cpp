
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "AtWork.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "atgShaderLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildView

CChildView::CChildView()
    : m_LbuttonDown(false)
    , m_lastPoint(0)
{
    m_RandererInit = false;
    m_LbuttonDown = false;

    MatIdentity(m_RotMat.m);

    m_VB = NULL;
    m_PassColor = NULL;
    m_PassTexture = NULL;
    m_Texture = NULL;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
    // 用于绘制的设备上下文
	//CPaintDC dc(this); 
	// 不要为绘制消息而调用  
    CWnd::OnPaint();
}

void CChildView::OnDestroy()
{
    CWnd::OnDestroy();

    ResetResource();

    // TODO: 在此处添加消息处理程序代码
    g_Renderer->Shutdown();
    SAFE_DELETE(g_Renderer);
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
    static bool first = true; 
    CWnd::OnSize(nType, cx, cy);
    // TODO: 在此处添加消息处理程序代码
    if(!g_MainFrame->IsEnterSizeMoved() && cx > 0 && cy > 0)
    {
        if (!first)
        {
            //ResetResource();
            if (g_Renderer)
                g_Renderer->Resize(cx, cy);       
            //InitResource();
        }
        if (g_Renderer)
            g_Renderer->SetViewport(0, 0, cx, cy);
        first = false;
    }
}

void CChildView::InitResource()
{
    // 做一些初始化工作在这里....
    static const float vertexBufferData[] = 
    {
        //-1.0f, -1.0f, -0.00f,
        //0.9999f,  0.9999f, -0.099f,
        //.9f, -.9f, -0.09f,
        1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 1.0f
    };

    atgVertexDecl decl;
    decl.AppendElement(0, atgVertexDecl::VA_Pos3);
    //decl.AppendElement(0, atgVertexDeclaration::VA_Diffuse);
    decl.AppendElement(0, atgVertexDecl::VA_Texture0);
    m_VB = new atgVertexBuffer();
    m_VB->Create(&decl, (void*)vertexBufferData, sizeof(vertexBufferData), BAM_Static);

    //m_PassColor = atgShaderLibFactory::FindOrCreatePass(SOLIDCOLOR_PASS_IDENTITY);
    m_PassTexture = atgShaderLibFactory::FindOrCreatePass(NOT_LIGNTE_TEXTURE_PASS_IDENTITY);

    //TGA_Image image;
    //TGA_Loader::Load(&image, "model/golemstatue.tga", false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);

    //m_Texture = atgTexture::Create(image.width, image.height, image.bpp, image.imageData);
    //TGA_Loader::Release(&image);

	PNG_Image image;
	PNG_Loader::Load(&image, "ui/ui1.png", false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);

	m_Texture = atgTexture::Create(image.width, image.height, image.bpp, image.imageData);
	PNG_Loader::Release(&image);

	//JPEG_Image image;
	//JPEG_Loader::Load(&image, "ui/ui1.jpg", false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);

	//m_Texture = atgTexture::Create(image.width, image.height, image.bpp, image.imageData);
	//JPEG_Loader::Release(&image);

}

void CChildView::ResetResource()
{
    SAFE_DELETE(m_VB);
    //SAFE_DELETE(m_PassColor);
    SAFE_DELETE(m_PassTexture)
    SAFE_DELETE(m_Camera);
}

void CChildView::Render()
{
    // TODO: 在此处添加消息处理程序代码
    if(!m_RandererInit)
    {
        // 设置
        g_hWnd = m_hWnd;
        RECT rect;
        GetClientRect(&rect);
        g_Renderer = new atgRenderer();
        g_Renderer->Initialize(rect.right-rect.left, rect.bottom-rect.top, 32);
        g_Renderer->SetViewport(0,0,rect.right-rect.left, rect.bottom-rect.top);
        g_Renderer->SetFaceCull(FCM_NONE);
        InitResource();

        m_Camera = new atgCamera();
		m_Camera->SetPosition(Vec3(0, 1.0f, 1.0f));
        g_Renderer->SetMatrix(MD_VIEW, m_Camera->GetView().m);
        m_RandererInit = true;
    }

    g_Renderer->SetMatrix(MD_WORLD, MatrixIdentity.m);
    if (m_Camera)
    {
        g_Renderer->SetMatrix(MD_VIEW, m_Camera->GetView().m);
        g_Renderer->SetMatrix(MD_PROJECTION, m_Camera->GetProj().m);
    }

    g_Renderer->Clear();
    g_Renderer->BeginFrame();

    g_Renderer->BindPass(m_PassTexture);
    g_Renderer->BindVertexBuffer(m_VB);

    g_Renderer->BindTexture(0, m_Texture);

    g_Renderer->SetAlphaTestEnable(true, 0.05f);

    g_Renderer->DrawPrimitive(PT_TRIANGLES, 1, 3);

    g_Renderer->SetAlphaTestEnable(false);

    //m_Texture->Unbind();

    //m_VB->Unbind();

    RenderAxis();

    g_Renderer->BindPass(NULL);

    g_Renderer->EndFrame();
    g_Renderer->Present();
}

void CChildView::RenderAxis()
{
    RECT rect;
    GetClientRect(&rect);
    uint32 oldVP[4];
    g_Renderer->GetViewPort(oldVP[0], oldVP[1], oldVP[2], oldVP[3]);

    uint32 newVP[4] = {0, IsOpenGLGraph() ? 0 : oldVP[3] - 100, 100, 100 };
    g_Renderer->SetViewport(newVP[0], newVP[1], newVP[2], newVP[3]);


    Matrix world(MatrixIdentity);
    static float ang = 0.0f;
    if (ang < -359.0f)
    {
        ang = 0.0f;
    }
    ang += 1.0f;
    world.RotationZXY(30.0f, 15.0f, ang);
    g_Renderer->SetMatrix(MD_WORLD, world.m);

    Matrix oldView(MatrixIdentity);
    g_Renderer->GetMatrix(&oldView.m, MD_VIEW);

    Matrix newView(MatrixIdentity);
    Vec3 p(0.0f, 0.0f, 3.5f);
    LookAt(p.m, Vec3Zero.m, Vec3Up.m, newView.m);
    g_Renderer->SetMatrix(MD_VIEW, newView.m);

    Matrix newProj;
    Perspective(37.5f,1,0.1f,500.0f,newProj.m);
    //OrthoProject((rect.right - rect.left), (rect.bottom - rect.top), 0.1f, 500.0f, newProj.m);
    g_Renderer->SetMatrix(MD_PROJECTION, newProj.m);

    g_Renderer->DrawLine(VectorZero, VectorUp, VectorUp);
    g_Renderer->DrawLine(VectorZero, VectorRight, VectorRight);
    g_Renderer->DrawLine(VectorZero, VectorForward, VectorForward);

    //g_Renderer->DrawLine(VectorZero, (Vec3Up * 100.f).m, VectorUp);
    //g_Renderer->DrawLine(VectorZero, (Vec3Right * 100.f).m, VectorRight);
    //g_Renderer->DrawLine(VectorZero, (Vec3Forward * 100.f).m, VectorForward);

    g_Renderer->SetViewport(oldVP[0], oldVP[1], oldVP[2], oldVP[3]);
}



void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CWnd::OnLButtonDown(nFlags, point);
    m_LbuttonDown = true;
    m_lastPoint = point;
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CWnd::OnLButtonUp(nFlags, point);
    m_LbuttonDown = false;
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CWnd::OnMouseMove(nFlags, point);


    static LONG last_x = point.x;
    static LONG last_y = point.y;

    static float oYaw = DegreesToRadians(180.f);
    static float oPitch = DegreesToRadians(45.f);

    if (m_LbuttonDown)
    {
        float dx = static_cast<float>(point.x - last_x);
        float dy = static_cast<float>(point.y - last_y);

        if(m_Camera)
        {
            oYaw -= dx * 0.001f;
            oPitch += dy * 0.001f;
            m_Camera->SetYaw(oYaw);
            m_Camera->SetPitch(oPitch);
        }
    }
    last_x = point.x;
    last_y = point.y;
}

const Matrix& CChildView::GetRotMatrix()
{
    return m_RotMat;
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    BOOL rs = CWnd::OnMouseWheel(nFlags, zDelta, pt);

    LOG("nFlags=%d, zDelta=%d\n", nFlags, zDelta);

    float moveSpeed = 1.5f;
    short wheel = (short)zDelta / WHEEL_DELTA;
	if (wheel > 0)
    {
        Vec3 forward = m_Camera->GetForward();
        Vec3 pos = m_Camera->GetPosition();
        VecScale(forward.m, moveSpeed, forward.m);
        VecAdd(pos.m, forward.m, pos.m);
        m_Camera->SetPosition(pos.m);
    }
    else
    {
        Vec3 forward = m_Camera->GetForward();
        Vec3 pos = m_Camera->GetPosition();
        VecScale(forward.m, -moveSpeed, forward.m);
        VecAdd(pos.m, forward.m, pos.m);
        m_Camera->SetPosition(pos.m);
    }
    return rs;
}
