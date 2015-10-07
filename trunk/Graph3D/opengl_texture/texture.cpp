#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include <stdio.h>
#include <stdlib.h>

int width = 1024;
int height = 768;

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle

GLuint		texture[2];

AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
    FILE *File=NULL;									// File Handle
    if (!Filename)										// Make Sure A Filename Was Given
    {
        return NULL;									// If Not Return NULL
    }
    File=fopen(Filename,"r");							// Check To See If The File Exists
    if (File)											// Does The File Exist?
    {
        fclose(File);									// Close The Handle
        return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
    }
    return NULL;										// If Load Failed Return NULL
}

void DestroyScene();

BOOL InitScene(){
    AUX_RGBImageRec* TextureImage[2];					// Create Storage Space For The Texture
    memset(TextureImage,0,sizeof(AUX_RGBImageRec *) * 2);           	// Set The Pointer To NULL

    char Pictures[2][32] = {"MeiXi.bmp" , "NeHe.bmp" };
    // Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
    for(int i = 0; i < 2; i++){
        if (TextureImage[i]=LoadBMP(Pictures[i]))
        {
            glGenTextures(1, &texture[i]);					// Create The Texture


            // Typical Texture Generation Using Data From The Bitmap
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX, TextureImage[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[i]->data);
        }
    }

    for(int i = 0; i < 2; i++){
        if (TextureImage[i])									// If Texture Exists
        {
            if (TextureImage[i]->data)							// If Texture Image Exists
            {
                free(TextureImage[i]->data);					// Free The Texture Image Memory
            }
            free(TextureImage[i]);								// Free The Image Structure
        }
    }

    return TRUE;
}

void DrawOpenGLScene(){
    glClearColor(0.0f, 0.0f, 1.0f, 0.5f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
    glMatrixMode(GL_MODELVIEW);	
    glLoadIdentity();									// Reset The Current Modelview Matrix

    glTranslatef(0.0f,0.0f,-5.0f);

    static GLfloat RotY = 0.0f;
    glRotatef(RotY,0.0f,1.0f,0.0f);
    RotY += 0.01f;
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glBegin(GL_TRIANGLES);							// 绘制三角形
    glTexCoord2f(0.5f, 1.0f);
    //glColor3f(1.0f,0.0f,0.0f);
    glVertex3f( 0.0f, 1.0f, 0.0f);					// 上顶点
    glTexCoord2f(0.0f, 0.0f); 
    //glColor3f(1.0f,1.0f,1.0f);
    glVertex3f(-1.0f,-1.0f, 0.0f);					// 左下
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f( 1.0f,-1.0f, 0.0f);					// 右下
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glLoadIdentity();
    glTranslatef(0.0f,0.0f,-5.0f);

    static GLfloat RotX = 0.0f;
    glRotatef(RotX,1.0f,0.0f,0.0f);
    RotX += 0.01f;

    glBegin(GL_TRIANGLES);							// 绘制三角形
    glTexCoord2f(0.5f, 1.0f);
    //glColor3f(1.0f,1.0f,0.0f);
    glVertex3f( 0.0f, 1.0f, 0.0f);					// 上顶点
    glTexCoord2f(0.0f, 0.0f);
    //glColor3f(1.0f,1.0f,1.0f);
    glVertex3f(-1.0f,-1.0f, 0.0f);					// 左下
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f,-1.0f, 0.0f);					// 右下
    glEnd();

    SwapBuffers(hDC);
}

void DestroyScene(){

}

void DestortyOpenGL();

BOOL InitOpenGL(int width, int height, int bits){

    static PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
        1,											// Version Number
        PFD_DRAW_TO_WINDOW |						// Format Must Support Window
        PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
        PFD_DOUBLEBUFFER,							// Must Support Double Buffering
        PFD_TYPE_RGBA,								// Request An RGBA Format
        bits,										// Select Our Color Depth
        0, 0, 0, 0, 0, 0,							// Color Bits Ignored
        0,											// No Alpha Buffer
        0,											// Shift Bit Ignored
        0,											// No Accumulation Buffer
        0, 0, 0, 0,									// Accumulation Bits Ignored
        16,											// 16Bit Z-Buffer (Depth Buffer)  
        0,											// No Stencil Buffer
        0,											// No Auxiliary Buffer
        PFD_MAIN_PLANE,								// Main Drawing Layer
        0,											// Reserved
        0, 0, 0										// Layer Masks Ignored
    };

    if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
    {
        MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    GLuint		PixelFormat;
    if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
    {
        // Reset The Display
        MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);

        return FALSE;								// Return FALSE
    }

    if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
    {
        DestortyOpenGL();
        MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
    {	
        DestortyOpenGL();
        MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
    {
        DestortyOpenGL();
        MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;								// Return FALSE
    }

    glViewport(0,0,width,height);						// Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();									// Reset The Modelview Matrix

    glEnable(GL_TEXTURE_2D);	
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations


    InitScene();

    return TRUE;
}

void DestortyOpenGL(){
    if(hRC){
        wglMakeCurrent(NULL,NULL);
        wglDeleteContext(hRC);
    }
    if(hDC){
        ReleaseDC(hWnd, hDC);   
    }
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("OpenGL Test");
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
        TEXT ("OpenGL"),
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

    InitOpenGL(width, height, 32);

    ZeroMemory (&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else{
            DrawOpenGLScene();
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