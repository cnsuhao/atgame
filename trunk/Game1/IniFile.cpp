#include "atgMisc.h"
#include "IniFile.h"

using namespace std;

namespace G1
{

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::Load()
    // 功  能: 加载ini文件
    // 参  数: [&file] - 打开了文件的ASLFile类对象
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    bool IniFile::Load(LPCSTR pFile)
    {
        AASSERT(pFile != NULL);

        string::size_type pos, pos2;
        SectionInfo si;


        atgReadFile reader;
        if(!reader.Open(pFile))
        {
            LOG("open ini file[%s] failure", pFile);
            return false;
        }

        // 逐行读取文件, 放入链表中
        std::string buffer;
        while (reader.ReadLine(buffer))
        {
            m_lData.push_back(buffer);
            buffer.clear();
        }

        // 读取文件名
        strncpy(m_szFileName, pFile, 260);

        // 把所有段位置记录下来, 加快查找速度
        for (Iter it = m_lData.begin(); it != m_lData.end(); ++it)
        {
            // 去掉前导空白
            pos = (*it).find_first_not_of(' ');

            // 空白行, 直接跳过
            if (pos == string::npos)
            {
                continue;
            }

            // 查找段
            if ((*it)[pos] == '[')
            {
                pos2 = (*it).find(']', pos);
                if (pos2 != string::npos)		// 找到一个段
                {
                    si.strName = (*it).substr(pos+1, pos2-pos-1);
                    si.itPos = it;
                    si.itPos++;
                    m_vSection.push_back(si);
                }
            }
        }

        m_nCurrent = m_vSection.size() > 0 ? 0 : -1;

        return true;
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::Save()
    // 功  能: 保存ini文件
    // 参  数: [szFileName] - 要保存的文件名
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::Save(LPCSTR szFileName)
    {
        FILE *fp = fopen(szFileName, "wt");

        for (Iter it = m_lData.begin(); it != m_lData.end(); ++it)
        {
            LPCSTR str = (*it).c_str();
            fprintf(fp, "%s\n", str);
        }

        fclose(fp);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::SetSection()
    // 功  能: 设置当前段, 若不存在则创建新段
    // 参  数: [szSection] - 段名
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::SetSection(LPCSTR szSection)
    {
        // 查找段
        for (int i = 0; i < (int)m_vSection.size(); ++i)
        {
            if (m_vSection[i].strName == szSection)
            {
                m_nCurrent = i;
                return;
            }
        }

        // 查找失败, 在尾部插入一个新段
        string strSection = "[";
        strSection += szSection;
        strSection += "]";
        m_lData.push_back(strSection);

        // 将新段信息放入段信息数组中
        SectionInfo si;
        si.strName = szSection;
        si.itPos = m_lData.end();
        m_vSection.push_back(si);

        m_nCurrent = (int)m_vSection.size() - 1;
    }

    //int IniFile::GetSectionKeyNumber()
    //{
    //    if (m_nCurrent < 0)
    //        return 0;

    //    return GetSectionKeyNumber(m_vSection[m_nCurrent].strName.c_str());
    //}

    //int IniFile::GetSectionKeyNumber( LPCSTR szSection )
    //{
    //    if (m_nCurrent < 0)
    //        return 0;

    //    int count = 0;
    //    Iter it;
    //    if(m_vSection[m_nCurrent].strName == szSection)
    //    {
    //        it = m_vSection[m_nCurrent].itPos;
    //    }
    //    else
    //    {
    //        it = m_lData.begin();
    //    }

    //    while (it != m_lData.end())
    //    {
    //        string::size_type pos;

    //        // 去掉前导空白
    //        pos = (*it).find_first_not_of(' ');

    //        // 空白行, 直接跳过
    //        if (pos == string::npos)
    //        {
    //            ++it;
    //            continue;
    //        }

    //        // 已到下个段, 查找失败
    //        if ((*it)[pos] == '[')
    //        {
    //            break;
    //        }
    //      
    //        ++it; ++count;
    //    }

    //    return count;
    //}

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::ReadString()
    // 功  能: 读字符串
    // 参  数: [szSection] - 段名
    //         [szKey] - 键名
    //         [szDefault] - 默认值
    // 返回值: [string] - 读出值
    //-----------------------------------------------------------------------------
    string IniFile::ReadString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault) 
    {
        SetSection(szSection);
        return ReadString(szKey, szDefault);
    }

    //-----------------------------------------------------------------------------
    // 函数名: IsSpace()
    // 功  能: 判断一个字符是否为空白. 全局函数, 用于ASLIni::ReadString().
    // 参  数: [c] - 字符
    // 返回值: [bool] - true如果该字符不是空白
    //-----------------------------------------------------------------------------
    bool IsNotSpace(char c)
    {
        return c != ' ' && c != '\t';
    }

    bool IniFile::HasKey(LPCSTR szSection, LPCSTR szKey)
    {
        SetSection(szSection);
        return HasKey(szKey);
    }

    bool IniFile::HasKey(LPCSTR szKey) const
    {
        string str =  ReadString(szKey, "");
        return (str != "");
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::ReadString()
    // 功  能: 读字符串, 要求先调用SetSection()
    // 参  数: [szKey] - 键名
    //         [szDefault] - 默认值
    // 返回值: [string] - 读出值
    //-----------------------------------------------------------------------------
    string IniFile::ReadString(LPCSTR szKey, LPCSTR szDefault) const
    {
        AASSERT(m_nCurrent >= 0);

        string strRtn;
        Iter it = m_vSection[m_nCurrent].itPos;

        while (it != m_lData.end())
        {
            string::size_type pos;

            // 去掉前导空白
            pos = (*it).find_first_not_of(' ');

            // 空白行, 直接跳过
            if (pos == string::npos)
            {
                ++it;
                continue;
            }

            // 已到下个段, 查找失败
            if ((*it)[pos] == '[')
            {
                return szDefault;
            }

            // 查找键名
            if ((*it).substr(pos, strlen(szKey)) == szKey)
            {
                pos += strlen(szKey);

                // 去掉等号前空白
                pos = (*it).find_first_not_of(' ', pos);

                // 等号存在, 已经找到指定键
                if (pos != string::npos && (*it)[pos] == '=')	
                {
                    ++pos;
                    if ((*it)[pos] != '\0')
                    {
                        // 去掉等号后空白
                        pos = (*it).find_first_not_of(' ', pos);
                        if (pos != string::npos)
                        {
                            strRtn = (*it).substr(pos);

                            // 去掉字符串尾部空白
                            while (strRtn[strRtn.size() - 1] == ' ')
                            {
                                strRtn.resize(strRtn.size() - 1);
                            }

                            return strRtn;
                        }
                    }

                    strRtn = szDefault;
                    return strRtn;
                }
            } // end if ((*it).substr(pos, strlen(szKey)) == szKey)

            ++it;
        } // end while (it != m_lData.end())

        strRtn = szDefault;
        return strRtn;
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::ReadInteger()
    // 功  能: 读整型值
    // 参  数: [szSection] - 段名
    //         [szKey] - 键名
    //         [nDefault] - 默认值
    // 返回值: [int] - 读出值
    //-----------------------------------------------------------------------------
    int IniFile::ReadInteger(LPCSTR szSection, LPCSTR szKey, int nDefault)
    {
        SetSection(szSection);
        return ReadInteger(szKey, nDefault);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::ReadInteger()
    // 功  能: 读整型值, 要求先调用SetSection()
    // 参  数: [szKey] - 键名
    //         [nDefault] - 默认值
    // 返回值: [int] - 读出值
    //-----------------------------------------------------------------------------
    int IniFile::ReadInteger(LPCSTR szKey, int nDefault) const
    {
        string str = ReadString(szKey, "");
        if (str == "")
        {
            return nDefault;
        }
        else
        {
            return atoi(str.c_str());
        }
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::ReadFloat()
    // 功  能: 读浮点值
    // 参  数: [szSection] - 段名
    //         [szKey] - 键名
    //         [lfDefault] - 默认值
    // 返回值: [double] - 读出值
    //-----------------------------------------------------------------------------
    float IniFile::ReadFloat(LPCSTR szSection, LPCSTR szKey, float lfDefault)
    {
        SetSection(szSection);
        return ReadFloat(szKey, lfDefault);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::ReadFloat()
    // 功  能: 读浮点值, 要求先调用SetSection()
    // 参  数: [szKey] - 键名
    //         [lfDefault] - 默认值
    // 返回值: [double] - 读出值
    //-----------------------------------------------------------------------------
    float IniFile::ReadFloat(LPCSTR szKey, float lfDefault) const
    {
        string str = ReadString(szKey, "");
        if (str == "")
        {
            return lfDefault;
        }
        else
        {
            return (float)atof(str.c_str());
        }
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::ReadBoolean()
    // 功  能: 读布尔值
    // 参  数: [szSection] - 段名
    //         [szKey] - 键名
    //         [bDefault] - 默认值
    // 返回值: [bool] - 读出值
    //-----------------------------------------------------------------------------
    bool IniFile::ReadBoolean(LPCSTR szSection, LPCSTR szKey, bool bDefault)
    {
        SetSection(szSection);
        return ReadBoolean(szKey, bDefault);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::ReadBoolean()
    // 功  能: 读布尔值, 要求先调用SetSection()
    // 参  数: [szKey] - 键名
    //         [bDefault] - 默认值
    // 返回值: [bool] - 读出值
    //-----------------------------------------------------------------------------
    bool IniFile::ReadBoolean(LPCSTR szKey, bool bDefault) const
    {
        string str = ReadString(szKey, "");
        if (str[0] == '1')
        {
            return true;
        }
        else if (str[0] == '0')
        {
            return false;
        }
        else
        {
            return bDefault;
        }
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::WriteString()
    // 功  能: 写字符串
    // 参  数: [szSection] - 段名
    //         [szKey] - 键名
    //         [szValue] - 写入值
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::WriteString(LPCSTR szSection, LPCSTR szKey, LPCSTR szValue)
    {
        SetSection(szSection);
        WriteString(szKey, szValue);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::WriteString()
    // 功  能: 写字符串, 要求先调用SetSection()
    // 参  数: [szKey] - 键名
    //         [szValue] - 写入值
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::WriteString(LPCSTR szKey, LPCSTR szValue)
    {
        AASSERT(m_nCurrent >= 0);

        Iter it = m_vSection[m_nCurrent].itPos;	// 当前段开始行指针
        string strWrite = szKey;				// 待写入内容
        strWrite += "=";
        strWrite += szValue;

        while (it != m_lData.end())
        {
            string::size_type pos;

            // 去掉前导空白
            pos = (*it).find_first_not_of(' ');

            // 空白行, 直接跳过
            if (pos == string::npos)
            {
                ++it;
                continue;
            }

            // 已到下个段, 查找失败
            if ((*it)[pos] == '[')
            {
                // 新建一个键, 返回
                m_lData.insert(m_vSection[m_nCurrent].itPos, strWrite);
                return;
            }

            // 查找键名
            if ((*it).substr(pos, strlen(szKey)) == szKey)
            {
                pos += strlen(szKey);

                // 去掉等号前空白
                pos = (*it).find_first_not_of(' ', pos);

                // 等号存在, 已经找到指定键
                if (pos != string::npos && (*it)[pos] == '=')
                {
                    (*it) = strWrite;	// 换掉原值
                    return;
                }
            }

            ++it;
        } // end while (it != m_lData.end())

        // 到链表尾仍没找到, 新建一个键
        m_lData.insert(m_vSection[m_nCurrent].itPos, strWrite);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::WriteInteger()
    // 功  能: 写整型值
    // 参  数: [szSection] - 段名
    //         [szKey] - 键名
    //         [nValue] - 写入值
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::WriteInteger(LPCSTR szSection, LPCSTR szKey, int nValue)
    {
        SetSection(szSection);
        WriteInteger(szKey, nValue);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::WriteInteger()
    // 功  能: 写整型值, 要求先调用SetSection()
    // 参  数: [szKey] - 键名
    //         [nValue] - 写入值
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::WriteInteger(LPCSTR szKey, int nValue)
    {
        char szBuffer[20];
        sprintf(szBuffer, "%d", nValue);

        WriteString(szKey, szBuffer);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::WriteFloat()
    // 功  能: 写浮点值
    // 参  数: [szSection] - 段名
    //         [szKey] - 键名
    //         [lfValue] - 写入值
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::WriteFloat(LPCSTR szSection, LPCSTR szKey, double lfValue)
    {
        SetSection(szSection);
        WriteFloat(szKey, lfValue);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::WriteFloat()
    // 功  能: 写浮点值, 要求先调用SetSection()
    // 参  数: [szKey] - 键名
    //         [lfValue] - 写入值
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::WriteFloat(LPCSTR szKey, double lfValue)
    {
        char szBuffer[20];
        sprintf(szBuffer, "%lf", lfValue);

        WriteString(szKey, szBuffer);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::WriteBoolean()
    // 功  能: 写布尔值
    // 参  数: [szSection] - 段名
    //         [szKey] - 键名
    //         [bValue] - 写入值
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::WriteBoolean(LPCSTR szSection, LPCSTR szKey, bool bValue)
    {
        SetSection(szSection);
        WriteBoolean(szKey, bValue);
    }

    //-----------------------------------------------------------------------------
    // 函数名: ASLIni::WriteBoolean()
    // 功  能: 写布尔值, 要求先调用SetSection()
    // 参  数: [szKey] - 键名
    //         [bValue] - 写入值
    // 返回值: [void] - 无
    //-----------------------------------------------------------------------------
    void IniFile::WriteBoolean(LPCSTR szKey, bool bValue)
    {
        char szBuffer[2] = "0";
        if (bValue)
        {
            szBuffer[0] = '1';
        }

        WriteString(szKey, szBuffer);
    }

    const std::string& IniFile::GetSectionName( unsigned int index )
    {
        static std::string emptyString("");

        if(index < m_vSection.size()) 
        { 
            return m_vSection[index].strName; 
        }
        return emptyString;
    }

    //std::string IniFile::GetSectionKey( unsigned int index )
    //{
    //
    //}

    //std::string IniFile::GetSectionString( unsigned int index )
    //{
    //
    //}

}