#include "stdafx.h"
#include "LuaEngine.h"
#include "atgBase.h"
#include "ClassBinding.h"

LuaEngine::LuaEngine()
{
	m_luaState = NULL;
}

LuaEngine::~LuaEngine()
{

}

void LuaEngine::DumpStack()
{
	return;	//todo:现有实现可能崩溃
	//gLog.PrintConsole("\nlua stack begin\n---------------");
	//int top = lua_gettop(m_luaState);
	//for(int i = 1; i <= top;i ++)
	//{
	//	int t = lua_type(m_luaState, i);
	//	switch (t)
	//	{
	//	case LUA_TSTRING:
	//		gLog.PrintConsole("%s", lua_tostring(m_luaState, i));
	//		break;

	//	case LUA_TBOOLEAN:
	//		gLog.PrintConsole(lua_toboolean(m_luaState, i) ? "true" : "false");
	//		break;

	//	case LUA_TNUMBER:
	//		gLog.PrintConsole("%g", lua_tonumber(m_luaState, i));
	//		break;

	//	default:
	//		gLog.PrintConsole("%s", lua_typename(m_luaState, t));
	//		break;

	//	}
	//	gLog.PrintConsole("");
	//}
	//gLog.PrintConsole("\n---------------");
}

bool LuaEngine::Init()
{
	m_luaState = luaL_newstate();
	luaL_openlibs(m_luaState);

	RegisterCamera(m_luaState);

	return true;
}

void LuaEngine::Close()
{
	lua_close(m_luaState);
}

lua_State* LuaEngine::GetModule()
{
	return m_luaState;
}

void LuaEngine::clearStack()
{
	lua_settop(m_luaState,0);
}

void LuaEngine::RegisterAPI(const char* api_name,lua_CFunction fun)
{
	lua_register(m_luaState,api_name,fun);
}

bool LuaEngine::ExecuteFile(const char* lua_file)
{
	int error = luaL_dofile(m_luaState,lua_file);
	if(error != 0)
	{
		DumpStack();
		clearStack();
		return false;
	}
	else
	{
		return true;
	}
}

bool LuaEngine::IsExistFunction(const char* pFunc)
{
	lua_getglobal(m_luaState, pFunc);
	bool bFun = lua_isfunction(m_luaState, -1);
	lua_pop(m_luaState, 1);

	return bFun;
}

//根据 "函数名_%d"的规则在lua全局函数表中查找对应的函数
//lua栈的大小：函数执行前后应该保证栈的大小不变,所以首先记录原始栈大小,函数退出时恢复
int LuaEngine::ExeFuncInt(const char* pFunc, int nID, unsigned char nNum, ...)
{
	int oldStackSize = GetStackSize();

	if(nID <= 0)
	{
		lua_getglobal(m_luaState, pFunc);
	}
	else
	{
		char buff[256];
		sprintf(buff, "%s_%d",pFunc, nID);
		lua_getglobal(m_luaState, buff);
	}

	if(lua_isfunction(m_luaState, -1))
	{
		// 执行可变参数, 压入参数
		if(nNum > 0)
		{
			va_list pArg;
			va_start(pArg, nNum);

			int nParam = 0;		
			for(unsigned char i = 0; i < nNum; ++i)
			{
				nParam = va_arg(pArg, int);
				lua_pushinteger(m_luaState, nParam);
			}

			va_end(pArg);
		}

#ifdef ENABLE_FUNCTION_CPU_TIME_TEST
		uint64 tStart = Framework::Timer::DateTime::GetCurMillis();
#endif

		if(lua_pcall(m_luaState, nNum, 1, 0) != 0)	// 执行函数,参数个数,返回值个数,错误回调函数
		{
			atgLog("脚本函数执行失败！%s ID:%d",pFunc,nID);
			DumpStack();
			lua_settop(m_luaState,oldStackSize);
			return 0;
		}

#ifdef ENABLE_FUNCTION_CPU_TIME_TEST
		uint64 tEnd = Framework::Timer::DateTime::GetCurMillis();
		int dTime = (int)(tEnd - tStart);
		if(dTime >= 15)
		{
			gLog.WriteLog(enLogType_Warning,"脚本函数 %s ID:%d 执行时间:%d",pFunc,nID,dTime);
		}
#endif

		if(!lua_isnumber(m_luaState, -1))
		{
			atgLog("脚本函数[%s],返回值不是整数,返回值类型:%d - %s 或者脚本函数中调用的API未在代码中注册",pFunc,lua_type(m_luaState,-1),lua_typename(m_luaState,-1));
			DumpStack();
			lua_settop(m_luaState,oldStackSize);
			return 0;
		}

		//被调用lua函数会将返回值放在栈顶,栈顶的位置是-1
		int res = (int)lua_tonumber(m_luaState, -1);
		lua_pop(m_luaState, 1);

		int top = GetStackSize();
		if(top != oldStackSize)
		{
			atgLog("after execute,lua stack size:%d funName:%s id:%d",top,pFunc,nID);
			DumpStack();
		}

		lua_settop(m_luaState,oldStackSize);
		return res;
	}
	else
	{
		lua_settop(m_luaState,oldStackSize);
		int nType = lua_type(m_luaState, -1);
		atgLog("[%s]脚本函数不存在,ID=%d type:%d", pFunc,nID,nType);
		return 0;
	}
}

int LuaEngine::GetStackSize()
{
	return lua_gettop(m_luaState);
}
