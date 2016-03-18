#pragma once

//-----------------------------------------------------------------------------
//
//    ____ Azure Star Game Engine ������Ϸ���� ____
//
//    Copyright (c) 2006, ���ǹ�����
//    All rights reserved.
//
//-----------------------------------------------------------------------------

#include <string>
#include <list>
#include <vector>

#define IniEmptyString ""

namespace G1
{

class IniFile
{
    // �νṹ�����Ͷ���
private:
	struct SectionInfo								// ����Ϣ�ṹ
	{
		std::string strName;						// ����
		std::list<std::string>::iterator itPos;		// ���������е�λ��
	};

	typedef std::list<std::string> List;			// ���string������
	typedef std::list<std::string>::iterator Iter;	// �����α�
	typedef std::vector<SectionInfo> Vect;			// ��Ŷ���Ϣ������

#ifndef LPCSTR
    #define  LPCSTR const char*
#endif // !LPCSTR


public:
    IniFile(void): m_nCurrent(-1) {}
    ~IniFile(void) {}

    // ����ini�ļ�
    bool Load(LPCSTR pFile);

    // ����ini�ļ�
    void Save(LPCSTR szFileName);

    // ���õ�ǰ��
    void SetSection(LPCSTR szSection);

    // ����Ϣ����
public:
    unsigned int GetSectionNumber() const { return m_vSection.size(); }
    const std::string& GetSectionName(unsigned int index);

    // ��ȡ��key����
    //int GetSectionKeyNumber(LPCSTR szSection);
    //int GetSectionKeyNumber();

    //std::string GetSectionKey(unsigned int index);
    //std::string GetSectionString(unsigned int index);

    bool HasKey(LPCSTR szSection, LPCSTR szKey);
    bool HasKey(LPCSTR szKey) const;

    // ���ַ���
    std::string ReadString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault);
    std::string ReadString(LPCSTR szKey, LPCSTR szDefault) const;

    // ������ֵ
    int ReadInteger(LPCSTR szSection, LPCSTR szKey, int nDefault);
    int ReadInteger(LPCSTR szKey, int nDefault) const;

    // ������ֵ
    float ReadFloat(LPCSTR szSection, LPCSTR szKey, float lfDefault);
    float ReadFloat(LPCSTR szKey, float lfDefault) const;

    // ������ֵ
    bool ReadBoolean(LPCSTR szSection, LPCSTR szKey, bool bDefault);
    bool ReadBoolean(LPCSTR szKey, bool bDefault) const;

    // д��Ϣ����
public:
    // д�ַ���
    void WriteString(LPCSTR szSection, LPCSTR szKey, LPCSTR szValue);
    void WriteString(LPCSTR szKey, LPCSTR szValue);

    // д����ֵ
    void WriteInteger(LPCSTR szSection, LPCSTR szKey, int nValue);
    void WriteInteger(LPCSTR szKey, int nValue);

    // д����ֵ
    void WriteFloat(LPCSTR szSection, LPCSTR szKey, double lfValue);
    void WriteFloat(LPCSTR szKey, double lfValue);

    // д����ֵ
    void WriteBoolean(LPCSTR szSection, LPCSTR szKey, bool bValue);
    void WriteBoolean(LPCSTR szKey, bool bValue);


    // ��Ա����
private:
    List m_lData;					// �ļ���������
    Vect m_vSection;				// ����Ϣ����
    int  m_nCurrent;				// ��ǰ��
    char m_szFileName[260];	        // �ļ���
};

}