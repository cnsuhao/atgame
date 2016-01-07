#include "atgBase.h"
#include "atgMisc.h"
#include "atgMath.h"
#include <sstream>      // std::istringstream

const int IFACE_OK = 0;
const int IFACE_FAILED = 1;

InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;

InterfaceReg::InterfaceReg( InstantiateInterfaceFn fn, const char *pName ):m_CreateFn(fn),m_pName(pName)
{
    m_pNext = s_pInterfaceRegs;
    s_pInterfaceRegs = this;
}

void* CreateInterface(const char *pName, int *pReturnCode)
{
    InterfaceReg *pCur;

    for(pCur=InterfaceReg::s_pInterfaceRegs; pCur; pCur=pCur->m_pNext)
    {
        if(strcmp(pCur->m_pName, pName) == 0)
        {
            if ( pReturnCode )
            {
                *pReturnCode = IFACE_OK;
            }
            return pCur->m_CreateFn();
        }
    }

    if ( pReturnCode )
    {
        *pReturnCode = IFACE_FAILED;
    }
    return NULL;
}

#ifdef _WIN32
    static uint32 atgTimeTicksPerMillis;
#endif // _WIN32

#ifdef _ANDROID
    #include <android/asset_manager.h>
    extern AAssetManager* __assetManager;
#endif



extern uint64 GetAbsoluteMsecTime()
{
#ifdef _WIN32
    static bool Initialized = false;
    if (!Initialized)
    {
        LARGE_INTEGER tps;
        QueryPerformanceFrequency(&tps);
        atgTimeTicksPerMillis = uint32(tps.QuadPart / 1000L);
        Initialized = true;
    }

    LARGE_INTEGER queryTime;
    QueryPerformanceCounter(&queryTime);
    return queryTime.QuadPart / atgTimeTicksPerMillis;
#else
    struct timespec queryTime;
    clock_gettime(CLOCK_MONOTONIC, &queryTime);
    return (1000.0 * queryTime.tv_sec) + (0.000001 * queryTime.tv_nsec);
#endif // _WIN32
}

static uint32 gRandomSeed = 1;
void SetRandomSeed(uint32 seed)
{
    gRandomSeed = seed;
}

uint32 Rand()
{
    gRandomSeed = gRandomSeed * 214013 + 2531011;
    return (gRandomSeed >> 16) & 32767;
}

atgReadFile::atgReadFile()
{
#ifdef _ANDROID
    _asset = NULL;
#else
    _pf = NULL; 
#endif
}
atgReadFile::~atgReadFile()
{
    Close();
}

bool atgReadFile::Open(const char* fullFileName, int model)
{ 
#ifdef _ANDROID
    _asset = AAssetManager_open(__assetManager, fullFileName, AASSET_MODE_RANDOM);
    if (_asset == NULL) { LOG("Open file(%s) failure.\n", fullFileName); }
    return _asset != NULL;
#else
    _pf = fopen(fullFileName, "r+b");
    if (_pf == NULL) { LOG("Open file(%s) failure.\n", fullFileName); }
    return _pf != NULL; 
#endif 
}

void atgReadFile::Close() 
{ 
#ifdef _ANDROID
    if (_asset){
        AAsset_close(_asset);
        _asset = NULL;
    }
#else
    if(_pf)
    { 
        fclose(_pf); 
        _pf = NULL; 
    } 
#endif
}
    
uint32 atgReadFile::GetLength() 
{
#ifdef _ANDROID
    return (uint32)AAsset_getLength(_asset);
#else
    int positon = ftell(_pf);
    fseek(_pf, 0, SEEK_END);
    int length = ftell(_pf);
    fseek(_pf, positon, SEEK_SET);
    return length;
#endif
}
     
bool  atgReadFile::Seek(uint32 offset)
{ 
#ifdef _ANDROID
    return AAsset_seek(_asset, offset, SEEK_CUR) != -1;
#else
    return fseek(_pf, offset, SEEK_CUR) == 0; 
#endif
}
    
uint32 atgReadFile::Read(void* buffer, uint32 size, uint32 count) 
{ 
#ifdef _ANDROID
    int32 result = AAsset_read(_asset, buffer, size * count);
    return result > 0 ? ((size_t)result) / size : 0;
#else
    return fread(buffer, size, count, _pf); 
#endif
}

int8 atgReadFile::ReadByte()  
{ 
    static int const byteSize = sizeof(int8);
    int8 byte; 
    Read((char*)&byte, byteSize, 1); 
    return byte; 
}

int32 atgReadFile::ReadDWord() 
{ 
    static int const dwordSize = sizeof(int32);
    int32 dword; 
    Read((char*)&dword, dwordSize, 1); 
    return dword; 
}

float atgReadFile::ReadFloat() 
{ 
    static int const floatSize = sizeof(float);
    float real; 
    Read((char*)&real, floatSize, 1);
    return real; 
}

#define FVCC(a, b, c, d) (uint32)( ((uint32)d) + (((uint32)c)<<8) + (((uint32)b)<<16) + (((uint32)a)<<24) )
#define TAG(x) (uint32)( (((uint32)x&0x000000ff)<<24) + (((uint32)x&0x0000ff00)<<8) + (((uint32)x&0x00ff0000)>>8) + (((uint32)x&0xff000000)>>24) )

//////////////////////////////////////////////////////////////////////////
// TGA_Loader
bool TGA_Loader::Load( TGA_Image* image, const char* filename, bool yReversed, ColorOrder co )
{
    if ( filename == NULL )
        return false;

    atgReadFile reader;
    if (!reader.Open(filename))
    {
        return false;
    }
    uint32 srcBufLen = reader.GetLength();
    int8* SourceBuffer = new int8[srcBufLen];
    reader.Read(SourceBuffer, srcBufLen, 1);

    bool rt = LoadMemory(image, SourceBuffer, srcBufLen, yReversed, co);

    delete [] SourceBuffer;
    return true;
}

bool TGA_Loader::LoadMemory( TGA_Image* image, const char* buffer, uint32 bufferSize, bool yReversed, ColorOrder co )
{
    TGA_Header_Info uTGAcompare = { 0,0,2,0,0,0,0,0 };      //2为非压缩RGB格式        3  -  未压缩的，黑白图像
    TGA_Header_Info cTGAcompare = { 0,0,10,0,0,0,0,0 };     //10为压缩RGB格式

    TGA_Header header;
    const char* srcBuffer = buffer;
    uint32 srcBufLen = bufferSize;

    uint32 sizeOfHeader = sizeof(TGA_Header);
    memcpy(&header, srcBuffer, sizeOfHeader);
    srcBuffer += sizeOfHeader;
    srcBufLen -= sizeOfHeader;

    image->width = header.width;
    image->height = header.height;
    image->bpp = header.bpp;

    if ( header.bpp == 32 || header.bpp == 24 )
    {
    }
    else
    {
        LOG("Image type error!\n");
        return false;
    }

    if (header.head.descLen >0){
        srcBuffer += header.head.descLen;
        bufferSize -= header.head.descLen;
    }

    if ( memcmp( &uTGAcompare.cmapType, &header.head.cmapType, sizeof(header.head) - sizeof(uint8) ) == 0 ){            //未压缩TGA
        image->bCompressed = false;
        if ( !LoadUncompressedTGA( image, srcBuffer, srcBufLen, &header, yReversed, co ) ){
            LOG("Load uncompressed TGA failed!\n");
            return false;
        }
    }else if ( memcmp( &cTGAcompare.cmapType, &header.head.cmapType, sizeof(header.head) - sizeof(uint8) ) == 0 ){  //压缩TGA
        image->bCompressed = true;
        if ( !LoadCompressedTGA( image, srcBuffer, srcBufLen, &header, yReversed, co ) ){
            LOG("Load compressed TGA failed!\n");
            return false;
        }
    }else{
        LOG("Error TGA type!\n");
        return false;
    }

    return true;
}

bool TGA_Loader::LoadUncompressedTGA( TGA_Image* image, const char* buffer, uint32 bufferSize, TGA_Header* header, bool yReversed, ColorOrder co )
{
    AASSERT( buffer != NULL && image != NULL );
    uint16 bytePerPixel = image->bpp/8;
    uint32 imgSize = image->width * image->height * bytePerPixel;               //图像总字节数
    image->imageData = new uint8[imgSize];
    uint32 pitch =  image->width * bytePerPixel;
    bool bXReversed = ((header->attrib & 16) == 16);
    bool bYReversed = ((header->attrib & 32) == 32);

    const char* srcBuffer = buffer;
    uint32 srcBufLen = bufferSize;
    if (yReversed)
    {
        memcpy(image->imageData, buffer, imgSize);
        for (uint32 i = 0; i < imgSize; i+=bytePerPixel)
        {
            //bgra ==> rgba
            if (co == CO_RGBA)
            {
                atgMath::Swap(image->imageData[i], image->imageData[i + 2]);
            }else
            {
                // argb
                atgMath::Swap(image->imageData[i], image->imageData[i+3]);
                atgMath::Swap(image->imageData[i+1], image->imageData[i+2]);
            }
        }
    }
    else
    {
        for(uint32 i = 1; i <= image->height; ++i)
        { 
            memcpy(image->imageData + (pitch * (image->height - i)), srcBuffer, pitch);
            srcBuffer += pitch;
        }
    }

    return true;
}

bool TGA_Loader::LoadCompressedTGA( TGA_Image* image, const char* buffer, uint32 bufferSize, TGA_Header* header, bool yReversed, ColorOrder co )
{
    AASSERT( buffer != NULL && image!=NULL );

    uint32 bytePerPixel = image->bpp/8;
    uint32 imgSize = image->width * image->height * bytePerPixel;
    image->imageData = new uint8[imgSize];

    uint32 pixelcount = image->width * image->height;
    uint32 currentPixel = 0;        //当前正在读取的像素
    uint32 currentByte = 0;         //当前正在向图像中写入的像素
    uint8 *colorbuffer = (uint8*)malloc(bytePerPixel);  // 一个像素的存储空间s

    const char* srcBuffer = buffer;
    uint32 srcBufLen = bufferSize;
    do
    {
        uint8 chunkHeader  = *srcBuffer;        //存储ID块值的变量
        srcBuffer += sizeof(uint8);
        srcBufLen -= sizeof(uint8);
        if ( chunkHeader < 128 )            //RAW块
        {
            chunkHeader++;              // 变量值加1以获取RAW像素的总数

            for( int i = 0; i < chunkHeader; i++ )
            {
                memcpy(colorbuffer, srcBuffer, image->bpp);
                srcBuffer += image->bpp;
                srcBufLen -= image->bpp;
                image->imageData[currentByte] = colorbuffer[2];
                image->imageData[currentByte+1] = colorbuffer[1];
                image->imageData[currentByte+2] = colorbuffer[0];
                if ( bytePerPixel == 4 )
                    image->imageData[ currentByte+3] = colorbuffer[3];

                currentPixel++;
                currentByte += bytePerPixel;
            }
        }
        //下一段处理描述RLE段的“块”头。首先我们将chunkheader减去127来得到获取下一个颜色重复的次数。 
        else        
        {
            chunkHeader -= 127;         //减去127获得ID bit 的rid    开始循环拷贝我们多次读到内存中的像素，这由RLE头中的值规定。

            memcpy(colorbuffer, srcBuffer, image->bpp);
            srcBuffer += image->bpp;
            srcBufLen -= image->bpp;

            for( int i = 0; i < chunkHeader; i++ ){
                image->imageData[currentByte] = colorbuffer[2];             // 拷贝“R”字节
                image->imageData[currentByte + 1] = colorbuffer[1];         // 拷贝“G”字节
                image->imageData[currentByte + 2] = colorbuffer[0];         // 拷贝“B”字节
                if ( bytePerPixel == 4 )
                    image->imageData[currentByte+3] = colorbuffer[3];           // 拷贝“A”字节

                currentPixel++;
                currentByte += bytePerPixel;
            }
        }
    }while( currentPixel < pixelcount );

    free(colorbuffer);
    return true;
}

void TGA_Loader::Release( TGA_Image* image )
{
    if ( image )
    {
        if ( image->imageData )
            delete[] image->imageData;
    }
}

extern "C"
{
#include "jpeglib.h"
#include "jerror.h"
}

// Source manager structure
struct JPEG_SOURCE_MANAGER
{
    JPEG_SOURCE_MANAGER()
    {
        SourceBuffer = NULL;
        SourceBufferSize = 0;
        Buffer = NULL;
    }

    jpeg_source_mgr Manager;
    const int8* SourceBuffer;
    uint32 SourceBufferSize;
    const JOCTET* Buffer;
};

bool JPEG_Loader::Load( JPEG_Image* image, const char* filename, bool yReversed, ColorOrder co )
{
    atgReadFile reader;
    if (!reader.Open(filename))
    {
        return false;
    }
    uint32 srcBufLen = reader.GetLength();
    int8* SourceBuffer = new int8[srcBufLen];
    reader.Read(SourceBuffer, srcBufLen, 1);
    reader.Close();

    bool rs = LoadMemory(image, SourceBuffer, srcBufLen, yReversed, co);

    delete [] SourceBuffer;
    return rs;
}

bool JPEG_Loader::LoadMemory( JPEG_Image* image, const int8* buffer, uint32 bufferSize, bool yReversed, ColorOrder co )
{
    int32 i;
    int32 Stride;
    int32 Offset;
    int8 Opaque;
    JSAMPARRAY Pointer;
    jpeg_decompress_struct Info;
    jpeg_error_mgr ErrorManager;
    int8 * pTargetBuffer;

    uint32 srcBufLen = bufferSize;
    const int8* SourceBuffer = buffer;

    Info.err = jpeg_std_error(&ErrorManager);

    jpeg_create_decompress(&Info);
    SetMemorySource(&Info, SourceBuffer, srcBufLen);
    jpeg_read_header(&Info, TRUE);
    jpeg_start_decompress(&Info);

    if((Info.output_components != 3) && (Info.output_components != 4))
    {
        // Nr of channels must be 3 or 4!
        return FALSE;
    }
    
    if (0)
    {
        //>默认生成32位的RGBA数据
        uint32 DestLen = Info.output_width * Info.output_height * 4;
        int8* DecodeBuffer = new int8[DestLen];
        image->imageData = DecodeBuffer;
        image->width = Info.output_width;
        image->height = Info.output_height;
        image->bpp = 32;

        pTargetBuffer = DecodeBuffer;
        if(!pTargetBuffer) 
            return FALSE;

        Stride = Info.output_width * Info.output_components;
        Offset = yReversed ? (Info.output_height - 1) * Stride : 0;
        int32 moveStride = yReversed ? -Stride : Stride;

        Pointer = (*Info.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&Info), JPOOL_IMAGE, Stride, 1);
        while(Info.output_scanline < Info.output_height)
        {
            jpeg_read_scanlines(&Info, Pointer, 1);
            memcpy(&pTargetBuffer[Offset], Pointer[0], Stride);
            Offset += moveStride;
        }

        jpeg_finish_decompress(&Info);

        (*reinterpret_cast<uint8*>(&Opaque)) = 255;

        if(Info.output_components == 3)
        {
            AASSERT(Info.out_color_space == JCS_RGB);
            for(i = (Info.output_width * Info.output_height - 1); i >= 0; i--)
            {
                if (co == CO_RGBA)
                {//>使用倒序复制,所以不会导致数据覆盖
                    pTargetBuffer[(i * 4) + 3] = Opaque;
                    pTargetBuffer[(i * 4) + 2] = pTargetBuffer[(i * 3) + 2];
                    pTargetBuffer[(i * 4) + 1] = pTargetBuffer[(i * 3) + 1];
                    pTargetBuffer[(i * 4) + 0] = pTargetBuffer[(i * 3) + 0];
                }else
                {   // CO_ARGB
                    pTargetBuffer[(i * 4) + 3] = Opaque;
                    pTargetBuffer[(i * 4) + 2] = pTargetBuffer[(i * 3) + 0];
                    pTargetBuffer[(i * 4) + 1] = pTargetBuffer[(i * 3) + 1];
                    pTargetBuffer[(i * 4) + 0] = pTargetBuffer[(i * 3) + 2];
                }
            }
        }
    }
    else
    {
        //>生成24位的RGB数据
        uint32 DestLen = Info.output_width * Info.output_height * 3;
        int8* DecodeBuffer = new int8[DestLen];
        image->imageData = DecodeBuffer;
        image->width = Info.output_width;
        image->height = Info.output_height;
        image->bpp = 24;

        pTargetBuffer = DecodeBuffer;
        if(!pTargetBuffer) 
            return FALSE;

        Stride = Info.output_width * Info.output_components;
        Offset = yReversed ? (Info.output_height - 1) * Stride : 0;
        int32 moveStride = yReversed ? -Stride : Stride;

        Pointer = (*Info.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&Info), JPOOL_IMAGE, Stride, 1);
        while(Info.output_scanline < Info.output_height)
        {
            jpeg_read_scanlines(&Info, Pointer, 1);
            memcpy(&pTargetBuffer[Offset], Pointer[0], Stride);
            Offset += moveStride;
        }

        jpeg_finish_decompress(&Info);
    }
   
    jpeg_destroy_decompress(&Info);

    return TRUE;
}

void JPEG_Loader::Release( JPEG_Image* image )
{
    if ( image )
    {
        if ( image->imageData )
            delete[] image->imageData;
    }
}

// Sets the memory source
void JPEG_Loader::SetMemorySource(struct jpeg_decompress_struct* Info, const int8* Buffer, uint32 Size)
{
    JPEG_SOURCE_MANAGER* SourceManager;

    Info->src = reinterpret_cast<jpeg_source_mgr*>((*Info->mem->alloc_small)(reinterpret_cast<j_common_ptr>(Info), JPOOL_PERMANENT, sizeof(JPEG_SOURCE_MANAGER)));
    SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

    SourceManager->Buffer = reinterpret_cast<JOCTET*>((*Info->mem->alloc_small)(reinterpret_cast<j_common_ptr>(Info), JPOOL_PERMANENT, Size * sizeof(JOCTET)));
    SourceManager->SourceBuffer = Buffer;
    SourceManager->SourceBufferSize = Size;
    SourceManager->Manager.init_source = SourceInit;
    SourceManager->Manager.fill_input_buffer = reinterpret_cast<boolean(*)(j_decompress_ptr cinfo)>(SourceFill);
    SourceManager->Manager.skip_input_data = SourceSkip;
    SourceManager->Manager.resync_to_restart = jpeg_resync_to_restart;
    SourceManager->Manager.term_source = SourceTerminate;
    SourceManager->Manager.bytes_in_buffer = 0;
    SourceManager->Manager.next_input_byte = NULL;
}

// Initiates the memory source
void JPEG_Loader::SourceInit(struct jpeg_decompress_struct* Info)
{
    //Empty
}

// Fills the memory source
uint8 JPEG_Loader::SourceFill(struct jpeg_decompress_struct* Info)
{
    JPEG_SOURCE_MANAGER* SourceManager;

    SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

    SourceManager->Buffer = reinterpret_cast<const JOCTET*>(SourceManager->SourceBuffer);
    SourceManager->Manager.next_input_byte = SourceManager->Buffer;
    SourceManager->Manager.bytes_in_buffer = SourceManager->SourceBufferSize;

    return 1;
}

// Skips the memory source
void JPEG_Loader::SourceSkip(struct jpeg_decompress_struct* Info, long NrOfBytes)
{
    JPEG_SOURCE_MANAGER* SourceManager;

    SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

    if(NrOfBytes > 0)
    {
        while(NrOfBytes > static_cast<long>(SourceManager->Manager.bytes_in_buffer))
        {
            NrOfBytes -= static_cast<long>(SourceManager->Manager.bytes_in_buffer);
            SourceFill(Info);
        }

        SourceManager->Manager.next_input_byte += NrOfBytes;
        SourceManager->Manager.bytes_in_buffer -= NrOfBytes;
    }
}

// Terminates the memory source
void JPEG_Loader::SourceTerminate(struct jpeg_decompress_struct* Info)
{
    //Empty
}

#include "png.h"

typedef struct 
{
    const unsigned char * data;
    size_t size;
    int offset;
}tImageSource;

static void PngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);

	if((int)(isource->offset + length) <= isource->size)
	{
		memcpy(data, isource->data+isource->offset, length);
		isource->offset += length;
	}
	else
	{
		png_error(png_ptr, "PngReaderCallback failed");
	}
}

bool PNG_Loader::Load( PNG_Image* image, const char* filename, bool yReversed, ColorOrder co )
{

    atgReadFile reader;
    if (!reader.Open(filename))
    {
        return false;
    }
    uint32 srcBufLen = reader.GetLength();
    int8* SourceBuffer = new int8[srcBufLen];
    reader.Read(SourceBuffer, srcBufLen, 1);
    reader.Close();

    bool rs = LoadMemory(image, SourceBuffer, srcBufLen, yReversed, co);

    delete [] SourceBuffer;
    return rs;
}

bool PNG_Loader::LoadMemory(PNG_Image* image, const int8* buffer, uint32 bufferSize, bool yReversed, ColorOrder co)
{
    // length of bytes to check if it is a valid png file
#define PNGSIGSIZE  8
    bool ret = false;
    png_byte        header[PNGSIGSIZE]   = {0}; 
    png_structp     png_ptr     =   0;
    png_infop       info_ptr    = 0;
    uint32          dataLen     = bufferSize;
    const int8*     data        = buffer;

    do 
    {
        // png header len is 8 bytes
        if(dataLen < PNGSIGSIZE)
            return false;

        // check the data is png or not
        memcpy(header, data, PNGSIGSIZE);
        if(png_sig_cmp(header, 0, PNGSIGSIZE))
            return false;

        // init png_struct
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if(!png_ptr)
            return false;

        // init png_info
        info_ptr = png_create_info_struct(png_ptr);
        if(!info_ptr)
            return false;

        if(setjmp(png_jmpbuf(png_ptr)))
            return false;

        // set the read call back function
        tImageSource imageSource;
        imageSource.data    = (unsigned char*)data;
        imageSource.size    = dataLen;
        imageSource.offset  = 0;
        png_set_read_fn(png_ptr, &imageSource, PngReadCallback);

        // read png header info

        // read png file info
        png_read_info(png_ptr, info_ptr);

        image->width = png_get_image_width(png_ptr, info_ptr);
        image->height = png_get_image_height(png_ptr, info_ptr);
        png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

        //CCLOG("color type %u", color_type);

        // force palette images to be expanded to 24-bit RGB
        // it may include alpha channel
        if (color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_palette_to_rgb(png_ptr);
        }
        // low-bit-depth grayscale images are to be expanded to 8 bits
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        {
            bit_depth = 8;
            png_set_expand_gray_1_2_4_to_8(png_ptr);
        }
        // expand any tRNS chunk data into a full alpha channel
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        {
            png_set_tRNS_to_alpha(png_ptr);
        }  
        // reduce images with 16-bit samples to 8 bits
        if (bit_depth == 16)
        {
            png_set_strip_16(png_ptr);            
        } 

        // Expanded earlier for grayscale, now take care of palette and rgb
        if (bit_depth < 8)
        {
            png_set_packing(png_ptr);
        }
        // update info
        png_read_update_info(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);

        switch (color_type)
        {
        case PNG_COLOR_TYPE_GRAY:
            image->bpp = 8;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            image->bpp = 16;
            break;
        case PNG_COLOR_TYPE_RGB:
            image->bpp = 24;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            image->bpp = 32;
            break;
        default:
            break;
        }

        // read png data
        png_size_t rowbytes;
        png_bytep* row_pointers = (png_bytep*)malloc( sizeof(png_bytep) * image->height );

        rowbytes = png_get_rowbytes(png_ptr, info_ptr);

        png_size_t dataLen = rowbytes * image->height;
        image->imageData = static_cast<int8*>(malloc(dataLen * sizeof(int8)));
        if (!image->imageData)
        {
            if (row_pointers != NULL)
            {
                free(row_pointers);
            }
            break;
        }

        for (unsigned short i = 0; i < image->height; ++i)
        {
            row_pointers[i] = (png_bytep)image->imageData + i*rowbytes;
        }

        png_read_image(png_ptr, row_pointers);

        png_read_end(png_ptr, NULL);
        if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        {
            if (co == CO_ARGB)
            {
                int8* fourBytes = (int8*)image->imageData;
                for(int i = 0; i < image->width * image->height; i++)
                {
					int8* p = fourBytes + i * 4;
                    *(int32*)p = FVCC(p[3], p[0], p[1], p[2]);
                }
            }
        }

        if (row_pointers != NULL)
        {
            free(row_pointers);
        }

        ret = true;
    } while (0);

    if (png_ptr)
    {
        png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
    }
    return ret;
}

void PNG_Loader::Release( PNG_Image* image )
{
	if ( image )
	{
		if ( image->imageData )
			delete[] image->imageData;
	}
}

bool strToBool(const std::string &s)
{
    if (s=="0" || s=="false"){
        return false;
    }
    if (s=="1" || s=="true"){
        return true;
    }

    //LOG("%s", "Error converting string to bool, expected 0 or 1, found: " + s);
    AASSERT(0); 
    return false;
}

int strToInt(const  std::string &s)
{
    char *endChar;
    int intValue= strtol(s.c_str(), &endChar, 10);

    if(*endChar!='\0'){
        //LOG("%s", "Error converting from string to int, found: " + s);
        AASSERT(0);
    }

    return intValue;
}
float strToFloat(const  std::string &s)
{
    char *endChar;
    float floatValue= static_cast<float>(strtod(s.c_str(), &endChar));

    if(*endChar!='\0'){
        //LOG("%s", "Error converting from string to float, found: "+s);
        AASSERT(0);
    }

    return floatValue;
}

bool strToBool(const  std::string &s, bool *b)
{
    if (s=="0" || s=="false"){
        *b= false;
        return true;
    }
    if (s=="1" || s=="true"){
        *b= true;
        return true;
    }

    return false;
}

bool strToInt(const  std::string &s, int *i)
{
    char *endChar;
    *i= strtol(s.c_str(), &endChar, 10);

    if(*endChar!='\0'){
        return false;
    }
    return true;
}

bool strToFloat(const  std::string &s, float *f)
{
    char *endChar;
    *f= static_cast<float>(strtod(s.c_str(), &endChar));

    if(*endChar!='\0'){
        return false;
    }
    return true;
}

std::string boolToStr(bool b){
    if(b){
        return "1";
    }
    else{
        return "0";
    }
}

std::string intToStr(int i){
    char str[64];
    sprintf(str, "%d", i);
    return str; 
}

std::string intToHex(int i){
    char str[64];
    sprintf(str, "%x", i);
    return str;
}

std::string floatToStr(float f){
    char str[64];
    sprintf(str, "%.2f", f);
    return str; 
}

bool Properties::load( const std::string &path )
{
    this->path= path;

    atgReadFile reader;
    if(!reader.Open(path.c_str()))
    {
        LOG("can't open file=%s.\n", path.c_str());
        return false;
    }

    uint32 fileLength = reader.GetLength();
    std::string buffer;
    buffer.resize(fileLength);
    reader.Read((void*)buffer.c_str(), fileLength);
    std::istringstream fileStream(buffer);


    char lineBuffer[1024];
    std::string line, key, value;
    int pos;

    propertyMap.clear();
    while(!fileStream.eof()){

        fileStream.getline(lineBuffer, 1024);

        if(lineBuffer[0]==';'){
            continue;
        }

        // gracefully handle win32 \r\n line endings
        size_t len= strlen(lineBuffer);
        if(len > 0 && lineBuffer[len-1] == '\r'){
            lineBuffer[len-1]= 0;
        }

        line= lineBuffer;
        pos= line.find('=');

        if(pos == std::string::npos){
            continue;
        }

        key= line.substr(0, pos);
        value= line.substr(pos+1);
        propertyMap.insert(PropertyPair(key, value));
        propertyVector.push_back(PropertyPair(key, value));

    }
    return true;
}

bool Properties::save( const std::string &path ){
    return true;
}

bool Properties::getBool( const std::string &key ) const{
    return strToBool(getString(key));
}


int Properties::getInt(const std::string &key) const{
    return strToInt(getString(key));
}

int Properties::getInt(const std::string &key, int min, int max) const{
    int i= getInt(key);
    if(i<min || i>max){
        //LOG("%s", "Value out of range: " + key + ", min: " + intToStr(min) + ", max: " + intToStr(max));
        AASSERT(0);
    }
    return i;
}

float Properties::getFloat(const std::string &key) const{
    return strToFloat(getString(key));
}

float Properties::getFloat(const std::string &key, float min, float max) const{
    float f= getFloat(key);
    if(f<min || f>max){
        //LOG("%s", "Value out of range: " + key + ", min: " + floatToStr(min) + ", max: " + floatToStr(max));
        AASSERT(0);
    }
    return f;
}

const std::string& Properties::getString(const std::string &key) const{
    PropertyMap::const_iterator it;
    it= propertyMap.find(key);
    if(it==propertyMap.end()){
        //LOG("%s", "Value not found in propertyMap: " + key + ", loaded from: " + path);
        AASSERT(0);
        static std::string str("unknown");
        return str;
    }
    else{
        return it->second;
    }
}

void Properties::setInt(const std::string &key, int value){
    setString(key, intToStr(value));
}

void Properties::setBool(const std::string &key, bool value){
    setString(key, boolToStr(value));
}

void Properties::setFloat(const std::string &key, float value){
    setString(key, floatToStr(value));
}

void Properties::setString(const std::string &key, const std::string &value){
    propertyMap.erase(key);
    propertyMap.insert(PropertyPair(key, value));
}

Water::Water(int w, int h): Width(w), Height(h)
{
    size = w * h * sizeof(float);
    buf1 = (float*)malloc(size);
    buf2 = (float*)malloc(size);

    memset(buf1, 0, size);
    memset(buf2, 0, size);
}

Water::~Water(void)
{
    SAFE_DELETE_ARRAY(buf1);
    SAFE_DELETE_ARRAY(buf2);
}

void Water::Updata()
{
    for(int y = 0; y < Height; y++)
    {
        int n = y * Width;
        for (int x = 0; x < Width; x++, n++)
        {
            float s = GetValue(buf1, x, y - 1) + GetValue(buf1, x, y + 1) + GetValue(buf1, x - 1, y) + GetValue(buf1, x + 1, y);
            s = (s / 2 - buf2[n]);
            s -= s / 32;
            if (s > 2) s = 2;
            if (s < -2) s = -2;
            buf2[n] = s;
        }
    }
    float* temp = buf1;
    buf1 = buf2;
    buf2 = temp;
}

void Water::Drop(float xi, float yi)
{
    int px = (int)(xi * (Width - 1));
    int py = (int)(yi * (Height - 1));
    for (int j = py - r; j <= py + r; j++)
    {
        for (int i = px - r; i <= px + r; i++)
        {
            float dx = (float)i - px;
            float dy = (float)j - py;
            float a = (float)(1 - (dx * dx + dy * dy) / (r * r));
            if (a > 0 && a <= 1)
            {
                SetValue(buf1, i, j, a * h);
            }
        }
    }
}

float Water::GetValue(float* buf, int x, int y)
{
    x = atgMath::Clamp(x, 0, Width - 1);
    y = atgMath::Clamp(y, 0, Height - 1);

    return buf[y * Width + x];
}

void Water::SetValue(float* buf, int x, int y, float value)
{
    x = atgMath::Clamp(x, 0, Width - 1);
    y = atgMath::Clamp(y, 0, Height - 1);

    buf[y * Width + x] = value;
}

namespace MdxSizeOf_Ns
{
    const uint32 sizeOfuint8   = sizeof(uint8);
    const uint32 sizeOfDW     = sizeof(uint32);
    const uint32 sizeOfF1     = sizeof(float);
    const uint32 sizeOfF2     = sizeof(MdxFloat2);
    const uint32 sizeOfF3     = sizeof(MdxFloat3);
    const uint32 sizeOfF4     = sizeof(MdxFloat4);
    const uint32 sizeOfU      = sizeof(uint32);
    const uint32 sizeOfU3     = sizeof(uint32) * 3;
}
using namespace MdxSizeOf_Ns;

class atgReadFile* g_pMdxReader = NULL;

MdxModel::MdxModel()
{
    _version = 0;

    _numberSequences = 0;
    _numberGolbalSequences = 0;
    _numberMaterials = 0;
    _numberTextures = 0;
    _numberMesh = 0;
    _numberBones = 0;
    _numberHelps = 0;
    _numberNodes = 0;

    memset(_nodes, 0, sizeof(_nodes));
}

MdxModel::~MdxModel(void)
{
}


bool MdxModel::Load( const char* fullFileName )
{
    atgReadFile MdxReader;
    g_pMdxReader = &MdxReader;
    if(!MdxReader.Open(fullFileName))
    {
        LOG("can't open file=%s.\n", fullFileName);
        return false;
    }
    uint32 fileSize = g_pMdxReader->GetLength();
    uint32 date = g_pMdxReader->ReadDWord();
    if(FVCC('M','D','L','X') != TAG(date))
    {
        return false;
    }
    fileSize -= sizeOfDW;
    int sectionSize = 0; 
    do
    {
        if(!(fileSize > 0))
        {
            break;
        }
        sectionSize = 0; 
        date = g_pMdxReader->ReadDWord();
        fileSize -= sizeOfDW;
        switch(TAG(date))
        {
        case FVCC('V','E','R','S') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadVersionSection(sectionSize)) return false;
            break;}
        case FVCC('M','O','D','L') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadModelInfoSection(sectionSize)) return false;;
            break;}
        case FVCC('S','E','Q','S'):{
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadSecquencesSection(sectionSize)) return false;
            break;}
        case FVCC('G','L','B','S') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadGlobalSequencesSection(sectionSize)) return false;
            break;}
        case FVCC('M','T','L','S') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadMaterialSeciton(sectionSize)) return false;
            break;}
        case FVCC('T','E','X','S') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadTexturesSection(sectionSize)) return false;
            break;}
        case FVCC('G','E','O','S') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadGeosetSection(sectionSize)) return false;
            break;}
        case FVCC('G','E','O','A') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadGeosetAnimationSection(sectionSize)) return false;
            break; }
        case FVCC('B','O','N','E') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadBoneSection(sectionSize)) return false;
            break;} 
        case FVCC('L','I','T','E') : {
            sectionSize = g_pMdxReader->ReadDWord();
            g_pMdxReader->Seek(sectionSize);
            break;}
        case FVCC('H','E','L','P') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if (!loadHelpesSection(sectionSize)) return false;            
            break;}
        case FVCC('A','T','C','H') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadAttachmentSection(sectionSize)) return false;
            break;}
        case FVCC('P','R','E','M') : { // Particle emitters
            sectionSize = g_pMdxReader->ReadDWord();
            g_pMdxReader->Seek(sectionSize);
            break;}
        case FVCC('P','R','E','2') : { // Particle emitters 2
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadParticleEmitters2Section(sectionSize)) return false;
            break;}
        case FVCC('R','I','B','B') : { // Ribbon emitters
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadRibbonEmittersSection(sectionSize)) return false;
            break;}
        case FVCC('K','E','V','T') : { // Tracks
            sectionSize = g_pMdxReader->ReadDWord();
            g_pMdxReader->Seek(sectionSize);
            break;}
        case FVCC('E','V','T','S') : { // Event objects
            sectionSize = g_pMdxReader->ReadDWord();
            g_pMdxReader->Seek(sectionSize);
            break;}
        case FVCC('C','A','M','S') : { // Cameras
            sectionSize = g_pMdxReader->ReadDWord();
            g_pMdxReader->Seek(sectionSize);
            break;}
        case FVCC('P','I','V','T') : {
            sectionSize = g_pMdxReader->ReadDWord();
            if(!loadPivotPointSection(sectionSize)) return false;
            break;}
        case FVCC('C','L','I','D') : { // Collision shapes
            sectionSize = g_pMdxReader->ReadDWord();
            if (!loadCollisionShapeSection(sectionSize)) return false;
            break;}
        default:{
            return false;
                }
        }
        fileSize -= (sectionSize + sizeOfDW);
    } while (1);

    g_pMdxReader = NULL;
    LOG("load MdxModel(%s) Success!\n", fullFileName);
    return true;
}

int MdxModel::GetNumberOfMesh()
{
    return _numberMesh;
}

int MdxModel::GetNumberOfVertexs( uint32 meshIndex )
{
    if (meshIndex < _numberMesh)
    {
        return _meshs[meshIndex].numberVertexs;
    }
    return 0;
}

MdxFloat3* MdxModel::GetVertexs( uint32 meshIndex )
{
    if (meshIndex < _numberMesh)
    {
        return _meshs[meshIndex].vertexPositions;
    }
    return NULL;
}

int MdxModel::GetNumberOfFaces( uint32 meshIndex )
{
    if (meshIndex < _numberMesh)
    {
        return _meshs[meshIndex].numberFaces;
    }
    return 0;
}

MdxFace* MdxModel::GetFace( uint32 meshIndex )
{
    if (meshIndex < _numberMesh)
    {
        return _meshs[meshIndex].faces;
    }
    return NULL;
}

MdxFloat3* MdxModel::GetNormalsOfVertexs( uint32 meshIndex )
{
    if (meshIndex < _numberMesh)
    {
        return _meshs[meshIndex].vertexNormals;
    }
    return NULL;
}

MdxFloat2* MdxModel::GetTexturesOfVertexs( uint32 meshIndex )
{
    if (meshIndex < _numberMesh)
    {
        return _meshs[meshIndex].vertexTexturePositions;
    }
    return NULL;
}

MdxMaterial* MdxModel::GetMeshMaterial(uint32 meshIndex)
{
    if (meshIndex < _numberMesh)
    {
        uint32 materialId = _meshs[meshIndex].materialId;
        return &_materials[materialId];
    }
    return NULL;
}

MdxTexture* MdxModel::GetTexture(uint8 textureId)
{
    if (textureId < _numberTextures)
    {
        return &_textures[textureId];
    }
    return NULL;
}

uint8 MdxModel::GetNumberOfNodes()
{
    return _numberNodes;
}

MdxNode* MdxModel::GetNode(uint8 nodeId)
{
    if (nodeId < _numberNodes)
    {
        return _nodes[nodeId];
    }
    return NULL;
}

MdxFloat3* MdxModel::GetPivot(uint8 nodeId)
{
    if (nodeId < _numberPivots)
    {
        return &_pivots[nodeId];
    }
    return NULL;
}

const MdxMatrixList& MdxModel::GetMatrixList(int meshIndex, int vertexIndex)
{
    return _meshs[meshIndex].matrixLists[_meshs[meshIndex].groupIndices[vertexIndex]];
}

int MdxModel::GetNumberAnimation()
{
    return _numberSequences;
}

const MdxSequence& MdxModel::GetAnimation(int index)
{
    return _sequences[index];
}

bool MdxModel::loadVersionSection(int sectionSize)
{
    _version = g_pMdxReader->ReadDWord();
    if(800 != _version){
        _version = 0;
        return false;
    }
    return true;
}
bool MdxModel::loadModelInfoSection(int sectionSize)
{
    AASSERT(sectionSize == sizeof(MdxInfo));
    g_pMdxReader->Read(_info.name, MdxNameLength);
    g_pMdxReader->Read(_info.path, MAX_PATH);

    if(!loadBoundingBox(_info.boundingBox)) return false;
    _info.blendTime = g_pMdxReader->ReadDWord();
    return true;
}

bool MdxModel::loadSecquencesSection(int sectionSize)
{
    const int seqlen = sizeof(MdxSequence);
    int numSequences = sectionSize / seqlen;
    for(int n = 0; n < numSequences; n++)
    {
        AASSERT(_numberSequences < MdxSequenceMaxLength);
        int r = loadSecquence(_sequences[_numberSequences++]);
        if(r < 0)
        {
            return false;
        }
    }
    return true;
}

bool MdxModel::loadGlobalSequencesSection(int sectionSize)
{
    int number = sectionSize / sizeOfDW;
    for(int n = 0; n < number; n++)
    {
        AASSERT(_numberGolbalSequences < MdxSequenceMaxLength);
        _golbalSequences[_numberGolbalSequences++] = g_pMdxReader->ReadDWord();
    }
    return true;
}
bool MdxModel::loadMaterialSeciton(int sectionSize)
{
    int currentSize = 0;
    int numberMaterials = 0;
    while(sectionSize > currentSize)
    {
        AASSERT(numberMaterials < MdxDefaultContainerMaxLength);
        int materialSize = loadMaterial(_materials[numberMaterials++]);
        currentSize += materialSize;
    }
    return true;
}
bool MdxModel::loadTexturesSection(int sectionSize)
{
    const int texlen = sizeof(MdxTexture);
    int number = sectionSize / texlen;
    for(int n = 0; n < number; n++)
    {
        AASSERT(_numberTextures < MdxDefaultContainerMaxLength);
        MdxTexture& texture = _textures[_numberTextures++];
        texture.replaceableId = g_pMdxReader->ReadDWord();
        g_pMdxReader->Read(texture.name, MAX_PATH);
        texture.flags = g_pMdxReader->ReadDWord();
    }
    return true;
}
bool MdxModel::loadGeosetSection(int sectionSize)
{
    int currentSize = 0;
    while(sectionSize > currentSize)
    {
        AASSERT(_numberMesh < MdxDefaultContainerMaxLength);
        int geosetsSize = loadGeoset(_meshs[_numberMesh++]);
        currentSize += geosetsSize;
    }
    return true;
}

bool MdxModel::loadGeosetAnimationSection(int sectionSize)
{
    g_pMdxReader->Seek(sectionSize);
    return true;
}

bool MdxModel::loadBoneSection(int sectionSize)
{
    int currentSize = 0;
    while(sectionSize > currentSize)
    {
        AASSERT(_numberBones < MdxNodesMaxLength);
        int boneSize = loadNode(_bones[_numberBones].node);
        _bones[_numberBones].geosetId = g_pMdxReader->ReadDWord();
        _bones[_numberBones].geoAnimationId = g_pMdxReader->ReadDWord();
        _numberBones++;
        currentSize += (boneSize + 8);
    }
    return true;
}


bool MdxModel::loadHelpesSection( int sectionSize )
{
    int currentSize = 0;
    while(sectionSize > currentSize)
    {
        AASSERT(_numberHelps < MdxNodesMaxLength);
        int boneSize = loadNode(_helps[_numberHelps++].node);
        currentSize += boneSize;
    }
    return true;
}


bool MdxModel::loadAttachmentSection(int sectionSize)
{
    g_pMdxReader->Seek(sectionSize);
    return true;
}

bool MdxModel::loadPivotPointSection(int sectionSize)
{
    _numberPivots = sectionSize / sizeOfF3;
    g_pMdxReader->Read((char*)_pivots[0].c, sectionSize);
    return true;
}

bool MdxModel::loadParticleEmitters2Section(int sectionSize)
{
    int currentSize = 0;
    while(sectionSize > currentSize)
    {
        AASSERT(_numberPE2s < MdxNodesMaxLength);
        int PE2Size = loadParticleEmitters2(_PE2s[_numberPE2s++]);
        currentSize += PE2Size;
    }
    return true;
}

bool MdxModel::loadRibbonEmittersSection(int sectionSize)
{
    int currentSize = 0;
    while(sectionSize > currentSize)
    {
        AASSERT(_numberREs < MdxNodesMaxLength);
        int RESize = loadRibbonEmitters(_REs[_numberREs++]);
        currentSize += RESize;
    }
    return true;
}

bool MdxModel::loadCollisionShapeSection(int sectionSize)
{
    int currentSize = 0;
    while(sectionSize > currentSize)
    {
        AASSERT(_numberCSs < MdxNodesMaxLength);
        int CSSize = loadCollisionShape(_CSs[_numberCSs++]);
        currentSize += CSSize;
    }
    return true;
}

int MdxModel::loadBoundingBox( MdxBoundingBox& box )
{
    box.radius = g_pMdxReader->ReadFloat();
    g_pMdxReader->Read((char*)&box.min, sizeOfF3);
    g_pMdxReader->Read((char*)&box.max, sizeOfF3);
    return sizeOfF1 + sizeOfF3 + sizeOfF3;
}

int MdxModel::loadSecquence( MdxSequence& sequence )
{

    g_pMdxReader->Read(sequence.name, MdxNameLength);
    sequence.intervalStart = g_pMdxReader->ReadDWord();
    sequence.intervalEnd = g_pMdxReader->ReadDWord();
    sequence.moveSpeed = g_pMdxReader->ReadFloat();
    sequence.nonLooping = g_pMdxReader->ReadDWord();
    sequence.rarity = g_pMdxReader->ReadFloat();
    sequence.syncPoint = g_pMdxReader->ReadDWord();
    if(!loadBoundingBox(sequence.boundingBox)) return false;
    return true;
}

int MdxModel::loadMaterial( MdxMaterial& material )
{
    int size = g_pMdxReader->ReadDWord();

    material.priorityPlane = g_pMdxReader->ReadDWord();
    material.flags = g_pMdxReader->ReadDWord();

    unsigned int date = date = g_pMdxReader->ReadDWord();
    if(TAG(date) == FVCC('L','A','Y','S'))
    {
        int number = g_pMdxReader->ReadDWord();
        AASSERT(number <= MdxDefaultContainerMaxLength);
        material.numberLayers = number;
        for(int n = 0; n < number; n++)
        {
            MdxMaterialLayer& materialLayer = material.layers[n];
            int materialLayerSize = g_pMdxReader->ReadDWord();
            int currentSize = 0;
            materialLayer.filterMode = g_pMdxReader->ReadDWord();
            materialLayer.shadingFlags = g_pMdxReader->ReadDWord();

            materialLayer.textureId = g_pMdxReader->ReadDWord();
            materialLayer.textureAnimationId = g_pMdxReader->ReadDWord();
            materialLayer.coordId = g_pMdxReader->ReadDWord();
            materialLayer.alphaValue = g_pMdxReader->ReadFloat();

            currentSize = sizeOfU * 6 + sizeOfF1;
            while(materialLayerSize > currentSize)
            {
                unsigned int date = date = g_pMdxReader->ReadDWord();
                currentSize += sizeOfDW;
                if(TAG(date) == FVCC('K','M','T','A'))
                {
                    int numberOfTrack = g_pMdxReader->ReadDWord();
                    AASSERT(numberOfTrack <= MdxDefaultContainerMaxLength);
                    materialLayer.alpha.interpolationType = g_pMdxReader->ReadDWord();
                    materialLayer.alpha.globalSequenceId = g_pMdxReader->ReadDWord();
                    materialLayer.alpha.numberTracks = numberOfTrack;
                    for(int n = 0; n < numberOfTrack; n++)
                    {
                        MdxTrackDataF& alpha = materialLayer.alpha.alphaTracks[n];
                        alpha.time = g_pMdxReader->ReadDWord();
                        alpha.value = g_pMdxReader->ReadFloat();
                        if(materialLayer.alpha.interpolationType > 1)
                        {
                            alpha.inTran = g_pMdxReader->ReadFloat();
                            alpha.outTran = g_pMdxReader->ReadFloat();
                        }
                    }
                    currentSize += materialLayer.alpha.getSize();
                }
                else if(TAG(date) == FVCC('K','M','T','F'))
                {
                    int numberOfTrack = g_pMdxReader->ReadDWord();
                    AASSERT(numberOfTrack <= MdxDefaultContainerMaxLength);
                    materialLayer.texture.numberTracks = numberOfTrack;
                    materialLayer.texture.globalSequenceId = g_pMdxReader->ReadDWord();
                    materialLayer.texture.interpolationType = g_pMdxReader->ReadDWord();
                    for(int n = 0; n < numberOfTrack; n++)
                    {
                        MdxTrackDataDW& Texture = materialLayer.texture.textrueTarcks[n];
                        Texture.time = g_pMdxReader->ReadDWord();
                        Texture.value = g_pMdxReader->ReadDWord();
                        if(materialLayer.texture.interpolationType > 1)
                        {
                            Texture.inTran = g_pMdxReader->ReadDWord();
                            Texture.outTran = g_pMdxReader->ReadDWord();
                        }
                    }
                    currentSize += materialLayer.texture.getSize();
                }
                else
                {
                    AASSERT(0);
                    return -1;
                }
            }
        }
        return size;
    }
    return size;
}

int MdxModel::loadGeoset( MdxGeoset& geoset )
{
    uint32 n;
    int size = g_pMdxReader->ReadDWord();
    uint32 data = g_pMdxReader->ReadDWord();
    // read position
    if(FVCC('V','R','T','X') != TAG(data)) return false;
    uint32 numberOfVertex = g_pMdxReader->ReadDWord();
    geoset.numberVertexs = numberOfVertex;
    geoset.vertexPositions = new MdxFloat3[numberOfVertex];
    for(n = 0; n < numberOfVertex; n++)
    {
        g_pMdxReader->Read((char*)&geoset.vertexPositions[n], sizeOfF3);
    }
    // read normal
    data = g_pMdxReader->ReadDWord();
    if(FVCC('N','R','M','S') != TAG(data)) return false;
    uint32 numberOfNormal = g_pMdxReader->ReadDWord();
    geoset.numberNormals = numberOfVertex;
    geoset.vertexNormals = new MdxFloat3[numberOfNormal];
    for(n = 0; n < numberOfNormal; n++)
    {
        g_pMdxReader->Read((char*)&geoset.vertexNormals[n], sizeOfF3);
    }
    // read face type group
    data = g_pMdxReader->ReadDWord();
    if(FVCC('P','T','Y','P') != TAG(data)) return false;
    uint32 numberOfFaceTypeGroup = g_pMdxReader->ReadDWord();
    geoset.numberFaceTypeGroups = numberOfFaceTypeGroup;
    geoset.faceTypeGroups = new uint32[numberOfFaceTypeGroup];
    for(n = 0; n < numberOfFaceTypeGroup; n++)
    {
        uint32 faceTypeGroup = g_pMdxReader->ReadDWord(); 
        geoset.faceTypeGroups[n] = faceTypeGroup;
    }
    // read face group
    data = g_pMdxReader->ReadDWord();
    if(FVCC('P','C','N','T') != TAG(data)) return false;
    unsigned int numberOfFaceGroup = g_pMdxReader->ReadDWord();
    geoset.numberFaceGroups = numberOfFaceGroup;
    geoset.faceGroups = new uint32[numberOfFaceGroup];
    for(n = 0; n < numberOfFaceGroup; n++)
    {
        uint32 faceGroup = g_pMdxReader->ReadDWord(); 
        geoset.faceGroups[n] = faceGroup;
    }
    // read face
    data = g_pMdxReader->ReadDWord();
    if(FVCC('P','V','T','X') != TAG(data)) return false;
    unsigned int numberOfFaceIndex = g_pMdxReader->ReadDWord();
    unsigned int numberOfFace = numberOfFaceIndex / 3;
    geoset.numberFaces = numberOfFace;
    geoset.faces = new MdxFace[numberOfFace];
    for(n = 0; n < numberOfFace; n++)
    {
        g_pMdxReader->Read((char*)&geoset.faces[n], sizeof(MdxFace));
    }
    // group index
    data = g_pMdxReader->ReadDWord();
    if(FVCC('G','N','D','X') != TAG(data)) return false;
    unsigned int numberOfGroupIndices = g_pMdxReader->ReadDWord();
    geoset.numberGroupIndices = numberOfGroupIndices;
    geoset.groupIndices = new uint8[numberOfGroupIndices];
    for(n = 0; n < numberOfGroupIndices; n++)
    {
        uint8 vertexGroup = g_pMdxReader->ReadByte();
        geoset.groupIndices[n] = vertexGroup;
    }
    // matrix group
    data = g_pMdxReader->ReadDWord();
    if(FVCC('M','T','G','C') != TAG(data)) return false;
    unsigned int numberOfMatrixList = g_pMdxReader->ReadDWord();
    geoset.numberMatrixList = numberOfMatrixList;
    geoset.matrixLists = new MdxMatrixList[numberOfMatrixList];
    for(n = 0; n < numberOfMatrixList; n++)
    {
        geoset.matrixLists[n].numberOfNode = g_pMdxReader->ReadDWord();
    }
    data = g_pMdxReader->ReadDWord();
    if(FVCC('M','A','T','S') != TAG(data)) return false;
    unsigned int numberOfMatrixIndexes = g_pMdxReader->ReadDWord();
    geoset.numberMatrixIndices = numberOfMatrixIndexes;
    int curMatrixIndex = 0;
    for(n = 0; n < numberOfMatrixList; n++)
    {
        for (int mi = 0; mi < geoset.matrixLists[curMatrixIndex].numberOfNode; mi++)
        {
            uint32 matrixIndex = g_pMdxReader->ReadDWord();
            geoset.matrixLists[curMatrixIndex].nodeIndices[mi] = matrixIndex;
        }
        curMatrixIndex++;
    }
    geoset.materialId = g_pMdxReader->ReadDWord();
    geoset.selectionGroup = g_pMdxReader->ReadDWord();
    geoset.selectionFlags = g_pMdxReader->ReadDWord();
    // bounding box
    loadBoundingBox(geoset.boundingBox);
    // extents
    uint32 numberOfExtents = g_pMdxReader->ReadDWord();
    geoset.numberExtents = numberOfExtents;
    geoset.extents = new MdxBoundingBox[numberOfExtents];
    for(n = 0; n < numberOfExtents; n++)
    {
        MdxBoundingBox& extent = geoset.extents[n];
        loadBoundingBox(extent);
    }
    // texture group
    data = g_pMdxReader->ReadDWord();
    if(FVCC('U','V','A','S') != TAG(data)) return false;
    geoset.numberOfTextureVertexGroups = g_pMdxReader->ReadDWord();
    // texture coordinate
    data = g_pMdxReader->ReadDWord();
    if(FVCC('U','V','B','S') != TAG(data)) return false;
    uint32 numberOfVertexTexturePositions = g_pMdxReader->ReadDWord();
    geoset.numberVertexTexturePositions = numberOfVertexTexturePositions;
    geoset.vertexTexturePositions = new MdxFloat2[numberOfVertexTexturePositions];
    for(n = 0; n < numberOfVertexTexturePositions; n++)
    {
        MdxFloat2& texturePosition = geoset.vertexTexturePositions[n];
        g_pMdxReader->Read((char*)&texturePosition, sizeOfF2);
    }
    return size;
}

int MdxModel::loadNode( MdxNode& node )
{
    int size = g_pMdxReader->ReadDWord();
    int length = size;
    g_pMdxReader->Read(node.name, MdxNameLength);
    node.objectId = g_pMdxReader->ReadDWord();
    node.parentId = g_pMdxReader->ReadDWord();

    uint32 flags = g_pMdxReader->ReadDWord();

    node.helper                     = (flags == NODE_FLAG_HELPER)  ? 1 : 0;
    node.dontInheritTranslation     = (flags & NODE_FLAG_DONT_INHERIT_TRANSLATION) ? 1 : 0;
    node.dontInheritRotation        = (flags & NODE_FLAG_DONT_INHERIT_ROTATION) ? 1 : 0;
    node.dontInheritScaling         = (flags & NODE_FLAG_DONT_INHERIT_SCALING) ? 1 : 0;
    node.billboarded                = (flags & NODE_FLAG_BILLBOARDED) ? 1 : 0;
    node.billboardedLockX           = (flags & NODE_FLAG_BILLBOARDED_LOCK_X) ? 1 : 0;
    node.billboardedLockY           = (flags & NODE_FLAG_BILLBOARDED_LOCK_Y) ? 1 : 0;
    node.billboardedLockZ           = (flags & NODE_FLAG_BILLBOARDED_LOCK_Z) ? 1 : 0;
    node.cameraAnchored             = (flags & NODE_FLAG_CAMERA_ANCHORED) ? 1 : 0;
    node.bone                       = (flags & NODE_FLAG_BONE) ? 1 : 0;
    node.light                      = (flags & NODE_FLAG_LIGHT) ? 1 : 0;
    node.eventObject                = (flags & NODE_FLAG_EVENT_OBJECT) ? 1 : 0;
    node.attachment                 = (flags & NODE_FLAG_ATTACHMENT) ? 1 : 0;
    node.particleEmitter            = (flags & NODE_FLAG_PARTICLE_EMITTER) ? 1 : 0;
    node.collisionShape             = (flags & NODE_FLAG_COLLISION_SHAPE) ? 1 : 0;
    node.ribbonEmitter              = (flags & NODE_FLAG_RIBBON_EMITTER) ? 1 : 0;
    node.unshadedOrEmitterUsesMdl   = (flags & NODE_FLAG_UNSHADED) ? 1 : 0;
    node.sortPrimitivesFarZOrEmitterUsesTag = (flags & NODE_FLAG_SORT_PRIMITIVES_FAR_Z) ? 1 : 0;
    node.lineEmitter                = (flags & NODE_FLAG_LINE_EMITTER) ? 1 : 0;
    node.unfogged                   = (flags & NODE_FLAG_UNFOGGED) ? 1 : 0;
    node.modelSpace                 = (flags & NODE_FLAG_MODEL_SPACE) ? 1 : 0;
    node.xyQuad                     = (flags & NODE_FLAG_XY_QUAD) ? 1 : 0;

    //TRACE("%d--%d %s flag:%d\n", pNode->ObjectId, pNode->ParentId, pNode->name, pNode->Flags);
    TRANSLATE_LIST  *pTranslateList = &node.translateList;
    SCALING_LIST    *pScalingList   = &node.scalingList;
    ROTATION_LIST   *pRotationList  = &node.rotationList;

    length -= 96;
    while(length > 0)
    {
        uint32 data = g_pMdxReader->ReadDWord();
        if(FVCC('K','G','T','R') == TAG(data))
        {
            int numKeys = g_pMdxReader->ReadDWord();
            int type = g_pMdxReader->ReadDWord();

            pTranslateList->interpolationType = type;
            int glsd = g_pMdxReader->ReadDWord();
            AASSERT(glsd < 100);
            pTranslateList->globalSequenceId = glsd;
            for(int i = 0; i<numKeys; i++)
            {
                keyTranslate key;

                key.time = g_pMdxReader->ReadDWord();

                g_pMdxReader->Read((char*)key.vector.c, sizeOfF3);
                if(pTranslateList->interpolationType > 1)
                {
                    g_pMdxReader->Read((char*)key.inTran.c, sizeOfF3);
                    g_pMdxReader->Read((char*)key.outTran.c, sizeOfF3);
                }

                pTranslateList->keyList.push_back(key);
            }

            int factor = 0;
            if(pTranslateList->interpolationType > 1)
                factor = 3;
            else
                factor = 1;
            length -= pTranslateList->keyList.size() * (factor * 12 + 4)  + 16;
        }
        else if(FVCC('K','G','R','T') == TAG(data))
        {
            uint32 numKeys = g_pMdxReader->ReadDWord();
            uint32 type = g_pMdxReader->ReadDWord();

            pRotationList->interpolationType = type;
            int glsd = (int)g_pMdxReader->ReadDWord();
            AASSERT(glsd < 100);
            pRotationList->globalSequenceId = glsd;
            for(uint32 i = 0; i<numKeys; i++)
            {
                keyRotation key;

                key.time = g_pMdxReader->ReadDWord();
                g_pMdxReader->Read((char*)key.rotation.c, sizeOfF4);
                if(pRotationList->interpolationType > 1)
                {
                    g_pMdxReader->Read((char*)key.inTran.c, sizeOfF4);
                    g_pMdxReader->Read((char*)key.outTran.c, sizeOfF4);
                }

                pRotationList->keyList.push_back(key);
            }

            int factor = 0;
            if(pRotationList->interpolationType > 1)
                factor = 3;
            else
                factor = 1;
            length -= pRotationList->keyList.size() * (factor * sizeOfF4 + sizeOfDW)  + 4 * sizeOfDW;

        }
        else if(FVCC('K','G','S','C') == TAG(data))
        {
            uint32 numKeys =  g_pMdxReader->ReadDWord();
            uint32 type =  g_pMdxReader->ReadDWord();
            pScalingList->interpolationType = type;

            int glsd =  (int)g_pMdxReader->ReadDWord();;
            AASSERT(glsd < 100);
            pScalingList->globalSequenceId = glsd;
            for(uint32 i = 0; i<numKeys; i++)
            {
                keyScaling key;

                key.time =  g_pMdxReader->ReadDWord();;
                g_pMdxReader->Read((char*)key.vector.c, sizeOfF3);
                if(pScalingList->interpolationType > 1)
                {
                    g_pMdxReader->Read((char*)key.inTran.c, sizeOfF3);
                    g_pMdxReader->Read((char*)key.outTran.c, sizeOfF3);
                }

                pScalingList->keyList.push_back(key);
            }

            int factor = 0;
            if(pScalingList->interpolationType > 1)
                factor = 3;
            else
                factor = 1;
            length -= pScalingList->keyList.size() * (factor * sizeOfF3 + sizeOfDW)  + 4 * sizeOfDW;
        }
        else
        {
            AASSERT(FALSE);
            return -1;
        }
    }
    _numberNodes++;
    _nodes[node.objectId] = &node;
    return size;
}

int MdxModel::loadParticleEmitters2(MdxParticleEmitter2& PE2)
{
    int size = g_pMdxReader->ReadDWord();
    int currentSize = sizeOfDW;
    int nodeSize = loadNode(PE2.node);
    currentSize += nodeSize;

    PE2.speed       = g_pMdxReader->ReadFloat();
    PE2.variation   = g_pMdxReader->ReadFloat();
    PE2.latitude    = g_pMdxReader->ReadFloat();
    PE2.gravity     = g_pMdxReader->ReadFloat();
    PE2.lifespan    = g_pMdxReader->ReadFloat();
    PE2.emissionRate= g_pMdxReader->ReadFloat();
    PE2.length      = g_pMdxReader->ReadFloat();
    PE2.width       = g_pMdxReader->ReadFloat();

    PE2.filterMode  = g_pMdxReader->ReadDWord();
    if (0 > 4)
        return 0;

    PE2.rows        = g_pMdxReader->ReadDWord();
    PE2.columns     = g_pMdxReader->ReadDWord();
    PE2.headOrTail  = g_pMdxReader->ReadDWord();
    PE2.tailLength  = g_pMdxReader->ReadFloat();
    PE2.time        = g_pMdxReader->ReadFloat();
    currentSize += 10 * sizeOfF1 + 4 * sizeOfDW;


    int sizeOfB3 = sizeof(uint8) * 3;
    g_pMdxReader->Read((char*)PE2.segmentColors, 3 * sizeOfF3);
    g_pMdxReader->Read((char*)PE2.segmentAlpha, sizeOfB3);
    g_pMdxReader->Read((char*)PE2.segmentScaling.c, sizeOfF3);
    g_pMdxReader->Read((char*)PE2.headInterval, sizeOfU3);
    g_pMdxReader->Read((char*)PE2.headDecayInterval, sizeOfU3);
    g_pMdxReader->Read((char*)PE2.tailInterval, sizeOfU3);
    g_pMdxReader->Read((char*)PE2.tailDecayInterval, sizeOfU3);
    currentSize += 4 * sizeOfF3 + sizeOfB3 + 4 * sizeOfU3;

    PE2.textureId   = g_pMdxReader->ReadDWord();
    PE2.squirt      = g_pMdxReader->ReadDWord();
    PE2.priorityPlane = g_pMdxReader->ReadDWord();
    PE2.replaceableId = g_pMdxReader->ReadDWord();
    currentSize += 4 * sizeOfDW;

    while(size > currentSize)
    {
        uint32 data = g_pMdxReader->ReadDWord();
        MdxPE2TrackList* tracks = NULL;
        if (FVCC('K','P','2','S') == TAG(data))
        {
            tracks = &PE2.speedTracks;
        }else if (FVCC('K','P','2','R') == TAG(data))
        {
            tracks = &PE2.variationTracks;
        }else if (FVCC('K','P','2','L') == TAG(data))
        {
            tracks = &PE2.latitudeTracks;
        }else if (FVCC('K','P','2','G') == TAG(data))
        {
            tracks = &PE2.gravityTracks;
        }else if (FVCC('K','P','2','E') == TAG(data))
        {
            tracks = &PE2.emissionRateTracks;
        }else if (FVCC('K','P','2','N') == TAG(data))
        {
            tracks = &PE2.lengthTracks;
        }else if (FVCC('K','P','2','W') == TAG(data))
        {
            tracks = &PE2.widthTracks;
        }else if (FVCC('K','P','2','V') == TAG(data))
        {
            tracks = &PE2.visibilityTracks;
        }
        else
        {
            AASSERT(FALSE);
            return 0;
        }
        
        if (tracks)
        {
            int numKeys = g_pMdxReader->ReadDWord();
            tracks->interpolationType = g_pMdxReader->ReadDWord();

            int glsd = g_pMdxReader->ReadDWord();
            AASSERT(glsd < 100);
            tracks->globalSequenceId = glsd;
            for(int i = 0; i<numKeys; i++)
            {
                MdxTrackDataF key;

                key.time = g_pMdxReader->ReadDWord();
                key.value = g_pMdxReader->ReadFloat();
                if(tracks->interpolationType > 1)
                {
                    key.inTran = g_pMdxReader->ReadFloat();
                    key.outTran = g_pMdxReader->ReadFloat();
                }

                tracks->keyList.push_back(key);
            }

            int factor = 0;
            if(tracks->interpolationType > 1)
                factor = 3;
            else
                factor = 1;
            currentSize += tracks->keyList.size() * (factor * sizeOfF1 + sizeOfDW)  + 4 * sizeOfDW;
        }
    }
    return size;
}

int MdxModel::loadRibbonEmitters(MdxRibbonEmitter& RE)
{
    int size = g_pMdxReader->ReadDWord();
    int currentSize = sizeOfDW;
    int nodeSize = loadNode(RE.node);
    currentSize += nodeSize;

    RE.heightAbove = g_pMdxReader->ReadFloat();
    RE.heightBelow = g_pMdxReader->ReadFloat();
    RE.alpha       = g_pMdxReader->ReadFloat();
    g_pMdxReader->Read((char*)RE.color.c, sizeOfF3);
    RE.lifespan    = g_pMdxReader->ReadFloat();
    RE.textureSlot = g_pMdxReader->ReadDWord();
    RE.emissionRate= g_pMdxReader->ReadDWord();
    RE.rows        = g_pMdxReader->ReadDWord();
    RE.columns     = g_pMdxReader->ReadDWord();
    RE.materialId  = g_pMdxReader->ReadDWord();
    RE.gravity     = g_pMdxReader->ReadFloat();
    currentSize += 5 * sizeOfF1 + sizeOfF3 + 5 * sizeOfDW;
    
    while(size > currentSize)
    {
        MdxRE2TrackList* tracksF1 = NULL;
        MdxRE2TrackListF3* tracksF3 = NULL;
        MdxRE2TrackListDW* tracksDW = NULL;

        uint32 data = g_pMdxReader->ReadDWord();
        if (FVCC('K','R','H','A') == TAG(data))
        {
            tracksF1 = &RE.heightAboveTracks;
        }else if (FVCC('K','R','H','B') == TAG(data))
        {
            tracksF1 = &RE.heightBelowTracks;
        }else if (FVCC('K','R','A','L') == TAG(data))
        {
            tracksF1 = &RE.alphaTracks;
        }else if (FVCC('K','R','C','O') == TAG(data))
        {
            tracksF3 = &RE.colorTracks;
        }else if (FVCC('K','R','T','X') == TAG(data))
        {
            tracksDW = &RE.textureSlotTracks;
        }else if (FVCC('K','R','V','S') == TAG(data))
        {
            tracksF1 = &RE.visibilityTracks;
        }
        else
        {
            AASSERT(0);
            return 0;
        }

        if (tracksF1)
        {
            uint32 numKeys = g_pMdxReader->ReadDWord();
            tracksF1->interpolationType = g_pMdxReader->ReadDWord();

            int glsd = g_pMdxReader->ReadDWord();
            AASSERT(glsd < 100);
            tracksF1->globalSequenceId = glsd;
            for(uint32 i = 0; i<numKeys; i++)
            {
                MdxTrackDataF key;

                key.time = g_pMdxReader->ReadDWord();
                key.value = g_pMdxReader->ReadFloat();
                if(tracksF1->interpolationType > 1)
                {
                    key.inTran = g_pMdxReader->ReadFloat();
                    key.outTran = g_pMdxReader->ReadFloat();
                }

                tracksF1->keyList.push_back(key);
            }

            int factor = 0;
            if(tracksF1->interpolationType > 1)
                factor = 3;
            else
                factor = 1;
            currentSize += tracksF1->keyList.size() * (factor * sizeOfF1 + sizeOfDW)  + 4 * sizeOfDW;
        }else if (tracksF3)
        {
            int numKeys = g_pMdxReader->ReadDWord();
            tracksF3->interpolationType = g_pMdxReader->ReadDWord();

            int glsd = g_pMdxReader->ReadDWord();
            AASSERT(glsd < 100);
            tracksF3->globalSequenceId = glsd;
            for(int i = 0; i<numKeys; i++)
            {
                MdxTrackDataF3 key;

                key.time = g_pMdxReader->ReadDWord();
                g_pMdxReader->Read((char*)key.value.c, sizeOfF3);
                if(tracksF1->interpolationType > 1)
                {
                    g_pMdxReader->Read((char*)key.inTran.c, sizeOfF3);
                    g_pMdxReader->Read((char*)key.outTran.c, sizeOfF3);
                }
                tracksF3->keyList.push_back(key);
            }

            int factor = 0;
            if(tracksF1->interpolationType > 1)
                factor = 3;
            else
                factor = 1;
            currentSize += tracksF1->keyList.size() * (factor * sizeOfF3 + sizeOfDW)  + 4 * sizeOfDW;
        }else if (tracksDW)
        {
            int numKeys = g_pMdxReader->ReadDWord();
            tracksDW->interpolationType = g_pMdxReader->ReadDWord();

            int glsd = g_pMdxReader->ReadDWord();
            AASSERT(glsd < 100);
            tracksDW->globalSequenceId = glsd;
            for(int i = 0; i<numKeys; i++)
            {
                MdxTrackDataDW key;

                key.time = g_pMdxReader->ReadDWord();
                key.value = g_pMdxReader->ReadDWord();
                if(tracksF1->interpolationType > 1)
                {
                    key.inTran =  g_pMdxReader->ReadDWord();
                    key.outTran = g_pMdxReader->ReadDWord();
                }
                tracksDW->keyList.push_back(key);
            }

            int factor = 0;
            if(tracksF1->interpolationType > 1)
                factor = 3;
            else
                factor = 1;
            currentSize += tracksF1->keyList.size() * (factor * sizeOfDW + sizeOfDW)  + 4 * sizeOfDW;
        }
    }
    return size;

}

int MdxModel::loadCollisionShape(MdxCollisionShape& CS)
{
    int currentSize = 0;
    int nodeSize = loadNode(CS.node);
    currentSize += nodeSize + 4;
    
    CS.type = g_pMdxReader->ReadDWord();
    if (CS.type == 0)
    {
        CS.numberVertex = 2;
        g_pMdxReader->Read((char*)&CS.vertices[0].c, sizeOfF3);
        g_pMdxReader->Read((char*)&CS.vertices[1].c, sizeOfF3);
        currentSize += 2 * sizeOfF3;
    }else if (CS.type == 2)
    {
        CS.numberVertex = 1;
        g_pMdxReader->Read((char*)&CS.vertices[0].c, sizeOfF3);
        CS.boundsRadius = g_pMdxReader->ReadFloat();
        currentSize += sizeOfF3 + sizeOfF1;
    }
    return currentSize;
}





//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <sstream>
#include <set>

using namespace std;

SmdModel::SmdModel()
{
    texDirection = true;

    numVertices = 0;
    numTriangles = 0;
}

SmdModel::~SmdModel( void )
{

}

bool SmdModel::Load( const char* fullFileName )
{
    atgReadFile reader;
    if (!reader.Open(fullFileName))
    {
        cout<<"无法打开文件"<<fullFileName<<endl;
        return false;
    }
    uint32 srcBufLen = reader.GetLength();
    int8* SourceBuffer = new int8[srcBufLen];
    reader.Read(SourceBuffer, srcBufLen, 1);
    reader.Close();

    istringstream is(SourceBuffer);
    //检查文件头
    string token;
    int num;
    is>>token>>num;
    if (token!="version" || num!=1)
    {
        cout<<"文件头错误"<<endl;
        delete [] SourceBuffer;
        return false;
    }

    //第一次读入mesh文件需要创建初始动作
    SmdAnimation animation;
    animation.name = "init_pose";
    is>>token;
    while (!is.eof()) //读入文件的所有信息
    {
        if (token == "nodes")
        {
            ParseNodes(is);
        }
        else if (token == "skeleton")
        {
            ParseSkeleton(is, animation);
        }
        else if (token == "triangles")
        {
            ParseTriangles(is);
        }
        is>>token;
    }

    animations.push_back(animation);

    //统计顶点和三角形数
    numVertices = vertices.size();
    numTriangles = 0;
    for (unsigned int i=0; i<groups.size(); i++)
        numTriangles += groups[i].vertexIndices.size()/3;

    delete [] SourceBuffer;
    return true;
}

bool SmdModel::LoadAnimation( const char* fullFileName )
{
    if (hierarchy.empty())
    {
        cout<<"必须先读入模型文件"<<endl;
        return false;
    }

    atgReadFile reader;
    if (!reader.Open(fullFileName))
    {
        cout<<"无法打开文件"<<fullFileName<<endl;
        return false;
    }
    uint32 srcBufLen = reader.GetLength();
    int8* SourceBuffer = new int8[srcBufLen];
    reader.Read(SourceBuffer, srcBufLen, 1);
    reader.Close();

    istringstream is(SourceBuffer);

    //检查文件头
    string token;
    int num;
    is>>token>>num;
    if (token!="version" || num!=1)
    {
        cout<<"文件头错误"<<endl;
        delete [] SourceBuffer;
        return false;
    }

    SmdAnimation temp; //读入的动画信息保存在临时对象中
    is>>token;
    //读入文件的所有信息
    while (!is.eof())
    {
        if (token == "nodes")
        {
            ParseNodes(is);
        }
        else if (token == "skeleton")
        {
            ParseSkeleton(is, temp);
        }
        is>>token;
    }

    animations.push_back(SmdAnimation());
    SmdAnimation &animation = animations.back();
    //从文件名中提取动画名称
    animation.name = fullFileName;
    size_t found = animation.name.find_last_of("/\\"); //去除文件路径
    if (found != string::npos)
        animation.name = animation.name.substr(found + 1); //去除扩展名
    found = animation.name.find_last_of(".");
    if (found != string::npos)
        animation.name.erase(found);
    //按照模型文件的架构读入动画信息
    animation.frames.resize(temp.frames.size());
    for(unsigned int i=0; i<animation.frames.size(); i++)
    {
        animation.frames[i].time = temp.frames[i].time;
        animation.frames[i].jointkeys.resize(hierarchy.size());
        for (unsigned int j=0; j<hierarchy.size(); j++)
        {
            if (hierarchyMap[j] == -1)
                animation.frames[i].jointkeys[j] = animations[0].frames[0].jointkeys[j]; //如果没有动画信息就保持初始状态
            else
                animation.frames[i].jointkeys[j] = temp.frames[i].jointkeys[hierarchyMap[j]];
        }
    }

    delete [] SourceBuffer;
    return true;

}

//读入位于引号内的字符串
void ReadString(std::istringstream &is, string &str)
{
    char c = 0;

    while (c != '"')
        is>>c;

    is>>c;
    while (c != '"')
    {
        str.push_back(c);
        is>>c;
    }
}

//在字符串数组里寻找字符串
int FindName(vector<string> &names, string &name)
{
    for (unsigned int i=0; i<names.size(); i++)
    {
        if (name == names[i])
            return i;
    }

    return -1;
}

void SmdModel::ParseNodes( std::istringstream & is)
{
    string token;
    if (hierarchy.empty()) //第一次读入模型则创建骨骼架构
    {
        while (true)
        {
            SmdJointInfo info;
            is>>token;
            if (token == "end")
                break;

            ReadString(is, info.name);
            is>>info.parentID;

            hierarchy.push_back(info);
        }
    }
    else //建立动画骨架和原始模型骨架结构的对应关系
    {
        vector<string> names;
        hierarchyMap.clear();
        while (true)
        {
            is>>token;
            if (token == "end")
                break;

            string name;
            ReadString(is, name);
            names.push_back(name);

            int parentID;
            is>>parentID;
        }

        //对应关系保存在hierarchyMap中
        for (unsigned int i=0; i<hierarchy.size(); i++)
            hierarchyMap.push_back(FindName(names, hierarchy[i].name));
    }
}

void SmdModel::ParseSkeleton( std::istringstream & is, SmdAnimation & animation)
{
    string token;
    while (true)
    {
        is>>token;
        if (token == "end")
        {
            break;
        }
        else if (token == "time")
        {
            animation.frames.push_back(SmdFrame());
            is>>animation.frames.back().time;
            is>>token;
        }

        animation.frames.back().jointkeys.push_back(SmdKey());
        SmdKey &jointkey = animation.frames.back().jointkeys.back();

        int index = atoi(token.c_str());
        is>>jointkey.position[0]>>jointkey.position[1]>>jointkey.position[2];

        float rotation[3];
        is>>rotation[0]>>rotation[1]>>rotation[2];
        atgMath::QuatFromEulers(rotation, jointkey.rotation);
    }
}

//比较两个顶点是否相等
bool VertexCmp(SmdVertex &v1, SmdVertex &v2)
{
    if (abs(v1.position[0]-v2.position[0])<atgMath::EPSILON &&
        abs(v1.position[1]-v2.position[1])<atgMath::EPSILON &&
        abs(v1.position[2]-v2.position[2])<atgMath::EPSILON &&
        abs(v1.texcoord[0]-v2.texcoord[0])<atgMath::EPSILON &&
        abs(v1.texcoord[1]-v2.texcoord[1])<atgMath::EPSILON)
    {
        if (atgMath::VecDot(v1.normal, v2.normal) > 0.7f)
            return true;
    }

    return false;
}

void SmdModel::ParseTriangles( std::istringstream & is)
{
    unsigned int begin;
    set<int> normalSet;

    string token;
    is>>token;
    while (token != "end")
    {
        if (groups.empty() || groups.back().name!=token)
        {
            begin = vertices.size(); //搜索重复的顶点的起始位置位于每个group的开头

            groups.push_back(SmdGroup());
            groups.back().name = token;
        }

        for (int i=0; i<3; i++)
        {
            int jointID;
            SmdVertex vertex;
            is>>jointID>>vertex.position[0]>>vertex.position[1]>>vertex.position[2];
            is>>vertex.normal[0]>>vertex.normal[1]>>vertex.normal[2];
            is>>vertex.texcoord[0]>>vertex.texcoord[1]>>vertex.numJoints;
            //mdl反编译的SMD需要修改UV坐标
            if (1)
                vertex.texcoord[1] = -vertex.texcoord[1];

            //分配权重的存储空间
            vertex.jointIDs = new unsigned int[vertex.numJoints];
            vertex.weights = new float[vertex.numJoints];
            //读入权重
            for (unsigned int j=0; j<vertex.numJoints; j++)
                is>>vertex.jointIDs[j]>>vertex.weights[j];

            unsigned int index;
            int flag = -1;
            for (index=begin; index<vertices.size(); index++) //遍历现有顶点数组
            {
                if (VertexCmp(vertices[index], vertex)) //如果读入的顶点已经存在则记录该顶点index
                {
                    flag = index;
                    normalSet.insert(index); //记录需要重新计算法线的顶点
                    atgMath::VecAdd(vertices[index].normal, vertex.normal, vertices[index].normal); //法线相加
                    break;
                }
            }

            groups.back().vertexIndices.push_back(index); //记录新顶点的index

            if (flag == -1)
                vertices.push_back(vertex); //如果读入的顶点不在已有顶点列表中则保存该顶点
        }

        is>>token;
    }

    //去掉名字中的扩展名
    for (unsigned int i=0; i<groups.size(); i++)
    {
        size_t found = groups[i].name.find_last_of(".");
        if (found != string::npos)
            groups[i].name.erase(found);
    }

    //归一化法线
    set<int>::iterator it;
    for (it=normalSet.begin(); it!=normalSet.end(); it++)
        atgMath::VecNormalize(vertices[*it].normal);
}

