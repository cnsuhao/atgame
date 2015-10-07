#include "IrrGlobal.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace irr;
using namespace video;
using namespace scene;

irr::IrrlichtDevice* gDevice = NULL;
irr::video::IVideoDriver* gRenderer = NULL;
irr::scene::ISceneManager* gSceneMng = NULL;

bool InitIrrlicht()
{
    gDevice = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(800, 600), 32, false);
    if (gDevice)
    {
        gRenderer = gDevice->getVideoDriver();
        gSceneMng = gDevice->getSceneManager();

        return true;
    }
    return false;
}

void ShutdownIrrlicht()
{
    gDevice->drop();
}
