#pragma once

#ifdef WIN32
extern "C" { 
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#else
#include <lua.hpp>
#endif

#define  LuaCheckInt(n, r) 		\
	if(!lua_isnumber(L, n))			\
	{								\
		gLog.WriteLog(enLogType_Error,"参数[%d]不是整数,type:%d %s", n,lua_type(L,n),lua_typename(L,n)); \
		return 0;					\
	}								\
	r = (int) lua_tonumber(L, n);

#define LuaCheckFloat(L, n, f)		\
	if(!lua_isnumber(L, n))			\
	{								\
		gLog.WriteCriticalError( " 参数[%d]不是浮点数",  n); \
		f = 0.0f;						\
		return 0;					\
	}								\
	f = (float) lua_tonumber(L, n);

#define  LuaCheckStr(n, r)	\
	if(!lua_isstring(L, n))			\
	{								\
		gLog.WriteCriticalError(" 参数[%d]不是字符串",  n); \
		return 0;					\
	}								\
	r = lua_tostring(L, n);


#define LuaReturnInt(res)		\
	{							\
	lua_pushinteger(L, res);	\
	return 1;					\
	}

#define LuaReturnString(res)	\
{								\
	lua_pushstring(L, res);		\
	return 1;					\
}

#define LuaCheckArgsCount(funName,count) \
	if(lua_gettop(L) != count) \
	{	\
		gLog.WriteLog(enLogType_Error,"lua api : %s,参数个数错误 : %d - 正确数 : %d",funName,lua_gettop(L),count); \
		LuaReturnInt(0);	\
	}

class LuaEngine
{
protected:
	LuaEngine();
public:

	static LuaEngine& Instance() 
	{
		static LuaEngine eng;
		return eng;
	}
	
	~LuaEngine();

	lua_State* GetModule();

	//初始化lua虚拟机
	bool Init();

	//关闭lua虚拟机
	void Close();

	//注册API,函数原型：static int AddItem(lua_State* L)
	void RegisterAPI(const char* api_name,lua_CFunction fun);

	//执行lua文件
	bool ExecuteFile(const char* lua_file);

	// 函数是否存在
	bool IsExistFunction(const char* pFunc);

	//nID == 0表示全局函数
	int ExeFuncInt(const char* pFunc, int nID,unsigned char nNum, ...);	// 执行都是int类型的函数,使用short为了重载函数

	//打印lua堆栈
	void DumpStack();

	int GetStackSize();
private:
	lua_State* m_luaState;
	void clearStack();	//清空lua堆栈
};
