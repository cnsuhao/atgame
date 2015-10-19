#pragma once

#include "duktape.h"

class ScriptObject
{
public:
    ScriptObject(duk_context* ctx);
    ~ScriptObject(void);

    bool LoadFile(const char* scriptfile);

    void OnLoad();

    void OnUpdate(float dt);

    void OnUnload();
protected:
    bool RealLoad();
    bool IsNeedReload();

    duk_context * _ctx;
    bool _loaded;
    std::string _scriptFile;
    FILETIME _loadWriteTime;
};

