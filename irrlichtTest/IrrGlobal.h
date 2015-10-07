#pragma once

#include <irrlicht.h>

extern irr::IrrlichtDevice* gDevice;
extern irr::video::IVideoDriver* gRenderer;
extern irr::scene::ISceneManager* gSceneMng;

bool InitIrrlicht();
void ShutdownIrrlicht();