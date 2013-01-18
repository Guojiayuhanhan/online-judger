//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <cassert>

/** ����ָ��������*/
class  IBase
{
public:
    IBase(void) : m_bManaged_(true),m_nRefCounter_(0) { };
    virtual ~IBase(void) { }

    /** �������ü���*/
    void  addRef(void) { ++m_nRefCounter_; }

    /** �������ü���*/
    void  delRef(void)
    {
        --m_nRefCounter_;
        if (m_nRefCounter_<=0 && m_bManaged_) 
        {
            destroyThis();
        }
    }

    /** ������ü���*/
    int getRef(void){ return m_nRefCounter_; }

public://����

    /*������Դ����*/
    void setManaged(bool m){ m_bManaged_ = m; }
    bool getManaged(void){ return m_bManaged_ ; }
    virtual void destroyThis(void){ delete this; }

private:
    bool    m_bManaged_ ;   //�Զ�������Դ
    int     m_nRefCounter_ ;//���ü���
};


/*����ָ��*/
template<typename T>
class RefPtr 
{
public:
    RefPtr(T* ptr = 0)
    {
        m_ptr = NULL;
        *this = ptr;
    }

    RefPtr(const RefPtr<T>& ptr) 
    {
        m_ptr = NULL;
        *this = ptr;
    }

    ~RefPtr(void) 
    {
        if (m_ptr != NULL) 
        {
            m_ptr->delRef();
            m_ptr = NULL;
        }
    }

    RefPtr<T>& operator=(T* ptr) 
    {
        if (ptr != m_ptr) 
        {
            if (m_ptr != NULL) 
            {
                m_ptr->delRef();
            }
            m_ptr = ptr;
            if (m_ptr != NULL) 
            {
                m_ptr->addRef();
            }
        }
        return *this;
    }

    RefPtr<T>& operator=(const RefPtr<T>& ptr) 
    {
        *this = ptr.m_ptr;
        return *this;
    }


    //ǿ������ת��
    template<typename U>
    operator RefPtr<U>() 
    {
        return RefPtr<U>((U*)m_ptr);
    }

    T* operator->() const 
    {
        return m_ptr;
    }

    T& operator*() const 
    {
        return *m_ptr;
    }

    operator bool() const 
    {
        return (m_ptr != NULL);
    }

    T* get() const 
    {
        return m_ptr;
    }

private:
    T* m_ptr;
};


template<typename T, typename U>
bool operator==(const RefPtr<T>& a, const RefPtr<U>& b) 
{
    return (a.get() == b.get());
}

template<typename T>
bool operator==(const RefPtr<T>& a, const T* b)
{
    return (a.get() == b);
}

template<typename T>
bool operator==(const T* a, const RefPtr<T>& b) 
{
    return (a == b.get());
}


template<typename T, typename U>
bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b) 
{
    return (a.get() != b.get());
}

template<typename T>
bool operator!=(const RefPtr<T>& a, const T* b) 
{
    return (a.get() != b);
}

template<typename T>
bool operator!=(const T* a, const RefPtr<T>& b)
{
    return (a != b.get());
}
