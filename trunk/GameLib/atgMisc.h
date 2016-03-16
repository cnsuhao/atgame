#pragma once

#include "atgBase.h"

// All interfaces derive from this.
class IBaseInterface
{
public:
    virtual         ~IBaseInterface() {}
};

extern const int IFACE_OK;      // 0
extern const int IFACE_FAILED;  // -1

#define CREATEINTERFACE_PROCNAME    "CreateInterface"
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

typedef void* (*InstantiateInterfaceFn)();

// Used internally to register classes.
class InterfaceReg
{
public:
    InterfaceReg(InstantiateInterfaceFn fn, const char *pName);

public:
    InstantiateInterfaceFn  m_CreateFn;
    const char              *m_pName;

    InterfaceReg            *m_pNext; // For the global list.
    static InterfaceReg     *s_pInterfaceRegs;
};

#define EXPOSE_INTERFACE_FN(functionName, interfaceName, versionName) \
    static InterfaceReg __g_Create##interfaceName##_reg(functionName, versionName);

#define EXPOSE_INTERFACE(className, interfaceName, versionName) \
    static void* __Create##className##_interface() {return (interfaceName *)new className;}\
    static InterfaceReg __g_Create##className##_reg(__Create##className##_interface, versionName );

// Use this to expose a singleton interface with a global variable you've created.
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, globalVarName) \
    static void* __Create##className##interfaceName##_interface() {return (interfaceName *)&globalVarName;}\
    static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName);

// Use this to expose a singleton interface. This creates the global variable for you automatically.
#define EXPOSE_SINGLE_INTERFACE(className, interfaceName, versionName) \
    static className __g_##className##_singleton;\
    EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, __g_##className##_singleton)

//DLL_EXPORT 
void* CreateInterface(const char *pName, int *pReturnCode);

/// Return millisecond
uint64 GetAbsoluteMsecTime();

template <class T>
class Handler
{
public:
    // explicit construction.
    explicit Handler(T* pointer = NULL)
    {
        if (pointer)
            Register(pointer);
        else
            _index = size_t(-1);
    }
    // assign operation for new hander.
    void operator =(Handler* other)
    {
        _index = other->_index;
    }
    // get real class pointer
    T* Get() const
    {
        if (_index == size_t(-1))
            return NULL;

        return _container[_index];
    }
    // delete global class pointer in container.  
    void Delete(T* pointer, bool deletePointer = false);
private:
    void Register(T* pointer);
    size_t GetFreeIndex();
private:
    size_t _index;
    static std::vector<T*> _container;
};

// declare a class container.
template <class T> 
std::vector<T*> Handler<T>::_container;

template <class T>
void Handler<T>::Register(T* pointer)
{
    size_t count = _container.size(); 
    for (size_t i = 0; i != count; ++i)
    {
        if (_container[i] == pointer)
        {
            _index = i;
            return;
        }
    }

    size_t newIndex = GetFreeIndex();
    if (newIndex != size_t(-1))
    {
        _index = newIndex;
        _container[_index] = pointer;
        return;
    }

    _index = _container.size();
    _container.push_back(pointer);
}

template <class T>
size_t Handler<T>::GetFreeIndex()
{
    size_t count = _container.size(); 
    for (size_t i = 0; i != count; ++i)
    {
        if (_container[i] == NULL)
        {
            return i;
        }
    }
    return size_t(-1);
}

template <class T>
void Handler<T>::Delete(T* pointer, bool deletePointer/* = false*/)
{
    size_t count = _container.size(); 
    for (size_t i = 0; i != count; ++i)
    {
        if (_container[i] == pointer)
        {
            _container[i] = NULL;

            if (i == _index)
                _index = size_t(-1);

            break;
        }
    }

    if (deletePointer)
    {
        delete pointer;
    }
}

#define DUMP_BINARY_DATA(pData, pDataLen, numNewLine, logfunc)          \
                do                                                      \
                {                                                       \
                    unsigned char* _pDataPtr_ = (unsigned char*)pData;  \
                    unsigned int _count_n_ = 1;                         \
                    unsigned int _idx_;                                 \
                    for (_idx_ = 0; _idx_ < pDataLen; ++_idx_)          \
                    {                                                   \
                        if (--_count_n_ == 0)                           \
                        {                                               \
                            logfunc("\n");                              \
                            _count_n_ = numNewLine;                     \
                        }                                               \
                        logfunc("%02x ", *_pDataPtr_++);                \
                    }                                                   \
                    logfunc("\n");                                      \
                } while (0);


template <class Logfunc>
void DumpBinaryData(void* pData, unsigned int pDataLen, unsigned int numNewLine, Logfunc logfunc)
{
    unsigned char* _pDataPtr_ = (unsigned char*)pData;
    unsigned int _count_n_ = 1;
    for (unsigned int _idx_ = 0; _idx_ < pDataLen; ++_idx_)
    {
        if (--_count_n_ == 0)
        {
            logfunc("\n");
            _count_n_ = numNewLine;
        }
        logfunc("%02x ", *_pDataPtr_++);  
    }
    logfunc("\n");
}

class atgReadFile
{
public:
    atgReadFile();
    ~atgReadFile();
public:
    bool                Open(const char* fullFileName, int model = 0);
    void                Close();
    uint32              GetLength();
    bool                Seek(uint32 offset);
    uint32              Read(void* buffer, uint32 size, uint32 count = 1);
    int8                ReadByte();
    int32               ReadDWord();
    float               ReadFloat();
protected:
#ifdef _ANDROID
    struct AAsset* _asset;
#else
    FILE* _pf;
#endif // _ANDROID
};

enum ColorOrder
{
    CO_ARGB,
    CO_RGBA
};

struct Image
{
    uint8*      imageData;
    uint16      width;
    uint16      height;
    uint8       bpp;                //Image color depth in bits per pixel (24/32 ??)
    uint32      loaderIdx;
    Image():imageData(0),width(0),height(0),bpp(0),loaderIdx(0)
    {

    }
};

class ImageLoader
{
    ImageLoader(){ }
public:

    class LoaderInterface
    {
    public:
        virtual Image* Load(const char* filename, bool yReversed, ColorOrder co = CO_RGBA) = 0;
        virtual Image* LoadMemory(const char* buffer, uint32 bufferSize, bool yReversed, ColorOrder co = CO_RGBA) = 0;
        virtual void Release( Image* image ) = 0;
        virtual const std::string& FileExt() = 0; //file name extension
    };

    static ImageLoader& GetInstance();

    Image* Load(const char* filename, bool yReversed, ColorOrder co = CO_RGBA);
    void Release(Image* image);

    void AddLoader(LoaderInterface* loader);

public:
    static std::string GetFileExt(const char* filename);
    static std::string GetFilePath(const char* filename);
private:
    std::vector<LoaderInterface*> loaders;
};



//////////////////////////////////////////////////////////////////////////
// TAG Í¼Æ¬¶ÁÈ¡
// These defines are used to tell us about the type of TARGA file it is
#define TGA_RGB      2      // This tells us it's a normal RGB (really BGR) file
#define TGA_A        3      // This tells us it's a ALPHA file
#define TGA_RLE     10      // This tells us that the targa is Run-Length Encoded (RLE)

#pragma pack( push, 1 )         //½ûÖ¹×Ö½Ú×Ô¶¯¶ÔÆë
typedef struct
{
    uint8       descLen;
    uint8       cmapType;
    uint8       imageType;
    uint16      cmapStart;
    uint16      cmapEntries;
    uint8       cmapBits;
    uint16      xOffset;
    uint16      yOffset;
}TGA_Header_Info;

typedef struct 
{
    TGA_Header_Info head;
    uint16          width;
    uint16          height;
    uint8           bpp;
    uint8           attrib;
}TGA_Header;

typedef struct _tga_Image : public Image
{
    bool        bCompressed;        //Compressed or Uncompressed

    _tga_Image():bCompressed(false){}
}TGA_Image;
#pragma pack( pop )

class TGA_Loader : public ImageLoader::LoaderInterface
{
public:
    virtual Image*      Load(const char* filename, bool yReversed, ColorOrder co = CO_RGBA);
    virtual Image*      LoadMemory(const char* buffer, uint32 bufferSize, bool yReversed, ColorOrder co = CO_RGBA);
    virtual void        Release( Image* image );
    virtual const std::string& FileExt();

public:
    static bool         Load( TGA_Image* image, const char* filename, bool yReversed, ColorOrder co = CO_RGBA);
    static bool         LoadMemory(TGA_Image* image, const char* buffer, uint32 bufferSize, bool yReversed, ColorOrder co = CO_RGBA);
    static void         Release( TGA_Image* image );
protected:
    static bool         LoadUncompressedTGA( TGA_Image* image, const char* buffer, uint32 bufferSize, TGA_Header* header, bool yReversed, ColorOrder co );
    static bool         LoadCompressedTGA( TGA_Image* image, const char* buffer, uint32 bufferSize, TGA_Header* header, bool yReversed, ColorOrder co );
};

struct JPEG_Image : public Image
{
    JPEG_Image()
    {
    }
};

class JPEG_Loader : public ImageLoader::LoaderInterface
{
public:
    virtual Image*      Load(const char* filename, bool yReversed, ColorOrder co = CO_RGBA);
    virtual Image*      LoadMemory(const char* buffer, uint32 bufferSize, bool yReversed, ColorOrder co = CO_RGBA);
    virtual void        Release( Image* image );
    virtual const std::string& FileExt();

public:
    static bool         Load( JPEG_Image* image, const char* filename, bool yReversed, ColorOrder co = CO_RGBA );
    static bool         LoadMemory(JPEG_Image* image, const int8* buffer, uint32 bufferSize, bool yReversed, ColorOrder co = CO_RGBA);
    static void         Release( JPEG_Image* image );
protected:
    static void         SetMemorySource(struct jpeg_decompress_struct* Info, const int8* Buffer, uint32 Size);

    static void         SourceInit(struct jpeg_decompress_struct* Info);
    static uint8        SourceFill(struct jpeg_decompress_struct* Info);
    static void         SourceSkip(struct jpeg_decompress_struct* Info, long NrOfBytes);
    static void         SourceTerminate(struct jpeg_decompress_struct* Info);
};

struct PNG_Image : Image
{
    PNG_Image()
    {
    }
};

class PNG_Loader : public ImageLoader::LoaderInterface
{
public:
    virtual Image*      Load(const char* filename, bool yReversed, ColorOrder co = CO_RGBA);
    virtual Image*      LoadMemory(const char* buffer, uint32 bufferSize, bool yReversed, ColorOrder co = CO_RGBA);
    virtual void        Release( Image* image );
    virtual const std::string& FileExt();

public:
    static bool         Load( PNG_Image* image, const char* filename, bool yReversed, ColorOrder co = CO_RGBA );
    static bool         LoadMemory(PNG_Image* image, const int8* buffer, uint32 bufferSize, bool yReversed, ColorOrder co = CO_RGBA);
    static void         Release( PNG_Image* image );
};

class Properties
{
public:	
    typedef std::pair<std::string, std::string> PropertyPair;
    typedef std::map<std::string, std::string> PropertyMap;
    typedef std::vector<PropertyPair> PropertyVector;

private:
    PropertyVector propertyVector;
    PropertyMap propertyMap;
    std::string path;

public:
    bool load(const std::string &path);
    bool save(const std::string &path);

    int getPropertyCount()	{return propertyVector.size();}
    std::string getKey(int i)	{return propertyVector[i].first;}
    std::string getString(int i)	{return propertyVector[i].second;}
    bool hasKey(const std::string &key) const;

    bool getBool(const std::string &key) const;
    int getInt(const std::string &key) const;
    int getInt(const std::string &key, int min, int max) const;
    float getFloat(const std::string &key) const;
    float getFloat(const std::string &key, float min, float max) const;
    const std::string &getString(const std::string &key) const;

    void setInt(const std::string &key, int value);
    void setBool(const std::string &key, bool value);
    void setFloat(const std::string &key, float value);
    void setString(const std::string &key, const std::string &value);
};

class Water
{
public:
    Water(int w, int h);
    ~Water(void);

    void                Updata();
    void                Drop(float xi, float yi);   //>xi,yiÎª°Ù·Ö±ÈÎ»ÖÃ
    void                CopyTo(void* buffer) { memcpy(buffer, buf2, size); }
    int                 GetBuffSize() const { return size; }
    void*               GetBuff() { return buf2; }

    const int Width;
    const int Height;

private:
    static const int r = 5;     //>Ë®²¨·¶Î§°ë¾¶
    static const int h = -1;    //>dorpµãµÄË®²¨Éî¶È
    int size;
    float* buf1;
    float* buf2;

    float               GetValue(float* buf, int x, int y);
    void                SetValue(float* buf, int x, int y, float value);
};

// Í¨¹ýR,G,BÉú³ÉÒ»ÕûÊý±íÊ¾ÑÕÉ«
#define YD_RGB(r,g,b)                                   \
( (uint32) ( (uint8)(b) |                               \
             ( (uint32(uint8(g))) << 8 ) |              \
             ( (uint32(uint8(r))) << 16 ) |             \
             0xff000000 )                               \
)

struct MiscVec3
{
    float x,y,z;

    MiscVec3():x(0),y(0),z(0) {}
    MiscVec3(float x, float y, float z):x(x),y(y),z(z) {}
};

struct MiscVec4
{
    float x,y,z,w;

    MiscVec4():x(0),y(0),z(0) {}
    MiscVec4(float x, float y, float z, float w):x(x),y(y),z(z),w(w) {}
};

struct MiscVec3 GetVec3Color(uint32 YD_COOLOR);
struct MiscVec4 GetVec4Color(uint32 YD_COOLOR);

// --------------------------------------------------------------------------------------

#define YD_COLOR_LIGHT_PINK             YD_RGB(255, 182, 193)        // 0  Ç³·Ûºì
#define YD_COLOR_PINK                   YD_RGB(255, 192, 203)        // 1  ·Ûºì
#define YD_COLOR_CRIMSON_RED            YD_RGB(220,  20,  60)        // 2  ÐÉºì (Éîºì)
#define YD_COLOR_LAVENDER_BLUSH         YD_RGB(255, 240, 245)        // 3  µ­×Ïºì
#define YD_COLOR_PALE_VIOLET_RED        YD_RGB(219, 112, 147)        // 4  Èõ×ÏÂÞÀ¼ºì
#define YD_COLOR_HOT_PINK               YD_RGB(255, 105, 180)        // 5  ÈÈÇéµÄ·Ûºì
#define YD_COLOR_DEEP_PINK              YD_RGB(255,  20, 147)        // 6  Éî·Ûºì
#define YD_COLOR_MEDIUM_VIOLET_RED      YD_RGB(199,  21, 133)        // 7  ÖÐ×ÏÂÞÀ¼ºì
#define YD_COLOR_ORCHID                 YD_RGB(218, 112, 214)        // 8  À¼»¨×Ï
#define YD_COLOR_THISTLE                YD_RGB(216, 191, 216)        // 9  ¼»
#define YD_COLOR_PLUM                   YD_RGB(221, 160, 221)        // 10 Àî×Ó×Ï
#define YD_COLOR_VIOLET                 YD_RGB(238, 130, 238)        // 11 ×ÏÂÞÀ¼
#define YD_COLOR_MAGENTA                YD_RGB(255,   0, 255)        // 12 Ñóºì (Æ·ºì Ãµ¹åºì)
#define YD_COLOR_FUCHSIA                YD_RGB(244,   0, 161)        // 13 µÆÁýº£ÌÄ(×ÏºìÉ«)
#define YD_COLOR_DARK_MAGENTA           YD_RGB(139,   0, 139)        // 14 ÉîÑóºì
#define YD_COLOR_PURPLE                 YD_RGB(128,   0, 128)        // 15 ×ÏÉ«
#define YD_COLOR_MEDIUM_ORCHID          YD_RGB(186,  85, 211)        // 16 ÖÐÀ¼»¨×Ï
#define YD_COLOR_DARK_VIOLET            YD_RGB(148,   0, 211)        // 17 °µ×ÏÂÞÀ¼
#define YD_COLOR_DARK_ORCHID            YD_RGB(153,  50, 204)        // 18 °µÀ¼»¨×Ï
#define YD_COLOR_INDIGO                 YD_RGB( 75,   0, 130)        // 19 µåÇà (×ÏÀ¼É«)
#define YD_COLOR_BLUE_VIOLET            YD_RGB(138,  43, 226)        // 20 À¶×ÏÂÞÀ¼
#define YD_COLOR_MEDIUM_PURPLE          YD_RGB(147, 112, 219)        // 21 ÖÐ×ÏÉ«
#define YD_COLOR_MEDIUM_SLATE_BLUE      YD_RGB(123, 104, 238)        // 22 ÖÐ°åÑÒÀ¶
#define YD_COLOR_SLATE_BLUE             YD_RGB(106,  90, 205)        // 23 °åÑÒÀ¶
#define YD_COLOR_DARK_SLATE_BLUE        YD_RGB( 72,  61, 139)        // 24 °µ°åÑÒÀ¶
#define YD_COLOR_LAVENDER               YD_RGB(230, 230, 250)        // 25 Ñ¬ÒÂ²Ýµ­×Ï
#define YD_COLOR_GHOST_WHITE            YD_RGB(248, 248, 255)        // 26 ÓÄÁé°×
#define YD_COLOR_BLUE                   YD_RGB(  0,   0, 255)        // 27 ´¿À¶
#define YD_COLOR_MEDIUM_BLUE            YD_RGB(  0,   0, 205)        // 28 ÖÐÀ¶É«
#define YD_COLOR_MIDNIGHT_BLUE          YD_RGB( 25,  25, 112)        // 29 ÎçÒ¹À¶
#define YD_COLOR_DARK_BLUE              YD_RGB(  0,   0, 139)        // 30 °µÀ¶É«
#define YD_COLOR_NAVY_BLUE              YD_RGB(  0,   0, 128)        // 31 º£¾üÀ¶
#define YD_COLOR_ROYAL_BLUE             YD_RGB( 65, 105, 225)        // 32 »Ê¼ÒÀ¶ (±¦À¶)
#define YD_COLOR_CORN_FLOWER_BLUE       YD_RGB(100, 149, 237)        // 33 Ê¸³µ¾ÕÀ¶
#define YD_COLOR_LIGHT_STEEL_BLUE       YD_RGB(176, 196, 222)        // 34 ÁÁ¸ÖÀ¶
#define YD_COLOR_LIGHT_SLATE_GRAY       YD_RGB(119, 136, 153)        // 35 ÁÁÊ¯°å»Ò
#define YD_COLOR_SLATE_GRAY             YD_RGB(112, 128, 144)        // 36 Ê¯°å»Ò
#define YD_COLOR_DODGER_BLUE            YD_RGB( 30, 144, 255)        // 37 µÀÆæÀ¶
#define YD_COLOR_ALICE_BLUE             YD_RGB(240, 248, 255)        // 38 °®ÀöË¿À¶
#define YD_COLOR_STEEL_BLUE             YD_RGB( 70, 130, 180)        // 39 ¸ÖÀ¶ (ÌúÇà)
#define YD_COLOR_LIGHT_SKY_BLUE         YD_RGB(135, 206, 250)        // 40 ÁÁÌìÀ¶É«
#define YD_COLOR_SKY_BLUE               YD_RGB(135, 206, 235)        // 41 ÌìÀ¶É«
#define YD_COLOR_DEEP_SKY_BLUE          YD_RGB(  0, 191, 255)        // 42 ÉîÌìÀ¶
#define YD_COLOR_LIGHT_BLUE             YD_RGB(173, 216, 230)        // 43 ÁÁÀ¶
#define YD_COLOR_POWDER_BLUE            YD_RGB(176, 224, 230)        // 44 »ðÒ©Çà
#define YD_COLOR_CADET_BLUE             YD_RGB( 95, 158, 160)        // 45 ¾ü·þÀ¶
#define YD_COLOR_AZURE                  YD_RGB(240, 255, 255)        // 46 ÎµÀ¶É«
#define YD_COLOR_LIGHT_CYAN             YD_RGB(224, 255, 255)        // 47 µ­ÇàÉ«
#define YD_COLOR_PALE_TURQUOISE         YD_RGB(175, 238, 238)        // 48 ÈõÂÌ±¦Ê¯
#define YD_COLOR_CYAN                   YD_RGB(  0, 255, 255)        // 49 ÇàÉ«
#define YD_COLOR_AQUA                   YD_RGB(175, 223, 228)        // 50 Ë®É«
#define YD_COLOR_DARK_TURQUOISE         YD_RGB(  0, 206, 209)        // 51 °µÂÌ±¦Ê¯
#define YD_COLOR_DARK_SLATE_GRAY        YD_RGB( 47,  79,  79)        // 52 °µÊ¯°å»Ò
#define YD_COLOR_DARK_CYAN              YD_RGB(  0, 139, 139)        // 53 °µÇàÉ«
#define YD_COLOR_TEAL                   YD_RGB(  0, 128, 128)        // 54 Ë®Ñ¼É«
#define YD_COLOR_MEDIUM_TURQUOISE       YD_RGB( 72, 209, 204)        // 55 ÖÐÂÌ±¦Ê¯
#define YD_COLOR_LIGHT_SEA_GREEN        YD_RGB( 32, 178, 170)        // 56 Ç³º£ÑóÂÌ
#define YD_COLOR_TURQUOISE              YD_RGB( 64, 224, 208)        // 57 ÂÌ±¦Ê¯
#define YD_COLOR_AQUAMARINE             YD_RGB(127, 255, 212)        // 58 ±¦Ê¯±ÌÂÌ
#define YD_COLOR_MEDIUM_AQUAMARINE      YD_RGB(102, 205, 170)        // 59 ÖÐ±¦Ê¯±ÌÂÌ
#define YD_COLOR_MEDIUM_SPRINGGREEN     YD_RGB(  0, 250, 154)        // 60 ÖÐ´ºÂÌÉ«
#define YD_COLOR_MINT_CREAM             YD_RGB(245, 255, 250)        // 61 ±¡ºÉÄÌÓÍ
#define YD_COLOR_SPRING_GREEN           YD_RGB(  0, 255, 127)        // 62 ´ºÂÌÉ«
#define YD_COLOR_MEDIUM_SEA_GREEN       YD_RGB( 60, 179, 113)        // 63 ÖÐº£ÑóÂÌ
#define YD_COLOR_SEA_GREEN              YD_RGB( 46, 139,  87)        // 64 º£ÑóÂÌ
#define YD_COLOR_HONEYDEW               YD_RGB(240, 255, 240)        // 65 ÃÛ¹ÏÉ«
#define YD_COLOR_LIGHT_GREEN            YD_RGB(144, 238, 144)        // 66 µ­ÂÌÉ«
#define YD_COLOR_PALE_GREEN             YD_RGB(152, 251, 152)        // 67 ÈõÂÌÉ«
#define YD_COLOR_DARK_SEA_GREEN         YD_RGB(143, 188, 143)        // 68 °µº£ÑóÂÌ
#define YD_COLOR_LIME_GREEN             YD_RGB( 50, 205,  50)        // 69 ÉÁ¹âÉîÂÌ
#define YD_COLOR_LIME                   YD_RGB(  0, 255,   0)        // 70 ÉÁ¹âÂÌ
#define YD_COLOR_FOREST_GREEN           YD_RGB( 34, 139,  34)        // 71 É­ÁÖÂÌ
#define YD_COLOR_GREEN                  YD_RGB(  0, 128,   0)        // 72 ´¿ÂÌ
#define YD_COLOR_DARK_GREEN             YD_RGB(  0, 100,   0)        // 73 °µÂÌÉ«
#define YD_COLOR_CHARTREUSE             YD_RGB(127, 255,   0)        // 74 ²éÌØ¾ÆÂÌ (»ÆÂÌÉ«)
#define YD_COLOR_LAWN_GREEN             YD_RGB(124, 252,   0)        // 75 ²ÝÆºÂÌ
#define YD_COLOR_GREEN_YELLOW           YD_RGB(173, 255,  47)        // 76 ÂÌ»ÆÉ«
#define YD_COLOR_DARK_OLIVE_GREEN       YD_RGB( 85, 107,  47)        // 77 °µéÏé­ÂÌ
#define YD_COLOR_YELLOW_GREEN           YD_RGB(154, 205,  50)        // 78 »ÆÂÌÉ«
#define YD_COLOR_OLIVE_DRAB             YD_RGB(107, 142,  35)        // 79 éÏé­ºÖÉ«
#define YD_COLOR_BEIGE                  YD_RGB(245, 245, 220)        // 80 Ã×É«(»Ò×ØÉ«)
#define YD_COLOR_LIGHT_GOLDENROD_YELLOW YD_RGB(250, 250, 210)        // 81 ÁÁ¾Õ»Æ
#define YD_COLOR_IVORY                  YD_RGB(255, 255, 240)        // 82 ÏóÑÀ
#define YD_COLOR_LIGHT_YELLOW           YD_RGB(255, 255, 224)        // 83 Ç³»ÆÉ«
#define YD_COLOR_YELLOW                 YD_RGB(255, 255,   0)        // 84 ´¿»Æ
#define YD_COLOR_OLIVE                  YD_RGB(128, 128,   0)        // 85 éÏé­
#define YD_COLOR_DARK_KHAKI             YD_RGB(189, 183, 107)        // 86 Éî¿¨ß´²¼
#define YD_COLOR_LEMON_CHIFFON          YD_RGB(255, 250, 205)        // 87 ÄûÃÊ³ñ
#define YD_COLOR_PALE_GOLDENROD         YD_RGB(238, 232, 170)        // 88 »Ò¾Õ»Æ
#define YD_COLOR_KHAKI                  YD_RGB(240, 230, 140)        // 89 ¿¨ß´²¼
#define YD_COLOR_GOLD                   YD_RGB(255, 215,   0)        // 90 ½ðÉ«
#define YD_COLOR_CORNSILK               YD_RGB(255, 248, 220)        // 91 ÓñÃ×Ë¿É« 
#define YD_COLOR_GOLDENROD              YD_RGB(218, 165,  32)        // 92 ½ð¾Õ»Æ 
#define YD_COLOR_DARK_GOLDENROD         YD_RGB(184, 134,  11)        // 93 °µ½ð¾Õ»Æ 
#define YD_COLOR_FLORAL_WHITE           YD_RGB(255, 250, 240)        // 94 »¨µÄ°×É« 
#define YD_COLOR_OLD_LACE               YD_RGB(253, 245, 230)        // 95 ¾ÉÀÙË¿ 
#define YD_COLOR_WHEAT                  YD_RGB(245, 222, 179)        // 96 Ð¡ÂóÉ« 
#define YD_COLOR_MOCCASIN               YD_RGB(255, 228, 181)        // 97 Â¹Æ¤Ñ¥ 
#define YD_COLOR_ORANGE                 YD_RGB(255, 165,   0)        // 98 ³ÈÉ« 
#define YD_COLOR_PAPAYA_WHIP            YD_RGB(255, 239, 213)        // 99 ·¬Ä¾¹Ï 
#define YD_COLOR_BLANCHED_ALMOND        YD_RGB(255, 235, 205)        // 100 ·¢°×µÄÐÓÈÊÉ« 
#define YD_COLOR_NAVAJO_WHITE           YD_RGB(255, 222, 173)        // 101 ÍÁÖø°× 
#define YD_COLOR_ANTIQUE_WHITE          YD_RGB(250, 235, 215)        // 102 ¹Å¶­°× 
#define YD_COLOR_TAN                    YD_RGB(210, 180, 140)        // 103 ²èÉ« 
#define YD_COLOR_BURLY_WOOD             YD_RGB(222, 184, 135)        // 104 Ó²Ä¾É« 
#define YD_COLOR_BISQUE                 YD_RGB(255, 228, 196)        // 105 ÌÕÅ÷»Æ 
#define YD_COLOR_DARK_ORANGE            YD_RGB(255, 140,   0)        // 106 Éî³ÈÉ« 
#define YD_COLOR_LINEN                  YD_RGB(250, 240, 230)        // 107 ÑÇÂé²¼ 
#define YD_COLOR_PERU                   YD_RGB(205, 133,  63)        // 108 ÃØÂ³ 
#define YD_COLOR_PEACH_PUFF             YD_RGB(255, 218, 185)        // 109 ÌÒÈâÉ« 
#define YD_COLOR_SANDY_BROWN            YD_RGB(244, 164,  96)        // 110 É³×ØÉ« 
#define YD_COLOR_CHOCOLATE              YD_RGB(210, 105,  30)        // 111 ÇÉ¿ËÁ¦ 
#define YD_COLOR_SADDLE_BROWN           YD_RGB(139,  69,  19)        // 112 Âí°°×ØÉ« 
#define YD_COLOR_SEASHELL               YD_RGB(255, 245, 238)        // 113 º£±´¿Ç 
#define YD_COLOR_SIENNA                 YD_RGB(160,  82,  45)        // 114 »ÆÍÁô÷É« 
#define YD_COLOR_LIGHT_SALMON           YD_RGB(255, 160, 122)        // 115 Ç³öÙÓãÈâÉ« 
#define YD_COLOR_CORAL                  YD_RGB(255, 127,  80)        // 116 Éºº÷ 
#define YD_COLOR_ORANGE_RED             YD_RGB(255,  69,   0)        // 117 ³ÈºìÉ« 
#define YD_COLOR_DARK_SALMON            YD_RGB(233, 150, 122)        // 118 ÉîÏÊÈâ(öÙÓã)É« 
#define YD_COLOR_TOMATO                 YD_RGB(255,  99,  71)        // 129 ·¬ÇÑºì 
#define YD_COLOR_MISTY_ROSE             YD_RGB(255, 228, 225)        // 120 ±¡ÎíÃµ¹å 
#define YD_COLOR_SALMON                 YD_RGB(250, 128, 114)        // 121 ÏÊÈâ(öÙÓã)É« 
#define YD_COLOR_SNOW                   YD_RGB(255, 250, 250)        // 122 Ñ© 
#define YD_COLOR_LIGHT_CORAL            YD_RGB(240, 128, 128)        // 123 µ­Éºº÷É« 
#define YD_COLOR_ROSY_BROWN             YD_RGB(188, 143, 143)        // 124 Ãµ¹å×ØÉ« 
#define YD_COLOR_INDIAN_RED             YD_RGB(205,  92,  92)        // 125 Ó¡¶Èºì 
#define YD_COLOR_RED                    YD_RGB(255,   0,   0)        // 126 ´¿ºì 
#define YD_COLOR_BROWN                  YD_RGB(165,  42,  42)        // 127 ×ØÉ« 
#define YD_COLOR_FIRE_BRICK             YD_RGB(178,  34,  34)        // 128 ÄÍ»ð×© 
#define YD_COLOR_DARK_RED               YD_RGB(139,   0,   0)        // 139 ÉîºìÉ« 
#define YD_COLOR_MAROON                 YD_RGB(128,   0,   0)        // 130 ÀõÉ« 
#define YD_COLOR_WHITE                  YD_RGB(255, 255, 255)        // 131 ´¿°× 
#define YD_COLOR_WHITE_SMOKE            YD_RGB(245, 245, 245)        // 132 °×ÑÌ 
#define YD_COLOR_GAINSBORO              YD_RGB(220, 220, 220)        // 133 ¸ýË¹²©ÂÞ»ÒÉ« 
#define YD_COLOR_LIGHT_GREY             YD_RGB(211, 211, 211)        // 134 Ç³»ÒÉ« 
#define YD_COLOR_SILVER                 YD_RGB(192, 192, 192)        // 135 Òø»ÒÉ« 
#define YD_COLOR_DARK_GRAY              YD_RGB(169, 169, 169)        // 136 Éî»ÒÉ« 
#define YD_COLOR_GRAY                   YD_RGB(128, 128, 128)        // 137 »ÒÉ«
#define YD_COLOR_DIM_GRAY               YD_RGB(105, 105, 105)        // 138 °µµ­µÄ»ÒÉ« 
#define YD_COLOR_BLACK                  YD_RGB(  0,   0,   0)        // 139 ´¿ºÚ 

// --------------------------------------------------------------------------------------

// Ö§³ÖÄ§ÊÞÕù°ÔmdxÄ£ÐÍ

#ifndef MAX_PATH
    #define MAX_PATH 260 
#endif

#define MdxNodesMaxLength 256

struct MdxFloat2
{
    float c[2];
};

struct MdxFloat3
{
    float c[3]; // components
};

struct MdxFloat4
{
    float c[4];
};

struct MdxBoundingBox
{
    float radius;
    MdxFloat3 min;
    MdxFloat3 max;
};

#define MdxNameLength 80
struct MdxInfo
{
    char name[MdxNameLength];
    char path[MAX_PATH];
    MdxBoundingBox boundingBox;
    uint32 blendTime;
};

#define MdxSequenceMaxLength 16
struct MdxSequence
{
    char    name[MdxNameLength];
    int     intervalStart;
    int     intervalEnd;
    float   moveSpeed;
    uint32   nonLooping;
    float   rarity;
    uint32   syncPoint;
    MdxBoundingBox boundingBox;
};

#define MdxSequencesMaxLength 16
struct MdxSequenceSet
{
    uint8 number;
    MdxSequence sequences[MdxSequencesMaxLength];
};


struct MdxTrackDataF
{
    uint32 time;
    float value;

    float inTran;
    float outTran;
};

struct MdxTrackDataDW
{
    uint32 time;
    uint32 value;

    uint32 inTran;
    uint32 outTran;
};

struct MdxTrackDataF3
{
    uint32 time;
    MdxFloat3 value;

    MdxFloat3 inTran;
    MdxFloat3 outTran;
};

#define MdxDefaultContainerMaxLength 8
struct MdxMaterialLayerAlpha  //  uint32 'KMTA';
{
    // uint32 numberOfTracks
    uint32 interpolationType;
    int globalSequenceId;

    uint8 numberTracks;
    MdxTrackDataF alphaTracks[MdxDefaultContainerMaxLength];

    int getSize()
    {
        int selfSize;
        selfSize = 3 * sizeof(uint32);

        int count = numberTracks;
        selfSize += count * (sizeof(uint32) + sizeof(float));
        if(interpolationType > 1)
        {
            selfSize += count * 2 * sizeof(float);
        }
        return selfSize;
    }
};

struct MdxMaterialLayerTexture // uint32 'KMTF';
{
    // uint32 numberOfTracks
    int interpolationType;
    uint32 globalSequenceId;

    uint8 numberTracks;
    MdxTrackDataDW textrueTarcks[MdxDefaultContainerMaxLength];

    int getSize()
    {
        int selfSize;
        selfSize = 3 * sizeof(uint32);

        int count = numberTracks;
        selfSize += count * 2 * sizeof(uint32);
        if(interpolationType > 1)
        {
            selfSize += count * 2 * sizeof(float);
        }

        return selfSize;
    }
};

struct MdxMaterialLayer // uint32 'LAYS';
{
    uint32 filterMode;   //0 - None
                        //1 - Transparent
                        //2 - Blend
                        //3 - Additive
                        //4 - AddAlpha
                        //5 - Modulate
                        //6 - Modulate2x
    uint32 shadingFlags;
                        //#1   - Unshaded
                        //#2   - SphereEnvironmentMap
                        //#4   - ???
                        //#8   - ???
                        //#16  - TwoSided
                        //#32  - Unfogged
                        //#64  - NoDepthTest
                        //#128 - NoDepthSet
    uint32 textureId;
    int textureAnimationId;
    uint32 coordId;
    float alphaValue;

    MdxMaterialLayerAlpha alpha;
    MdxMaterialLayerTexture texture;
};

struct MdxMaterial
{
    enum eFlag {
        ConstantColor          = 1,
        Unknown1               = ConstantColor << 1,
        Unknown2               = ConstantColor << 2,
        SortPrimitivesNearZ    = ConstantColor << 3,
        SortPrimitivesFarZ     = ConstantColor << 4,
        FullResolution         = ConstantColor << 5
    };
    uint32 priorityPlane;
    uint32 flags;

    uint8 numberLayers;
    MdxMaterialLayer layers[MdxDefaultContainerMaxLength];
};

struct MdxTexture // 'TEXS'
{
    uint32   replaceableId;
    char    name[MAX_PATH];
    uint32   flags;      //#1 - WrapWidth
                        //#2 - WrapHeight
};

struct MdxFace
{
    unsigned short index[3];
};


struct MdxMatrixList
{
    int numberOfNode;
    int nodeIndices[MdxNodesMaxLength];
};

struct MdxGeoset
{
    uint32 numberVertexs;
    MdxFloat3* vertexPositions;    // 'VRTX'
    uint32 numberNormals;
    MdxFloat3* vertexNormals;      // 'NRMS'
    int numberFaceTypeGroups;
    uint32* faceTypeGroups;        // 'PTYP'   
                                    // 0: points
                                    // 1: lines
                                    // 2: line loop
                                    // 3: line strip
                                    // 4: triangles
                                    // 5: triangle strip
                                    // 6: triangle fan
                                    // 7: quads
                                    // 8: quad strip
                                    // 9: polygons
    int numberFaceGroups;
    uint32* faceGroups;            // 'PCNT'
    int numberFaces;
    MdxFace* faces;               // 'PVTX'
    int numberGroupIndices;
    uint8* groupIndices;           // 'GNDX'
    int numberMatrixList;
    MdxMatrixList* matrixLists;     // 'MTGC'
    int numberMatrixIndices;

    uint32 materialId;
    uint32 selectionGroup;
    uint32 selectionFlags;           //0  - None
                                    //#1 - ???
                                    //#2 - ???
                                    //#4 - Unselectable
    MdxBoundingBox boundingBox;
    int numberExtents;
    MdxBoundingBox* extents;
    uint32 numberOfTextureVertexGroups;        // 'UVAS'
    int numberVertexTexturePositions;
    MdxFloat2* vertexTexturePositions;  // 'UVBS'
};

// --------------------------------
enum MDX_INTERPOLATION_TYPE
{
    INTERPOLATION_TYPE_NONE = 0,
    INTERPOLATION_TYPE_LINEAR,
    INTERPOLATION_TYPE_HERMITE,
    INTERPOLATION_TYPE_BEZIER,
};

struct keyTranslate
{
    uint32 time;
    MdxFloat3 vector;
    MdxFloat3 inTran;
    MdxFloat3 outTran;
};

struct keyRotation
{
    uint32 time;
    MdxFloat4 rotation;
    MdxFloat4 inTran;
    MdxFloat4 outTran;
};

struct keyScaling
{
    uint32 time;
    MdxFloat3 vector;
    MdxFloat3 inTran;
    MdxFloat3 outTran;
};

struct TRANSLATE_LIST
{
    uint32 interpolationType;
    int globalSequenceId;

    std::vector<keyTranslate> keyList;
};

struct ROTATION_LIST
{
    uint32 interpolationType;
    int globalSequenceId;

    std::vector<keyRotation> keyList;
};

struct SCALING_LIST
{
    uint32 interpolationType;
    int globalSequenceId;

    std::vector<keyScaling> keyList;
};

enum MdxdeFlags
{
    NODE_FLAG_HELPER                   = 0,
    NODE_FLAG_DONT_INHERIT_TRANSLATION = 1,
    NODE_FLAG_DONT_INHERIT_ROTATION    = 2,
    NODE_FLAG_DONT_INHERIT_SCALING     = 4,
    NODE_FLAG_BILLBOARDED              = 8,
    NODE_FLAG_BILLBOARDED_LOCK_X       = 16,
    NODE_FLAG_BILLBOARDED_LOCK_Y       = 32,
    NODE_FLAG_BILLBOARDED_LOCK_Z       = 64,
    NODE_FLAG_CAMERA_ANCHORED          = 128,
    NODE_FLAG_BONE                     = 256,
    NODE_FLAG_LIGHT                    = 512,
    NODE_FLAG_EVENT_OBJECT             = 1024,
    NODE_FLAG_ATTACHMENT               = 2048,
    NODE_FLAG_PARTICLE_EMITTER         = 4096,
    NODE_FLAG_COLLISION_SHAPE          = 8192,
    NODE_FLAG_RIBBON_EMITTER           = 16384,
    NODE_FLAG_UNSHADED                 = 32768,
    NODE_FLAG_EMITTER_USES_MDL         = 32768,
    NODE_FLAG_SORT_PRIMITIVES_FAR_Z    = 65536,
    NODE_FLAG_EMITTER_USES_TGA         = 65536,
    NODE_FLAG_LINE_EMITTER             = 131072,
    NODE_FLAG_UNFOGGED                 = 262144,
    NODE_FLAG_MODEL_SPACE              = 524288,
    NODE_FLAG_XY_QUAD                  = 1048576
};

struct MdxNode
{
    char name[MdxNameLength];
    int objectId;
    int parentId;

    TRANSLATE_LIST  translateList;
    ROTATION_LIST   rotationList;
    SCALING_LIST    scalingList;

    uint32 helper:1;
    uint32 dontInheritTranslation:1;
    uint32 dontInheritRotation:1;
    uint32 dontInheritScaling:1;
    uint32 billboarded:1;
    uint32 billboardedLockX:1;
    uint32 billboardedLockY:1;
    uint32 billboardedLockZ:1;
    uint32 cameraAnchored:1;
    uint32 bone:1;
    uint32 light:1;
    uint32 eventObject:1;
    uint32 attachment:1;
    uint32 particleEmitter:1;
    uint32 collisionShape:1;
    uint32 ribbonEmitter:1;
    uint32 unshadedOrEmitterUsesMdl:1;
    uint32 sortPrimitivesFarZOrEmitterUsesTag:1;
    uint32 lineEmitter:1;
    uint32 unfogged:1;
    uint32 modelSpace:1;
    uint32 xyQuad:1;

    void* userData;

    ~MdxNode()
    {

    }
};

struct MdxBone
{
    MdxNode node;

    // bone specific
    int geosetId;
    uint32 geoAnimationId;
};

struct MdxHelp
{
    MdxNode node;
};

struct MdxAttachment 
{
    MdxNode node;

    char path[MAX_PATH];
    int attachmentId;
    float visibility; //KATV
};

struct MdxPE2TrackList
{
    float defaultValue;
    uint32 interpolationType;
    uint32 globalSequenceId;

    std::vector<MdxTrackDataF> keyList;
};

struct MdxParticleEmitter2 
{
    MdxNode node;
    float speed;
    float variation;
    float latitude;
    float gravity;
    float lifespan;
    float emissionRate;
    float length;
    float width;
    uint32 filterMode;    // 0: blend
                        // 1: additive
                        // 2: modulate
                        // 3: modulate 2x
                        // 4: alpha key
    uint32 rows;
    uint32 columns;
    uint32 headOrTail;    // 0: head
                        // 1: tail
                        // 2: both
    float tailLength;
    float time;

    MdxFloat3 segmentColors[3];
    uint8 segmentAlpha[3];
    MdxFloat3 segmentScaling;
    uint32 headInterval[3];
    uint32 headDecayInterval[3];
    uint32 tailInterval[3];
    uint32 tailDecayInterval[3];
    uint32 textureId;
    uint32 squirt;
    uint32 priorityPlane;
    uint32 replaceableId;

    MdxPE2TrackList speedTracks;
    MdxPE2TrackList variationTracks;
    MdxPE2TrackList latitudeTracks;
    MdxPE2TrackList gravityTracks;
    MdxPE2TrackList emissionRateTracks;
    MdxPE2TrackList lengthTracks;
    MdxPE2TrackList widthTracks;
    MdxPE2TrackList visibilityTracks;
};

struct MdxRE2TrackList
{
    uint32 interpolationType;
    uint32 globalSequenceId;

    std::vector<MdxTrackDataF> keyList;
};

struct MdxRE2TrackListF3
{
    uint32 interpolationType;
    uint32 globalSequenceId;

    std::vector<MdxTrackDataF3> keyList;
};

struct MdxRE2TrackListDW
{
    uint32 interpolationType;
    uint32 globalSequenceId;

    std::vector<MdxTrackDataDW> keyList;
};

struct MdxRibbonEmitter 
{
    MdxNode node;
    float heightAbove;
    float heightBelow;
    float alpha;
    MdxFloat3 color;
    float lifespan;
    uint32 textureSlot;
    uint32 emissionRate;
    uint32 rows;
    uint32 columns;
    uint32 materialId;
    float gravity;

    MdxRE2TrackList heightAboveTracks;
    MdxRE2TrackList heightBelowTracks;
    MdxRE2TrackList alphaTracks;
    MdxRE2TrackListF3 colorTracks;
    MdxRE2TrackListDW textureSlotTracks;
    MdxRE2TrackList visibilityTracks;
};

struct MdxCollisionShape
{
    MdxNode node;
    uint32 type;     //0 - Box
                    //1 - ???
                    //2 - Sphere
    uint8 numberVertex;
    MdxFloat3 vertices[2];
    float boundsRadius;     // if type == 2
};


class MdxModel
{
public:
    MdxModel();
    ~MdxModel(void);

    bool                    Load(const char* fullFileName);

    int                     GetNumberOfMesh();
    int                     GetNumberOfVertexs(uint32 meshIndex);
    MdxFloat3*              GetVertexs(uint32 meshIndex);

    int                     GetNumberOfFaces(uint32 meshIndex);
    MdxFace*                GetFace(uint32 meshIndex);

    MdxFloat3*              GetNormalsOfVertexs(uint32 meshIndex);
    MdxFloat2*              GetTexturesOfVertexs(uint32 meshIndex);

    MdxMaterial*            GetMeshMaterial(uint32 meshIndex);
    MdxTexture*             GetTexture(uint8 textureId);
            
    uint8                   GetNumberOfNodes();
    MdxNode*                GetNode(uint8 nodeId);
    MdxFloat3*              GetPivot(uint8 nodeId);

    const MdxMatrixList&    GetMatrixList(int meshIndex, int vertexIndex);

    int                     GetNumberAnimation();
    const MdxSequence&      GetAnimation(int index);
protected:

    bool                    loadVersionSection(int sectionSize);
    bool                    loadModelInfoSection(int sectionSize);
    bool                    loadSecquencesSection(int sectionSize);
    bool                    loadGlobalSequencesSection(int sectionSize);
    bool                    loadMaterialSeciton(int sectionSize);
    bool                    loadTexturesSection(int sectionSize);
    bool                    loadGeosetSection(int sectionSize);
    bool                    loadGeosetAnimationSection(int sectionSize);
    bool                    loadBoneSection(int sectionSize);
    bool                    loadHelpesSection(int sectionSize);
    bool                    loadAttachmentSection(int sectionSize);
    bool                    loadPivotPointSection(int sectionSize);
    bool                    loadParticleEmitters2Section(int sectionSize);
    bool                    loadRibbonEmittersSection(int sectionSize);
    bool                    loadCollisionShapeSection(int sectionSize);
    
    int                     loadBoundingBox(MdxBoundingBox& box);
    int                     loadSecquence(MdxSequence& sequence);
    int                     loadMaterial(MdxMaterial& material);
    int                     loadGeoset(MdxGeoset& geoset);
    int                     loadNode(MdxNode& node);
    int                     loadParticleEmitters2(MdxParticleEmitter2& PE2);
    int                     loadRibbonEmitters(MdxRibbonEmitter& RE);
    int                     loadCollisionShape(MdxCollisionShape& CS);

    uint32 _version;
    uint8 _numberSequences;
    MdxSequence _sequences[MdxSequenceMaxLength];
    uint8 _numberGolbalSequences;
    uint32 _golbalSequences[MdxSequenceMaxLength];
    uint8 _numberMaterials;
    MdxMaterial _materials[MdxDefaultContainerMaxLength];
    uint8 _numberTextures;
    MdxTexture _textures[MdxDefaultContainerMaxLength];
    uint8 _numberMesh;
    MdxGeoset _meshs[MdxDefaultContainerMaxLength];
    uint8 _numberBones;
    MdxBone _bones[MdxNodesMaxLength];
    uint8 _numberHelps;
    MdxHelp _helps[MdxNodesMaxLength];
    uint8 _numberNodes;
    MdxNode* _nodes[MdxNodesMaxLength];
    uint8 _numberPivots;
    MdxFloat3 _pivots[MdxNodesMaxLength];
    uint8 _numberPE2s;
    MdxParticleEmitter2 _PE2s[MdxNodesMaxLength];
    uint8 _numberREs;
    MdxRibbonEmitter _REs[MdxNodesMaxLength];
    uint8 _numberCSs;
    MdxCollisionShape _CSs[MdxNodesMaxLength];
    MdxInfo _info;
};


struct SmdVertex
{
    float position[3];
    float normal[3];
    float texcoord[2];

    //¿ØÖÆ¶¥µãµÄ¹Ç÷À½ÚµãºÍÈ¨ÖØ
    unsigned int numJoints;
    unsigned int *jointIDs;
    float *weights;

    SmdVertex()
    {
        numJoints = 0;
        jointIDs = NULL;
        weights = NULL;
    }

    //¿½±´¹¹Ôìº¯Êý
    SmdVertex(const SmdVertex &vertex)
    {
        memcpy(position, vertex.position, sizeof(float)*3);
        memcpy(normal, vertex.normal, sizeof(float)*3);
        memcpy(texcoord, vertex.texcoord, sizeof(float)*2);
        numJoints = vertex.numJoints;
        if (numJoints != 0)
        {
            jointIDs = new unsigned int[vertex.numJoints];
            weights = new float[vertex.numJoints];
            memcpy(jointIDs, vertex.jointIDs, sizeof(unsigned int)*numJoints);
            memcpy(weights, vertex.weights, sizeof(float)*numJoints);
        }
        else
        {
            jointIDs = NULL;
            weights = NULL;
        }
    }

    ~SmdVertex()
    {
        if (jointIDs != NULL)
            delete [] jointIDs;
        if (weights != NULL)
            delete [] weights;
    }
};



class SmdMaterial
{
public:
    //Í¸Ã÷ÊôÐÔ
    bool translucent;

private:
    std::string name;
    std::string basetexture;
    std::string bumpmap;
    std::string iris;
};

struct SmdGroup
{
    std::string name;
    std::vector<int> vertexIndices;
    SmdMaterial material;
};

struct SmdJointInfo
{
    std::string name;
    int parentID;
};

struct SmdKey
{
    float position[3];
    float rotation[4]; //Quaternion
};

struct SmdFrame
{
    unsigned int time;
    std::vector<SmdKey> jointkeys;
};

struct SmdAnimation
{
    std::string name;
    std::vector<SmdFrame> frames;
};


class SmdModel
{
public:
    SmdModel();
    ~SmdModel(void);

    bool            Load(const char* fullFileName);

    bool            LoadAnimation(const char* fullFileName);

protected:

    //¶ÁÈë¹Ç÷À¼Ü¹¹
    void            ParseNodes(std::istringstream &);
    //¶ÁÈë¹Ç÷ÀÔË¶¯ÐÅÏ¢
    void            ParseSkeleton(std::istringstream &, SmdAnimation &);
    //¶ÁÈë¶à±ßÐÎÐÅÏ¢
    void            ParseTriangles(std::istringstream &);


    std::vector<SmdVertex> vertices;
    std::vector<SmdJointInfo> hierarchy;
    std::vector<SmdAnimation> animations;
    std::vector<SmdGroup> groups;

    std::vector<int> hierarchyMap;

    bool texDirection;

    unsigned int numVertices;
    unsigned int numTriangles;
};

class ObjLoader
{

public:
    bool LoadGeometryFromOBJ( const char* strObjFile );
    bool LoadMaterialFromMTL( const char* strMtlile );





};