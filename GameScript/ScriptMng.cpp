#include "stdafx.h"
#include "ScriptMng.h"
#include "duktape.c"

ScriptMng::ScriptMng(void)
{
	_ctx = duk_create_heap_default();
}


ScriptMng::~ScriptMng(void)
{
	duk_destroy_heap(_ctx);
}

void ScriptMng::Load( const char* fileName )
{
	if (fileName != NULL)
	{
		_scriptFile.clear();
		_scriptFile.append(fileName);

		RealLoad();
	}
}

void ScriptMng::HotLoad()
{
	if (!IsNeedHotLoad())
	{
		return;
	}

	End();
	RealLoad();
}

bool ScriptMng::IsNeedHotLoad()
{
	if (!_scriptFile.empty())
	{
		HANDLE hFile = CreateFileA(_scriptFile.c_str(),0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		FILETIME creationTime;
	    FILETIME lastAccessTime;
		FILETIME lastWriteTime;
		GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime);

		if (memcmp(&_loadWriteTime, &lastWriteTime, sizeof(FILETIME)) != 0)
		{
			atgLog("检测到脚本[%s]有变更.\n", _scriptFile.c_str());
			return true;
		}
		/*
		SYSTEMTIME sysTime;
		SYSTEMTIME localTime;
		FileTimeToSystemTime(&creationTime, &sysTime);
		SystemTimeToTzSpecificLocalTime(NULL, &sysTime, &localTime);
		atgLog("\n创建时间:\t%02d/%02d/%d  %02d:%02d\t", localTime.wDay, localTime.wMonth, localTime.wYear, localTime.wHour, localTime.wMinute);

		FileTimeToSystemTime(&lastAccessTime, &sysTime);
		SystemTimeToTzSpecificLocalTime(NULL, &sysTime, &localTime);
		atgLog("最后访问时间:\t%02d/%02d/%d  %02d:%02d\t", localTime.wDay, localTime.wMonth, localTime.wYear, localTime.wHour, localTime.wMinute);

		FileTimeToSystemTime(&lastWriteTime, &sysTime);
		SystemTimeToTzSpecificLocalTime(NULL, &sysTime, &localTime);
		atgLog("最后修改时间:\t%02d/%02d/%d  %02d:%02d\t", localTime.wDay, localTime.wMonth, localTime.wYear, localTime.wHour, localTime.wMinute);
		*/
	}

	return false;
}

void ScriptMng::RealLoad()
{
	if (duk_peval_file(_ctx, _scriptFile.c_str()) != 0) {
		atgLog("Error: %s\n", duk_safe_to_string(_ctx, -1));
	}

	FILETIME creationTime;
	FILETIME lastAccessTime;
	HANDLE hFile = CreateFileA(_scriptFile.c_str(),0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	GetFileTime(hFile, &creationTime, &lastAccessTime, &_loadWriteTime);

	Start();
}

void ScriptMng::Start()
{
	if (!_scriptFile.empty())
	{
		duk_push_global_object(_ctx);
		duk_get_prop_string(_ctx, -1 /*index*/, "start");
		if (duk_pcall(_ctx, 0 /*nargs*/) != 0) {
			printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
		}
		duk_pop(_ctx);  /* pop result/error */
	}
}

void ScriptMng::Update()
{
	if (!_scriptFile.empty())
	{
		duk_push_global_object(_ctx);
		duk_get_prop_string(_ctx, -1 /*index*/, "update");
		if (duk_pcall(_ctx, 0 /*nargs*/) != 0) {
			printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
		}
		duk_pop(_ctx);  /* pop result/error */
	}
}

void ScriptMng::End()
{
	if (!_scriptFile.empty())
	{
		duk_push_global_object(_ctx);
		duk_get_prop_string(_ctx, -1 /*index*/, "end");
		if (duk_pcall(_ctx, 0 /*nargs*/) != 0) {
			printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
		}
		duk_pop(_ctx);  /* pop result/error */
	}
}


