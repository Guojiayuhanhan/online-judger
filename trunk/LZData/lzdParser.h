//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "LZData.h"

///�﷨��������������һ��ģ�飬���ӵ�ǰλ���𣬵���һ����}����ֹ
class cParser : public IBase
{
public:
    /** ���캯����
        @param text : �������ı���
        @pram curNode : ��ǰģ���㡣
        */
    cParser(std::string text, LZDataPtr curNode);
    ~cParser(void);

    ///��ʼ����
    void parse(void);

    ///��ø�ģ���������ı�
    std::string getText(void);

    ///����к�
    int getLines(void);

    ///�Ƿ��д���
    bool error(void);

    ///������
    int errorID(void);

    ///��������
    std::string errorDesc(void);

protected:
    ///�������
    void onLBraket(void);

    ///�һ�����
    void onRBraket(void);

    ///�Ⱥ�
    void onEqual(void);

    ///�հ׷�
    void onSpace(void);

    ///�س���
    void onReturn(void);

    ///ע�͡�#��
    void onComment(void);

    ///�ֺ�
    void onSemicolon(void);

    ///�����ַ�
    void onOther(char ch);

    ///������
    void onError(void);

    ///������
    void onEnd(void);


    ///�����հ׷�
    void skipSpace(void);

    ///��������β
    void skipToEnd(void);

    ///������β
    void skipToReturn(void);

    ///�ӵ�ǰλ���𣬻��һ���Ϸ��ַ�����
    std::string getAStr(void);

    ///���ñ�����
    void setVar(std::string name, int line = -1);

    ///����������
    std::string parseVarName( void );

    ///���ô���
    void setError(int id_);

private:
    std::string     m_strText;
    LPCSTR          m_pText;
    LZDataPtr    m_pCurNode;
    bool            m_bEnd;

    int             m_errorID;
    std::string     m_errorDesc;

    std::string     m_variable;
    int             m_varLine;
    std::string     m_curVariable;
    int             m_curLine;
};
