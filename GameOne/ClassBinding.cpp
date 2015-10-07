#include "stdafx.h"
#include "ClassBinding.h"
#include "Game.h"
#include "atgCamera.h"

//////////////////////////////////////////////////////////////////////////
// Camera Begin
//////////////////////////////////////////////////////////////////////////
int CreateCamera(lua_State *L)
{
	atgCamera* camera = new atgCamera();

	lua_pushlightuserdata(L, camera);

	return 1;
}

int DestoryCamera(lua_State *L)
{
	atgCamera* camera = (atgCamera*)lua_topointer(L, -1);
	delete camera;
	return 0;
}

int GetDefaultCamera(lua_State *L)
{
	lua_pushlightuserdata(L, gGame->GetCamera());

	return 1;
}

int CameraSetPosition(lua_State *L)
{
	atgCamera* camera = (atgCamera*)lua_topointer(L, -4); 

	float x = (float)lua_tonumber(L, -3);
	float y = (float)lua_tonumber(L, -2);
	float z = (float)lua_tonumber(L, -1);

	camera->SetPosition(Vec3(x,y,z));

	return 0;
}

int CameraGetPosition(lua_State *L)
{
	atgCamera* camera = (atgCamera*)lua_topointer(L, -1); 

	const Vec3& pos = camera->GetPosition();

	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	lua_pushnumber(L, pos.z);

	return 3;
}

void RegisterCamera( lua_State *L )
{
	LuaEngine& eng = LuaEngine::Instance();

	eng.RegisterAPI("CreateCamera", CreateCamera);
	eng.RegisterAPI("DestoryCamera", DestoryCamera);
	eng.RegisterAPI("GetDefaultCamera", GetDefaultCamera);
	eng.RegisterAPI("CameraSetPosition", CameraSetPosition);
	eng.RegisterAPI("CameraGetPosition", CameraGetPosition);
}

//////////////////////////////////////////////////////////////////////////
// Camera End
//////////////////////////////////////////////////////////////////////////