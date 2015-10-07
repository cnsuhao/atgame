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
	return;	//todo:����ʵ�ֿ��ܱ���
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

//���� "������_%d"�Ĺ�����luaȫ�ֺ������в��Ҷ�Ӧ�ĺ���
//luaջ�Ĵ�С������ִ��ǰ��Ӧ�ñ�֤ջ�Ĵ�С����,�������ȼ�¼ԭʼջ��С,�����˳�ʱ�ָ�
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
		// ִ�пɱ����, ѹ�����
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

		if(lua_pcall(m_luaState, nNum, 1, 0) != 0)	// ִ�к���,��������,����ֵ����,����ص�����
		{
			atgLog("�ű�����ִ��ʧ�ܣ�%s ID:%d",pFunc,nID);
			DumpStack();
			lua_settop(m_luaState,oldStackSize);
			return 0;
		}

#ifdef ENABLE_FUNCTION_CPU_TIME_TEST
		uint64 tEnd = Framework::Timer::DateTime::GetCurMillis();
		int dTime = (int)(tEnd - tStart);
		if(dTime >= 15)
		{
			gLog.WriteLog(enLogType_Warning,"�ű����� %s ID:%d ִ��ʱ��:%d",pFunc,nID,dTime);
		}
#endif

		if(!lua_isnumber(m_luaState, -1))
		{
			atgLog("�ű�����[%s],����ֵ��������,����ֵ����:%d - %s ���߽ű������е��õ�APIδ�ڴ�����ע��",pFunc,lua_type(m_luaState,-1),lua_typename(m_luaState,-1));
			DumpStack();
			lua_settop(m_luaState,oldStackSize);
			return 0;
		}

		//������lua�����Ὣ����ֵ����ջ��,ջ����λ����-1
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
		atgLog("[%s]�ű�����������,ID=%d type:%d", pFunc,nID,nType);
		return 0;
	}
}

int LuaEngine::GetStackSize()
{
	return lua_gettop(m_luaState);
}
