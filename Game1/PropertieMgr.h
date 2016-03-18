#pragma once

#include "atgMisc.h"

namespace G1
{
    class PropertieMgr
    {
    public:
        PropertieMgr(void);
        ~PropertieMgr(void);


        bool OpenProperite(std::string path);

        Properties& GetProperite(std::string path);

        std::map<std::string, Properties>  _properities; 
    };
}



