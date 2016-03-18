#pragma once

//-----------------------------------------------------------------------------
//
//    ____ Azure Star Game Engine 蓝星游戏引擎 ____
//
//    Copyright (c) 2006, 蓝星工作室
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
    // 段结构级类型定义
private:
	struct SectionInfo								// 段信息结构
	{
		std::string strName;						// 段名
		std::list<std::string>::iterator itPos;		// 段在链表中的位置
	};

	typedef std::list<std::string> List;			// 存放string的链表
	typedef std::list<std::string>::iterator Iter;	// 链表游标
	typedef std::vector<SectionInfo> Vect;			// 存放段信息的数组

#ifndef LPCSTR
    #define  LPCSTR const char*
#endif // !LPCSTR


public:
    IniFile(void): m_nCurrent(-1) {}
    ~IniFile(void) {}

    // 加载ini文件
    bool Load(LPCSTR pFile);

    // 保存ini文件
    void Save(LPCSTR szFileName);

    // 设置当前段
    void SetSection(LPCSTR szSection);

    // 读信息函数
public:
    unsigned int GetSectionNumber() const { return m_vSection.size(); }
    const std::string& GetSectionName(unsigned int index);

    // 读取段key数量
    //int GetSectionKeyNumber(LPCSTR szSection);
    //int GetSectionKeyNumber();

    //std::string GetSectionKey(unsigned int index);
    //std::string GetSectionString(unsigned int index);

    bool HasKey(LPCSTR szSection, LPCSTR szKey);
    bool HasKey(LPCSTR szKey) const;

    // 读字符串
    std::string ReadString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault);
    std::string ReadString(LPCSTR szKey, LPCSTR szDefault) const;

    // 读整型值
    int ReadInteger(LPCSTR szSection, LPCSTR szKey, int nDefault);
    int ReadInteger(LPCSTR szKey, int nDefault) const;

    // 读浮点值
    float ReadFloat(LPCSTR szSection, LPCSTR szKey, float lfDefault);
    float ReadFloat(LPCSTR szKey, float lfDefault) const;

    // 读布尔值
    bool ReadBoolean(LPCSTR szSection, LPCSTR szKey, bool bDefault);
    bool ReadBoolean(LPCSTR szKey, bool bDefault) const;

    // 写信息函数
public:
    // 写字符串
    void WriteString(LPCSTR szSection, LPCSTR szKey, LPCSTR szValue);
    void WriteString(LPCSTR szKey, LPCSTR szValue);

    // 写整型值
    void WriteInteger(LPCSTR szSection, LPCSTR szKey, int nValue);
    void WriteInteger(LPCSTR szKey, int nValue);

    // 写浮点值
    void WriteFloat(LPCSTR szSection, LPCSTR szKey, double lfValue);
    void WriteFloat(LPCSTR szKey, double lfValue);

    // 写布尔值
    void WriteBoolean(LPCSTR szSection, LPCSTR szKey, bool bValue);
    void WriteBoolean(LPCSTR szKey, bool bValue);


    // 成员变量
private:
    List m_lData;					// 文件内容链表
    Vect m_vSection;				// 段信息数组
    int  m_nCurrent;				// 当前段
    char m_szFileName[260];	        // 文件名
};

}