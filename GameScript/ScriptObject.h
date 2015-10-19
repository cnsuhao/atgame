#pragma once

#include "duktape.h"

class ScriptObject
{
public:
    ScriptObject(duk_context* ctx);
    ~ScriptObject(void);

    bool LoadFile(const char* scriptfile, const char* objectName);

    void OnLoad();

    void OnUpdate(float deltaTime);

    void OnUnload();
    
    const std::string& GetObjectName() const {  return _objectName; }
protected:
    bool RealLoad();
    bool IsNeedReload();

    duk_context * _ctx;
    bool _loaded;
    std::string _scriptFile;
    std::string _objectName;
    FILETIME _loadWriteTime;
};

