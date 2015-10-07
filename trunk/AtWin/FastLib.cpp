#include "FastLib.h"
#include <algorithm>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace FastLib
{

Task::Task(const unsigned int priority):m_priority(priority)
{

}

Task::~Task()
{

}

//////////////////////////////////////////////////////////////////////////
Kernel::Kernel()
{

}

Kernel::~Kernel()
{

}

void Kernel::PriorityAdd(Task* pTask)
{
    TaskListIterator it;
    TaskListIterator itEnd = m_tasks.end();
    for (it = m_tasks.begin(); it != itEnd; ++it)
    {
        Task* pCurTask = *it;
        if (pCurTask->Priority() > pTask->Priority())
        {
            break;
        }
    }
    m_tasks.insert(it, pTask);
}


void Kernel::Execute()
{
    while(!m_tasks.empty())
    {
        TaskListIterator itEnd = m_tasks.end();
        for (TaskListIterator it = m_tasks.begin(); it != itEnd; ++it)
        {
            Task* pTask = *it;
            if (pTask && !pTask->CanKill())
            {
                pTask->Update();
            }
        }

        for (TaskListIterator it = m_tasks.begin(); it != itEnd; )
        {
            Task* pTask = *it;
            ++it;
            if (pTask && pTask->CanKill())
            {
                pTask->Stop();
                m_tasks.remove(pTask);
                pTask = NULL;
            }
        }
    }
}

bool Kernel::AddTask(Task* pTask)
{
    if (pTask)
    {
        bool started = pTask->Start();
        if (started)
        {
            PriorityAdd(pTask);
        }
        return started;
    }
    return false;
}
    
void Kernel::SuspendTask(Task* pTask)
{
    if (std::find(m_tasks.begin(), m_tasks.end(), pTask) != m_tasks.end())
    {
        pTask->OnSuspend();
        m_tasks.remove(pTask);
        m_pausedTasks.push_back(pTask);
    }
}
   
void Kernel::ResumeTask(Task* pTask)
{
    if (std::find(m_tasks.begin(), m_tasks.end(), pTask) != m_tasks.end())
    {
        pTask->OnResume();
        m_tasks.remove(pTask);

        PriorityAdd(pTask);
    }
}
    
void Kernel::RemoveTask(Task* pTask)
{
    if (std::find(m_tasks.begin(), m_tasks.end(), pTask) != m_tasks.end())
    {
        m_tasks.remove(pTask);
    }

    if (std::find(m_pausedTasks.begin(), m_pausedTasks.end(), pTask) != m_pausedTasks.end())
    {
        m_pausedTasks.remove(pTask);
    }
}

void Kernel::KillAllTasks()
{
    for (TaskListIterator iter = m_tasks.begin(); iter != m_tasks.end(); ++iter)
    {
        (*iter)->SetCanKill(true);
    }
}


TimeUnits TimeTicksPerMillis;

Timer::Timer(const unsigned int priority):Task(priority)
{
    LARGE_INTEGER tps;
    QueryPerformanceFrequency(&tps);
    TimeTicksPerMillis = TimeUnits(tps.QuadPart / 1000L);
    m_simMultiplier = 1.0f;
}

Timer::~Timer()
{

}

TimeUnits Timer::nanoTime()
{
#ifdef _WIN32
    LARGE_INTEGER queryTime;
    QueryPerformanceCounter(&queryTime);
    return queryTime.QuadPart / TimeTicksPerMillis;
#else
    struct timespec queryTime;
    clock_gettime(CLOCK_MONOTONIC, &queryTime);
    return TimeUnits(1000 * queryTime.tv_sec) + TimeUnits(0.000001 * queryTime.tv_nsec);
#endif // _WIN32
}

float Timer::GetTimeFrame() const
{
    return m_frameDt;
}

float Timer::GetTimeSim() const
{
    return m_simDt;
}

void Timer::SetSimMultiplier( const float simMultiplier )
{
    m_simMultiplier = simMultiplier;
}

bool Timer::Start()
{
    return true;
}

void Timer::OnSuspend()
{

}

void Timer::Update()
{
    // Get the delta between the last frame and this
    TimeUnits currentTime = nanoTime();
    m_frameDt = (currentTime-m_timeLastFrame)*1.0f;
    m_timeLastFrame = currentTime;
    m_simDt = m_frameDt * m_simMultiplier;
}

void Timer::OnResume()
{
    m_timeLastFrame = nanoTime();
}

void Timer::Stop()
{

}



}