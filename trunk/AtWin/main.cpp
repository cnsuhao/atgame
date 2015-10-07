#include "atgBase.h"
#include "atgMisc.h"
#include "Game.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include <wchar.h>

/// FreeType library subsystem.
class FreeTypeLibrary
{
public:
    /// Construct.
    FreeTypeLibrary()
    {
        FT_Error error = FT_Init_FreeType(&library_);
        if (error){
            LOG("Could not initialize FreeType library");
        }
    }

    /// Destruct.
    virtual ~FreeTypeLibrary()
    {
        FT_Done_FreeType(library_);
    }

      FT_Library GetLibrary() const { return library_; }

private:
    /// FreeType library.
    FT_Library library_;
};

static const int FONT_DPI = 96;
const char fontFile[] = "msyhbd.ttf";

void Test1()
{
    Vec3 v1(25.0f, 50.0f, 100.0f);

    Matrix m1;
    m1.RotationY(30);
    
    Vec3 v2 = m1.Transfrom(v1);
    m1.AffineInverse();
    v2 = m1.Transfrom(v2);

    AASSERT(v1 == v2);
}

void Test2()
{   
    Vec4 v1(25.0f, 50.0f, 100.0f, 1.0f);

    Matrix m1;
    m1.Translation(Vec3(50, -25, 12.5));
    
    Matrix m2;
    m2.RotationY(30);
    m2.Concatenate(m1);

   Vec4 v2 = m2.Transfrom(v1);
   m2.AffineInverse();
   v2 = m2.Transfrom(v2);

   AASSERT(v1 == v2);

   m1.RotationX(30.0f);
   m2.Translation(Vec3(5,6,7));
   m2.Concatenate(m1);

   v2 = m2.Transfrom(v1);
   m2.AffineInverse();
   v2 = m2.Transfrom(v2);

   AASSERT(v1 == v2);
}

void TestPlane()
{
    Plane back(Vec3(0,0,1.0f), Vec3(0,0,-1.0f));
    Plane top(Vec3(0,-1.0f,0), Vec3(0,1.0f,0.0f));
    Plane left(Vec3(0.5f,0,-0.5f), Vec3(-1.0f,0.0f,0.0f));
    Vec4 pt;
    if(Plane::Intersection(back, top, left, pt))
    {
        LOG("intersection point(%f,%f,%f,%f)", pt.x, pt.y, pt.z, pt.w);
    }
    bool r;
    r = back.IsFront(Vec3Zero);
    r = back.IsFront(Vec3(10000, 1000000, 0));
    r = back.IsFront(Vec3(10000, 1000000, -2.0f));
    r = back.IsBack(Vec3(10000, 1000000, -2.0f));
    r = back.OnPlane(Vec3(-10000, 1000000, -1.0f));
    r = left.IsFront(Vec3(100.0f,0.0f, 99.0f));

    Plane p1;
    p1.FromPoints(Vec3(0,1.0f,-1.0f),Vec3(1.0f,0,-1.0f), Vec3(0,0,-1.0f));

    Plane p2;
    p2.FromVectors(Vec3(1.0f,0.0f,0), Vec3(0,1.0f,0), Vec3(0,0,-1.0f));
}

bool MBToUnicode(std::vector<wchar_t>& pun, const char* pmb, int32 mLen)  
{  
    // convert an MBCS string to widechar   
    int32 uLen = MultiByteToWideChar(CP_ACP, 0, pmb, mLen, NULL, 0);
    if (uLen<=0)  
    {
        return false;
    }
    pun.resize(uLen);
         
    int32 nRtn = MultiByteToWideChar(CP_ACP, 0, pmb, mLen, &*pun.begin(), uLen);
        
    if(nRtn != uLen)
    {  
        pun.clear();
        return false;
    }  
    return true;  
}

//Unicode编码转为多字节编码
bool UnicodeToMB(std::vector<char>& pmb, const wchar_t* pun, int32 uLen)
{
    // convert an widechar string to Multibyte 
    int32 MBLen = WideCharToMultiByte(CP_ACP, 0, pun, uLen, NULL, 0, NULL, NULL);
    if (MBLen <=0)
    {
        return false;
    }
    pmb.resize(MBLen);
    int nRtn = WideCharToMultiByte(CP_ACP, 0, pun, uLen, &*pmb.begin(), MBLen, NULL, NULL);

    if(nRtn != MBLen)
    {
        pmb.clear();
        return false;
    }
    return true;
}

// UTF8编码转为Unicode
bool UTF8ToUnicode(std::vector<wchar_t>& pun, const char* pu8, int32 utf8Len)
{
    // convert an UTF8 string to widechar 
    int32 nLen = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, NULL, 0);
    if (nLen <=0)
    {
        return false;
    }
    pun.resize(nLen);
    int32 nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, &*pun.begin(), nLen);

    if(nRtn != nLen)
    {
        pun.clear();
        return false;
    }

    return true;
}

// Unicode编码转为UTF8
bool UnicodeToUTF8(std::vector<char>& pu8, const wchar_t* pun, int32 uLen)
{
    // convert an widechar string to utf8
    int32 utf8Len = WideCharToMultiByte(CP_UTF8, 0, pun, uLen, NULL, 0, NULL, NULL);
    if (utf8Len<=0)
    {
        return false;
    }
    pu8.resize(utf8Len);
    int32 nRtn = WideCharToMultiByte(CP_UTF8, 0, pun, uLen, &*pu8.begin(), utf8Len, NULL, NULL);

    if (nRtn != utf8Len)
    {
        pu8.clear();
        return false;
    }
    return true;
}

// 多字节编码转为UTF8编码
bool MBToUTF8(std::vector<char>& pu8, const char* pmb, int32 mLen)
{
    // convert an MBCS string to widechar 
    int32 nLen = MultiByteToWideChar(CP_ACP, 0, pmb, mLen, NULL, 0);

    WCHAR* lpszW = NULL;
    try
    {
        lpszW = new WCHAR[nLen];
    }
    catch(std::bad_alloc &memExp)
    {
        return false;
    }

    int32 nRtn = MultiByteToWideChar(CP_ACP, 0, pmb, mLen, lpszW, nLen);

    if(nRtn != nLen)
    {
        delete[] lpszW;
        return false;
    }
    // convert an widechar string to utf8
    int32 utf8Len = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, NULL, 0, NULL, NULL);
    if (utf8Len <= 0)
    {
        return false;
    }
    pu8.resize(utf8Len);
    nRtn = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, &*pu8.begin(), utf8Len, NULL, NULL);
    delete[] lpszW;

    if (nRtn != utf8Len)
    {
        pu8.clear();
        return false;
    }
    return true;
}

// UTF8编码转为多字节编码
bool UTF8ToMB(std::vector<char>& pmb, const char* pu8, int32 utf8Len)
{
    // convert an UTF8 string to widechar 
    int32 nLen = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, NULL, 0);

    WCHAR* lpszW = NULL;
    try
    {
        lpszW = new WCHAR[nLen];
    }
    catch(std::bad_alloc &memExp)
    {
        return false;
    }

    int32 nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, lpszW, nLen);

    if(nRtn != nLen)
    {
        delete[] lpszW;
        return false;
    }

    // convert an widechar string to Multibyte 
    int32 MBLen = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);
    if (MBLen <=0)
    {
        return false;
    }
    pmb.resize(MBLen);
    nRtn = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, &*pmb.begin(), MBLen, NULL, NULL);
    delete[] lpszW;

    if(nRtn != MBLen)
    {
        pmb.clear();
        return false;
    }
    return true;
}

int main(int argc,char ** argv)
{

    //char pText[]="第_三代智2能英特,尔酷睿处理器。";
    //std::vector<wchar_t> text;
    //MBToUnicode(text, pText, strlen(pText));  

    //int fontSize = 14;
    //FreeTypeLibrary freeType;

    //FT_Face face;
    //FT_Error error;
    //FT_Glyph glyph;
    //FT_Library library = freeType.GetLibrary();

    //atgReadFile reader;
    //if(!reader.Open(fontFile))
    //{
    //    return 0;
    //}
    //int fileSize = reader.GetLength();
    //char* buffer = new char[fileSize];
    //reader.Read(buffer, fileSize);

    //error = FT_New_Memory_Face(library, (FT_Byte*)buffer, fileSize, 0, &face);
    //if (error)
    //{
    //    LOG("Could not create font face");
    //    return 0;
    //}
    //error = FT_Select_Charmap(face,FT_ENCODING_UNICODE);
    //if (error)
    //{
    //    LOG("Could not select font charmap.");
    //    return 0;
    //}

    //error = FT_Set_Char_Size(face, fontSize << 6, fontSize << 6, FONT_DPI, FONT_DPI);
    ////error = FT_Set_Pixel_Sizes(face, 0, fontSize);
    //if (error)
    //{
    //    FT_Done_Face(face);
    //    LOG("Could not set font point size =%s", fontSize);
    //    return 0;
    //}

    //for (size_t n = 0 ; n< text.size(); n++)
    //{

    //    FT_Load_Glyph(face, FT_Get_Char_Index(face,  text[n]), FT_LOAD_DEFAULT);
    //    error  =  FT_Get_Glyph(face->glyph,  &glyph);
    //    if (error)
    //    {
    //        FT_Done_Face(face);
    //        LOG("Could not get glyph.");
    //        return 0;
    //    }

    //    //if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
    //    //{
    //    //    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
    //    //    if (error)
    //    //    {
    //    //        LOG("Could not render glyph.");
    //    //        return 0;
    //    //    }
    //    //}

    //    ////  convert glyph to bitmap with 256 gray
    //    FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal,  0,  1);  
    //    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;  
    //    FT_Bitmap& bitmap  =  bitmap_glyph->bitmap;
    //    std::string map;
    //    for ( int  i = 0 ; i < bitmap.rows;  ++ i)  
    //    {  
    //        for ( int  j = 0 ; j < bitmap.width;  ++ j)  
    //        {  
    //            //  if it has gray>0 we set show it as 1, o otherwise
    //            char ch = bitmap.buffer[i * bitmap.width + j] > 0 ? 'O' : ' ';
    //            map.append(&ch, 1);
    //        }  
    //        map.append( "\n" ); 
    //    }
    //    LOG("%s",map.c_str());
    //    //  free glyph  
    //    FT_Done_Glyph(glyph);  
    //    glyph  =  NULL;
    //}
    //delete [] buffer;
    //return 0;

	//Vec3 v1,v2,v3;
	//v1.x = 1.0f;
 //   v1.y = 1.0f;
 //   v1.z = 0.0f;

 //   v2.x = 0.0f;
 //   v2.y = 0.0f;
 //   v2.z = 1.0f;

 //   float r = VecDot(v1.m, v2.m);
 //   printf("VecDot=%f.\n", r);

 //   Quat q1;
 //   QuatFromAxisAngle(q1.m, v2.m, MATH_PI_4);
 //   Matrix mat;
 //   QuatToMat(mat.m, q1.m);
 //   VecRotate(v3.m, v1.m, mat.m);

 //   Vec3 v4;
 //   VecRotate(v4.m, v1.m, q1.m);

	//float form = 1.0f;
	//float to = 0.0f;
	//for (float i = form ; i >= to; i -= 0.01f)
	//{
	//	float v = LinearMapping(form, to, i, -1, 1);
	//	printf("map vaule=[%f]\n", v);
	//}

    Test1();
    Test2();
    TestPlane();

    Game game;
    if(game.Initialize(NULL))
    {
        game.Run();
        game.Shutdown();
    }

	return EXIT_SUCCESS;
}