//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once


typedef std::vector<tstring> StringPool;

bool fileExist(const tstring & name);

bool removeFile(const tstring & name);

bool makeDirDirect(const tstring & dir);

bool makeDir(const tstring & dir);

void formatSlash(tstring & name);

//��ʽ���ļ����е�б�ߡ�β��׷��б�ߡ�
void formatDirName(tstring & dirName, bool appendSlash=true);

//���·���е��ļ�������ȥ��·��������getFilePath�෴��
tstring getPathFile(const tstring & filename);

//����ļ����е�·������ȥ���ļ�������getPathFile�෴��
tstring getFilePath(const tstring & filename);

//����ļ�������ȥ����չ����
tstring getFileName(const tstring & filename);

//����ļ���չ��������'.'
tstring getFileExt(const tstring & filename);

//���Ҹ���Ŀ¼�µ��ļ�
bool findFiles(StringPool & vect, 
               const tstring & path, 
               const tstring & ext,
               bool unionPath=true);

//��ȡ�ļ�����
bool readFileBuffer(std::vector<TCHAR> & buffer, const tstring & fname, bool isBinary);

//////////////////////////////////////////////////////////////////////////
//�ļ�����
//////////////////////////////////////////////////////////////////////////
class cFindFile
{
public:
    cFindFile() : m_handle(NULL) {   }
    ~cFindFile(){ reset(); }

    //ִ�е�һ�β���
    bool findFirst(const tstring & name);

    //�������ҡ��������һ��findFirst�ſ��á�
    bool findNext();

    bool valid(){ return NULL != m_handle; }

    tstring fileName(){ return m_data.cFileName; }

    bool isDot()
    { 
        tstring fname = fileName();
        return fname==_T(".") || fname==_T(".."); 
    }

    bool isDir()
    {
        return (m_data.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) != 0;
    }

private:

    void reset();

private:
    WIN32_FIND_DATA m_data;
    HANDLE          m_handle;
};
