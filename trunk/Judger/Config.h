//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2013.01
 * 
 * This file is part of Online Judger for HUSTOJ. about HUSTOJ 
 * see <http://code.google.com/p/hustoj>.
 *
 * HUSTOJ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * HUSTOJ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HUSTOJ. if not, see <http://www.gnu.org/licenses/>.
 */
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "RefPtr.h"
#include "..\LZData\LZData.h"

//////////////////////////////////////////////////////////////////////////
//�������ò���
//////////////////////////////////////////////////////////////////////////
class Config : public IBase
{
public:
    Config();

    virtual bool loadConfig(const std::string & config);

    virtual bool loadConfig(LZDataPtr root);

public:
    //״̬����

    int JE_PENDING;             //����
    int JE_ACCEPT;              //����
    int JE_REJUDGE;             //�ȴ�����
    int JE_COMPLIE;             //�������
    int JE_WRONG_ANWSER;        //�𰸴���
    int JE_TIME_LIMITED;        //��ʱ
    int JE_MEMORY_LIMITED;      //�ڴ泬��
    int JE_PRESENT;             //��ʾ����
    int JE_SYSTEM;              //ϵͳ����
    int JE_RUNTIME;             //����ʱ����
    int JE_UNKNOWN;             //δ֪����

    //����������
    int CL_GPLUS;   //g++
    int CL_GCC;     //gcc
    int CL_JAVA;    //java

    int         cores;
    std::string testDataPath;

    std::string dbusername;
    std::string dbpassword;
    std::string dbip;
    int         dbport;
    std::string dbname;
};
