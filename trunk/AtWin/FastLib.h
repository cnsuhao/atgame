#pragma once

#include <list>

namespace FastLib
{
    class Task
    {
    private:
        unsigned int       m_priority;  // 任务级别
        bool               m_canKill;

    public:
        explicit Task(const unsigned int priority);
        virtual ~Task();

        virtual bool        Start()              = 0;
        virtual void        OnSuspend()          = 0;
        virtual void        Update()             = 0;
        virtual void        OnResume()           = 0;
        virtual void        Stop()               = 0;

        void                SetCanKill(const bool canKill)  { m_canKill = canKill; }
        bool                CanKill() const                 { return m_canKill; }
        unsigned int        Priority() const                { return m_priority; }   
    };

    class Kernel
    {
    private:
        typedef std::list<Task*>                 TaskList;
        typedef std::list<Task*>::iterator       TaskListIterator;

        TaskList            m_tasks;
        TaskList            m_pausedTasks;

        void                PriorityAdd(Task* pTask);

    public:
        Kernel();
        virtual ~Kernel();

        void                Execute();

        bool                AddTask(Task* pTask);
        void                SuspendTask(Task* task);
        void                ResumeTask(Task* task);
        void                RemoveTask(Task* task);
        void                KillAllTasks();

        bool                HasTasks()       { return !m_tasks.empty(); }
    };

    typedef long long   TimeUnits;
    class Timer : public Task
    {
    private:
        TimeUnits           nanoTime();

        TimeUnits           m_timeLastFrame;
        float               m_frameDt;
        float               m_simDt;
        float               m_simMultiplier;

    public:
        Timer(const unsigned int priority);
        ~Timer();

        float               GetTimeFrame() const;
        float               GetTimeSim() const;
        void                SetSimMultiplier(const float simMultiplier);

        virtual bool        Start();
        virtual void        OnSuspend();
        virtual void        Update();
        virtual void        OnResume();
        virtual void        Stop();
    };


    class Geometry
    {
    private:
        static const unsigned int NAME_MAX_LENGTH = 32;

        char                m_name[NAME_MAX_LENGTH];
        int                 m_numVertices;
        int                 m_numIndices;
        void*               m_pVertices;
        void*               m_pIndices;

        int                 m_numVertexPositionElements;
        int                 m_numColorElements;
        int                 m_numTexCoordElements;
        int                 m_vertexStride;

    public:
        Geometry();
        virtual ~Geometry();

        void                SetName(const char* name)               { strcpy(m_name, name); }
        const char*         GetName() const                         { return m_name; }
        void                SetNumVertices(const int numVertices)   { m_numVertices = numVertices; }
        const int           GetNumVertices() const                  { return m_numVertices; }
        void                SetNumIndices(const int numIndices)     { m_numIndices = numIndices; }
        const int           GetNumIndices() const                   { return m_numIndices; }
    
        void                SetVertexBuffer(void* pVertices)        { m_pVertices = pVertices; }
        void*               GetVertexBuffer() const                 { return m_pVertices; }
        void                SetIndexBuffer(void* pIndices)          { m_pIndices = pIndices; }
        void*               GetIndexBuffer() const                  { return m_pIndices; }

        void                SetNumVertexPositionElements(const int numVertexPositionElements);
        int                 GetNumVertexPositionElements() const    { return m_numVertexPositionElements; }
        void                SetNumColorElements(const int numColorElements);
        int                 GetNumColorElements() const             { return m_numColorElements; }
        void                SetNumTexCoordElements(const int numTexCoordElements);
        int                 GetNumTexCoordElements() const          { return m_numTexCoordElements; }

        void                SetVertexStride(const int vertexStride) { m_vertexStride = vertexStride; }
        int                 GetVertexStride() const                 { return m_vertexStride; }
    };

    inline void Geometry::SetNumVertexPositionElements(const int numVertexPositionElements)
    {
        m_numVertexPositionElements = numVertexPositionElements;
    }
    inline void Geometry::SetNumTexCoordElements(const int numTexCoordElements)
    {
        m_numTexCoordElements = numTexCoordElements;
    }
    inline void Geometry::SetNumColorElements(const int numColorElements)
    {
        m_numColorElements = numColorElements;
    }

    class Shader
    {
    private:
        //void LoadShader(GLenum shaderType, std::string& shaderCode);

    protected:
        //GLuint               m_vertexShaderId;
        //GLuint               m_fragmentShaderId;
        //GLint                m_programId;

        std::string          m_vertexShaderCode;
        std::string          m_fragmentShaderCode;

        bool                 m_isLinked;

    public:
        Shader();
        virtual ~Shader();

        virtual void        Link();
        virtual void        Setup(class Renderable& renderable);

        bool                IsLinked()      { return m_isLinked; }
    };

    class Renderable
    {
    private:
        Geometry*            m_pGeometry;
        Shader*              m_pShader;
    public:
        Renderable();
        ~Renderable();
        void                 SetGeometry(Geometry* pGeometry);
        Geometry*            GetGeometry();

        void                 SetShader(Shader* pShader);
        Shader*              GetShader();
    };

	inline void Renderable::SetGeometry(Geometry* pGeometry)
	{
		m_pGeometry = pGeometry;
	}

	inline Geometry* Renderable::GetGeometry()
	{
		return m_pGeometry;
	}

	inline void Renderable::SetShader(Shader* pShader)
	{
		m_pShader = pShader;
	}

	inline Shader* Renderable::GetShader()
	{
		return m_pShader;
	}
}




