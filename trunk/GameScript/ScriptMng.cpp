#include "stdafx.h"
#include "ScriptMng.h"
#include "duktape.c"


int ClearScreen(duk_context *ctx) {
    system("cls");
    return 0;  /* one return value */
}

int AddScriptObject(duk_context *ctx) {
    const char * scriptFile = duk_get_string(ctx, -1);
    if (scriptFile)
    {
        ScriptMng::Get().AddScriptObject(scriptFile);
    }

    return 0;  /* one return value */
}

ScriptMng::ScriptMng(void)
{
	_ctx = duk_create_heap_default();
}


ScriptMng::~ScriptMng(void)
{
    auto it = _scriptObjects.begin();
    if (it != _scriptObjects.end())
    {
        delete it->second;
    }
    _scriptObjects.clear();

	duk_destroy_heap(_ctx);
}

void ScriptMng::Init()
{
    //register can using engine API to duk_js context.

    duk_push_global_object(_ctx);
    duk_push_c_function(_ctx, ClearScreen, 0);
    duk_put_prop_string(_ctx, -2, "ClearScreen");
    
    duk_push_object(_ctx);
    duk_put_prop_string(_ctx, -2, "ScriptMng");
    duk_pop(_ctx);


    duk_get_global_string(_ctx, "ScriptMng");
    duk_push_c_function(_ctx, ::AddScriptObject, 1);
    duk_put_prop_string(_ctx, -2, "AddScriptObject");



    duk_pop(_ctx);
}

bool ScriptMng::LoadScriptMain( const char* scriptFile )
{
	if (scriptFile != NULL)
	{
		_scriptFile.clear();
		_scriptFile.append(scriptFile);

		return RealLoad();
	}
    return false;
}

void ScriptMng::Shutdown()
{

}

bool ScriptMng::AddScriptObject(const char* scriptFile)
{
    if (_scriptObjects.find(scriptFile) != _scriptObjects.end())
        return true;

    ScriptObject* pObj = new ScriptObject(_ctx);
    if (pObj)
    {
        bool rs = pObj->LoadFile(scriptFile);
        if (rs)
        {
            _scriptObjects.insert(std::pair<std::string, ScriptObject*>(scriptFile, pObj));
            return true;
        }
    }
    return false;
}

bool ScriptMng::RemoveScriptObject(const char* scriptFile)
{
    auto it = _scriptObjects.find(scriptFile);
    if (it != _scriptObjects.end())
    {
        _scriptObjects.erase(it);
        return true;
    }

    return false;
}

ScriptObject* ScriptMng::GetScriptObject(const char* scriptFile)
{
    auto it = _scriptObjects.find(scriptFile);
    if (it != _scriptObjects.end())
    {
        return it->second;
    }
    return NULL;
}

bool ScriptMng::RealLoad()
{
	if (duk_peval_file(_ctx, _scriptFile.c_str()) != 0) {
		atgLog("Error: %s\n", duk_safe_to_string(_ctx, -1));
        return false;
	}

    return true;
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

        auto it = _scriptObjects.begin();
        while (it != _scriptObjects.end())
        {
            it->second->OnUpdate(0.0f);
            ++it;
        }
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


