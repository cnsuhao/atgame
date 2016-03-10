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

#include "atgBlenderImport.h"

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

        //Matrix pmat;
        //Matrix vmat;
        //atgMath::LookAt(Vec3Zero.m, Vec3Back.m, Vec3Up.m, vmat.m);
        //atgMath::Perspective(30.0f, 1.0f, 1.0f, 1000.0f, pmat.m);
        //
        //
        //for (float i = -1.0f; i >= -1000.f; --i)
        //{
        //    Vec4 v4_1(0,0,i, 1.0f);
        //    Vec4 v4_2 = vmat.Transfrom(v4_1);
        //    v4_2 = pmat.Transfrom(v4_2);
        //    LOG("depth z=%f(i=%f)\n", v4_2.z / v4_2.w, i);
        //}

        LOG("Game::OnInitialize!\n");

        //SetRandomSeed((uint32)GetAbsoluteMsecTime());
        //LoadConfig();

        //atgBlenderImport::load("model\\box.fbx");
        //atgBlenderImport::load("model\\modelBuilding_House1.fbx");
        

        g_Renderer->SetViewPort(0, 0, GetWindowWidth(), GetWindowHeight());
        g_Renderer->SetFaceCull(FCM_CCW);
        g_Renderer->SetVSyncState(true);

        Matrix world(MatrixIdentity);
        //world.RotationZXY(0.0f, -90.0f, -90.0f);
        g_Renderer->SetMatrix(MD_WORLD, world);

        if(false == _camera.Create(new atgCamera()))
        {
            return false;
        }
        _camera.GetCamera()->SetFov(30.f);
        _camera.GetCamera()->SetClipFar(10000.0f);

        //if (false == shadowMapping.Create())
        //{
        //    return false;
        //}

        if (meshTest.Init())
        {
            return false;
        }

        //TGA_Loader::Load(&image, "model/CHRNPCICOHER156_DIFFUSE.tga", false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);
        //if (image.width > 0)
        //{
        //    if (g_Texture == NULL || g_Texture->IsLost())
        //    {
        //        g_Texture = new atgTexture();
        //        g_Texture->Create(image.width, image.height, image.bpp == 24 ? TF_R8G8B8 : TF_R8G8B8A8, image.imageData);
        //        g_Texture->SetFilterMode(TFM_FILTER_BILINEAR);
        //    }
        //}

        return true;
    }

    virtual void OnShutdown()
    {
        shadowMapping.Destory();
        SAFE_DELETE(g_Texture);
    }


    void OnKeyScanDown(Key::Scan keyscan)
    {
        _camera.OnKeyScanDown(keyscan);

        //shadowMapping.OnKeyScanDown(keyscan);
        
        meshTest.OnKeyScanDown(keyscan);

        switch (keyscan)
        {
        case Key::F1:
            {//> 关闭垂直同步
                g_Renderer->SetVSyncState(false);
            }break;
        case Key::F2:
            {//> 开启垂直同步
                g_Renderer->SetVSyncState(true);
            }break;
        case Key::R:
            {//> 释放所有gpu资源
                g_Renderer->ReleaseAllGpuResource();
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

        shadowMapping.OnPointerMove(id, x, y);
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

        //g_Renderer->BindTexture(0, g_Texture);

        //shadowMapping.Render(_camera.GetCamera());

        meshTest.Render(_camera.GetCamera());


        g_Renderer->EndFrame();
        g_Renderer->Present();
    }

    void DrawAxis()
    {
    }

    atgFlyCamera _camera;
    atgSimpleShadowMapping shadowMapping;
    TGA_Image image;
    //atgPerfMonitor _monitor;

    MeshTest meshTest;
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
