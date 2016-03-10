#include "atgBase.h"
#include "atgGame.h"
#include "atgRenderer.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    HWND        g_hWnd = NULL;
    HINSTANCE   g_hInstance = NULL;
    atgGame*    __g_game = NULL;

    #ifdef USE_OPENGL

        #ifndef USE_OPENGLES
            #include <gl/wglew.h>       // Header File For The OpenGL32 Library

            HDC         hDC = NULL;     // Private GDI Device Context
            HGLRC       hRC = NULL;     // Permanent Rendering Context
        #else
            static EGLDisplay __eglDisplay = EGL_NO_DISPLAY;
            static EGLContext __eglContext = EGL_NO_CONTEXT;
            static EGLSurface __eglSurface = EGL_NO_SURFACE;
            static EGLConfig __eglConfig = 0;

            static EGLenum checkErrorEGL(const char* msg)
            {
                LOG(msg);
                static const char* errmsg[] =
                {
                    "EGL function succeeded",
                    "EGL is not initialized, or could not be initialized, for the specified display",
                    "EGL cannot access a requested resource",
                    "EGL failed to allocate resources for the requested operation",
                    "EGL fail to access an unrecognized attribute or attribute value was passed in an attribute list",
                    "EGLConfig argument does not name a valid EGLConfig",
                    "EGLContext argument does not name a valid EGLContext",
                    "EGL current surface of the calling thread is no longer valid",
                    "EGLDisplay argument does not name a valid EGLDisplay",
                    "EGL arguments are inconsistent",
                    "EGLNativePixmapType argument does not refer to a valid native pixmap",
                    "EGLNativeWindowType argument does not refer to a valid native window",
                    "EGL one or more argument values are invalid",
                    "EGLSurface argument does not name a valid surface configured for rendering",
                    "EGL power management event has occurred",
                };
                EGLenum error = eglGetError();
                LOG("%s: %s.", msg, errmsg[error - EGL_SUCCESS]);
                return error;
            }

        #endif // !USE_OPENGLES

        void SetVSyncState(bool enable);
    #endif

    const int win32_default_width = 800;
    const int win32_default_height = 600;

    

    bool win32_init(void* initData, atgGame* game)
    {
        // Create Windows
        static TCHAR szAppName[] = TEXT ("AtGame!");
        static TCHAR szAppTitle[] = TEXT("<<AtGame Window>>");
        g_hInstance = ::GetModuleHandle(NULL);
        UINT windowStyle = 0;
        WNDCLASSEX   wndclassex = {0};
        wndclassex.cbSize        = sizeof(WNDCLASSEX);
        wndclassex.style         = windowStyle;
        wndclassex.lpfnWndProc   = atgGameWndProc;
        wndclassex.cbClsExtra    = 0;
        wndclassex.cbWndExtra    = 0;
        wndclassex.hInstance     = g_hInstance;
        wndclassex.hIcon         = LoadIcon (NULL, IDI_WINLOGO);
        wndclassex.hCursor       = LoadCursor (NULL, IDC_ARROW);
        wndclassex.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
        wndclassex.lpszMenuName  = NULL;
        wndclassex.lpszClassName = szAppName;
        wndclassex.hIconSm       = wndclassex.hIcon;

        if (!RegisterClassEx (&wndclassex))
        {
            MessageBox (NULL, TEXT ("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
            return false;
        }
        windowStyle = WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
        // Set the window's initial width
        RECT rc;
        SetRect( &rc, 0, 0, win32_default_width, win32_default_height);
        AdjustWindowRect( &rc, windowStyle, FALSE );

        g_hWnd = CreateWindowEx (0, szAppName, szAppTitle, windowStyle, 0, 0, 
            (rc.right - rc.left), (rc.bottom - rc.top), NULL, NULL, g_hInstance, NULL);

        ShowWindow(g_hWnd, SW_SHOWDEFAULT);
        UpdateWindow(g_hWnd);
        __g_game = game;
        return true;
    }

    void win32_shutdown(atgGame* game)
    {

    }

    void win32_main_loop(atgGame* game)
    {
        MSG msg;
        ZeroMemory (&msg, sizeof(msg));
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            else{
                game->OnFrame();
                //::Sleep(15);
            }
        }
    }

    bool win32_init_ogl()
    {
    #if defined (USE_OPENGL) 

        #ifndef USE_OPENGLES
            static PIXELFORMATDESCRIPTOR pfd =              // pfd Tells Windows How We Want Things To Be
            {
                sizeof(PIXELFORMATDESCRIPTOR),              // Size Of This Pixel Format Descriptor
                1,                                          // Version Number
                PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
                PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
                PFD_DOUBLEBUFFER,                           // Must Support Double Buffering
                PFD_TYPE_RGBA,                              // Request An RGBA Format
                32,                                         // Select Our Color Depth
                0, 0, 0, 0, 0, 0,                           // Color Bits Ignored
                0,                                          // No Alpha Buffer
                0,                                          // Shift Bit Ignored
                0,                                          // No Accumulation Buffer
                0, 0, 0, 0,                                 // Accumulation Bits Ignored
                24,                                         // 24Bit Z-Buffer (Depth Buffer)
                8,                                          // No Stencil Buffer
                0,                                          // No Auxiliary Buffer
                PFD_MAIN_PLANE,                             // Main Drawing Layer
                0,                                          // Reserved
                0, 0, 0                                     // Layer Masks Ignored
            };

            g_hInstance = ::GetModuleHandle(NULL);
            UINT windowStyle = 0;
            WNDCLASSEX   wndclassex = {0};
            wndclassex.cbSize        = sizeof(WNDCLASSEX);
            wndclassex.style         = windowStyle;
            wndclassex.lpfnWndProc   = DefWindowProc;
            wndclassex.hInstance     = g_hInstance;
            wndclassex.hIcon         = LoadIcon (NULL, IDI_WINLOGO);
            wndclassex.hCursor       = LoadCursor (NULL, IDC_ARROW);
            wndclassex.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
            wndclassex.lpszClassName = "TempWindow";
            wndclassex.hIconSm       = wndclassex.hIcon;
            HWND temphWnd = NULL;
            HDC temphDC = NULL;
            HGLRC temphRc = NULL;
            if (RegisterClassEx (&wndclassex))
            {
                temphWnd = CreateWindowEx (0, wndclassex.lpszClassName, wndclassex.lpszClassName, windowStyle, 0, 0, 
                    100, 100, NULL, NULL, g_hInstance, NULL);

                if (temphWnd)
                {
                    if (temphDC = GetDC(temphWnd))
                    {
                        int tempPixelFormat;
                        if ((tempPixelFormat=ChoosePixelFormat(temphDC, &pfd)))
                        {
                            if(SetPixelFormat(temphDC,tempPixelFormat,&pfd))
                            {
                                if ((temphRc=wglCreateContext(temphDC)))
                                {
                                    wglMakeCurrent(temphDC,temphRc);
                                }
                            }
                        }
                    }
                }
            }

            // Initialize GLEW
            if (GLEW_OK != glewInit())
            {
                wglMakeCurrent(NULL, NULL);
                wglDeleteContext(hRC);
                MessageBox(NULL,"Failed to initialize GLEW.","ERROR",MB_OK|MB_ICONEXCLAMATION);
                DestroyWindow(g_hWnd);
                return false;
            }

            // 销毁临时窗口
            wglMakeCurrent(NULL, NULL);

            if (temphRc)
                wglDeleteContext(temphRc);
            if(temphWnd)
                DestroyWindow(temphWnd);
            UnregisterClass(wndclassex.lpszClassName, g_hInstance);

            // 设置主窗口
            if (!(hDC=GetDC(g_hWnd)))                           // Did We Get A Device Context?
            {
                MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
                DestroyWindow(g_hWnd);
                return false;                               // Return FALSE
            }

            bool multisampling = true;
            int PixelFormat;
            if (multisampling && wglChoosePixelFormatARB /*&& wglCreateContextAttribsARB*/)
            {
                // MSAA PixelFormat
                int attributes[] =
                {
                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                    WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
                    WGL_COLOR_BITS_ARB,     32,
                    WGL_ALPHA_BITS_ARB,     8,
                    WGL_DEPTH_BITS_ARB,     24,
                    WGL_STENCIL_BITS_ARB,   8,
                    WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
                    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
                    WGL_SAMPLES_ARB,        0,
                    0, 0
                };

                UINT numFormats = 0;
                BOOL bStatus = FALSE;

                for (int samples = 16; samples > 0; samples /= 2)
                {
                    attributes[17] = samples;

                    bStatus = wglChoosePixelFormatARB(hDC, attributes, 0, 1,
                        &PixelFormat, &numFormats);

                    if (bStatus == TRUE && numFormats)
                    {
                        break;
                        /* 不指定环境版本
                        int attribs[] =
                        {
                            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                            WGL_CONTEXT_MINOR_VERSION_ARB, 1,
                            WGL_CONTEXT_FLAGS_ARB, 0,
                            0
                        };

                        if (!(hRC=wglCreateContextAttribsARB(hDC, 0, attribs)))               // Are We Able To Get A Rendering Context?
                        {
                            MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
                            DestroyWindow(g_hWnd);
                            return false;                               // Return FALSE
                        }

                        glEnable(GL_MULTISAMPLE_ARB);
                        */
                    }
                }
                if (!(bStatus == TRUE && numFormats))
                {
                    MessageBox(NULL,"Can't Find A Suitable wglChoosePixelFormatARB.","ERROR",MB_OK|MB_ICONEXCLAMATION);
                    DestroyWindow(g_hWnd);
                    return false;
                }
                /*
                if (wglChoosePixelFormatARB)
                {
                    struct CSAAPixelFormat
                    {
                        int numColorSamples;
                        int numCoverageSamples;
                        const char *pszDescription;
                    };

                    CSAAPixelFormat csaaPixelFormats[] =
                    {
                        { 4, 16, "16x CSAA" },
                        { 4, 8,  "8x CSAA" }
                    };

                    CSAAPixelFormat csaaQualityPixelFormats[] =
                    {
                        { 8, 16, "16xQ (Quality) CSAA" },
                        { 8, 8,  "8xQ (Quality) CSAA" }
                    };

                    CSAAPixelFormat *pCSAAFormats = 0;

                    int attributes[] =
                    {
                        WGL_SAMPLE_BUFFERS_ARB,  1,
                        WGL_COLOR_SAMPLES_NV,    0,
                        WGL_COVERAGE_SAMPLES_NV, 0,
                        WGL_DOUBLE_BUFFER_ARB,   1,
                        0, 0
                    };

                    int returnedPixelFormat = 0;
                    UINT numFormats = 0;
                    BOOL bStatus = FALSE;

                    int samples = 8;
                    if (samples >= 8)
                        pCSAAFormats = csaaQualityPixelFormats;
                    else
                        pCSAAFormats = csaaPixelFormats;

                    for (int i = 0; i < 2; ++i)
                    {
                        attributes[3] = pCSAAFormats[i].numColorSamples;
                        attributes[5] = pCSAAFormats[i].numCoverageSamples;

                        bStatus = wglChoosePixelFormatARB(hDC, attributes, 0, 1,
                            &returnedPixelFormat, &numFormats);

                        if (bStatus == TRUE && numFormats)
                        {
                            break;
                        }
                    }
                }
                */
            }
            else
            {
            
                if (!(PixelFormat=ChoosePixelFormat(hDC, &pfd)))    // Did Windows Find A Matching Pixel Format?
                {
                    MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
                    DestroyWindow(g_hWnd);
                    return false;
                }
            }
      
            if(!SetPixelFormat(hDC,PixelFormat,&pfd))       // Are We Able To Set The Pixel Format?
            {
                MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
                DestroyWindow(g_hWnd);
                return false;
            }

            if (!(hRC=wglCreateContext(hDC)))               // Are We Able To Get A Rendering Context?
            {
                MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
                DestroyWindow(g_hWnd);
                return false;                               // Return FALSE
            }

            if(!wglMakeCurrent(hDC,hRC))                    // Try To Activate The Rendering Context
            {
                wglDeleteContext(hRC);
                MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
                DestroyWindow(g_hWnd);
                return false;                               // Return FALSE
            }

            return true;
        #else
            int samples = 8;
            // Hard-coded to 32-bit/OpenGL ES 2.0.
            // NOTE: EGL_SAMPLE_BUFFERS, EGL_SAMPLES and EGL_DEPTH_SIZE MUST remain at the beginning of the attribute list
            // since they are expected to be at indices 0-5 in config fallback code later.
            // EGL_DEPTH_SIZE is also expected to
            EGLint eglConfigAttrs[] =
            {
                EGL_SAMPLE_BUFFERS,     samples > 0 ? 1 : 0,
                EGL_SAMPLES,            samples,
                EGL_DEPTH_SIZE,         24,
                EGL_RED_SIZE,           8,
                EGL_GREEN_SIZE,         8,
                EGL_BLUE_SIZE,          8,
                EGL_ALPHA_SIZE,         8,
                EGL_STENCIL_SIZE,       8,
                EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
                EGL_NONE
            };

            EGLint eglConfigCount;
            const EGLint eglContextAttrs[] =
            {
                EGL_CONTEXT_CLIENT_VERSION,    2,
                EGL_NONE
            };

            const EGLint eglSurfaceAttrs[] =
            {
                EGL_RENDER_BUFFER,    EGL_BACK_BUFFER,
                EGL_NONE
            };

            if (__eglDisplay == EGL_NO_DISPLAY && __eglContext == EGL_NO_CONTEXT)
            {
                // Get the EGL display and initialize.
                __eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                if (__eglDisplay == EGL_NO_DISPLAY)
                {
                    checkErrorEGL("eglGetDisplay");
                    return false;
                }

                if (eglInitialize(__eglDisplay, NULL, NULL) != EGL_TRUE)
                {
                    checkErrorEGL("eglInitialize");
                    return false;
                }

                // Try both 24 and 16-bit depth sizes since some hardware (i.e. Tegra) does not support 24-bit depth
                bool validConfig = false;
                EGLint depthSizes[] = { 24, 16 };
                for (unsigned int i = 0; i < 2; ++i)
                {
                    eglConfigAttrs[1] = samples > 0 ? 1 : 0;
                    eglConfigAttrs[3] = samples;
                    eglConfigAttrs[5] = depthSizes[i];

                    if (eglChooseConfig(__eglDisplay, eglConfigAttrs, &__eglConfig, 1, &eglConfigCount) == EGL_TRUE && eglConfigCount > 0)
                    {
                        validConfig = true;
                        break;
                    }

                    if (samples)
                    {
                        // Try lowering the MSAA sample size until we find a supported config
                        int sampleCount = samples;
                        while (sampleCount)
                        {
                            LOG("No EGL config found for depth_size=%d and samples=%d. Trying samples=%d instead.\n", depthSizes[i], sampleCount, sampleCount / 2);
                            sampleCount /= 2;
                            eglConfigAttrs[1] = sampleCount > 0 ? 1 : 0;
                            eglConfigAttrs[3] = sampleCount;
                            if (eglChooseConfig(__eglDisplay, eglConfigAttrs, &__eglConfig, 1, &eglConfigCount) == EGL_TRUE && eglConfigCount > 0)
                            {
                                validConfig = true;
                                break;
                            }
                        }

                        if (validConfig)
                            break;
                    }
                    else
                    {
                        LOG("No EGL config found for depth_size=%d.\n", depthSizes[i]);
                    }
                }

                if (!validConfig)
                {
                    checkErrorEGL("eglChooseConfig");
                    return false;
                }

                LOG(" EGL_SAMPLE_BUFFERS=%d\n EGL_SAMPLES=%d\n EGL_DEPTH_SIZE=%d\n EGL_RED_SIZE=%d\n EGL_GREEN_SIZE=%d\n EGL_BLUE_SIZE=%d\n EGL_ALPHA_SIZE=%d\n EGL_STENCIL_SIZE=%d\n EGL_SURFACE_TYPE=%d\n EGL_RENDERABLE_TYPE=%d.\n", 
                    eglConfigAttrs[1],eglConfigAttrs[3], eglConfigAttrs[5], eglConfigAttrs[7], eglConfigAttrs[9],eglConfigAttrs[11],
                    eglConfigAttrs[13],eglConfigAttrs[15],eglConfigAttrs[17],eglConfigAttrs[19]);

                __eglContext = eglCreateContext(__eglDisplay, __eglConfig, EGL_NO_CONTEXT, eglContextAttrs);
                if (__eglContext == EGL_NO_CONTEXT)
                {
                    checkErrorEGL("eglCreateContext");
                    return false;
                }
            }

            // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
            // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
            // As soon as we picked a EGLConfig, we can safely reconfigure the
            // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.

            EGLint format;
            eglGetConfigAttrib(__eglDisplay, __eglConfig, EGL_NATIVE_VISUAL_ID, &format);

            __eglSurface = eglCreateWindowSurface(__eglDisplay, __eglConfig, g_hWnd, 0);
            if (__eglSurface == EGL_NO_SURFACE)
            {
                checkErrorEGL("eglCreateWindowSurface");
                return false;
            }

            if (eglMakeCurrent(__eglDisplay, __eglSurface, __eglSurface, __eglContext) != EGL_TRUE)
            {
                checkErrorEGL("eglMakeCurrent");
                return false;
            }

            //eglQuerySurface(__eglDisplay, __eglSurface, EGL_WIDTH, &__width);
            //eglQuerySurface(__eglDisplay, __eglSurface, EGL_HEIGHT, &__height);
            LOG("atgRenderer::Initialize end!\n");

            return true;

        #endif // !USE_OPENGLES

        
    #endif
        return false;
    }

#if defined (USE_OPENGL) 
    // 开启和关闭垂直同步
    void __atgSetVSyncState(bool enable)
    {

    #ifndef USE_OPENGLES
            if (enable)
                wglSwapIntervalEXT(1);
            else 
                wglSwapIntervalEXT(0);  
    #else
            if (__eglDisplay)
            {
                if (enable)
                    eglSwapInterval(__eglDisplay, 1);
                else
                    eglSwapInterval(__eglDisplay, 0);
            }
    #endif // !USE_OPENGLES
    }
#endif

    void win32_shutdown_ogl()
    {
    #if defined (USE_OPENGL) 
        #ifndef USE_OPENGLES
            if(hRC)
            {
                wglMakeCurrent(NULL,NULL);
                wglDeleteContext(hRC);
                hRC = NULL;
            }
            if(hDC){
                ReleaseDC(g_hWnd, hDC);
                hDC = NULL;
            }
        #else
            if (__eglDisplay)
            {
                if (__eglSurface)
                {
                    eglDestroySurface(__eglDisplay, __eglSurface);
                    __eglSurface = EGL_NO_SURFACE;
                }

                if (__eglContext)
                {
                    eglDestroyContext(__eglDisplay, __eglContext);
                    __eglContext = EGL_NO_CONTEXT;
                }
                
                eglTerminate(__eglDisplay);
                __eglDisplay = EGL_NO_DISPLAY;
            }
        #endif // !USE_OPENGLES
    #endif
    }

    int win32_ogl_present()
    {
    #if defined (USE_OPENGL) 
        
        glFlush();

        #ifndef USE_OPENGLES
            return SwapBuffers(hDC);
        #else
            return eglSwapBuffers(__eglDisplay, __eglSurface) ? 1 : 0;
        #endif // !USE_OPENGLES
    #endif
        return 0;
    }
#endif // _WIN32

#if defined(_ANDROID)
    #include <EGL/egl.h>
    #include <GLES2/gl2.h>

    #include <android_native_app_glue.h>
    #include <android/sensor.h>
    #include <android/asset_manager.h>
    #include <jni.h>
    //#include <unistd.h>

    struct saved_state {
        float angle;
        int32_t x;
        int32_t y;
    };

    struct android_app* __state = NULL;
    struct saved_state __saved_state;

    static ASensorManager* __sensorManager = NULL;
    static ASensorEventQueue* __sensorEventQueue = NULL;
    static ASensorEvent __sensorEvent;
    static const ASensor* __accelerometerSensor = NULL;
    static const ASensor* __gyroscopeSensor = NULL;
    AAssetManager* __assetManager;

    static EGLDisplay __eglDisplay = EGL_NO_DISPLAY;
    static EGLContext __eglContext = EGL_NO_CONTEXT;
    static EGLSurface __eglSurface = EGL_NO_SURFACE;
    static EGLConfig __eglConfig = 0;

    static int __initialized = 0;
    static int __animating = 0;
    static int __width = 0;
    static int __height = 0;
    static int __state_x = 0;
    static int __state_y = 0;
    static int __game_initialized =  0;

    std::string __assetsPath;

    bool android_init_ogl();
    void android_shutdown_ogl();
    bool android_ogl_present();

    static EGLenum checkErrorEGL(const char* msg)
    {
        GL_ASSERT(msg);
        static const char* errmsg[] =
        {
            "EGL function succeeded",
            "EGL is not initialized, or could not be initialized, for the specified display",
            "EGL cannot access a requested resource",
            "EGL failed to allocate resources for the requested operation",
            "EGL fail to access an unrecognized attribute or attribute value was passed in an attribute list",
            "EGLConfig argument does not name a valid EGLConfig",
            "EGLContext argument does not name a valid EGLContext",
            "EGL current surface of the calling thread is no longer valid",
            "EGLDisplay argument does not name a valid EGLDisplay",
            "EGL arguments are inconsistent",
            "EGLNativePixmapType argument does not refer to a valid native pixmap",
            "EGLNativeWindowType argument does not refer to a valid native window",
            "EGL one or more argument values are invalid",
            "EGLSurface argument does not name a valid surface configured for rendering",
            "EGL power management event has occurred",
        };
        EGLenum error = eglGetError();
        LOG("%s: %s.", msg, errmsg[error - EGL_SUCCESS]);
        return error;
    }


    /**
     * Just the current frame in the display.
     */
    static void android_draw_frame(atgGame* game) {
        if (__eglDisplay == NULL) {
            LOG("No display.");
            return;
        }

        // Just fill the screen with a color.
        glClearColor(((float)__state_x)/__width, 0,
                ((float)__state_y)/__height, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    static void android_term_surface(atgGame* game)
    {
        if (__eglDisplay != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(__eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }

        if (__eglSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(__eglDisplay, __eglSurface);
            __eglSurface = EGL_NO_SURFACE;
        }
    }

    static void android_term_display(atgGame* game)
    {
        LOG("################################################################################android_term_display");
        android_term_surface(game);

        if (__eglContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(__eglDisplay, __eglContext);
            __eglContext = EGL_NO_CONTEXT;
        }

        if (__eglDisplay != EGL_NO_DISPLAY)
        {
            eglTerminate(__eglDisplay);
            __eglDisplay = EGL_NO_DISPLAY;
        }
    }

    /**
     * Process the next input event.
     */
    static int32_t android_handle_input(struct android_app* app, AInputEvent* event) {
        atgGame* game = (atgGame*)app->userData;
        if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) 
        {
            int32_t action = AMotionEvent_getAction(event);
            size_t pointerIndex;
            size_t pointerId;
            size_t pointerCount;
            float pressure; // 手指压力大小 (0.0 - 1.0)

            switch (action & AMOTION_EVENT_ACTION_MASK)
            {
            case AMOTION_EVENT_ACTION_DOWN:
                {
                    pointerId = AMotionEvent_getPointerId(event, 0);
                    __state_x = AMotionEvent_getX(event, 0);
                    __state_y = AMotionEvent_getY(event, 0);
                    pressure = AMotionEvent_getPressure(event, 0);

                    //LOG("[ACTION_DOWN]===>pointerId=%d, (%d, %d, %f)", pointerId, __state_x, __state_y, pressure);
                    game->OnPointerDown(pointerId, __state_x, __state_y);
                    break;
                }
            case AMOTION_EVENT_ACTION_UP:
                {
                    pointerId = AMotionEvent_getPointerId(event, 0);
                    __state_x = AMotionEvent_getX(event, 0);
                    __state_y = AMotionEvent_getY(event, 0);
                    pressure = AMotionEvent_getPressure(event, 0);

                    //LOG("[ACTION_UP]===>pointerId=%d, (%d, %d, %f)", pointerId, __state_x, __state_y, pressure);
                    game->OnPointerUp(pointerId, __state_x, __state_y);
                    break;
                }
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                {
                    pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    pointerId = AMotionEvent_getPointerId(event, pointerIndex);
                    __state_x = AMotionEvent_getX(event, pointerId);
                    __state_y = AMotionEvent_getY(event, pointerId);
                    pressure = AMotionEvent_getPressure(event, pointerId);

                    //LOG("[POINTER_DOWN]===>pointerId=%d, (%d, %d, %f)", pointerId, __state_x, __state_y, pressure);
                    game->OnPointerDown(pointerId, __state_x, __state_y);
                    break;
                }
            case AMOTION_EVENT_ACTION_POINTER_UP:
                {
                    pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    pointerId = AMotionEvent_getPointerId(event, pointerIndex);
                    __state_x = AMotionEvent_getX(event, pointerId);
                    __state_y = AMotionEvent_getY(event, pointerId);
                    pressure = AMotionEvent_getPressure(event, pointerId);

                    //LOG("[POINTER_UP]===>pointerId=%d, (%d, %d, %f)", pointerId, __state_x, __state_y, pressure);
                    game->OnPointerUp(pointerId, __state_x, __state_y);
                    break;
                }
            case AMOTION_EVENT_ACTION_MOVE:
                {
                    pointerCount = AMotionEvent_getPointerCount(event);
                    for (size_t i = 0; i < pointerCount; ++i)
                    {
                        pointerId = AMotionEvent_getPointerId(event, i);
                        __state_x = AMotionEvent_getX(event, i);
                        __state_y = AMotionEvent_getY(event, i);
                        pressure = AMotionEvent_getPressure(event, i);

                        //LOG("[ACTION_MOVE]===>pointerId=%d, (%d, %d, %f)", pointerId, __state_x, __state_y, pressure);
                        game->OnPointerMove(pointerId, __state_x, __state_y);
                    }
                    break;
                }
            default:
                break;
            }
            return 1;
        }
        return 0;
    }

    // Process the next main command.
    static void android_handle_cmd(struct android_app* app, int32_t cmd)
    {
        atgGame* game = (atgGame*)app->userData;
        LOG("android_handle_cmd=%d", cmd);
        switch (cmd)
        {
        case APP_CMD_SAVE_STATE: // code : 12
            LOG("APP_CMD_SAVE_STATE");
            // The system has asked us to save our current state.  Do so.
            __state->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)__state->savedState) = __saved_state;
            __state->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW: // code : 1
            LOG("APP_CMD_INIT_WINDOW");
            // The window is being shown, get it ready.
            if (app->window != NULL)
            {  
                if(g_Renderer->Initialize(0, 0, 0))
                {
                    __initialized = true;
                    if (!__game_initialized)
                    {
                        if (game->OnInitialize())
                            __game_initialized = 1;
                        else
                            __initialized = false;
                    }
                }
            }
            break;
        case APP_CMD_TERM_WINDOW: // code : 2
            LOG("APP_CMD_TERM_WINDOW");
            android_term_surface(game);
            __initialized = false;
            break;
        case APP_CMD_CONFIG_CHANGED:
            LOG("APP_CMD_CONFIG_CHANGED");
            break;
        case APP_CMD_DESTROY: // code : 15
            LOG("APP_CMD_DESTROY");
            android_term_display(game);
            __initialized = false;
            break;
        case APP_CMD_GAINED_FOCUS: // code : 6
            LOG("APP_CMD_GAINED_FOCUS");
            // When our app gains focus, we start monitoring the sensors.
            if (__accelerometerSensor != NULL)
            {
                ASensorEventQueue_enableSensor(__sensorEventQueue, __accelerometerSensor);
                // We'd like to get 60 events per second (in microseconds).
                ASensorEventQueue_setEventRate(__sensorEventQueue, __accelerometerSensor, (1000L/60)*1000);
            }
            if (__gyroscopeSensor != NULL)
            {
                ASensorEventQueue_enableSensor(__sensorEventQueue, __gyroscopeSensor);
                // We'd like to get 60 events per second (in microseconds).
                ASensorEventQueue_setEventRate(__sensorEventQueue, __gyroscopeSensor, (1000L/60)*1000);
            }

            //if (Game::getInstance()->getState() == Game::UNINITIALIZED)
            //{
            //    Game::getInstance()->run();
            //}
            //else
            //{
            //    Game::getInstance()->resume();
            //}
            break;
        case APP_CMD_LOST_FOCUS: // code : 7
            LOG("APP_CMD_LOST_FOCUS");
            // When our app loses focus, we stop monitoring the sensors.
            // This is to avoid consuming battery while not being used.
            if (__accelerometerSensor != NULL)
            {
                ASensorEventQueue_disableSensor(__sensorEventQueue, __accelerometerSensor);
            }
            if (__gyroscopeSensor != NULL)
            {
                ASensorEventQueue_disableSensor(__sensorEventQueue, __gyroscopeSensor);
            }
            __animating = 0;
            break;
        case APP_CMD_RESUME: // code : 11
            LOG("APP_CMD_RESUME");
            if (__initialized && game)
            {
                game->Resume();
            }
            __animating = true;
            break;
        case APP_CMD_PAUSE: // code : 13
            LOG("APP_CMD_PAUSE");
            if (game)
            {
                game->Pause();
            }
            __animating = false;
            break;
        case APP_CMD_START: // code : 10
            LOG("APP_CMD_START");
            {
                break;
            }
        case APP_CMD_STOP : // code : 14
            LOG("APP_CMD_STOP");
            {
                break;
            }
        }
    }

    extern bool android_init(struct android_app* state, atgGame* game)
    {
        // Set the event call back functions.
        __state = state;
        __state->userData = game;
        __state->onAppCmd = android_handle_cmd;
        __state->onInputEvent = android_handle_input;
        if (state->savedState != NULL){
            __saved_state = *(struct saved_state*)state->savedState;
        }

        // Prepare to monitor accelerometer.
        __sensorManager = ASensorManager_getInstance();
        __accelerometerSensor = ASensorManager_getDefaultSensor(__sensorManager, ASENSOR_TYPE_ACCELEROMETER);
        __gyroscopeSensor = ASensorManager_getDefaultSensor(__sensorManager, ASENSOR_TYPE_GYROSCOPE);
        __sensorEventQueue = ASensorManager_createEventQueue(__sensorManager, __state->looper, LOOPER_ID_USER, NULL, NULL);

        // Get the asset manager to get the resources from the .apk file.
        __assetManager = __state->activity->assetManager; 

        // Get usable JNI context
        JNIEnv* env = __state->activity->env;
        JavaVM* vm = __state->activity->vm;
        jint res = vm->AttachCurrentThread(&env, NULL);
        if (res == JNI_ERR)
        {
            LOG("Failed to retrieve JVM environment when entering message pump.");
            return false;
        }

        // Get a handle on our calling NativeActivity instance
        jclass activityClass = env->GetObjectClass(__state->activity->clazz);

        // Get path to cache dir (/data/data/org.wikibooks.OpenGL/cache)
        jmethodID getCacheDir = env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
        jobject file = env->CallObjectMethod(__state->activity->clazz, getCacheDir);
        jclass fileClass = env->FindClass("java/io/File");
        jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
        jstring jpath = (jstring)env->CallObjectMethod(file, getAbsolutePath);
        const char* app_dir = env->GetStringUTFChars(jpath, NULL);

        // chdir in the application cache directory
        LOG("app_dir: %s", app_dir);

        jboolean isCopy;
        jclass clazz = env->GetObjectClass(__state->activity->clazz);
        jmethodID methodGetExternalStorage = env->GetMethodID(clazz, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");

        jclass clazzFile = env->FindClass("java/io/File");
        jmethodID methodGetPath = env->GetMethodID(clazzFile, "getPath", "()Ljava/lang/String;");

        // Now has java.io.File object pointing to directory
        jobject objectFile  = env->CallObjectMethod(__state->activity->clazz, methodGetExternalStorage, NULL);

        // Now has String object containing path to directory
        jstring stringExternalPath = static_cast<jstring>(env->CallObjectMethod(objectFile, methodGetPath));
        const char* externalPath = env->GetStringUTFChars(stringExternalPath, &isCopy);

        // Set the default path to store the resources.
        __assetsPath.assign(externalPath);
        if (externalPath[strlen(externalPath)-1] != '/')
            __assetsPath += "/";
        LOG("assetsPath = %s", __assetsPath.c_str());

        __initialized = 0;
        __animating = 0;

        __state_x = 10;
        __state_y = 10;

        return true;
    }

    void android_main_loop(atgGame* game)
    {
        LOG("android_main_loop : enter!");
        while (1) {
            // Read all pending events.
            int ident;
            int events;
            struct android_poll_source* source;

            // If not animating, we will block forever waiting for events.
            // If animating, we loop until all events are read, then continue
            // to draw the next frame of animation.
            while ((ident= ALooper_pollAll(0, NULL, &events,
                (void**)&source)) >= 0) 
            {
                // Process this event.
                if (source != NULL) {
                    source->process(__state, source);
                }

                // If a sensor has data, process it now.
                if (ident == LOOPER_ID_USER && __accelerometerSensor != NULL) 
                {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(__sensorEventQueue,
                        &event, 1) > 0) 
                    {
                        //LOG("accelerometer: x=%f y=%f z=%f",
                        //    event.acceleration.x, event.acceleration.y,
                        //    event.acceleration.z); // 重力加速度

                        //if (__sensorEvent.type == ASENSOR_TYPE_ACCELEROMETER)
                        //{
                        //    LOG("accelerometer: x=%f y=%f z=%f\n", __sensorEvent.acceleration.x,
                        //    __sensorEvent.acceleration.y,
                        //    __sensorEvent.acceleration.z);
                        //}
                        //else if (__sensorEvent.type == ASENSOR_TYPE_GYROSCOPE) // 指南针
                        //{
                        //    LOG("gyroscope: x=%f y=%f z=%f\n", __sensorEvent.vector.x,
                        //    __sensorEvent.vector.y,
                        //    __sensorEvent.vector.z);
                        //}
                    }
                }

                // Check if we are exiting.
                if (__state->destroyRequested != 0) {
                    android_term_display(game);
                    return;
                }
            }

            if (__initialized && __animating)
            {
                //android_draw_frame(game);
                if (game)
                {
                    game->OnFrame();
                }

                // Post the new frame to the display.
                // Note that there are a couple cases where eglSwapBuffers could fail
                // with an error code that requires a certain level of re-initialization:
                //
                // 1) EGL_BAD_NATIVE_WINDOW - Called when the surface we're currently using
                //    is invalidated. This would require us to destroy our EGL surface,
                //    close our OpenKODE window, and start again.
                //
                // 2) EGL_CONTEXT_LOST - Power management event that led to our EGL context
                //    being lost. Requires us to re-create and re-initalize our EGL context
                //    and all OpenGL ES state.
                //
                // For now, if we get these, we'll simply exit.
                int rc = android_ogl_present();
                if (rc != EGL_TRUE)
                {
                    EGLint error = eglGetError();
                    if (error == EGL_BAD_NATIVE_WINDOW)
                    {
                        if (__state->window != NULL)
                        {
                            g_Renderer->ReleaseAllGpuResource();
                            android_term_display(game);
                            g_Renderer->Initialize(0, 0, 0);
                        }
                        __initialized = true;
                    }
                    else if(error != EGL_SUCCESS)
                    {
                        perror("eglSwapBuffers");
                        break;
                    }
                }
            }
            // Display the keyboard.
        }
        LOG("android_main_loop : exit!\n");
    }

    extern void android_shutdown(atgGame* game){
        // ....
    }

    bool android_init_ogl()
    {

        if (__eglContext == EGL_NO_CONTEXT)
        {
            int samples = 8;
            // Hard-coded to 32-bit/OpenGL ES 2.0.
            // NOTE: EGL_SAMPLE_BUFFERS, EGL_SAMPLES and EGL_DEPTH_SIZE MUST remain at the beginning of the attribute list
            // since they are expected to be at indices 0-5 in config fallback code later.
            // EGL_DEPTH_SIZE is also expected to
            EGLint eglConfigAttrs[] =
            {
                EGL_SAMPLE_BUFFERS,     samples > 0 ? 1 : 0,
                EGL_SAMPLES,            samples,
                EGL_DEPTH_SIZE,         24,
                EGL_RED_SIZE,           8,
                EGL_GREEN_SIZE,         8,
                EGL_BLUE_SIZE,          8,
                EGL_ALPHA_SIZE,         8,
                EGL_STENCIL_SIZE,       8,
                EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
                EGL_NONE
            };

            EGLint eglConfigCount;
            const EGLint eglContextAttrs[] =
            {
                EGL_CONTEXT_CLIENT_VERSION,    2,
                EGL_NONE
            };

            if (__eglDisplay == EGL_NO_DISPLAY && __eglContext == EGL_NO_CONTEXT)
            {
                // Get the EGL display and initialize.
                __eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                if (__eglDisplay == EGL_NO_DISPLAY)
                {
                    checkErrorEGL("eglGetDisplay");
                    return false;
                }

                if (eglInitialize(__eglDisplay, NULL, NULL) != EGL_TRUE)
                {
                    checkErrorEGL("eglInitialize");
                    return false;
                }

                // Try both 24 and 16-bit depth sizes since some hardware (i.e. Tegra) does not support 24-bit depth
                bool validConfig = false;
                EGLint depthSizes[] = { 24, 16 };
                for (unsigned int i = 0; i < 2; ++i)
                {
                    eglConfigAttrs[1] = samples > 0 ? 1 : 0;
                    eglConfigAttrs[3] = samples;
                    eglConfigAttrs[5] = depthSizes[i];

                    if (eglChooseConfig(__eglDisplay, eglConfigAttrs, &__eglConfig, 1, &eglConfigCount) == EGL_TRUE && eglConfigCount > 0)
                    {
                        validConfig = true;
                        break;
                    }

                    if (samples)
                    {
                        // Try lowering the MSAA sample size until we find a supported config
                        int sampleCount = samples;
                        while (sampleCount)
                        {
                            LOG("No EGL config found for depth_size=%d and samples=%d. Trying samples=%d instead.\n", depthSizes[i], sampleCount, sampleCount / 2);
                            sampleCount /= 2;
                            eglConfigAttrs[1] = sampleCount > 0 ? 1 : 0;
                            eglConfigAttrs[3] = sampleCount;
                            if (eglChooseConfig(__eglDisplay, eglConfigAttrs, &__eglConfig, 1, &eglConfigCount) == EGL_TRUE && eglConfigCount > 0)
                            {
                                validConfig = true;
                                break;
                            }
                        }

                        if (validConfig)
                            break;
                    }
                    else
                    {
                        LOG("No EGL config found for depth_size=%d.\n", depthSizes[i]);
                    }
                }

                if (!validConfig)
                {
                    checkErrorEGL("eglChooseConfig");
                    return false;
                }

                LOG(" EGL_SAMPLE_BUFFERS=%d\n EGL_SAMPLES=%d\n EGL_DEPTH_SIZE=%d\n EGL_RED_SIZE=%d\n EGL_GREEN_SIZE=%d\n EGL_BLUE_SIZE=%d\n EGL_ALPHA_SIZE=%d\n EGL_STENCIL_SIZE=%d\n EGL_SURFACE_TYPE=%d\n EGL_RENDERABLE_TYPE=%d.\n", 
                    eglConfigAttrs[1],eglConfigAttrs[3], eglConfigAttrs[5], eglConfigAttrs[7], eglConfigAttrs[9],eglConfigAttrs[11],
                    eglConfigAttrs[13],eglConfigAttrs[15],eglConfigAttrs[17],eglConfigAttrs[19]);

                __eglContext = eglCreateContext(__eglDisplay, __eglConfig, EGL_NO_CONTEXT, eglContextAttrs);
                if (__eglContext == EGL_NO_CONTEXT)
                {
                    checkErrorEGL("eglCreateContext");
                    return false;
                }
            }
        }

        const EGLint eglSurfaceAttrs[] =
        {
            EGL_RENDER_BUFFER,    EGL_BACK_BUFFER,
            EGL_NONE
        };

        // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
        // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
        // As soon as we picked a EGLConfig, we can safely reconfigure the
        // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
        EGLint format;
        eglGetConfigAttrib(__eglDisplay, __eglConfig, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(__state->window, 0, 0, format);

        __eglSurface = eglCreateWindowSurface(__eglDisplay, __eglConfig, __state->window, eglSurfaceAttrs);
        if (__eglSurface == EGL_NO_SURFACE)
        {
            checkErrorEGL("eglCreateWindowSurface");
            return false;
        }

        if (eglMakeCurrent(__eglDisplay, __eglSurface, __eglSurface, __eglContext) != EGL_TRUE)
        {
            checkErrorEGL("eglMakeCurrent");
            return false;
        }

        eglQuerySurface(__eglDisplay, __eglSurface, EGL_WIDTH, &__width);
        eglQuerySurface(__eglDisplay, __eglSurface, EGL_HEIGHT, &__height);
        LOG("atgRenderer::Initialize end!\n");
        //__orientationAngle = getRotation() * 90;

        return true;
    }

    void android_shutdown_ogl()
    {

    }

    bool android_ogl_present()
    {
        eglWaitNative(EGL_CORE_NATIVE_ENGINE);
        eglWaitGL();
        return eglSwapBuffers(__eglDisplay, __eglSurface);
    }

    // 开启和关闭垂直同步
    void __atgSetVSyncState(bool enable)
    {
        if (__eglDisplay)
        {
            if (enable)
                eglSwapInterval(__eglDisplay, 1);
            else
                eglSwapInterval(__eglDisplay, 0);
        }
    }

#endif


atgGame::atgGame(void)
{
#ifdef _WIN32
    __g_game = this;
#endif
}

atgGame::~atgGame(void)
{

}

bool atgGame::Initialize(void* initData)
{
    bool rt;
#if defined(_WIN32)
    rt = win32_init(initData, this);
#elif defined(_ANDROID)
    rt = android_init((struct android_app*)initData, this);
#endif
    if(rt){
        g_Renderer = new atgRenderer(this);
#ifndef _ANDROID //>android原生自己实现renderder Init 和 OnInit
        if (g_Renderer)
        {
            rt = g_Renderer->Initialize(GetWindowWidth(), GetWindowHeight(), 32);
            if (rt)
            {
                rt = OnInitialize();
            }
        }
#endif
    }

    return rt;
}

void atgGame::Shutdown()
{
    OnShutdown();
    if (g_Renderer)
    {
        g_Renderer->Shutdown();
        SAFE_DELETE(g_Renderer);
    }
#ifdef _WIN32
    win32_shutdown(this);
#elif defined(_ANDROID)
    android_shutdown(this);
#endif // _WIN32
}

void atgGame::Run()
{
#ifdef _WIN32
    win32_main_loop(this);
#elif defined(_ANDROID)
    android_main_loop(this);
#endif // _WIN32

}

void atgGame::Pause()
{
    OnPause();
}

void atgGame::Resume()
{
    OnResume();
}

bool atgGame::OnInitialize()
{
    return true;
}

void atgGame::OnShutdown()
{

}

void atgGame::OnPause()
{

}

void atgGame::OnResume()
{

}

void atgGame::OnFrame()
{

}

void atgGame::OnPointerDown(uint8 id, int16 x, int16 y)
{

}

void atgGame::OnPointerMove(uint8 id, int16 x, int16 y)
{

}

void atgGame::OnPointerUp(uint8 id, int16 x, int16 y)
{

}

void atgGame::OnKeyScanDown(Key::Scan keyscan)
{
    switch (keyscan)
    {
    case Key::ArrowUp:
        break;
    case Key::ArrowDown:

        break;
    case Key::ArrowLeft:

        break;
    case Key::ArrowRight:
        break;
    case Key::Escape:
#ifdef _WIN32
        PostQuitMessage(0);
#endif // _WIN32
        break;
    }
}

void atgGame::OnKeyScanUp(Key::Scan keyscan)
{

}

int atgGame::GetWindowWidth()
{
#ifdef _WIN32
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    return (int)rect.right - rect.left;
#elif defined(_ANDROID)
    return __width;
#endif // _WIN32
}

int atgGame::GetWindowHeight()
{
#ifdef _WIN32
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    return (int)rect.bottom - rect.top;
#elif defined(_ANDROID)
    return __height;
#endif // _WIN32
}

const char* atgGame::GetResourceDir()
{
#ifdef _WIN32
    INT i;
    static std::string Directory;
    CHAR Buffer[MAX_PATH];

    ::GetModuleFileName(NULL, Buffer, MAX_PATH - 1);
    Directory.clear();
    Directory.assign(Buffer);

    i = static_cast<INT>(Directory.rfind("\\"));
    if(i != std::string::npos)
    {
        Directory = Directory.substr(0, i);
    }

    i = static_cast<INT>(Directory.rfind("\\"));
    if(i != std::string::npos)
    {
        Directory = Directory.substr(0, i);
    }

    i = static_cast<INT>(Directory.rfind("\\"));
    if(i != std::string::npos)
    {
        Directory = Directory.substr(0, i);
    }

    i = static_cast<INT>(Directory.rfind("\\"));
    if(i != std::string::npos)
    {
        Directory = Directory.substr(0, i+1);
    }

    return Directory.c_str();
#elif _ANDROID
    return __assetsPath.c_str();
#endif
}

void atgGame::SetTitle(const char* title)
{
#ifdef _WIN32
    if (g_hWnd)
    {
        ::SetWindowText(g_hWnd, title);
    }
#endif // _WIN32
}

#ifdef _WIN32


Key::Scan Virtualkey2KeyScan(WPARAM wParam, LPARAM lParam)
{
    UINT scancode = 0;
    if (HIWORD(lParam) & 0x0F00)
    { 
        scancode = MapVirtualKey(wParam, 0) | 0x80;
    }
    else
    {
        scancode = HIWORD(lParam) & 0x00FF;
    }
    return Key::Scan(scancode);
}

LRESULT CALLBACK atgGameWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifndef WM_MOUSEWHEEL
    #define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
    #define WHEEL_DELTA 120
#endif

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
            Key::Scan scan = Virtualkey2KeyScan(wParam, lParam);
            __g_game->OnKeyScanDown(scan);
        }
        break;
    case WM_KEYUP:
        {
            Key::Scan scan = Virtualkey2KeyScan(wParam, lParam);
            __g_game->OnKeyScanUp(scan);
        }
        break;
    case WM_SYSKEYDOWN:
        {
            //LOG("wParam=%d, lParam=%d\n", wParam, lParam);
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
            if (__g_game)
            {
                __g_game->OnPointerMove(MBID_UNKNOWN, x_pos, y_pos);
            }
            return 0;
        }
        break;
    case WM_LBUTTONDOWN:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
            if (__g_game)
            {
                __g_game->OnPointerDown(MBID_LEFT, x_pos, y_pos);
            }
        }
        break;
    case WM_LBUTTONUP:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
            if (__g_game)
            {
                __g_game->OnPointerUp(MBID_LEFT, x_pos, y_pos);
            }
        }
        break;
    case WM_RBUTTONDOWN:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
            if (__g_game)
            {
                __g_game->OnPointerDown(MBID_RIGHT, x_pos, y_pos);
            }
        }
        break;
    case WM_RBUTTONUP:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
            if (__g_game)
            {
                __g_game->OnPointerUp(MBID_RIGHT, x_pos, y_pos);
            }
        }
        break;
    case WM_MBUTTONDOWN:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
            if (__g_game)
            {
                __g_game->OnPointerDown(MBID_MIDDLE, x_pos, y_pos);
            }
        }
        break;
    case WM_MBUTTONUP:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
            if (__g_game)
            {
                __g_game->OnPointerUp(MBID_MIDDLE, x_pos, y_pos);
            }
        }
        break;
    case WM_MOUSEWHEEL:
        {
            int16 wheel = (int16)HIWORD(wParam) / WHEEL_DELTA;
            if (wheel > 0)
                __g_game->OnPointerMove(MBID_MIDDLE, 1, 0);
    
            else
                __g_game->OnPointerMove(MBID_MIDDLE, 0, 1);
        }
        break;
    case WM_CHAR:
        break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

#endif // _WIN32