//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "lzdParser.h"
#include "lzd.h"

#define PARSE_TEST

#ifdef PARSE_TEST
#include <iostream>
void testOutput(std::string str)
{
    //std::cout<<str<<std::endl;
    OutputDebugString(str.c_str());
}
#endif

bool isWhiteSpace(char ch)
{
    return (ch==' ' || ch=='\t' || ch=='\n' ||ch=='\r');
}

bool isVarNameValid(LPCSTR name)
{
    return true;
}

bool isVarNameValid(const std::string & name)
{
    return isVarNameValid(name.c_str());
}

//////////////////////////////////////////////////////////////////////////

cParser::cParser(std::string text, LZDataPtr curNode)
    : m_strText(text)
    , m_pCurNode(curNode)
{
    m_pText = m_strText.c_str();
    m_bEnd = false;
    m_varLine = 0;
    m_curLine = 0;
    m_errorID = 0;
}

cParser::~cParser(void)
{
}

std::string cParser::getText(void)
{ 
    return m_strText;
}

int cParser::getLines(void)
{
    return m_curLine;
}

void cParser::setVar(std::string name, int line /*= -1*/)
{
    m_variable = name;
    if (line != -1)
    {
        m_varLine = line;
    }
    else
    {
        m_varLine = m_curLine;
    }
}

bool cParser::error(void)
{
    return (m_errorID != 0);
}

int cParser::errorID(void)
{
    return m_errorID;
}


std::string cParser::errorDesc(void)
{
    return m_errorDesc;
}

void cParser::setError(int id_)
{
    m_errorID = id_;
}

void cParser::parse(void)
{
    m_bEnd = false;
    m_varLine = 0;
    m_curLine = 0;
    m_errorID = 0;
    while(!m_strText.empty() && !m_bEnd)
    {
        char ch = m_strText[0];
        m_strText = m_strText.substr(1);

        switch(ch)
        {
        case '{' :
            onLBraket();
            break;
        case '}' :
            onRBraket();
            break;
        case '=' :
            onEqual();
            break;
        case '\n' :
            onReturn();
            break;
        case ' ' :
        case '\t':
            onSpace();
            break;
        case '#' :
            onComment();
            break;
        case ';':
            break;
        default:
            onOther(ch);
            break;
        }
        if (error())
        {
            onError();
        }
    }
    onEnd();
}

void cParser::onLBraket(void)
{
    std::string name = parseVarName();
    m_curVariable.clear();
    m_variable.clear();
    if (error())
    {
        return;
    }

    LZDataPtr node = new lzd(name);
    m_pCurNode->addChild(node);
    cParser parser(m_strText, node);
    parser.parse();
    m_strText = parser.getText();
    m_curLine += parser.getLines();
}

void cParser::onRBraket(void)
{
    m_bEnd = true;
}

void cParser::onEqual(void)
{
    std::string name = parseVarName();
    m_curVariable.clear();
    m_variable.clear();
    if (error())
    {
        return;
    }
    
    m_pCurNode->write(name, getAStr());
}
void cParser::onReturn(void)
{
    std::string name = parseVarName();
    m_curVariable.clear();
    m_variable.clear();
    if (error())
    {
        setError(0);
        ++m_curLine;
        return;
    }
    else
    {
        setVar(name);
        ++m_curLine;
    }
}

void cParser::onSpace(void)
{
    std::string name;
    if (!m_variable.empty() && !m_curVariable.empty())//��������Ϊ��
    {
        if(m_varLine == m_curLine)
        {
            setError(2);//ͬһ���ϳ�����������
            return ;
        }
        else//��ͬ���ϵ���������
        {//������һ�����õı���
            setVar(m_curVariable);
            m_curVariable.clear();
        }
    }
    else if( m_variable.empty() && !m_curVariable.empty())
    {
        setVar(m_curVariable);
        m_curVariable.clear();
    }
    skipSpace();
}

void cParser::onComment(void)
{
    skipToReturn();
}

///�ֺ�
void cParser::onSemicolon(void)
{

}

void cParser::onOther(char ch)
{
    m_curVariable += ch;
    //testOutput(m_curVariable);
}

void cParser::onError(void)
{
    skipToEnd();
    char buffer[256];
    sprintf_s(buffer, 256, "�� %d : ",m_curLine+1);
    m_errorDesc = buffer;
    if (errorID() == 1)
    {
        m_errorDesc += "�ڳ��Ը�ֵ��ʱ��û�еõ��κοɸ�ֵ�ı�����";
    }
    else if (errorID() == 2)
    {
        m_errorDesc += "ͬһ���ϳ��������������� :";
        m_errorDesc += m_variable;
        m_errorDesc += " �� ";
        m_errorDesc += m_curVariable;
    }
    else if (errorID() == 3)
    {
        m_errorDesc += "�Ƿ���������";
    }
    else 
    {
        m_errorDesc += "δ֪����";
    }
    testOutput(m_errorDesc);
    m_curVariable.clear();
    m_variable.clear();
    setError(0);
}

void cParser::onEnd(void)
{

}
std::string cParser::parseVarName( void )
{
    std::string name;
    if (m_variable.empty() && m_curVariable.empty())
    {
        setError(1);
        return "";//û�б���
    }
    else if (!m_variable.empty() && !m_curVariable.empty())
    {
        if(m_varLine == m_curLine)
        {
            setError(2);//ͬһ���ϳ�����������
            return "";
        }
        else
        {
            name = m_curVariable; //������һ�����õı���
        }
    }
    else if( !m_variable.empty())
    {
        name = m_variable;
    }
    else
    {
        name = m_curVariable;
    }
    if(!isVarNameValid(name))
    {
        setError(3);
        return "";//���ƷǷ��ַ�
    }
    trimString(name);
    return name;
}

void cParser::skipSpace(void)
{
    size_t pos = 0;
    for( ; pos<m_strText.size(); ++pos)
    {
        if (m_strText[pos]!=' ' && m_strText[pos]!='\t' )
        {
            break;
        }
    }
    if (pos != 0)
    {
        m_strText = m_strText.substr(pos);
    }
}
void cParser::skipToEnd(void)
{
    size_t pos = 0;
    for( ; pos<m_strText.size(); ++pos)
    {
        if (m_strText[pos]=='\n' || m_strText[pos]=='}')
        {
            break;
        }
    }
    if (pos != 0)
    {
        m_strText = m_strText.substr(pos);
    }
}
void cParser::skipToReturn(void)
{
    size_t pos = 0;
    for( ; pos<m_strText.size(); ++pos)
    {
        if (m_strText[pos]=='\n')
        {
            break;
        }
    }
    if (pos != 0)
    {
        m_strText = m_strText.substr(pos);
    }
}

/** �ӵ�ǰλ���𣬷�����һ���Ϸ����ַ�����������������[}��#��\n����]λ�� ����
    */
std::string cParser::getAStr(void)
{
    std::string str = "";
    size_t i =0;
    for (; i<m_strText.size(); ++i)
    {
        char ch = m_strText[i];
        if (ch!='}' && ch!='#' && ch!='\n' && ch!=';') //������
        {
            if (ch == '\\')//ת���
            {
                if (i+1 < m_strText.size())
                {
                    char next = m_strText[i+1];
                    if (next=='}' || next=='#' || next==';' || next=='\\') //��ʱ'\'���Ž���ת��
                    {
                        ++i;
                    }
                }
            }
            str += m_strText[i];
        }
        else
        {
            break;
        }
    }
    if (i >= m_strText.size())
    {
        m_strText = "";
    }
    else
    {
        m_strText = m_strText.substr(i);
    }
    trimString(str);
    return str;
}