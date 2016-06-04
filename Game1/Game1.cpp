// Game1.cpp : 定义控制台应用程序的入口点。
//
#ifdef _WIN32
    #include "stdafx.h"
#endif
#include "atgBase.h"
#include "atgRenderer.h"
#include "atgGame.h"
#include "atgMisc.h"
#include "Scene.h"

class Game : public atgGame
{
public :
    ~Game()
    {
    }

    bool OnInitialize()
    {
        LOG("Game::OnInitialize!\n");

        g_Renderer->SetViewPort(0, 0, GetWindowWidth(), GetWindowHeight());
        g_Renderer->SetFaceCull(FCM_CCW);
        g_Renderer->SetVSyncState(true);

        if(!_scene.Load("scene.txt"))
        {
            return false;
        }

        return true;
    }

    virtual void OnShutdown()
    {

    }


    void OnKeyScanDown(Key::Scan keyscan)
    {
        _scene.OnKeyScanDown(keyscan);
        atgGame::OnKeyScanDown(keyscan);
    }


    void OnPointerDown(uint8 id, int16 x, int16 y)
    {
        
    }

    void OnPointerMove(uint8 id, int16 x, int16 y)
    {
      
    }

    void OnPointerUp(uint8 id, int16 x, int16 y)
    {
       
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

        if (addTime > 1.f)
        {
            addTime -= 1.f;
            LOG("FPS = %d, %lf\n", frame, elapsedTime);
            frame = 0;
        }
        ++frame;

        g_Renderer->Clear();
        g_Renderer->BeginFrame();

        _scene.Render();

        g_Renderer->EndFrame();
        g_Renderer->Present();
    }

private:
    G1::Scene _scene;
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