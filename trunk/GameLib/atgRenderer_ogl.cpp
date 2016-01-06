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
    atgIndexBufferImpl():vbo_indicesID(0),accessMode(BAM_Static),locked(false),lockOffset(0),lockSize(0),pLockMemory(0) {}
    ~atgIndexBufferImpl() 
    {
        SAFE_DELETE(pLockMemory); 
        if(vbo_indicesID)
        {
            GL_ASSERT( glDeleteBuffers(1, &vbo_indicesID) );
            vbo_indicesID = 0;
        }
    }
    bool Bind();
    bool Unbind();
public:
    GLuint vbo_indicesID;
    BufferAccessMode accessMode;
    bool locked;
    GLsizei lockOffset;
    GLuint lockSize;
    char* pLockMemory;
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
    Destory();
    g_Renderer->RemoveGpuResource(this);
}

bool atgIndexBuffer::Create( const void *pIndices, uint32 numIndices, IndexFormat format, BufferAccessMode accessMode )
{
    Destory();

    int indexSize = sizeof(uint16);
    int bufSize = numIndices * indexSize;
    _impl = new atgIndexBufferImpl;
    _impl->accessMode = accessMode;

    GL_ASSERT( glGenBuffers(1, &(_impl->vbo_indicesID)) );
    _impl->Bind();
    GL_ASSERT( glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufSize, pIndices, (_impl->accessMode == BAM_Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ) );
    _size = bufSize;
    _impl->pLockMemory = new char[_size];

    _impl->Unbind();

    atgGpuResource::ReSet();
    g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgIndexBuffer::Destory));
    return true;
}

bool atgIndexBuffer::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);

    return true;
}

void* atgIndexBuffer::Lock( uint32 offset, uint32 size )
{
    AASSERT(_impl);
    void* pIndexBuffer = NULL;
    if (!IsLost() && _impl && size <= _size)
    {
        _impl->lockOffset = offset;
        _impl->lockSize = size;
        pIndexBuffer = _impl->pLockMemory;
        _impl->locked = true;
    }

    return pIndexBuffer;
}

void atgIndexBuffer::Unlock()
{
    if(_impl->locked && _impl->lockSize > 0)
    {
        _impl->Bind();
        if (_impl->lockSize == _size)
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _impl->pLockMemory, (_impl->accessMode == BAM_Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        }
        else
        {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, _impl->lockOffset, _impl->lockSize, _impl->pLockMemory);
        }
        _impl->Unbind();
        _impl->locked = false;
    }
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
// vertex buffer
class atgVertexBufferImpl
{
public:
    atgVertexBufferImpl():vbo_vertexbufferID(0),accessMode(BAM_Static),locked(false),lockOffset(0),lockSize(0),pLockMemory(0) {}
    ~atgVertexBufferImpl()
    {
        SAFE_DELETE(pLockMemory); 
        if(vbo_vertexbufferID) 
        { 
            GL_ASSERT( glDeleteBuffers(1, &vbo_vertexbufferID) ); 
            vbo_vertexbufferID = 0; 
        }
    }
    void Bind(GLuint programID);
    void Unbind();
public:
    atgVertexDecl decl;
    GLuint vbo_vertexbufferID;
    BufferAccessMode accessMode;
    bool locked;
    GLsizei lockOffset;
    GLuint lockSize;
    char* pLockMemory;
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


atgVertexBuffer::atgVertexBuffer():_impl(NULL),_size(0)
{
}

atgVertexBuffer::~atgVertexBuffer()
{
    Destory();
    g_Renderer->RemoveGpuResource(this);
}

bool atgVertexBuffer::Create( atgVertexDecl* decl, const void *pData, uint32 size, BufferAccessMode accessMode )
{
    Destory();

    _impl = new atgVertexBufferImpl();
    _impl->decl = *decl;
    _impl->accessMode = accessMode;

    // generate VBO
    GL_ASSERT( glGenBuffers(1, &(_impl->vbo_vertexbufferID)) );
    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _impl->vbo_vertexbufferID) );
    GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, size, pData, (accessMode == BAM_Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
    _size = size;
    _impl->pLockMemory = new char[_size];

    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0));

    atgGpuResource::ReSet();
    g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgVertexBuffer::Destory));
    return true;
}

bool atgVertexBuffer::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);

    return true;
}

void* atgVertexBuffer::Lock( uint32 offset, uint32 size )
{
    AASSERT(_impl);
    void* pVertexBuffer = NULL;
    if (!IsLost() && _impl && size <= _size)
    {
        _impl->lockOffset = offset;
        _impl->lockSize = size;
        pVertexBuffer = _impl->pLockMemory;
        _impl->locked = true;
    }

    return pVertexBuffer;

//#ifdef OPENGL_USE_MAP
//    if (_impl)
//    {
//        if (!_impl->locked)
//        {
//            GLvoid* buffer = NULL;
//            GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _impl->vbo_vertexbufferID) );
//            GL_ASSERT( buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) );
//            _impl->locked = true;
//            return buffer;
//        }
//    }
//#else
//    SAFE_DELETE_ARRAY(_impl->pLockBuffer);
//    _impl->pLockBuffer = new char[size];
//    _impl->lockBufferSize = size;
//    _impl->offsetLockBuffer = offset;
//    _impl->locked = true;
//    return (void*)_impl->pLockBuffer;
//#endif
//    return NULL;

}

void atgVertexBuffer::Unlock()
{
    if(_impl->locked && _impl->lockSize > 0)
    {
        GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _impl->vbo_vertexbufferID) );
        if (_impl->lockSize == _size)
        {
            glBufferData(GL_ARRAY_BUFFER, _size, _impl->pLockMemory, (_impl->accessMode == BAM_Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        }
        else
        {
            glBufferSubData(GL_ARRAY_BUFFER, _impl->lockOffset, _impl->lockSize, _impl->pLockMemory);
        }
        GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0));
        _impl->locked = false;
    }


//#ifdef OPENGL_USE_MAP
//    if (_impl)
//    {
//        if (_impl->locked)
//        {
//            GL_ASSERT( glUnmapBuffer(GL_ARRAY_BUFFER) );
//            _impl->locked = false;
//        }
//    }
//#else
//    if (_impl)
//    {
//        if (_impl->locked)
//        {
//            if(_impl->pLockBuffer != NULL)
//            {
//                GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _impl->vbo_vertexbufferID) );
//                GL_ASSERT( glBufferSubData(GL_ARRAY_BUFFER, _impl->offsetLockBuffer, _impl->lockBufferSize, (GLvoid*)_impl->pLockBuffer) );
//                SAFE_DELETE_ARRAY(_impl->pLockBuffer);
//            }
//            _impl->locked = false;
//        }
//    }
//#endif
}

bool atgVertexBuffer::IsLocked() const
{
    if(_impl)
        return _impl->locked;

    return false;
}


class atgTextureImpl
{
public:
    atgTextureImpl():TextureID(0),locked(false) {}
    ~atgTextureImpl() 
    {
        if(TextureID)
        {
            GL_ASSERT( glDeleteTextures(1, &TextureID) );
            TextureID = 0;
        }
    }
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
    if (index >= atgRenderer::MaxNumberBindTexture)
        return;

    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, 0) );
}

atgTexture::atgTexture():_width(0),_height(0),_format(TF_R8G8B8A8),_impl(NULL)
{
}

atgTexture::~atgTexture()
{
    Destory();
    g_Renderer->RemoveGpuResource(this);
}

bool atgTexture::Create( uint32 width, uint32 height, TextureFormat format, const void *pData/*=NULL*/ )
{
    /*
    OpenGL ES 2.0

    void glTexImage2D(	GLenum target,
                        GLint level,
                        GLint internalformat,
                        GLsizei width,
                        GLsizei height,
                        GLint border,
                        GLenum format,
                        GLenum type,
                        const GLvoid * data);

    Parameters

    target
        Specifies the target texture of the active texture unit. Must be GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, or GL_TEXTURE_CUBE_MAP_NEGATIVE_Z.

    level
        Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.

    internalformat
        Specifies the internal format of the texture. Must be one of the following symbolic constants: GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA.

    width
        Specifies the width of the texture image. All implementations support 2D texture images that are at least 64 texels wide and cube-mapped texture images that are at least 16 texels wide.

    height
        Specifies the height of the texture image All implementations support 2D texture images that are at least 64 texels high and cube-mapped texture images that are at least 16 texels high.

    border
        Specifies the width of the border. Must be 0.

    format
        Specifies the format of the texel data. Must match internalformat. The following symbolic values are accepted: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA.

    type
        Specifies the data type of the texel data. The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, and GL_UNSIGNED_SHORT_5_5_5_1.

    data
        Specifies a pointer to the image data in memory.
    */

    Destory();

    _width = width;
    _height = height;
    _format = format;

    GLint internalFormat;
    GLenum inFormat;
    GLenum type;
    bool isColorFormat = true;

    switch (_format)
    {
    case TF_R8G8B8:
        inFormat = GL_RGB;
        internalFormat = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        isColorFormat = true;
        break;
    case TF_R5G6B5:
        inFormat = GL_RGB;
        internalFormat = GL_RGB;
        type = GL_UNSIGNED_SHORT_5_6_5;
        isColorFormat = true;
        break;
    case TF_R8G8B8A8:
        inFormat = GL_RGBA;
        internalFormat = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        isColorFormat = true;
        break;
    case TF_R5G5B5A1:
        inFormat = GL_RGBA;
        internalFormat = GL_RGBA;
        type = GL_UNSIGNED_SHORT_5_5_5_1;
        isColorFormat = true;
        break;
    case TF_R4G4B4A4:
        inFormat = GL_RGBA;
        internalFormat = GL_RGBA;
        type = GL_UNSIGNED_SHORT_4_4_4_4;
        isColorFormat = true;
        break;
    case TF_R32F:
        inFormat = GL_LUMINANCE;
        internalFormat = GL_LUMINANCE;
        type = GL_FLOAT;
        isColorFormat = false;
        break;
    case TF_R16F:
        inFormat = GL_LUMINANCE;
        internalFormat = GL_LUMINANCE;
        type = GL_FLOAT;
        isColorFormat = false;
        break;
    case TF_D24S8:
#ifndef GL_ES_VERSION_2_0
        internalFormat = GL_DEPTH24_STENCIL8;
#else
        internalFormat = GL_DEPTH24_STENCIL8_OES;
#endif // USE_OPENGLES
        
        isColorFormat = false;
        break;
    case TF_D16:
        internalFormat = GL_DEPTH_COMPONENT16;
        isColorFormat = false;
        break;
    default:
        break;
    }

    _impl = new atgTextureImpl;

    if (isColorFormat)
    {
        GL_ASSERT( glGenTextures(1, &_impl->TextureID) );
        GL_ASSERT( glBindTexture(GL_TEXTURE_2D, _impl->TextureID) );
        GL_ASSERT( glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, inFormat, type, pData) );

        //GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE) );
        GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST) );
        GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
        GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
        GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
        GL_ASSERT( glGenerateMipmap(GL_TEXTURE_2D) ); // 生产mipmap,这个代码要放后面
    }
    else
    {
        GL_ASSERT( glGenRenderbuffers(1, &_impl->TextureID) );  
        GL_ASSERT( glBindRenderbuffer(GL_RENDERBUFFER, _impl->TextureID) );  
        GL_ASSERT( glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height) );
    }

    atgGpuResource::ReSet();
    g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgTexture::Destory));
    return true;
}

bool atgTexture::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);
    _width = 0;
    _height = 0;
    _format = TF_R8G8B8A8;
    
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
#ifdef GL_TEXTURE_MAX_ANISOTROPY_EXT
        GL_ASSERT( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)1.0f) ); // 1.0f, 2.0f, 4.0f ?
#endif // GL_TEXTURE_MAX_ANISOTROPY_EXT
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

atgResourceShader::atgResourceShader(ResourceShaderType Type):compiled(false)
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
        SAFE_DELETE_ARRAY(vertexShaderCode);
        if(vertexShaderID)
        {
            GL_ASSERT(glDeleteShader(vertexShaderID));
            vertexShaderID = 0;
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
    Destory();
}

bool atgVertexShader::LoadFromFile(const char* SourceFilePath)
{
    if(IsCompiled())
    {
        LOG("vs already loaded.");
        return false;
    }

    if (!SourceFilePath)
    {
        return false;
    }

    char* pVertexShaderCode = NULL;
    bool result;
    // Read the Vertex Shader code from the file
    result = ReadShadeFile(&pVertexShaderCode, SourceFilePath);
    if(!result)
    {
        LOG("Load vertex shader failure: %s\n", SourceFilePath);
        AASSERT(0);
        return false;
    }
    return LoadFromSource(pVertexShaderCode);
}

bool atgVertexShader::LoadFromSource( const char* Source )
{
    Destory();

    if(IsCompiled())
    {
        LOG("vs already loaded.");
        return false;
    }

    if(!_impl)
    {
        _impl = new atgVertexShaderImpl;
    }
    if(!Source)
    {
        return false;
    }
    _impl->vertexShaderCode = (GLchar*)Source;
    bool rs =  Compile();
    return rs;
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
    
    atgGpuResource::ReSet();
    //g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgVertexShader::Destory));
    LOG("VertexShader Compile success. compileResult=%d\n", compileResult);
    return true;
}

bool atgVertexShader::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);

    return true;
}

class atgFragmentShaderImpl
{
public:
    atgFragmentShaderImpl():fragmentShaderID(0),fragmentShaderCode(0){}
    ~atgFragmentShaderImpl()
    {
        SAFE_DELETE_ARRAY(fragmentShaderCode);
        if (fragmentShaderID)
        {
            GL_ASSERT( glDeleteShader(fragmentShaderID) );
            fragmentShaderID = 0;
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
    Destory();
}

bool atgFragmentShader::LoadFromFile(const char* SourceFilePath)
{
    if (!SourceFilePath)
    {
        return false;
    }

    char* pFragmentShaderCode = NULL;
    bool result;
    // Read the Vertex Shader code from the file
    result = ReadShadeFile(&pFragmentShaderCode, SourceFilePath);
    if(!result)
    {
        LOG("Load vertex shader failure: %s\n", SourceFilePath);
        AASSERT(0);
        return false;
    }
    return LoadFormSource(pFragmentShaderCode);
}

bool atgFragmentShader::LoadFormSource( const char* Source )
{
    Destory();

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

    atgGpuResource::ReSet();
    //g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgFragmentShader::Destory));
    LOG("FragmentShader Compile success. compileResult=%d\n", compileResult);
    return true;
}

bool atgFragmentShader::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);

    return true;
}

//////////////////////////////////////////////////////////////////////////
class atgPassImpl
{
public:
    atgPassImpl():programID(0),pVS(0),pFS(0){}
    ~atgPassImpl() 
    { 
        SAFE_DELETE(pVS); 
        SAFE_DELETE(pFS); 
        if (programID)
        {
            GL_ASSERT( glDeleteProgram(programID) ); 
            programID = 0;
        }
    }

    void Bind();
    void Unbind();
public:
    GLuint programID;
    atgVertexShader* pVS;
    atgFragmentShader* pFS;
};

void atgPassImpl::Bind()
{
    GL_ASSERT( glUseProgram(programID) );
}

void atgPassImpl::Unbind()
{
    GL_ASSERT( glUseProgram(0) );
}

atgPass::atgPass():_impl(0)
{
}

atgPass::~atgPass()
{
    Destory();
    g_Renderer->RemoveGpuResource(this);
}

const char* atgPass::GetName()
{
    return _name.c_str();
}

bool atgPass::Create( const char* VSFilePath, const char* FSFilePath )
{
    Destory();

    _impl = new atgPassImpl;
    _impl->pVS = new atgVertexShader();
    if(!_impl->pVS->LoadFromFile(VSFilePath))
    {
        LOG("load vertex shader fail.\n");
        return false;
    }

    _impl->pFS = new atgFragmentShader();
    if (!_impl->pFS->LoadFromFile(FSFilePath))
    {
        LOG("load pixel shader fail.\n");
        return false;
    }

    GL_ASSERT( _impl->programID = glCreateProgram() );

    if(!Link())
    {
        return false;
    }

    atgGpuResource::ReSet();
    g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgPass::Destory));
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

bool atgPass::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);

    return true;
}

void atgPass::Bind()
{
    if (_impl)
    {
        _impl->Bind();
    }
}

bool atgPass::Link()
{
    AASSERT(_impl);

    GL_ASSERT( glAttachShader(_impl->programID, _impl->pVS->_impl->vertexShaderID) );
    GL_ASSERT( glAttachShader(_impl->programID, _impl->pFS->_impl->fragmentShaderID) );

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
        g_Renderer->GetMatrix(Wrld, MD_WORLD);
        g_Renderer->GetMatrix(View, MD_VIEW);
        g_Renderer->GetMatrix(Proj, MD_PROJECTION);
        View.Concatenate(Wrld, WVP);
        Proj.Concatenate(WVP, WVP);
        SetMatrix4x4(UNF_M_WVP, WVP);
    }
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

bool atgPass::SetMatrix4x4(const char* var_name, const Matrix& mat)
{
    // OPENGLES 不允许第三个参数为GL_TRUE， 所以自己转置矩阵
    //glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, (GLfloat*)MVP.m); 
    static float matTemp[4][4];
    atgMath::MatTranspose(mat.m, matTemp);

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

class atgRenderTargetImpl
{
public:
    atgRenderTargetImpl() {}

    GLuint renderBufferId;
};

atgRenderTarget::atgRenderTarget():_impl(NULL)
{
}

atgRenderTarget::~atgRenderTarget()
{
    Destroy();
}

bool atgRenderTarget::Create( std::vector<atgTexture*>& colorBuffer, atgTexture* depthStencilBuffer )
{
    //if (_impl == NULL)
    //{
    //    _impl = new atgRenderTargetImpl();
    //}
    glGenRenderbuffers(1, &_impl->renderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, _impl->renderBufferId);
    ////uint32 offsetX, offsetY, width, height;
    ////g_Renderer->GetViewPort(offsetX, offsetY, width, height);
    //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4 /* 4 samples */, GL_RGBA8,  width, height);

    GLuint depthbuffer;
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);

    GLuint fbID;
    glGenFramebuffers(1, &fbID);
    glBindFramebuffer(GL_FRAMEBUFFER, fbID);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _impl->renderBufferId);

    ////glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    return true;
}

bool atgRenderTarget::Destroy()
{
    return true;
}

bool atgRenderTarget::Active(uint8 index)
{

    return false;
}

void atgRenderTarget::Deactive()
{

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

    //>The Type Of Depth Testing To Do
    //>默认是小于(GL_LESS). 使用小于等于.也是和directx保持一致.
    GL_ASSERT( glDepthFunc(GL_LEQUAL) );                             
                  
    //>opengl 默认使用逆时针(counter-clockwise)的顶点组成的面作为前面.
    //>directx 默认剔除逆时针的顶点组成的面.即认为ccw作为后面.
    //>所以这里opengl设置剔除前面.就保证了调用SetFaceCull的统一.
    GL_ASSERT( glCullFace(GL_FRONT) );

    LOG("|||| Graphic Driver ===> Using %s!\n", atgRenderer::getName());
    return true;
}

void atgRenderer::Shutdown()
{
    ReleaseAllGpuResource();
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

void atgRenderer::SetMatrix(MatrixDefine index, const Matrix& mat)
{
    switch(index)
    {
    case MD_WORLD:
        {
            _matrixs[MD_WORLD] = mat;
            break;
        }
    case MD_VIEW:
        {
            _matrixs[MD_VIEW] = mat;
#ifdef OPENGL_FIX_PIPELINE
            Matrix temp;
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            _matrixs[MD_VIEW].Concatenate(_matrixs[MD_WORLD], temp);
            glLoadMatrixf((GLfloat*)&temp);
#endif
            break;
        }
    case MD_PROJECTION:
        {
            _matrixs[MD_PROJECTION] = mat;
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

void atgRenderer::GetMatrix(Matrix& mat, MatrixDefine index) const
{
    if (MD_WORLD <= index && index < MD_NUMBER)
    {
        mat = _matrixs[index];
    }
}

void atgRenderer::SetLightEnable(bool enable)
{
//    if (enable)
//    {
//        glEnable(GL_LIGHTING);
//    }else
//    {
//        glDisable(GL_LIGHTING);
//    }
}

void atgRenderer::SetDepthTestEnable(bool enable)
{
    if (enable)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
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
    {
        glDisable(GL_ALPHA_TEST);
    }
#endif // OPENGL_FIX_PIPELINE
}

void atgRenderer::SetFaceCull(FaceCullMode mode)
{
    if (mode == FCM_NONE)
    {
        GL_ASSERT( glDisable(GL_CULL_FACE) );
    }
    else if(mode == FCM_CW)
    {
        GL_ASSERT( glEnable(GL_CULL_FACE) );
        GL_ASSERT( glFrontFace(GL_CW) );
    }else
    {
        GL_ASSERT( glEnable(GL_CULL_FACE) );
        GL_ASSERT( glFrontFace(GL_CCW) );
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
    GL_ASSERT( glClearColor(0.0f, 0.141f, 0.141f, 1.0f) );              // Set Black Background
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

bool atgRenderer::DrawPrimitive( PrimitveType pt, uint32 primitveCount, uint32 verticesCount, uint32 offset )
{
    GLenum gl_pt;
    if(PrimitiveTypeConvertToOGL(pt, gl_pt)){
        GL_ASSERT( glDrawArrays(gl_pt, offset, verticesCount) );
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

void atgRenderer::SetPointSize(float size)
{
#if defined(_WIN32) && defined(__GLEW_H__)
    GLfloat pointSize = size;
    glPointSize(size);
#endif // !_ANDROID
}

#endif