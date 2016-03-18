#include "atgMisc.h"
#include "IniFile.h"

using namespace std;

namespace G1
{

    //-----------------------------------------------------------------------------
    // ������: ASLIni::Load()
    // ��  ��: ����ini�ļ�
    // ��  ��: [&file] - �����ļ���ASLFile�����
    // ����ֵ: [void] - ��
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

        // ���ж�ȡ�ļ�, ����������
        std::string buffer;
        while (reader.ReadLine(buffer))
        {
            m_lData.push_back(buffer);
            buffer.clear();
        }

        // ��ȡ�ļ���
        strncpy(m_szFileName, pFile, 260);

        // �����ж�λ�ü�¼����, �ӿ�����ٶ�
        for (Iter it = m_lData.begin(); it != m_lData.end(); ++it)
        {
            // ȥ��ǰ���հ�
            pos = (*it).find_first_not_of(' ');

            // �հ���, ֱ������
            if (pos == string::npos)
            {
                continue;
            }

            // ���Ҷ�
            if ((*it)[pos] == '[')
            {
                pos2 = (*it).find(']', pos);
                if (pos2 != string::npos)		// �ҵ�һ����
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
    // ������: ASLIni::Save()
    // ��  ��: ����ini�ļ�
    // ��  ��: [szFileName] - Ҫ������ļ���
    // ����ֵ: [void] - ��
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
    // ������: ASLIni::SetSection()
    // ��  ��: ���õ�ǰ��, ���������򴴽��¶�
    // ��  ��: [szSection] - ����
    // ����ֵ: [void] - ��
    //-----------------------------------------------------------------------------
    void IniFile::SetSection(LPCSTR szSection)
    {
        // ���Ҷ�
        for (int i = 0; i < (int)m_vSection.size(); ++i)
        {
            if (m_vSection[i].strName == szSection)
            {
                m_nCurrent = i;
                return;
            }
        }

        // ����ʧ��, ��β������һ���¶�
        string strSection = "[";
        strSection += szSection;
        strSection += "]";
        m_lData.push_back(strSection);

        // ���¶���Ϣ�������Ϣ������
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

    //        // ȥ��ǰ���հ�
    //        pos = (*it).find_first_not_of(' ');

    //        // �հ���, ֱ������
    //        if (pos == string::npos)
    //        {
    //            ++it;
    //            continue;
    //        }

    //        // �ѵ��¸���, ����ʧ��
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
    // ������: ASLIni::ReadString()
    // ��  ��: ���ַ���
    // ��  ��: [szSection] - ����
    //         [szKey] - ����
    //         [szDefault] - Ĭ��ֵ
    // ����ֵ: [string] - ����ֵ
    //-----------------------------------------------------------------------------
    string IniFile::ReadString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault) 
    {
        SetSection(szSection);
        return ReadString(szKey, szDefault);
    }

    //-----------------------------------------------------------------------------
    // ������: IsSpace()
    // ��  ��: �ж�һ���ַ��Ƿ�Ϊ�հ�. ȫ�ֺ���, ����ASLIni::ReadString().
    // ��  ��: [c] - �ַ�
    // ����ֵ: [bool] - true������ַ����ǿհ�
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
    // ������: ASLIni::ReadString()
    // ��  ��: ���ַ���, Ҫ���ȵ���SetSection()
    // ��  ��: [szKey] - ����
    //         [szDefault] - Ĭ��ֵ
    // ����ֵ: [string] - ����ֵ
    //-----------------------------------------------------------------------------
    string IniFile::ReadString(LPCSTR szKey, LPCSTR szDefault) const
    {
        AASSERT(m_nCurrent >= 0);

        string strRtn;
        Iter it = m_vSection[m_nCurrent].itPos;

        while (it != m_lData.end())
        {
            string::size_type pos;

            // ȥ��ǰ���հ�
            pos = (*it).find_first_not_of(' ');

            // �հ���, ֱ������
            if (pos == string::npos)
            {
                ++it;
                continue;
            }

            // �ѵ��¸���, ����ʧ��
            if ((*it)[pos] == '[')
            {
                return szDefault;
            }

            // ���Ҽ���
            if ((*it).substr(pos, strlen(szKey)) == szKey)
            {
                pos += strlen(szKey);

                // ȥ���Ⱥ�ǰ�հ�
                pos = (*it).find_first_not_of(' ', pos);

                // �ȺŴ���, �Ѿ��ҵ�ָ����
                if (pos != string::npos && (*it)[pos] == '=')	
                {
                    ++pos;
                    if ((*it)[pos] != '\0')
                    {
                        // ȥ���Ⱥź�հ�
                        pos = (*it).find_first_not_of(' ', pos);
                        if (pos != string::npos)
                        {
                            strRtn = (*it).substr(pos);

                            // ȥ���ַ���β���հ�
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
    // ������: ASLIni::ReadInteger()
    // ��  ��: ������ֵ
    // ��  ��: [szSection] - ����
    //         [szKey] - ����
    //         [nDefault] - Ĭ��ֵ
    // ����ֵ: [int] - ����ֵ
    //-----------------------------------------------------------------------------
    int IniFile::ReadInteger(LPCSTR szSection, LPCSTR szKey, int nDefault)
    {
        SetSection(szSection);
        return ReadInteger(szKey, nDefault);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::ReadInteger()
    // ��  ��: ������ֵ, Ҫ���ȵ���SetSection()
    // ��  ��: [szKey] - ����
    //         [nDefault] - Ĭ��ֵ
    // ����ֵ: [int] - ����ֵ
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
    // ������: ASLIni::ReadFloat()
    // ��  ��: ������ֵ
    // ��  ��: [szSection] - ����
    //         [szKey] - ����
    //         [lfDefault] - Ĭ��ֵ
    // ����ֵ: [double] - ����ֵ
    //-----------------------------------------------------------------------------
    float IniFile::ReadFloat(LPCSTR szSection, LPCSTR szKey, float lfDefault)
    {
        SetSection(szSection);
        return ReadFloat(szKey, lfDefault);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::ReadFloat()
    // ��  ��: ������ֵ, Ҫ���ȵ���SetSection()
    // ��  ��: [szKey] - ����
    //         [lfDefault] - Ĭ��ֵ
    // ����ֵ: [double] - ����ֵ
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
    // ������: ASLIni::ReadBoolean()
    // ��  ��: ������ֵ
    // ��  ��: [szSection] - ����
    //         [szKey] - ����
    //         [bDefault] - Ĭ��ֵ
    // ����ֵ: [bool] - ����ֵ
    //-----------------------------------------------------------------------------
    bool IniFile::ReadBoolean(LPCSTR szSection, LPCSTR szKey, bool bDefault)
    {
        SetSection(szSection);
        return ReadBoolean(szKey, bDefault);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::ReadBoolean()
    // ��  ��: ������ֵ, Ҫ���ȵ���SetSection()
    // ��  ��: [szKey] - ����
    //         [bDefault] - Ĭ��ֵ
    // ����ֵ: [bool] - ����ֵ
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
    // ������: ASLIni::WriteString()
    // ��  ��: д�ַ���
    // ��  ��: [szSection] - ����
    //         [szKey] - ����
    //         [szValue] - д��ֵ
    // ����ֵ: [void] - ��
    //-----------------------------------------------------------------------------
    void IniFile::WriteString(LPCSTR szSection, LPCSTR szKey, LPCSTR szValue)
    {
        SetSection(szSection);
        WriteString(szKey, szValue);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::WriteString()
    // ��  ��: д�ַ���, Ҫ���ȵ���SetSection()
    // ��  ��: [szKey] - ����
    //         [szValue] - д��ֵ
    // ����ֵ: [void] - ��
    //-----------------------------------------------------------------------------
    void IniFile::WriteString(LPCSTR szKey, LPCSTR szValue)
    {
        AASSERT(m_nCurrent >= 0);

        Iter it = m_vSection[m_nCurrent].itPos;	// ��ǰ�ο�ʼ��ָ��
        string strWrite = szKey;				// ��д������
        strWrite += "=";
        strWrite += szValue;

        while (it != m_lData.end())
        {
            string::size_type pos;

            // ȥ��ǰ���հ�
            pos = (*it).find_first_not_of(' ');

            // �հ���, ֱ������
            if (pos == string::npos)
            {
                ++it;
                continue;
            }

            // �ѵ��¸���, ����ʧ��
            if ((*it)[pos] == '[')
            {
                // �½�һ����, ����
                m_lData.insert(m_vSection[m_nCurrent].itPos, strWrite);
                return;
            }

            // ���Ҽ���
            if ((*it).substr(pos, strlen(szKey)) == szKey)
            {
                pos += strlen(szKey);

                // ȥ���Ⱥ�ǰ�հ�
                pos = (*it).find_first_not_of(' ', pos);

                // �ȺŴ���, �Ѿ��ҵ�ָ����
                if (pos != string::npos && (*it)[pos] == '=')
                {
                    (*it) = strWrite;	// ����ԭֵ
                    return;
                }
            }

            ++it;
        } // end while (it != m_lData.end())

        // ������β��û�ҵ�, �½�һ����
        m_lData.insert(m_vSection[m_nCurrent].itPos, strWrite);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::WriteInteger()
    // ��  ��: д����ֵ
    // ��  ��: [szSection] - ����
    //         [szKey] - ����
    //         [nValue] - д��ֵ
    // ����ֵ: [void] - ��
    //-----------------------------------------------------------------------------
    void IniFile::WriteInteger(LPCSTR szSection, LPCSTR szKey, int nValue)
    {
        SetSection(szSection);
        WriteInteger(szKey, nValue);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::WriteInteger()
    // ��  ��: д����ֵ, Ҫ���ȵ���SetSection()
    // ��  ��: [szKey] - ����
    //         [nValue] - д��ֵ
    // ����ֵ: [void] - ��
    //-----------------------------------------------------------------------------
    void IniFile::WriteInteger(LPCSTR szKey, int nValue)
    {
        char szBuffer[20];
        sprintf(szBuffer, "%d", nValue);

        WriteString(szKey, szBuffer);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::WriteFloat()
    // ��  ��: д����ֵ
    // ��  ��: [szSection] - ����
    //         [szKey] - ����
    //         [lfValue] - д��ֵ
    // ����ֵ: [void] - ��
    //-----------------------------------------------------------------------------
    void IniFile::WriteFloat(LPCSTR szSection, LPCSTR szKey, double lfValue)
    {
        SetSection(szSection);
        WriteFloat(szKey, lfValue);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::WriteFloat()
    // ��  ��: д����ֵ, Ҫ���ȵ���SetSection()
    // ��  ��: [szKey] - ����
    //         [lfValue] - д��ֵ
    // ����ֵ: [void] - ��
    //-----------------------------------------------------------------------------
    void IniFile::WriteFloat(LPCSTR szKey, double lfValue)
    {
        char szBuffer[20];
        sprintf(szBuffer, "%lf", lfValue);

        WriteString(szKey, szBuffer);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::WriteBoolean()
    // ��  ��: д����ֵ
    // ��  ��: [szSection] - ����
    //         [szKey] - ����
    //         [bValue] - д��ֵ
    // ����ֵ: [void] - ��
    //-----------------------------------------------------------------------------
    void IniFile::WriteBoolean(LPCSTR szSection, LPCSTR szKey, bool bValue)
    {
        SetSection(szSection);
        WriteBoolean(szKey, bValue);
    }

    //-----------------------------------------------------------------------------
    // ������: ASLIni::WriteBoolean()
    // ��  ��: д����ֵ, Ҫ���ȵ���SetSection()
    // ��  ��: [szKey] - ����
    //         [bValue] - д��ֵ
    // ����ֵ: [void] - ��
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