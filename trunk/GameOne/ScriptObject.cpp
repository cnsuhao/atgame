#include "stdafx.h"
#include "ScriptObject.h"
#include "LuaEngine.h"



ScriptObject::ScriptObject(void):_loaded(false)
{
}


ScriptObject::~ScriptObject(void)
{
}

bool ScriptObject::LoadFile(const char* scriptfile)
{
	if(!LuaEngine::Instance().ExecuteFile(scriptfile))
	{
		atgLog("加载脚本文件失败。");
		return false;
	}
	_fileName = scriptfile;

	lua_State* L = LuaEngine::Instance().GetModule();
	const char* pScriptName = lua_tostring(L, -1);
	if (pScriptName)
	{
		_name.clear();
		_name.assign(pScriptName);

		OnLoad();
	}
	
    return false;
}

void ScriptObject::OnLoad()
{
	lua_State* L = LuaEngine::Instance().GetModule();

	lua_getglobal(L,_name.c_str());
	lua_getfield(L, -1, "OnLoad");
	if(lua_isfunction(L, -1))
	{
		lua_getglobal(L, _name.c_str());
		if(0 == lua_pcall(L,1,0,0))
		{
			FILETIME creationTime;
			FILETIME lastAccessTime;
			HANDLE hFile = CreateFileA(_fileName.c_str(),0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			GetFileTime(hFile, &creationTime, &lastAccessTime, &_loadWriteTime);
			_loaded = true;	
		}
		else
		{
			printf("%s\n", lua_tostring(L, -1));
		}
	}
}

void ScriptObject::OnUpdate(float dt)
{
	if (_loaded)
	{
		if (IsNeedReload())
		{
			OnUnload();
			LoadFile(_fileName.c_str());
			return;
		}

		lua_State* L = LuaEngine::Instance().GetModule();

		lua_getglobal(L,_name.c_str());
		lua_getfield(L, -1, "OnUpdate");
		if(lua_isfunction(L, -1))
		{
			lua_getglobal(L, _name.c_str());
			lua_pushnumber(L, dt);
			if(lua_pcall(L,2,0,0))
			{
				printf("%s\n", lua_tostring(L, -1));
			}
		}
	}
}

void ScriptObject::OnUnload()
{
	if (_loaded)
	{
		lua_State* L = LuaEngine::Instance().GetModule();

		lua_getglobal(L,_name.c_str());
		lua_getfield(L, -1, "OnUnload");
		if(lua_isfunction(L, -1))
		{
			lua_getglobal(L, _name.c_str());
			if(lua_pcall(L,1,0,0))
			{
				printf("%s\n", lua_tostring(L, -1));
			}
		}
	}
}

bool ScriptObject::IsNeedReload()
{
	HANDLE hFile = CreateFileA(_fileName.c_str(),0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	FILETIME creationTime;
	FILETIME lastAccessTime;
	FILETIME lastWriteTime;
	GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime);

	if (memcmp(&_loadWriteTime, &lastWriteTime, sizeof(FILETIME)) != 0)
	{
		atgLog("检测到脚本[%s]有变更.\n", _fileName.c_str());
		return true;
	}
	return false;
}
