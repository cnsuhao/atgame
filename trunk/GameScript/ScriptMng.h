#pragma once

#include "ScriptObject.h"

class ScriptMng
{
	ScriptMng(void);
public:
	
	~ScriptMng(void);

	static ScriptMng& Get() 
	{ 
		static ScriptMng mng; 
		return mng; 
	}
    
    //>ÅäÖÃ
    void                Init();
	bool                LoadScriptMain(const char* scriptFile);
    void                Shutdown();
    
    //>½Å±¾
    bool                AddScriptObject(const char* scriptFile, const char* objectName);
    bool                RemoveScriptObject(const char* objectName);
    ScriptObject*       GetScriptObject(const char* objectName);

    //>Âß¼­
	void                Start();
	void                Update(float deltaTime);
	void                End();

protected:
	bool                RealLoad();

protected:
	std::string _scriptFile;
	duk_context * _ctx;
    std::map<std::string, ScriptObject*> _scriptObjects;
    std::vector<ScriptObject*> _memoveScriptObjects;
    std::vector<ScriptObject*> _loadScriptObjects;
};

