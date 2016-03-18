#pragma once

#include <atgBase.h>
#include "IniFile.h"

namespace G1
{
    class Propertie
    {
    public:
        //virtual int                 GetPropertyCount() = 0;
        //virtual const std::string&  GetKey(int i) = 0;
        //virtual const std::string&  GetString(int i) = 0;

        virtual bool                HasKey(const std::string &key) const = 0;

        virtual bool                GetBool(const std::string &key, bool defaule) const = 0;
        virtual int                 GetInt(const std::string &key, int defValue) const = 0;
        virtual float               GetFloat(const std::string &key, float defValue) const = 0;
        virtual std::string         GetString(const std::string &key, const std::string& defValue) const = 0;

        virtual void                SetInt(const std::string &key, int value) = 0;
        virtual void                SetBool(const std::string &key, bool value) = 0;
        virtual void                SetFloat(const std::string &key, float value) = 0;
        virtual void                SetString(const std::string &key, const std::string &value) = 0;
    };


    class IniPropertie : public Propertie, public IniFile
    {
    public:
        IniPropertie();
        virtual ~IniPropertie();


        //virtual int                 GetPropertyCount();
        //virtual const std::string&  GetKey(int i);
        //virtual const std::string&  GetString(int i);
        virtual bool                HasKey(const std::string &key) const;

        virtual bool                GetBool(const std::string &key, bool defValue) const;
        virtual int                 GetInt(const std::string &key, int defValue) const;
        virtual float               GetFloat(const std::string &key, float defValue) const;
        virtual std::string         GetString(const std::string &key, const std::string& defValue) const;

        virtual void                SetInt(const std::string &key, int value);
        virtual void                SetBool(const std::string &key, bool value);
        virtual void                SetFloat(const std::string &key, float value);
        virtual void                SetString(const std::string &key, const std::string &value);
    
    };

}



