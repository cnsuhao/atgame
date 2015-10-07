#pragma once

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

	void Load(const char* fileName);

	void HotLoad();

	bool IsNeedHotLoad();

	void Start();
	void Update();
	void End();
protected:
	void RealLoad();

protected:
	std::string _scriptFile;
	duk_context * _ctx;
	FILETIME _loadWriteTime;
};

