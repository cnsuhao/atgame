#include "stdafx.h"
#include "ScriptObject.h"

ScriptObject::ScriptObject(duk_context* ctx):_ctx(ctx), _loaded(false)
{
}


ScriptObject::~ScriptObject(void)
{
}

bool ScriptObject::LoadFile(const char* scriptfile, const char* objectName)
{
    if (scriptfile != NULL)
    {
        _scriptFile.clear();
        _scriptFile.append(scriptfile);

        _objectName.clear();
        _objectName.append(objectName);

        return RealLoad();
    }
    return false;
}

void ScriptObject::OnLoad()
{
    if (_loaded)
    {
        duk_get_global_string(_ctx, _objectName.c_str());
        if (duk_has_prop_string(_ctx, -1, "load"))
        {
            duk_push_string(_ctx, "load");
            if (duk_pcall_prop(_ctx, -2, 0 /*nargs*/) != 0) {
                printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
            }
            duk_pop(_ctx);  /* pop result/error */
        }
        duk_pop(_ctx);
        //printf("stack top is %ld\n", (long) duk_get_top(_ctx));
    }
}

void ScriptObject::OnUpdate(float deltaTime)
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
            duk_get_global_string(_ctx, _objectName.c_str());
            if (duk_has_prop_string(_ctx, -1, "update"))
            {
                duk_push_string(_ctx, "update");
                duk_push_number(_ctx, deltaTime);
                if (duk_pcall_prop(_ctx, -3, 1/*nargs*/) != 0) {
                    printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
                }
                duk_pop(_ctx);  /* pop result/error */
            }
            duk_pop(_ctx);
        }
    }
}

void ScriptObject::OnUnload()
{
    if (_loaded)
    {
        duk_get_global_string(_ctx, _objectName.c_str());
        if (duk_has_prop_string(_ctx, -1, "unload"))
        {
            duk_push_string(_ctx, "unload");
            if (duk_pcall_prop(_ctx, -2, 0/*nargs*/) != 0) {
                printf("Error: %s\n", duk_safe_to_string(_ctx, -1));
            }
            duk_pop(_ctx);  /* pop result/error */
        }
        duk_pop(_ctx);
    }
}

bool ScriptObject::RealLoad()
{
    if (duk_peval_file(_ctx, _scriptFile.c_str()) != 0) {
        atgLog("Error: %s\n", duk_safe_to_string(_ctx, -1));
        return false;
    }
    duk_pop(_ctx);  /* pop result/error */

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
