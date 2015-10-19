#include "stdafx.h"
#include "ScriptObject.h"

ScriptObject::ScriptObject(duk_context* ctx):_ctx(ctx), _loaded(false)
{
}


ScriptObject::~ScriptObject(void)
{
}

bool ScriptObject::LoadFile(const char* scriptfile)
{
    if (scriptfile != NULL)
    {
        _scriptFile.clear();
        _scriptFile.append(scriptfile);

        RealLoad();
    }
    return false;
}

void ScriptObject::OnLoad()
{
    if (_loaded)
    {
        duk_push_global_object(_ctx);
        duk_get_prop_string(_ctx, -1 /*index*/, "Load");
        if (duk_pcall(_ctx, 0 /*nargs*/) != 0) {
            printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
        }
        duk_pop(_ctx);  /* pop result/error */
    }
}

void ScriptObject::OnUpdate(float dt)
{
    if (_loaded)
    {
        if (IsNeedReload())
        {
            OnUnload();
            RealLoad();
        }

        if (!_scriptFile.empty())
        {
            duk_push_global_object(_ctx);
            duk_get_prop_string(_ctx, -1 /*index*/, "Update");
            if (duk_pcall(_ctx, 0 /*nargs*/) != 0) {
                printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
            }
            duk_pop(_ctx);  /* pop result/error */
        }
    }
}

void ScriptObject::OnUnload()
{
    if (_loaded)
    {
        duk_push_global_object(_ctx);
        duk_get_prop_string(_ctx, -1 /*index*/, "Unload");
        if (duk_pcall(_ctx, 0 /*nargs*/) != 0) {
            printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
        }
        duk_pop(_ctx);  /* pop result/error */
    }
}

bool ScriptObject::RealLoad()
{
    if (duk_peval_file(_ctx, _scriptFile.c_str()) != 0) {
        atgLog("Error: %s\n", duk_safe_to_string(_ctx, -1));
        return false;
    }

    FILETIME creationTime;
    FILETIME lastAccessTime;
    HANDLE hFile = CreateFileA(_scriptFile.c_str(),0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    GetFileTime(hFile, &creationTime, &lastAccessTime, &_loadWriteTime);

    _loaded = true;	
    OnLoad();
    return _loaded;
}

bool ScriptObject::IsNeedReload()
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
    }
    return false;
}
