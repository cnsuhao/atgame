#include <windows.h>		// Header File For Windows
#include <d3d9.h>           // Header File For D3D9
#include <d3dx9.h>          // Header File For D3D9 Helper

int width = 1024;
int height = 768;

HWND		hWnd = NULL;		// Holds Our Window Handle

IDirect3D9*             g_pD3D = NULL;              // D3D9 InterFace
IDirect3DDevice9*       g_pd3dDevice = NULL;        // D3D9 Devices InterFace
IDirect3DSwapChain9*    g_pd3dSwapChain = NULL;     // Devices main SwapCahin
IDirect3DSurface9*      g_pPrimarySurface = NULL;
IDirect3DVertexBuffer9* g_pVB = NULL;               // D3D9 Vertex Buffer

struct CUSTOMVERTEX
{
    FLOAT x, y, z;      // Position
    DWORD color;        // The vertex color.
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

void DestroyScene();

BOOL InitScene(){
    CUSTOMVERTEX vertices[] =
    {
        { -25.0f, -25.0f, 40.0f, 0xffff0000, }, // x, y, z, color
        {   0.0f,  25.0f, 40.0f, 0xff00ff00, },
        {  25.0f, -25.0f, 40.0f, 0xff00ffff, },
    };

    if(FAILED(g_pd3dDevice->CreateVertexBuffer( 3*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX, 
                                                  D3DPOOL_DEFAULT, 
                                                  &g_pVB, NULL))){
        return FALSE;
    }

    void* pVertices = NULL;
    if(FAILED(g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0))){
        return FALSE;
    }

    memcpy(pVertices, vertices, sizeof(vertices));

    g_pVB->Unlock();

    //Set matrix for world , view , projection 
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0, 0, -50), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
    g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIX matProjection;
    D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI/4.0f, 1.3333f, 0, 1000);
    if(FAILED(g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjection))){
        return FALSE;
    }
    
    D3DVIEWPORT9 viewPort;
    viewPort.X = viewPort.Y = 0;
    viewPort.Width = width;
    viewPort.Height = height;
    viewPort.MinZ = 0;
    viewPort.MaxZ = 1;
    g_pd3dDevice->SetViewport(&viewPort);

    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    return TRUE;
}

void DestroyScene(){
    return;
}

void DrawDirectx9Scene(){
    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);

    // Begin the scene
    g_pd3dDevice->BeginScene();

    // Rendering of scene objects happens here
    g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));

    g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    // End the scene
    g_pd3dDevice->EndScene();

    //g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    g_pd3dSwapChain->Present(NULL, NULL, NULL, NULL, NULL);
}

void DestortyDirectX();

BOOL InitDirectX(int bits){
    // Create DirectX 9 Device Interface.
    if(NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return FALSE;
    // 
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    if( FAILED( g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                     D3DCREATE_MIXED_VERTEXPROCESSING,
                                     &d3dpp, &g_pd3dDevice ) ) ){
        DestortyDirectX();
        return FALSE;
    }

    // 取得交换链
    if(FAILED(g_pd3dDevice->GetSwapChain(0, &g_pd3dSwapChain))){
        DestortyDirectX();
        return FALSE;
    }

    // 取得主表面
    if(FAILED(g_pd3dSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO,&g_pPrimarySurface))){
        DestortyDirectX();
        return FALSE;
    }

    //IDirect3DSurface9* pCurRT = NULL;
    //g_pd3dDevice->GetRenderTarget(0, &pCurRT);

    // 设置渲染目标,实际上就是 g_pPrimarySurface.(因为pCurRT == g_pPrimarySurface);
    g_pd3dDevice->SetRenderTarget(0, g_pPrimarySurface);

    InitScene();

    return TRUE;
}

void DestortyDirectX(){
    if(g_pd3dDevice != NULL){
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }
    if(g_pD3D != NULL){
        g_pD3D->Release();
        g_pD3D = NULL;
    }
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("DirectX Test");
    MSG          msg;
    WNDCLASSEX   wndclassex = {0};
    wndclassex.cbSize        = sizeof(WNDCLASSEX);
    wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc   = WndProc;
    wndclassex.cbClsExtra    = 0;
    wndclassex.cbWndExtra    = 0;
    wndclassex.hInstance     = hInstance;
    wndclassex.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclassex.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclassex.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclassex.lpszMenuName  = NULL;
    wndclassex.lpszClassName = szAppName;
    wndclassex.hIconSm       = wndclassex.hIcon;

    if (!RegisterClassEx (&wndclassex))
    {
        MessageBox (NULL, TEXT ("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
        return 0;
    }

    hWnd = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, 
        szAppName, 
        TEXT ("DirectX"),
        WS_OVERLAPPEDWINDOW,
        50, 
        50, 
        width, 
        height,
        NULL, 
        NULL, 
        hInstance,
        NULL);

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    InitDirectX(32);

    ZeroMemory (&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else{
            DrawDirectx9Scene();
        }
    }

    return msg.wParam;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
        {
            // center the window
            RECT rect;
            GetWindowRect(hwnd, &rect);

            int nWndWidth = rect.right - rect.left;
            int nWndHeight = rect.bottom - rect.top;

            int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
            int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

            rect.left = (nScreenWidth - nWndWidth) / 2;
            rect.top = (nScreenHeight - nWndHeight) / 2;

            SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_UP:
                break;
            case VK_DOWN:

                break;
            case VK_LEFT:

                break;
            case VK_RIGHT:
                break;
            case VK_ESCAPE:
                PostQuitMessage(0);
                break;
            }
        }
        break;
    case WM_KEYUP:
        {
            switch(wParam)
            {
            case VK_SNAPSHOT:
                //theApp.saveScreen();
                break;
            }
        }
        break;
    case WM_SYSKEYDOWN:
        {

        }
        break;
    case WM_SYSKEYUP:
        {

        }
        break;
    case WM_MOUSEMOVE:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);

            return 0;
        }
        break;
    case WM_LBUTTONDOWN:
        {

        }
        break;
    case WM_LBUTTONUP:
        {

        }
        break;
    case WM_RBUTTONDOWN:
        {

        }
        break;
    case WM_RBUTTONUP:
        {

        }
        break;
    case WM_MBUTTONDOWN:
        {

        }
        break;
    case WM_MBUTTONUP:
        {

        }
        break;
    case WM_MOUSEWHEEL:
        {
        }
        break;
    case WM_CHAR:
        break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}