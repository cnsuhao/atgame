#include "stdafx.h"
#include "ScriptMng.h"
#include "duktape.c"


int ClearScreen(duk_context *ctx) {
    system("cls");
    return 0;  /* one return value */
}

int AddScriptObject(duk_context *ctx) {
    const char * scriptFile = duk_get_string(ctx, -2);
    const char * objectName = duk_get_string(ctx, -1);
    if (scriptFile && objectName)
    {
        ScriptMng::Get().AddScriptObject(scriptFile, objectName);
    }

    return 0;  /* one return value */
}

int RemoveScriptObject(duk_context *ctx){
    const char * objectName = duk_get_string(ctx, -1);
    if (objectName)
    {
        ScriptMng::Get().RemoveScriptObject(objectName);
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
    duk_push_c_function(_ctx, ::AddScriptObject, 2);
    duk_put_prop_string(_ctx, -2, "AddScriptObject");

    duk_push_c_function(_ctx, ::RemoveScriptObject, 1);
    duk_put_prop_string(_ctx, -2, "RemoveScriptObject");

    duk_pop(_ctx);

    printf("stack top is %ld\n", (long) duk_get_top(_ctx));
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

bool ScriptMng::AddScriptObject(const char* scriptFile, const char* objectName)
{
    if (!_memoveScriptObjects.empty())
    {
        auto rmIt = _memoveScriptObjects.begin();
        while (rmIt != _memoveScriptObjects.end())
        {
            if ((*rmIt)->GetObjectName() == objectName)
            {
                ScriptObject* pObj = new ScriptObject(_ctx);
                if (pObj)
                {
                    bool rs = pObj->LoadFile(scriptFile, objectName);
                    if (rs)
                    {
                        _loadScriptObjects.push_back(pObj);
                        return true;
                    }
                }
                break;
            }

            ++rmIt;
        }
    }
    else
    {
        if (_scriptObjects.find(objectName) != _scriptObjects.end())
            return true;

        ScriptObject* pObj = new ScriptObject(_ctx);
        if (pObj)
        {
            bool rs = pObj->LoadFile(scriptFile, objectName);
            if (rs)
            {
                _scriptObjects.insert(std::pair<std::string, ScriptObject*>(objectName, pObj));
                return true;
            }
        }
    }

    return false;
}

bool ScriptMng::RemoveScriptObject(const char* objectName)
{
    auto it = _scriptObjects.find(objectName);
    if (it != _scriptObjects.end())
    {
        it->second->OnUnload();
        _memoveScriptObjects.push_back(it->second);
        return true;
    }

    return false;
}

ScriptObject* ScriptMng::GetScriptObject(const char* objectName)
{
    auto it = _scriptObjects.find(objectName);
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
    duk_pop(_ctx);  /* pop result/error */
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
        duk_pop(_ctx);

        printf("stack top is %ld\n", (long) duk_get_top(_ctx));
	}
}

void ScriptMng::Update(float deltaTime)
{
	if (!_scriptFile.empty())
	{
		duk_push_global_object(_ctx);
		duk_get_prop_string(_ctx, -1 /*index*/, "update");
        duk_push_number(_ctx, deltaTime);
		if (duk_pcall(_ctx, 1 /*nargs*/) != 0) {
			printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
		}
		duk_pop(_ctx);  /* pop result/error */
        duk_pop(_ctx);

        //printf("stack top is %ld\n", (long) duk_get_top(_ctx));

        auto it = _scriptObjects.begin();
        while (it != _scriptObjects.end())
        {
            bool isRemoved = false;
            auto it2 = _memoveScriptObjects.begin();
            for(auto end = _memoveScriptObjects.end(); it2 != end; ++it)
            {
                if (*it2 == it->second)
                {
                    isRemoved = true;
                    break;
                }
            }
            if (!isRemoved)
            {
                it->second->OnUpdate(deltaTime);
            }
            ++it;
        }

        auto rmIt = _memoveScriptObjects.begin();
        while (rmIt != _memoveScriptObjects.end())
        {
            it = _scriptObjects.find((*rmIt)->GetObjectName());
            if (it != _scriptObjects.end())
            {
                _scriptObjects.erase(it);
            }
            delete *rmIt;
            ++rmIt;
        }
        _memoveScriptObjects.clear();


        auto ldIt = _loadScriptObjects.begin();
        if (ldIt != _loadScriptObjects.end())
        {
            _scriptObjects.insert(std::pair<std::string, ScriptObject*>((*ldIt)->GetObjectName(), (*ldIt)));
            ++ldIt;
        }
        _loadScriptObjects.clear();
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
        duk_pop(_ctx);
	}
}


