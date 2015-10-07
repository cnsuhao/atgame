#pragma once

class ScriptObject
{
public:
    ScriptObject(void);
    ~ScriptObject(void);

    bool LoadFile(const char* scriptfile);

    virtual void OnLoad();

    virtual void OnUpdate(float dt);
    
    virtual void OnUnload();

protected:
	bool IsNeedReload();


	bool _loaded;
	std::string _fileName;
	std::string _name;
	FILETIME _loadWriteTime;
};



