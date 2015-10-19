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
    bool                AddScriptObject(const char* scriptFile);
    bool                RemoveScriptObject(const char* scriptFile);
    ScriptObject*       GetScriptObject(const char* scriptFile);

    //>Âß¼­
	void                Start();
	void                Update();
	void                End();

protected:
	bool                RealLoad();

protected:
	std::string _scriptFile;
	duk_context * _ctx;
    std::map<std::string, ScriptObject*> _scriptObjects;
};

