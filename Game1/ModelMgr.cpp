#include "atgBase.h"
#include "ModelMgr.h"
#include "atgModel.h"
#include "atgAssimpImport.h"
#include "atgMdxImport.h"

ModelMgr::ModelMgr(void)
{
}


ModelMgr::~ModelMgr(void)
{
    Clear();
}

atgModel* ModelMgr::Load( const char* fileName )
{
    atgModel* pModel = Find(fileName);
    if (pModel != 0)
    {
        return pModel;
    }

    pModel = new atgModel();
    if(!atgAssimpImport::loadModel(fileName, pModel))
    {
        if (!atgMdxImport::loadModel(fileName, pModel))
        {
            SAFE_DELETE(pModel);
            return 0;
        }
    }

    m_cacheModels.insert(std::pair<std::string, atgModel*>(fileName,pModel));

    return  pModel;
}

atgModel* ModelMgr::Find( const char* fileName )
{
    std::map<std::string, atgModel*>::iterator it = m_cacheModels.find(fileName);
    if (it != m_cacheModels.end())
    {
        return it->second;
    }

    return 0;
}

void ModelMgr::Clear()
{
    std::map<std::string, atgModel*>::iterator it = m_cacheModels.begin();
    for (std::map<std::string, atgModel*>::iterator itEnd = m_cacheModels.end(); it != itEnd; ++it)
    {
        SAFE_DELETE(it->second);
    }

    m_cacheModels.clear();
}
