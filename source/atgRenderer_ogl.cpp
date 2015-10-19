#include "atgBase.h"
#include "atgRenderer.h"
#include "atgGame.h"
#include "atgMisc.h"
#include "atgProfile.h"

#ifdef USE_OPENGL

inline bool IsSpace(char character)
{
    if (character == ' ' || character == '\t' || character == '\r')
    {
        return true;
    }
    return false;
}

bool ReadShadeFile(GLchar** shadeCode, const char* shadeFile)
{
    atgReadFile reader;
    long fileSize;
    GLchar lineBuffer[512];
    GLchar* lineBufferPtr = lineBuffer;
    GLchar* destCode;
    if(reader.Open(shadeFile))
    {
        fileSize = reader.GetLength();
        if (fileSize <= 0)
            return false;
        (*shadeCode) = new GLchar[fileSize + 1];
        reader.Read((*shadeCode), fileSize);
        (*shadeCode)[fileSize] = '\0';
        destCode = (*shadeCode);

        std::string tempShaderCode;
        std::string Line = "";
        int codeIndex = -1;
        bool fileEnd = false;
        while(!fileEnd)
        {
            Line.clear();
            for(++codeIndex;destCode[codeIndex] != '\n'; ++codeIndex)
            {
                Line += destCode[codeIndex];
                if (codeIndex >= fileSize)
                {
                    fileEnd = true;
                    break;
                }
            }

            size_t length = Line.length();
            size_t index;
            for (index = 0; index < length && IsSpace(Line[index]); index++)
                ;
            if (index >= length)
                continue;
            if (Line[index] == '/' && ((index + 1) < length) && Line[index + 1] == '/')
                continue;
            tempShaderCode += Line.substr(index) + '\n';
        }
        size_t codeSize = tempShaderCode.size();
        *shadeCode = new GLchar[codeSize + 1];
        memcpy(*shadeCode, tempShaderCode.c_str(), codeSize);
        (*shadeCode)[codeSize] = '\0';
        reader.Close();
        LOG("%s\n", *shadeCode);
        return true;
    }
    //std::string tempShaderCode;
    //std::ifstream VertexShaderStream (shadeFile, std::ios::in);
    //if(VertexShaderStream.is_open())
    //{
    //    std::string Line = "";
    //    while(getline(VertexShaderStream, Line))
    //    {
    //        size_t length = Line.length();
    //        size_t index;
    //        for (index = 0; index < length && IsSpace(Line[index]); index++)
    //            ;
    //        if (index >= length)
    //          continue;
    //        if (Line[index] == '/' && ((index + 1) < length) && Line[index + 1] == '/')
    //            continue;
    //        tempShaderCode += '\n' + Line.substr(index);
    //    }
    //    VertexShaderStream.close();
    //    size_t codeSize = tempShaderCode.size();
    //    *shadeCode = new GLchar[codeSize + 1];
    //    memcpy(*shadeCode, tempShaderCode.c_str(), codeSize);
    //    (*shadeCode)[codeSize] = '\0';
    //    return true;
    //}
    return false;
}

//////////////////////////////////////////////////////////////////////////
// index buffer implement
class atgIndexBufferImpl
{
public:
    atgIndexBufferImpl():vbo_indicesID(0),accessMode(BAM_Static),locked(false) {}
    bool Bind();
    bool Unbind();
public:
    GLuint vbo_indicesID;
    BufferAccessMode accessMode;
    bool locked;
};

bool atgIndexBufferImpl::Bind()
{
    GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indicesID) );
    return true;
}

bool atgIndexBufferImpl::Unbind()
{
    GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    return true;
}

atgIndexBuffer::atgIndexBuffer():_impl(NULL)
{
}

atgIndexBuffer::~atgIndexBuffer()
{
}

bool atgIndexBuffer::Create( const void *pIndices, uint32 numIndices, IndexFormat format, BufferAccessMode accessMode )
{
    int indexSize = sizeof(uint16);
    int bufSize = numIndices * indexSize;
    _impl = new atgIndexBufferImpl;
    _impl->accessMode = accessMode;

    GL_ASSERT( glGenBuffers(1, &(_impl->vbo_indicesID)) );
    GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (_impl->vbo_indicesID)) );
    GL_ASSERT( glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufSize, pIndices, (accessMode == BAM_Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ) );
    _size = bufSize;

    GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    return true;
}

bool atgIndexBuffer::Destroy()
{
    if(_impl)
    {
        glDeleteBuffers(1, &_impl->vbo_indicesID);
        SAFE_DELETE(_impl);
    }
    return true;
}

void* atgIndexBuffer::Lock( uint32 offset, uint32 size )
{
    return NULL;
}

void atgIndexBuffer::Unlock()
{
    if(_impl->locked)
        _impl->locked = false;
}

bool atgIndexBuffer::IsLocked() const
{
    if(_impl)
        return _impl->locked;
    return false;
}

//////////////////////////////////////////////////////////////////////////
// Vertex Declaration implement
class atgVertexDeclImpl
{
public:
    atgVertexDeclImpl():numberElements(0),stride(0) {}
    atgVertexDeclImpl(const atgVertexDeclImpl& other)
    {
        numberElements = other.numberElements;
        stride = other.stride;
        for (uint8 i = 0; i < numberElements; ++i)
        {
            element[i].streamIndex = other.element[i].streamIndex;
            element[i].attribute = other.element[i].attribute;
        }
    }
public:
    uint8 numberElements;
    uint8 stride;
    typedef struct DeclarationElement_t
    {
        uint32 streamIndex;
        atgVertexDecl::VertexAttribute attribute;
        DeclarationElement_t():streamIndex(0),attribute(atgVertexDecl::VA_Max){}
    }DeclarationElement;
    DeclarationElement element[atgVertexDecl::MaxNumberElement];
};

atgVertexDecl::atgVertexDecl()
{
    _impl = new atgVertexDeclImpl();
}

atgVertexDecl::atgVertexDecl(const atgVertexDecl& other):_impl(NULL)
{
    *this = other;
}

atgVertexDecl& atgVertexDecl::operator= (const atgVertexDecl& other)
{
    SAFE_DELETE(_impl);
    _impl = new atgVertexDeclImpl();
    *_impl = *other._impl;
    return *this;
}

atgVertexDecl::~atgVertexDecl()
{
    SAFE_DELETE(_impl);
}

uint8 atgVertexDecl::GetNumberElement() const
{
    return _impl->numberElements;
}

uint8 atgVertexDecl::GetElementsStride() const
{
    return _impl->stride;
}

bool atgVertexDecl::AppendElement( uint32 streamIndex, atgVertexDecl::VertexAttribute attribute )
{
    int numberElements = _impl->numberElements;
    if ((numberElements + 1) >= MaxNumberElement)
    {
        LOG("%s numberElements >= %d", __FUNCTION__, MaxNumberElement);
        return false;
    }

    _impl->element[numberElements].streamIndex = streamIndex;
    _impl->element[numberElements].attribute = attribute;
    switch(attribute)
    {
    case atgVertexDecl::VA_Pos3:
    case atgVertexDecl::VA_Normal:
    case atgVertexDecl::VA_Diffuse:
        {
            _impl->stride += 12;
        }break;
    case atgVertexDecl::VA_Pos4:
    case atgVertexDecl::VA_BlendFactor4:
        {
            _impl->stride += 16;
        }break;
    case atgVertexDecl::VA_Specular:
        {
            _impl->stride += 12;
        }break;
    case atgVertexDecl::VA_Texture0:
    case atgVertexDecl::VA_Texture1:
    case atgVertexDecl::VA_Texture2:
    case atgVertexDecl::VA_Texture3:
    case atgVertexDecl::VA_Texture4:
    case atgVertexDecl::VA_Texture5:
    case atgVertexDecl::VA_Texture6:
    case atgVertexDecl::VA_Texture7:
        {
            _impl->stride += 8;
        }break;
    default:
        {
            AASSERT(0 && "error declararion type.\n");
        }break;
    }
    ++(_impl->numberElements);
    return true;
}

atgVertexDecl::VertexAttribute atgVertexDecl::GetIndexElement(uint32 streamIndex, uint32 index) const
{
    return _impl->element[index].attribute;
}

//////////////////////////////////////////////////////////////////////////
class atgPassImpl
{
public:
    atgPassImpl():programID(0){}
    ~atgPassImpl();

    void Bind();
    void Unbind();
public:
    GLuint programID;
};

void atgPassImpl::Bind()
{
    GL_ASSERT( glUseProgram(programID) );
}

void atgPassImpl::Unbind()
{
    GL_ASSERT( glUseProgram(0) );
}

//////////////////////////////////////////////////////////////////////////
// vertex buffer
class atgVertexBufferImpl
{
public:
    atgVertexBufferImpl():vbo_vertexbufferID(0),accessMode(BAM_Static),locked(false)
    {
#ifndef OPENGL_USE_MAP
        pLockBuffer = NULL;
        lockBufferSize = 0;
        offsetLockBuffer = 0;
#endif
    }
    ~atgVertexBufferImpl(){}
    void Bind(GLuint programID);
    void Unbind();
public:
    atgVertexDecl decl;
    GLuint vbo_vertexbufferID;
    BufferAccessMode accessMode;
    bool locked;
#ifndef OPENGL_USE_MAP
    char* pLockBuffer;
    GLsizei lockBufferSize;
    GLint offsetLockBuffer;
#endif
};

void atgVertexBufferImpl::Bind(GLuint programID)
{
    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexbufferID) );
    GLint index = -1;
    GLint size = 0;
    GLenum type = GL_FLOAT;
    GLboolean normalized = GL_FALSE;
    GLuint offset = 0;
    uint32 numberElement = decl.GetNumberElement();
    GLuint stride = decl.GetElementsStride();
    for (uint32 i = 0; i < numberElement; ++i)
    {
        atgVertexDecl::VertexAttribute attribute = decl.GetIndexElement(0, i);
        switch(attribute)
        {
        case atgVertexDecl::VA_Pos3:
            {
                GL_ASSERT( index = glGetAttribLocation(programID, "vs_vertexPosition") );
                size = 3;
                break;
            }
        case atgVertexDecl::VA_Normal:
            {
                GL_ASSERT( index = glGetAttribLocation(programID, "vs_vertexNormal") );
                size = 3;
                break;
            }
        case atgVertexDecl::VA_Pos4:
            {
                GL_ASSERT( index = glGetAttribLocation(programID, "vs_vertexPosition") );
                size = 4;
                break;
            }
        case atgVertexDecl::VA_Diffuse:
            {
                GL_ASSERT( index = glGetAttribLocation(programID, "vs_vertexColor") );
                size = 3;
                break;
            }
        case atgVertexDecl::VA_Specular:
            {
                GL_ASSERT( index = glGetAttribLocation(programID, "vs_vertexSpecular") );
                size = 4;
                break;
            }
        case atgVertexDecl::VA_Texture0:
            {
                GL_ASSERT( index = glGetAttribLocation(programID, "vs_textureCoord") );
                size = 2;
                break;
            }
        case atgVertexDecl::VA_Texture1:
            {
                GL_ASSERT( index = glGetAttribLocation(programID, "vs_textureCoord1") );
                size = 2;
                break;
            }
        case atgVertexDecl::VA_Texture2:
        case atgVertexDecl::VA_Texture3:
        case atgVertexDecl::VA_Texture4:
        case atgVertexDecl::VA_Texture5:
        case atgVertexDecl::VA_Texture6:
        case atgVertexDecl::VA_Texture7:
            {
                size = 2;
                break;
            }
        case atgVertexDecl::VA_PointSize:
        case atgVertexDecl::VA_BlendFactor4:
        default:
            {
                AASSERT(0 && "error declararion type.\n");
                break;
            }
        }
        if(index != -1)
        {
            GL_ASSERT( glEnableVertexAttribArray(index) );
            GL_ASSERT( glVertexAttribPointer( index,
                size, type, normalized, stride, (const void*)offset) );
        }
        offset += (size * sizeof(float));
    }
}

void atgVertexBufferImpl::Unbind()
{
    uint32 numberElement = decl.GetNumberElement();
    for (uint32 i = 0; i < numberElement; ++i)
    {
        GL_ASSERT( glDisableVertexAttribArray( (GLuint)i ) );
    }
    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );
}


atgVertexBuffer::atgVertexBuffer()
{
    _impl = new atgVertexBufferImpl;
    _size = 0;
}

atgVertexBuffer::~atgVertexBuffer()
{
    Destroy();
}

bool atgVertexBuffer::Create( atgVertexDecl* decl, const void *pData, uint32 size, BufferAccessMode accessMode )
{
    _impl->decl = *decl;
    _impl->accessMode = accessMode;

    // generate VBO
    GL_ASSERT( glGenBuffers(1, &(_impl->vbo_vertexbufferID)) );
    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _impl->vbo_vertexbufferID) );
    GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, size, pData, (accessMode == BAM_Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
    _size = size;

    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0));
    return true;
}

bool atgVertexBuffer::Destroy()
{
    AASSERT(_impl);
    GL_ASSERT( glDeleteBuffers(1, &_impl->vbo_vertexbufferID) );
    SAFE_DELETE(_impl);
    return true;
}

void* atgVertexBuffer::Lock( uint32 offset, uint32 size )
{
#ifdef OPENGL_USE_MAP
    if (_impl)
    {
        if (!_impl->locked)
        {
            GLvoid* buffer = NULL;
            GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _impl->vbo_vertexbufferID) );
            GL_ASSERT( buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) );
            _impl->locked = true;
            return buffer;
        }
    }
#else
    SAFE_DELETE_ARRAY(_impl->pLockBuffer);
    _impl->pLockBuffer = new char[size];
    _impl->lockBufferSize = size;
    _impl->offsetLockBuffer = offset;
    _impl->locked = true;
    return (void*)_impl->pLockBuffer;
#endif
    return NULL;
}

void atgVertexBuffer::Unlock()
{
#ifdef OPENGL_USE_MAP
    if (_impl)
    {
        if (_impl->locked)
        {
            GL_ASSERT( glUnmapBuffer(GL_ARRAY_BUFFER) );
            _impl->locked = false;
        }
    }
#else
    if (_impl)
    {
        if (_impl->locked)
        {
            if(_impl->pLockBuffer != NULL)
            {
                GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _impl->vbo_vertexbufferID) );
                GL_ASSERT( glBufferSubData(GL_ARRAY_BUFFER, _impl->offsetLockBuffer, _impl->lockBufferSize, (GLvoid*)_impl->pLockBuffer) );
                SAFE_DELETE_ARRAY(_impl->pLockBuffer);
            }
            _impl->locked = false;
        }
    }
#endif
}

bool atgVertexBuffer::IsLocked() const
{
    if(_impl)
        return _impl->locked;
    return false;
}

atgResourceShader::atgResourceShader(ResourceShaderType Type)
{
    type = Type;
}

atgResourceShader::~atgResourceShader()
{
}

class atgVertexShaderImpl
{
public:
    atgVertexShaderImpl():vertexShaderID(0),vertexShaderCode(0){}
    ~atgVertexShaderImpl()
    {
        if (vertexShaderCode)
        {
            delete [] vertexShaderCode;
        }
    }
public:
    GLuint vertexShaderID;
    GLchar* vertexShaderCode;
};

atgVertexShader::atgVertexShader():atgResourceShader(RST_Vertex),_impl(NULL)
{
}

atgVertexShader::~atgVertexShader()
{
    if (_impl)
    {
        if(_impl->vertexShaderID != 0)
        {
            GL_ASSERT(glDeleteShader(_impl->vertexShaderID));
        }
        delete _impl;
    }
}

bool atgVertexShader::LoadFromFile(const char* SourceFilePath)
{
    if (!SourceFilePath)
    {
        return false;
    }
    //if (MediaIsExist(Source))
    //{
    //    return false;
    //}
    if(!_impl)
    {
        _impl = new atgVertexShaderImpl;
    }
    bool result;
    // Read the Vertex Shader code from the file
    result = ReadShadeFile(&(_impl->vertexShaderCode), SourceFilePath);
    if(!result)
    {
        LOG("Load vertex shader failure: %s\n", SourceFilePath);
        AASSERT(0);
        return false;
    }
    return Compile();
}

bool atgVertexShader::LoadFromSource( const char* Source )
{
    if(!_impl)
    {
        _impl = new atgVertexShaderImpl;
    }
    if(!Source)
    {
        return false;
    }
    _impl->vertexShaderCode = (GLchar*)Source;
    return Compile();
}

bool atgVertexShader::Compile()
{
    if(!_impl || !_impl->vertexShaderCode)
    {
        return false;
    }
    GL_ASSERT( _impl->vertexShaderID = glCreateShader(GL_VERTEX_SHADER) );
    GLint compileResult;
    int infoLogLength;
    // Compile Vertex Shader
    //printf("Compiling shader : %s\n", SourceFilePath);
    const GLchar* vertexShaderCodeCst = _impl->vertexShaderCode;
    GL_ASSERT( glShaderSource(_impl->vertexShaderID, 1, &vertexShaderCodeCst, NULL) );
    GL_ASSERT( glCompileShader(_impl->vertexShaderID) );

    // Check Vertex Shader
    GL_ASSERT( glGetShaderiv(_impl->vertexShaderID, GL_COMPILE_STATUS, &compileResult) );
    if(compileResult != GL_TRUE)
    {
        LOG("Compile fail. compileResult=%d\n", compileResult);
        GL_ASSERT( glGetShaderiv(_impl->vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength) );
        if ( infoLogLength > 0 ){
            GLchar* errorInfo = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(_impl->vertexShaderID, infoLogLength, NULL, errorInfo);
            LOG("%s\n", errorInfo);
            delete [] errorInfo;
        }
        return false;
    }
    LOG("VertexShader Compile success. compileResult=%d\n", compileResult);
    return true;
}

class atgFragmentShaderImpl
{
public:
    atgFragmentShaderImpl():fragmentShaderID(0),fragmentShaderCode(0){}
    ~atgFragmentShaderImpl()
    {
        if (fragmentShaderCode)
        {
            delete [] fragmentShaderCode;
        }
    }
public:
    GLuint fragmentShaderID;
    GLchar* fragmentShaderCode;
};

atgFragmentShader::atgFragmentShader():atgResourceShader(RST_Fragment),_impl(NULL)
{
}

atgFragmentShader::~atgFragmentShader()
{
    if (_impl)
    {
        if(_impl->fragmentShaderID != 0)
        {
            GL_ASSERT(glDeleteShader(_impl->fragmentShaderID));
        }
        delete _impl;
    }
}

bool atgFragmentShader::LoadFromFile(const char* SourceFilePath)
{
    if (!SourceFilePath)
    {
        return false;
    }
    //if (MediaIsExist(Source))
    //{
    //    return false;
    //}
    if(!_impl)
    {
        _impl = new atgFragmentShaderImpl;
    }
    bool result;
    // Read the Vertex Shader code from the file
    result = ReadShadeFile(&(_impl->fragmentShaderCode), SourceFilePath);
    if(!result)
    {
        LOG("Load vertex shader failure: %s\n", SourceFilePath);
        AASSERT(0);
        return false;
    }
    return Compile();
}

bool atgFragmentShader::LoadFormSource( const char* Source )
{
    if(!_impl)
    {
        _impl = new atgFragmentShaderImpl;
    }
    if(!Source)
    {
        return false;
    }
    _impl->fragmentShaderCode = (GLchar*)Source;
    return Compile();
}

bool atgFragmentShader::Compile()
{
    if(!_impl || !_impl->fragmentShaderCode)
    {
        return false;
    }
    GL_ASSERT( _impl->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER) );
    GLint compileResult;
    int infoLogLength;
    //printf("Compiling shader : %s\n", SourceFilePath);
    const GLchar* fragmentShaderCodeCst = _impl->fragmentShaderCode;
    GL_ASSERT( glShaderSource(_impl->fragmentShaderID, 1, &fragmentShaderCodeCst, NULL) );
    GL_ASSERT( glCompileShader(_impl->fragmentShaderID) );

    // Check Vertex Shader
    GL_ASSERT( glGetShaderiv(_impl->fragmentShaderID, GL_COMPILE_STATUS, &compileResult) );
    if(compileResult != GL_TRUE)
    {
        GL_ASSERT( glGetShaderiv(_impl->fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength) );
        if ( infoLogLength > 0 ){
            GLchar* errorInfo = new GLchar[infoLogLength + 1];
            GL_ASSERT( glGetShaderInfoLog(_impl->fragmentShaderID, infoLogLength, NULL, errorInfo) );
            LOG("%s\n", errorInfo);
            delete [] errorInfo;
        }
        return false;
    }
    LOG("FragmentShader Compile success. compileResult=%d\n", compileResult);
    return true;
}

atgPass::atgPass()
{
    _impl = new atgPassImpl();
    GL_ASSERT( _impl->programID = glCreateProgram() );
}

atgPass::~atgPass()
{

}

const char* atgPass::GetName()
{
    return _name.c_str();
}

bool atgPass::Create( const char* VSFilePath, const char* FSFilePath )
{
    if (!Load(VSFilePath, FSFilePath))
    {
        return false;
    }
    if(!Link())
    {
        return false;
    }
    LOG("create pass(%s,%s) success.\n", VSFilePath, FSFilePath);
    return true;
    /*
    GLint activeAttributes;
    GLint length;
    GL_ASSERT( glGetProgramiv(_impl->programID, GL_ACTIVE_ATTRIBUTES, &activeAttributes) );
    if (activeAttributes > 0)
    {
        GL_ASSERT( glGetProgramiv(_impl->programID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length) );
        if (length > 0)
        {
            GLchar* attribName = new GLchar[length + 1];
            GLint attribSize;
            GLenum attribType;
            GLint attribLocation;
            for (int i = 0; i < activeAttributes; ++i)
            {
                // Query attribute info.
                GL_ASSERT( glGetActiveAttrib(_impl->programID, i, length, NULL, &attribSize, &attribType, attribName) );
                attribName[length] = '\0';

                // Query the pre-assigned attribute location.
                GL_ASSERT( attribLocation = glGetAttribLocation(_impl->programID, attribName) );

                // Assign the vertex attribute mapping for the effect.
                //effect->_vertexAttributes[attribName] = attribLocation;
            }
            SAFE_DELETE_ARRAY(attribName);
        }
    }
    return true;*/
}

void atgPass::Bind()
{
    if (_impl)
    {
        _impl->Bind();
    }
}

bool atgPass::Load(const char* VSFilePath, const char* FSFilePath)
{
    atgVertexShader vs;
    if(!vs.LoadFromFile(VSFilePath))
        return false;
    
    GL_ASSERT( glAttachShader(_impl->programID, vs._impl->vertexShaderID) );

    atgFragmentShader fs;
    if(!fs.LoadFromFile(FSFilePath))
        return false;
    
    GL_ASSERT( glAttachShader(_impl->programID, fs._impl->fragmentShaderID) );

    return true;
}

bool atgPass::Link()
{
    AASSERT(_impl);
    int infoLogLength;
    GL_ASSERT( glLinkProgram(_impl->programID) );
    // Check the program
    GLint linkResult;
    GL_ASSERT( glGetProgramiv(_impl->programID, GL_LINK_STATUS, &linkResult) );
    if(linkResult != GL_TRUE)
    {
        LOG("_impl->programID=%d, linkResult=%d\n",_impl->programID, linkResult);
        GL_ASSERT( glGetProgramiv(_impl->programID, GL_INFO_LOG_LENGTH, &infoLogLength) );
        if ( infoLogLength > 0 ){
            GLchar* errorInfo = new GLchar[infoLogLength + 1];
            GL_ASSERT( glGetProgramInfoLog(_impl->programID, infoLogLength, NULL, errorInfo) );
            LOG("%s\n", errorInfo);
            delete [] errorInfo;
        }
        return false;
    }
    return true;
}

void atgPass::BeginContext(void* data)
{
    GLuint programID = _impl->programID;
    if(programID != 0)
    {
        Matrix WVP;
        Matrix Wrld;
        Matrix View;
        Matrix Proj;
        g_Renderer->GetMatrix(&Wrld.m, MD_WORLD);
        g_Renderer->GetMatrix(&View.m, MD_VIEW);
        g_Renderer->GetMatrix(&Proj.m, MD_PROJECTION);
        ConcatTransforms(View.m, Wrld.m, WVP.m);
        ConcatTransforms(Proj.m, WVP.m, WVP.m);
        SetMatrix4x4(UNF_M_WVP, WVP.m);
    }
    g_Renderer->SetBlendFunction(BF_NONE, BF_NONE);
}

void atgPass::EndContext(void* data)
{

}

bool atgPass::SetInt(const char* var_name, int value)
{
    GLint identityID = 0;
    GL_ASSERT( identityID = glGetUniformLocation(_impl->programID, var_name) );
    if(identityID != -1)
    {
        glUniform1i(identityID, value);
        return true;
    }
    return false;
}

bool atgPass::SetFloat(const char* var_name, float value)
{
    GLint identityID = 0;
    GL_ASSERT( identityID = glGetUniformLocation(_impl->programID, var_name) );
    if(identityID != -1)
    {
        glUniform1f(identityID, value);
        return true;
    }
    return false;
}

bool atgPass::SetFloat3(const char* var_name, const float f[3])
{
    GLint identityID = 0;
    GL_ASSERT( identityID = glGetUniformLocation(_impl->programID, var_name) );
    if(identityID != -1)
    {
        glUniform3f(identityID, f[0], f[1], f[2]);
        return true;
    }
    return false;
}

bool atgPass::SetFloat4(const char* var_name, const float f[4])
{
    GLint identityID = 0;
    GL_ASSERT( identityID = glGetUniformLocation(_impl->programID, var_name) );
    if(identityID != -1)
    {
        glUniform4f(identityID, f[0], f[1], f[2], f[3]);
        return true;
    }
    return false;
}

bool atgPass::SetMatrix4x4(const char* var_name, const float mat[4][4])
{
    // OPENGLES 不允许第三个参数为GL_TRUE， 所以自己转置矩阵
    //glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, (GLfloat*)MVP.m); 
    static float matTemp[4][4];
    MatTranspose(mat, matTemp);

    GLint identityID = 0;
    GL_ASSERT( identityID = glGetUniformLocation(_impl->programID, var_name) );
    if(identityID != -1)
    {
        glUniformMatrix4fv(identityID, 1, GL_FALSE, (GLfloat*)&matTemp[0]);
        return true;
    }
    return false;
}

bool atgPass::SetTexture(const char* var_name, uint8 index)
{
    GLuint identityID = 0;
    GL_ASSERT( identityID = glGetUniformLocation(_impl->programID, var_name) );
    if (identityID != -1)
    {
        glUniform1i(identityID, index);
        return true;
    }
    return false;
}

class atgTextureImpl
{
public:
    atgTextureImpl():TextureID(0),locked(false) {}
    void Bind(uint8 index);
    static void Unbind(uint8 index);
public:
    GLuint TextureID;
    bool locked;
};

void atgTextureImpl::Bind(uint8 index)
{
    if (index >= atgRenderer::MaxNumberBindTexture)
        return;

    // Bind our texture in Texture Unit 0
    GL_ASSERT( glActiveTexture(GL_TEXTURE0 + index) );
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, TextureID) );
}

void atgTextureImpl::Unbind(uint8 index)
{

}

atgTexture::atgTexture():_width(0),_height(0),_bbp(0),_impl(NULL)
{
}

atgTexture::~atgTexture()
{
}

atgTexture* atgTexture::Create( uint32 width, uint32 height, uint32 bbp, const void *pData/*=NULL*/ )
{
    atgTexture* pTexture = new atgTexture;
    pTexture->_width = width;
    pTexture->_height = height;
    pTexture->_bbp = bbp / 8;

    AASSERT(pTexture->_impl == NULL);
    pTexture->_impl = new atgTextureImpl;

    GL_ASSERT( glGenTextures(1, &pTexture->_impl->TextureID) );
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, pTexture->_impl->TextureID) );
    GL_ASSERT( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData) );

    //GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
    GL_ASSERT( glGenerateMipmap(GL_TEXTURE_2D) ); // 生产mipmap,这个代码要放后面

    return pTexture;
}

bool atgTexture::Destory()
{
    if (_impl)
    {
        //SAFE_RELEASE(_impl->pDXTX);
        GL_ASSERT( glDeleteTextures(1, &_impl->TextureID) );
        SAFE_DELETE(_impl);
        _width = 0;
        _height = 0;
        _bbp = 0;
    }

    return true;
}

void* atgTexture::Lock()
{
    return NULL;
}

void atgTexture::Unlock()
{
}

bool atgTexture::IsLocked() const
{
    return _impl->locked;
}

void atgTexture::SetFilterMode(TextureFilterMode filter)
{
    switch (filter)
    {
    case TFM_FILTER_NEAREST:
        GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST) );
        GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST) );
        break;
    case TFM_FILTER_BILINEAR:
        GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST) );
        GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR) );
        break;
    case TFM_FILTER_TRILINEAR:
    case TFM_FILTER_ANISOTROPIC:
#ifdef __GLEW_H__
        if (GLEW_EXT_texture_filter_anisotropic != 0)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)1.0f); // 1.0f, 2.0f, 4.0f ?
        }
        else
        {
            GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST) );
            GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR) );
        }
#else
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)1.0f); // 1.0f, 2.0f, 4.0f ?
#endif

        break;
    case TFM_FILTER_DEFAULT:
        break;
    case MAX_FILTERMODES:
        break;
    default:
        break;
    }
}
void atgTexture::SetAddressMode(TextureCoordinate coordinate, TextureAddressMode address)
{
    GLenum uv = coordinate == TC_COORD_U ? GL_TEXTURE_WRAP_S : GL_TEXTURE_WRAP_T;
    switch (address)
    {
    case TAM_ADDRESS_WRAP:
        glTexParameteri(coordinate, uv, GL_REPEAT);
        break;
    case TAM_ADDRESS_MIRROR:
        glTexParameteri(coordinate, uv, GL_MIRRORED_REPEAT);
        break;
    case TAM_ADDRESS_CLAMP:
        glTexParameteri(coordinate, uv, GL_CLAMP_TO_EDGE);
        break;
    case TAM_ADDRESS_BORDER:
        glTexParameteri(coordinate, uv, GL_CLAMP_TO_EDGE);
        break;
    case MAX_ADDRESSMODES:
        break;
    default:
        break;
    }
}

class atgRenderTargetImpl
{
public:
    atgRenderTargetImpl() {}

    GLuint renderBufferId;
};

atgRenderTarget::atgRenderTarget():_impl(NULL)
{
}

bool atgRenderTarget::Create( RenderTargetType type )
{
    if (_impl == NULL)
    {
        _impl = new atgRenderTargetImpl();
    }
    glGenRenderbuffers(1, &_impl->renderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, _impl->renderBufferId);
    uint32 offsetX, offsetY, width, height;
    g_Renderer->GetViewPort(offsetX, offsetY, width, height);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4 /* 4 samples */, GL_RGBA8,  width, height);

    GLuint fbID;
    glGenFramebuffers(1, &fbID);
    glBindFramebuffer(GL_FRAMEBUFFER, fbID);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _impl->renderBufferId);

    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    return true;
}

bool atgRenderer::Initialize( uint32 width, uint32 height, uint8 bpp )
{
    bool rt;
#ifdef _WIN32
        rt = win32_init_ogl();
#elif defined _ANDROID
        rt = android_init_ogl();
#endif

    if (!rt)
    {
        LOG("init opengl failure!\n");
        return false;
    }
#ifdef OPENGL_FIX_PIPELINE
    glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
    glLoadIdentity();                                   // Reset The Projection Matrix
    {
        // Method1. Calculate The Aspect Ratio Of The Window
        //gluPerspective(90.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);

        // Method2. OpenGL self function;
        //float aspect = width * 1.0f / height;
        //glFrustum(-1.0f, 1.0f, -(1.0f / aspect), (1.0f / aspect), 0.1f, 1000.0f);

        // Method3. Make slef matrix;
        float aspect = width * 1.0f / height;
        Matrix proj;
        Perspective(proj.m, 90.0f, aspect, 0.001f, 1000.0f);
        glLoadMatrixf((GLfloat*)proj.m);

        // Get Matrix Mode can call glGet* function; example
        //GLfloat ResultMatrix[16];
        //glGetFloatv(GL_PROJECTION_MATRIX, &ResultMatrix[0]);
        //for(int i=0; i < 16; i+=4)
        //{
        //    printf("%f, %f, %f, %f\n", ResultMatrix[i], ResultMatrix[i + 1],
        //        ResultMatrix[i + 2], ResultMatrix[i + 3]);
        //}
    }

    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();                                   // Reset The Modelview Matrix
    glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations
#endif // OPENGL_FIX_PIPELINE
    GL_ASSERT( glEnable(GL_DEPTH_TEST) );                            // Enables Depth Testing
    GL_ASSERT( glDepthFunc(GL_LEQUAL) );                             // The Type Of Depth Testing To Do
    GL_ASSERT( glCullFace(GL_FRONT) );

    LOG("|||| Graphic Driver ===> Using %s!\n", atgRenderer::getName());
    return true;
}

void atgRenderer::Shutdown()
{
#ifdef _WIN32
    win32_shutdown_ogl();
#endif
}

bool atgRenderer::Resize( uint32 width, uint32 height )
{
    return true;
}

void atgRenderer::SetViewport( uint32 offsetX, uint32 offsetY, uint32 width, uint32 height )
{
    glViewport(offsetX,offsetY,width,height);                       // Reset The Current Viewport

    _VP_offsetX = offsetX;
    _VP_offsetY = offsetY;
    _VP_width = width;
    _VP_height = height;
}

void atgRenderer::GetViewPort(uint32& offsetX, uint32& offsetY, uint32& width, uint32& height) const
{
    offsetX = _VP_offsetX;
    offsetY = _VP_offsetY;
#ifdef _WIN32
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    width  = rect.right - rect.left;
    height = rect.bottom - rect.left;
#else
    width  = _VP_width;
    height = _VP_height;
#endif // _WIN32

}

void atgRenderer::SetMatrix( MatrixDefine index, const float mat[4][4] )
{
    switch(index)
    {
    case MD_WORLD:
        {
            memcpy(&_matrixs[MD_WORLD], mat, sizeof(float[4][4]));
            break;
        }
    case MD_VIEW:
        {
            memcpy(&_matrixs[MD_VIEW], mat, sizeof(float[4][4]));
#ifdef OPENGL_FIX_PIPELINE
            float temp[4][4];
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            ConcatTransforms(_matrixs[MD_VIEW].m, _matrixs[MD_WORLD].m, temp);
            glLoadMatrixf((GLfloat*)&temp);
#endif
            break;
        }
    case MD_PROJECTION:
        {
            memcpy(&_matrixs[MD_PROJECTION], mat, sizeof(float[4][4]));
#ifdef OPENGL_FIX_PIPELINE
            glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
            glLoadIdentity();                                   // Reset The Projection Matrix
            glLoadMatrixf((GLfloat*)&_matrixs[MD_PROJECTION]);
#endif
            break;
        }
    default: break;
    }
}

void atgRenderer::GetMatrix(const float (*MtxPtr)[4][4], MatrixDefine index) const
{
    if (MD_WORLD <= index && index < MD_NUMBER)
    {
        memcpy((void*)MtxPtr, _matrixs[index].m, MATRIX44_SIZE);
    }
}

void atgRenderer::SetDepthTestEnable(bool enable)
{
    if (enable)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    else
        glDisable(GL_DEPTH_TEST);
}

void atgRenderer::SetAlphaTestEnable(bool enbale, float value)
{
#ifdef OPENGL_FIX_PIPELINE
    if (enbale)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, value);
    }
    else
        glDisable(GL_ALPHA_TEST);
#endif // OPENGL_FIX_PIPELINE
}

void atgRenderer::SetFaceCull(FaceCullMode mode)
{
    if (mode == FCM_NONE)
    {
        glDisable(GL_CULL_FACE);
    }else if(mode == FCM_CW)
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
    }else
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
    }
}

GLenum BlendFuncToGLBlendFunc(BlendFunction BlendFunc)
{
    switch(BlendFunc)
    {
    case BF_ZERO : return GL_ZERO;
    case BF_ONE : return GL_ONE;
    case BF_SRC_COLOR : return GL_SRC_COLOR;
    case BF_ONE_MINUS_SRC_COLOR : return GL_ONE_MINUS_SRC_COLOR;
    case BF_DST_COLOR : return GL_DST_COLOR;
    case BF_ONE_MINUS_DST_COLOR : return GL_ONE_MINUS_DST_COLOR;
    case BF_SRC_ALPHA : return GL_SRC_ALPHA;
    case BF_ONE_MINUS_SRC_ALPHA : return GL_ONE_MINUS_SRC_ALPHA;
    case BF_DST_ALPHA : return GL_DST_ALPHA;
    case BF_ONE_MINUS_DST_ALPHA : return GL_ONE_MINUS_DST_ALPHA;
    default:
        AASSERT(0);
        return 0;
    }
}

BlendFunction GLBlendFuncToEngineBlendFunc(GLenum BlendFunc)
{
    switch(BlendFunc)
    {
    case GL_ZERO : return BF_ZERO;
    case GL_ONE : return BF_ONE;
    case GL_SRC_COLOR : return BF_SRC_COLOR;
    case GL_ONE_MINUS_SRC_COLOR : return BF_ONE_MINUS_SRC_COLOR;
    case GL_DST_COLOR : return BF_DST_COLOR;
    case GL_ONE_MINUS_DST_COLOR : return BF_ONE_MINUS_DST_COLOR;
    case GL_SRC_ALPHA : return BF_SRC_ALPHA;
    case GL_ONE_MINUS_SRC_ALPHA : return BF_ONE_MINUS_SRC_ALPHA;
    case GL_DST_ALPHA : return BF_DST_ALPHA;
    case GL_ONE_MINUS_DST_ALPHA : return BF_ONE_MINUS_DST_ALPHA;
    default:
        AASSERT(0);
        return (BlendFunction)0;
    }
}

void atgRenderer::GetBlendFunction(BlendFunction& outSrcBlend, BlendFunction& outDestBlend)
{
    GLint GLSrc = 0;
    GLint GLDest = 0;
    GL_ASSERT( glGetIntegerv(GL_BLEND_SRC_RGB, &GLSrc) );
    GL_ASSERT( glGetIntegerv(GL_BLEND_DST_RGB, &GLDest) );
    outSrcBlend = GLBlendFuncToEngineBlendFunc(GLenum(GLSrc));
    outDestBlend = GLBlendFuncToEngineBlendFunc(GLenum(GLDest));
}

void atgRenderer::SetBlendFunction(BlendFunction SrcBlend, BlendFunction DestBlend)
{
    if (SrcBlend == BF_NONE || DestBlend == BF_NONE)
    {
        GL_ASSERT( glDisable(GL_BLEND) );

    }else
    {
        GLenum glSrc = BlendFuncToGLBlendFunc(SrcBlend);
        GLenum glDest = BlendFuncToGLBlendFunc(DestBlend);
        GL_ASSERT( glEnable(GL_BLEND) );
        GL_ASSERT( glBlendFunc(glSrc, glDest) );
    }
}

void atgRenderer::Clear()
{
    ATG_PROFILE("atgRenderer::Clear");
    GL_ASSERT( glClearColor(0.0f, 0.0f, 0.0f, 1.0f) );              // Set Black Background
    GL_ASSERT( glClearDepth(1.0f) );                                // Set Depth Buffer Setup
    GL_ASSERT( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );// Clear Screen And Depth Buffer
}

void atgRenderer::BeginFrame()
{
}

void atgRenderer::EndFrame()
{
    for (int i = 0; i < MaxNumberBindTexture; ++i)
        BindTexture(i, NULL);
    BindVertexBuffer(NULL);
    BindIndexBuffer(NULL);
    BindPass(NULL);
    BindMaterial(NULL);
}

void atgRenderer::Present()
{
    ATG_PROFILE("atgRenderer::Present");
#ifdef _WIN32
    win32_ogl_present();
#elif defined _ANDROID
    android_ogl_present();
#endif
    
}

bool PrimitiveTypeConvertToOGL(PrimitveType pt, GLenum& gl_pt)
{
    switch(pt)
    {
    case PT_POINTS:
        gl_pt = GL_POINTS;
        break;
    case PT_LINES:
        gl_pt = GL_LINES;
        break;
    case PT_LINE_STRIP:
        gl_pt = GL_LINE_STRIP;
        break;
    case PT_TRIANGLES:
        gl_pt = GL_TRIANGLES;
        break;
    case PT_TRIANGLE_STRIP:
        gl_pt = GL_TRIANGLE_STRIP;
        break;
    case PT_TRIANGLE_FAN:
        gl_pt = GL_TRIANGLE_FAN;
        break;
    default:
        return false;
    }
    return true;
}

bool atgRenderer::DrawPrimitive( PrimitveType pt, uint32 primitveCount, uint32 verticesCount )
{
    GLenum gl_pt;
    if(PrimitiveTypeConvertToOGL(pt, gl_pt)){
        GL_ASSERT( glDrawArrays(gl_pt, 0, verticesCount) );
    }
    return true;
}

bool atgRenderer::DrawIndexedPrimitive( PrimitveType pt, uint32 primitveCount,  uint32 indicesCount, uint32 verticesCount)
{
    GLenum gl_pt;
    if(PrimitiveTypeConvertToOGL(pt, gl_pt)){
        GL_ASSERT( glDrawElements(gl_pt, indicesCount, GL_UNSIGNED_SHORT, 0) ); // with VBO
    }
    return true;
}

void atgRenderer::BindPass( atgPass* pass )
{
    if (_bindPass)
    {
        _bindPass->EndContext(NULL);
        _bindPass->_impl->Unbind();
    }

    _bindPass = pass;

    if(_bindPass)
    {
        _bindPass->Bind();
        _bindPass->BeginContext(NULL);
    }
}

void atgRenderer::BindIndexBuffer( atgIndexBuffer* indexBuffer )
{
    if (_bindIndexBuffer)
    {
        _bindIndexBuffer->_impl->Unbind();
    }

    _bindIndexBuffer = indexBuffer;

    if (_bindIndexBuffer)
    {
        _bindIndexBuffer->_impl->Bind();
    }
}

void atgRenderer::BindVertexBuffer( atgVertexBuffer* vertexBuffer )
{
    if (_bindVertexBuffer)
    {
        _bindVertexBuffer->_impl->Unbind();
    }

    if(!_bindPass)
        return;

    GLuint programID = _bindPass->_impl->programID;
    if(programID == 0)
        return;

    _bindVertexBuffer = vertexBuffer;

    if (_bindVertexBuffer)
    {
        _bindVertexBuffer->_impl->Bind(programID);
    }
}

void atgRenderer::BindTexture( uint8 index, atgTexture* texture )
{
    if (0 <= index && index < MaxNumberBindTexture)
    {
        _bindTexture[index] = texture;
        if (texture)
        {
            texture->_impl->Bind(index);
        }else
        {
            atgTextureImpl::Unbind(index);
        }
    }
}

#endif