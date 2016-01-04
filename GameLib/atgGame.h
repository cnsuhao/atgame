#pragma once

#include "atgBase.h"

class atgGame
{
public:
    atgGame(void);
    virtual ~atgGame(void);
                                
    bool                    Initialize(void* initData);

    void                    Shutdown();

    void                    Run();

    void                    Pause();

    void                    Resume();

//////////////////////////////////////////////////////////////////////////
// derived class override 
    virtual bool            OnInitialize();

    virtual void            OnShutdown();

    virtual void            OnPause();
    
    virtual void            OnResume();

    virtual void            OnFrame();

    virtual void            OnPointerDown(uint8 id, int16 x, int16 y);

    virtual void            OnPointerMove(uint8 id, int16 x, int16 y);

    virtual void            OnPointerUp(uint8 id, int16 x, int16 y);

    virtual void            OnKeyScanDown(Key::Scan keyscan);

    virtual void            OnKeyScanUp(Key::Scan keyscan);

///////////////////////////////////////////////////////////////////////////
// helper
    int                     GetWindowWidth();

    int                     GetWindowHeight();

    const char*             GetResourceDir();

    void                    SetTitle(const char* title);
};

enum MouseButtonId
{
    MBID_LEFT = 0,
    MBID_RIGHT,
    MBID_MIDDLE,
    MBID_UNKNOWN
};


#ifdef _WIN32
    #include <Windows.h>
    extern HWND g_hWnd;
    extern HINSTANCE g_hInstance;
#endif // _WIN32

#if defined(_WIN32)
    bool win32_init_ogl();
    void win32_shutdown_ogl();
    int  win32_ogl_present();
    Key::Scan Virtualkey2KeyScan(WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK atgGameWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

#if defined(_ANDROID)
    bool android_init_ogl();
    void android_shutdown_ogl();
    bool android_ogl_present();
#endif

