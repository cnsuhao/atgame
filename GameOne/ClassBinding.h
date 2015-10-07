#pragma once

#include "LuaEngine.h"

//////////////////////////////////////////////////////////////////////////
// Camera
//////////////////////////////////////////////////////////////////////////

int CreateCamera(lua_State *L);

int DestoryCamera(lua_State *L);

int GetDefaultCamera(lua_State *L);

int CameraSetPosition(lua_State *L);

int CameraGetPosition(lua_State *L);

void RegisterCamera(lua_State *L);