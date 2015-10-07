// irrlichtTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <irrlicht.h>
using namespace irr;
using namespace video;
using namespace scene;


#include "IrrGlobal.h"
#include "GameModel.h"


class MyEventReceiver : public IEventReceiver
{
public:
    // We'll create a struct to record info on the mouse state
    struct SMouseState
    {
        core::position2di Position;
        bool LeftButtonDown;
        SMouseState() : LeftButtonDown(false) { }
    } MouseState;

    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember the mouse state
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            switch(event.MouseInput.Event)
            {
            case EMIE_LMOUSE_PRESSED_DOWN:
                MouseState.LeftButtonDown = true;
                break;

            case EMIE_LMOUSE_LEFT_UP:
                MouseState.LeftButtonDown = false;
                break;

            case EMIE_MOUSE_MOVED:
                MouseState.Position.X = event.MouseInput.X;
                MouseState.Position.Y = event.MouseInput.Y;
                break;

            default:
                // We won't use the wheel
                break;
            }
        }
        else if (event.EventType == EET_KEY_INPUT_EVENT)
        {
            if (event.KeyInput.Key == KEY_ESCAPE)
            {
                gDevice->closeDevice();
            }
        }

        return false;
    }


    const SMouseState & GetMouseState(void) const
    {
        return MouseState;
    }


    MyEventReceiver()
    {
    }
};

int _tmain(int argc, _TCHAR* argv[])
{

    if(!InitIrrlicht())
    {
        return -1;
    }
    // create device
    MyEventReceiver receiver;

    gDevice->setEventReceiver(&receiver);

    //gSceneMng->addCameraSceneNode(0, core::vector3df(0,0,0));

    gSceneMng->addCameraSceneNodeFPS(0,10,0.1);
    gDevice->getCursorControl()->setVisible(false);

    video::SMaterial Material;
    Material.Lighting = false;
    Material.setTexture(0, gRenderer->getTexture("../data/ui/ui1.jpg"));

    GameModel model;
    model.Load();

    while(gDevice->run())
    {
        if (gDevice->isWindowActive())
        {
            gRenderer->beginScene(true, true, 0);

            gSceneMng->drawAll();
            gRenderer->setMaterial(Material);
            gRenderer->draw3DTriangle(core::triangle3df(core::vector3df(0.0f,0.0f,0.4f), core::vector3df(1.0f,1.0f,0.9f), core::vector3df(1.0f,-1.0f,1.0f)));
            gRenderer->draw2DRectangle(core::rect<s32>(0,0,100,100), video::SColor(255,255,255,255), video::SColor(255,255,255,255), video::SColor(255,255,255,255),  video::SColor(255,255,255,255));
            // We're all done drawing, so end the scene.
            gRenderer->endScene();
        }
    }

    ShutdownIrrlicht();

	return 0;
}

