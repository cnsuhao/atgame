// GameLib.cpp : 定义控制台应用程序的入口点。
//
#ifdef _WIN32
#include "stdafx.h"
#endif // _WIN32

#include "atgGame.h"
#include "atgRenderer.h"
#include "atgCamera.h"
#include "atgIntersection.h"
#include "atgMisc.h"
#include "atgShaderLibrary.h"
#include "atgUtility.h"

//#include "perfMonitor.h"

static atgTexture* g_Texture = NULL;

class Game : public atgGame
{
public :
	~Game()
	{
	}

    bool OnInitialize()
    {
        LOG("Game::OnInitialize!\n");

        //SetRandomSeed((uint32)GetAbsoluteMsecTime());
        //LoadConfig();

        g_Renderer->SetViewPort(0, 0, GetWindowWidth(), GetWindowHeight());
        g_Renderer->SetFaceCull(FCM_CCW);

        Matrix world(MatrixIdentity);
        //world.RotationZXY(0.0f, -90.0f, -90.0f);
        g_Renderer->SetMatrix(MD_WORLD, world);

        if(false == _camera.Create(new atgCamera()))
        {
            return false;
        }

        _pShadowMapping = new atgSimpleShadowMapping();
        if (false == _pShadowMapping->Create())
        {
            return false;
        }

        //PNG_Loader::Load(&image, "ui/ui1.png", false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);
        //if (image.width > 0)
        //{
        //    if (g_Texture == NULL || g_Texture->IsLost())
        //    {
        //        g_Texture = new atgTexture();
        //        g_Texture->Create(image.width, image.height, TF_R8G8B8A8, image.imageData);
        //        g_Texture->SetFilterMode(TFM_FILTER_BILINEAR);
        //    }
        //}

        return true;
    }

    virtual void OnShutdown()
    {
        SAFE_DELETE(g_Texture);
    }


    void OnKeyScanDown(Key::Scan keyscan)
    {
        _camera.OnKeyScanDown(keyscan);

        if (_pShadowMapping)
        {
            _pShadowMapping->OnKeyScanDown(keyscan);
        }

        switch (keyscan)
        {
        case Key::F1:
            {
                g_Renderer->SetVSyncState(false);
            }break;
        case Key::F2:
            {
                g_Renderer->SetVSyncState(true);
            }break;
        default:
            break;
        }

        atgGame::OnKeyScanDown(keyscan);
    }


    void OnPointerDown(uint8 id, int16 x, int16 y)
    {
        _camera.OnPointerDown(id, x, y);
    }

    void OnPointerMove(uint8 id, int16 x, int16 y)
    {
        _camera.OnPointerMove(id, x, y);

    }

    void OnPointerUp(uint8 id, int16 x, int16 y)
    {
        _camera.OnPointerUp(id, x, y);
    }

    virtual void OnFrame()
    {
        static int frame = 0;
        static float lastTime = GetAbsoluteMsecTime() / 1000.f;
        static float addTime = 0;
        float time = GetAbsoluteMsecTime() / 1000.f;
        float elapsedTime = static_cast<float>(time - lastTime);
        lastTime = time;
        addTime += elapsedTime;

        //float fps;
        //_monitor.Update(fps);

        if (addTime > 1.f)
        {
            addTime -= 1.f;
            LOG("FPS = %d, %lf\n", frame, elapsedTime);
            frame = 0;
            //LOG("_monitor FPS = %f\n", fps);
        }
        ++frame;

        //>设置观察矩阵
        //>g_Renderer->SetMatrix(MD_VIEW, _camera.GetCamera()->GetView());
        //>g_Renderer->SetMatrix(MD_PROJECTION, _camera.GetCamera()->GetProj());
        
        g_Renderer->Clear();
        g_Renderer->BeginFrame();

        _pShadowMapping->Render(_camera.GetCamera());

        g_Renderer->EndFrame();
        g_Renderer->Present();
    }

    void DrawAxis()
    {
    }

    atgFlyCamera _camera;
    atgSimpleShadowMapping* _pShadowMapping;
    PNG_Image image;
    //atgPerfMonitor _monitor;
};

#ifdef _WIN32

int _tmain(int argc, _TCHAR* argv[])
{
    Game game;
    if(game.Initialize(NULL))
    {
        game.Run();
        game.Shutdown();
    }

	return 0;
}

#endif
