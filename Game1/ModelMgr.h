#pragma once

class atgModel;
class ModelMgr
{
    ModelMgr(void);
    ~ModelMgr(void);
public:

    static bool Init() {}
    static void Shutdown() {}

    static ModelMgr& Get() 
    {
        static ModelMgr* pInst = 0;
        if (pInst == 0)
            pInst = new ModelMgr();

        return *pInst;
    }

    atgModel* Load(const char* fileName);

    atgModel* Find(const char* fileName);

    void Clear();
private:
    std::map<std::string, atgModel*> m_cacheModels;
};

