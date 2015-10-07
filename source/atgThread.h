#pragma once

class atgThread
{
public:
    atgThread(void);
    virtual ~atgThread(void);

    void            Start();

    void            Wait();

    bool            Wait(uint32 timeoutMillis);

    void            Terminate(uint32 exitCode = 0);

    void*           GetHandle();

protected:
    virtual int     Run() = 0;          

private:
#ifdef _WIN32
    static uint32 __stdcall ThreadFunction(void *pV);
#else
    static void* ThreadFunction(void *pV);
#endif // _WIN32
    
    void*           m_handle;

private:
    // No copies do not implement
    atgThread(const atgThread &rhs);
    atgThread &operator=(const atgThread &rhs);
};

inline atgThread::atgThread():m_handle(0)
{
#ifdef _WIN32
    HANDLE* handle = (HANDLE*)&m_handle;
    *handle = INVALID_HANDLE_VALUE;
#else

#endif // _WIN32
}

inline atgThread::~atgThread()
{
#ifdef _WIN32
    if ((HANDLE)m_handle != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle((HANDLE)m_handle);
    }
#else
    if (m_handle)
    {
        pthread_t* m_handle = (pthread_t*)m_handle;
        delete m_handle;
    }
#endif // _WIN32
}

inline void atgThread::Start()
{
#ifdef _WIN32
    if ((HANDLE)m_handle == INVALID_HANDLE_VALUE)
    {
        unsigned int threadID = 0;

        m_handle = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (void*)this, 0, &threadID);

        if (m_handle == INVALID_HANDLE_VALUE)
        {
            AASSERT(0);
        }
    }
    else
    {

    }
#else
    if (m_handle == NULL)
    {
        m_handle = new pthread_t;
        pthread_attr_t type;
        pthread_attr_init(&type);
        pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);
        pthread_create((pthread_t*)m_handle, &type, ThreadFunction, this);
    }
#endif // _WIN32

}

void atgThread::Wait()
{
#ifdef _WIN32
    if (!Wait((uint32)INFINITE))
    {

    }
#endif // _WIN32
}

bool atgThread::Wait(uint32 timeoutMillis)
{
#ifdef _WIN32
    bool ok = false;

    DWORD result = ::WaitForSingleObject(m_handle, timeoutMillis);
    if (result == WAIT_TIMEOUT)
        ok = false;
    else if (result == WAIT_OBJECT_0)
        ok = true;
    else
    {

    }
    return ok;
#else
    pthread_t* thread = (pthread_t*)m_handle;
    if (thread)
        return (0 == pthread_join(*thread, 0));
#endif // _WIN32
}

void atgThread::Terminate(uint32 exitCode)
{
#ifdef _WIN32
    ::TerminateThread((HANDLE)m_handle, exitCode);
#else
    pthread_exit(0);
#endif // _WIN32
}

#ifdef _WIN32
inline uint32 __stdcall atgThread::ThreadFunction(void *pV)
#else
inline void* atgThread::ThreadFunction(void *pV)
#endif // _WIN32
{
    static int result = 0;
    atgThread* pThis = (atgThread*)pV;

    if (pThis)
    {
        try
        {
            result = pThis->Run();
        }
        catch(...)
        {
        }
    }

#ifdef _WIN32
    return result;
#else
    return &result;
#endif // _WIN32
    
}

//////////////////////////////////////////////////////////////////////////
// sample
//int _tmain(int argc, _TCHAR* argv[])
//{
//
//  // class MyThread : public IThread
//
//  const int count = 10;
//  MyThread threadArray[count];
//
//  for (int i = 0; i < count; ++i)
//  {
//      threadArray[i].Start();
//  }
//
//  for (int i = 0; i < count; ++i)
//  {
//      threadArray[i].Wait();
//  }
//
//  return 0;
//}
