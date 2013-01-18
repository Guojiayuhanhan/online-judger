//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Comm.h"


//���ֽ��ַ���ת���ɿ��ַ��ַ���
std::wstring charToWChar(const std::string & str, DWORD code=CP_ACP);

//���ַ���ת���ɶ��ֽ��ַ���
std::string wcharToChar(const std::wstring & wstr, DWORD code=CP_ACP);

///�ı乤��·��
bool changeCurDirectory(LPCTSTR szPath = NULL);

//����guid
void generateGUID(tstring & guid);

//����guid
tstring generateGUID();

//����Ψһ������id���̰߳�ȫ��
int generateID();

//��string��ȡ��ch�Ժ�Ĳ���
void trimPartStringA(std::string & str, char ch='\0');

//��string��ȡ��ch�Ժ�Ĳ���
void trimPartStringW(std::wstring & str, wchar_t ch='\0');

//ȥ��stringβ�����ض��ַ��� filter : Ҫ���˵����ַ�����
void trimRStringA(std::string & str, const std::string & filter=" \t\r\n");

//ȥ��stringβ�����ض��ַ��� filter : Ҫ���˵����ַ�����
void trimRStringW(std::wstring & str, const std::wstring & filter=L" \t\r\n");

#ifdef _UNICODE
#   define trimRString trimRStringW
#   define trimPartString trimPartStringW
#else
#   define trimRString trimRStringA
#   define trimPartString trimPartStringA
#endif

//////////////////////////////////////////////////////////////////////////
//�ַ�������
//���ڽ��ַ�����ת��������������ֵ��
//////////////////////////////////////////////////////////////////////////
class StringTool
{
public:
    StringTool(){};
    StringTool(const std::string & v) : m_value(v) {}
    StringTool(const StringTool& st) : m_value(st.value()) {}

    const StringTool& operator=(const StringTool& st)
    {
        m_value = st.m_value;
        return *this;
    }

    const StringTool& operator=(std::string & v)
    {
        m_value = v;
        return *this;
    }

public:
    const std::string & value() const { return m_value; }
    const char* cvalue()const { return m_value.c_str(); }

    int asInt() const;
    bool asBool() const;
    float asFloat() const;
    double asDouble() const;
    const char* asText() const;
    const std::string& asString () const;
    std::string asStringEx () const;

    void setInt(int data);
    void setBool(bool data);
    void setFloat(float data);
    void setDouble(double data);
    void setText(const char* data);
    void setString(const std::string& data);

protected:
    std::string m_value;
};
