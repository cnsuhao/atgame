#include "Propertie.h"

namespace G1
{


    IniPropertie::IniPropertie()
    {

    }

    IniPropertie::~IniPropertie()
    {

    }

    //int IniPropertie::GetPropertyCount()
    //{
    //}

    //const std::string& IniPropertie::GetKey( int i )
    //{
    //}

    //const std::string& IniPropertie::GetString( int i )
    //{
    //}


    bool IniPropertie::HasKey( const std::string &key ) const
    {
        return IniFile::HasKey(key.c_str());
    }

    bool IniPropertie::GetBool( const std::string &key, bool defValue ) const
    {
        return ReadBoolean(key.c_str(), defValue);
    }

    int IniPropertie::GetInt( const std::string &key, int defValue ) const
    {
        return ReadInteger(key.c_str(), defValue);
    }

    float IniPropertie::GetFloat( const std::string &key , float defValue) const
    {
        return ReadFloat(key.c_str(), defValue);
    }

    std::string  IniPropertie::GetString(const std::string &key, const std::string& defValue) const
    {
        return ReadString(key.c_str(), defValue.c_str());
    }

    void IniPropertie::SetInt( const std::string &key, int value )
    {

    }

    void IniPropertie::SetBool( const std::string &key, bool value )
    {

    }

    void IniPropertie::SetFloat( const std::string &key, float value )
    {

    }

    void IniPropertie::SetString( const std::string &key, const std::string &value )
    {

    }

}
