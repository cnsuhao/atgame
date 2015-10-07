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
		gLog.WriteLog(enLogType_Error,"����[%d]��������,type:%d %s", n,lua_type(L,n),lua_typename(L,n)); \
		return 0;					\
	}								\
	r = (int) lua_tonumber(L, n);

#define LuaCheckFloat(L, n, f)		\
	if(!lua_isnumber(L, n))			\
	{								\
		gLog.WriteCriticalError( " ����[%d]���Ǹ�����",  n); \
		f = 0.0f;						\
		return 0;					\
	}								\
	f = (float) lua_tonumber(L, n);

#define  LuaCheckStr(n, r)	\
	if(!lua_isstring(L, n))			\
	{								\
		gLog.WriteCriticalError(" ����[%d]�����ַ���",  n); \
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
		gLog.WriteLog(enLogType_Error,"lua api : %s,������������ : %d - ��ȷ�� : %d",funName,lua_gettop(L),count); \
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

	//��ʼ��lua�����
	bool Init();

	//�ر�lua�����
	void Close();

	//ע��API,����ԭ�ͣ�static int AddItem(lua_State* L)
	void RegisterAPI(const char* api_name,lua_CFunction fun);

	//ִ��lua�ļ�
	bool ExecuteFile(const char* lua_file);

	// �����Ƿ����
	bool IsExistFunction(const char* pFunc);

	//nID == 0��ʾȫ�ֺ���
	int ExeFuncInt(const char* pFunc, int nID,unsigned char nNum, ...);	// ִ�ж���int���͵ĺ���,ʹ��shortΪ�����غ���

	//��ӡlua��ջ
	void DumpStack();

	int GetStackSize();
private:
	lua_State* m_luaState;
	void clearStack();	//���lua��ջ
};
