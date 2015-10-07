#include <ft2build.h>
#include <freetype/freetype.h>  
#include "atgBase.h"
#include "FontFreeType.h"


//#include <freetype/ftglyph.h>

static const int FONT_DPI = 96;

FontFreeType::FontFreeType(const std::string& name, int size)
{
    _fontFile = name;
    _fontSize = size;
}

FontFreeType::~FontFreeType(void)
{
    FT_Done_FreeType(library_);
}

void FontFreeType::load()
{
    /*
    FT_Error error = FT_Init_FreeType(&library_);
    if (error){
        LOG("Could not initialize FreeType library");
    }

    //wchar_t pText[]=L"第三代智能英特尔酷睿处理器。";

    FT_Face face;
    FT_Error error;
    FT_Glyph glyph;
    FT_Library library = GetLibrary();

    atgReadFile reader;
    if(!reader.Open(_fontFile.c_str()))
    {
        LOG("Could not open font file[%s].\n", _fontFile.c_str());
        return;
    }
    int fileSize = reader.GetLength();
    char* buffer = new char[fileSize];
    reader.Read(buffer, fileSize);

    error = FT_New_Memory_Face(library, (FT_Byte*)buffer, fileSize, 0, &face);
    if (error)
    {
        LOG("Could not create font face");
        return;
    }
    error = FT_Set_Char_Size(face, _fontSize << 6, _fontSize << 6, FONT_DPI, FONT_DPI);
    if (error)
    {
        FT_Done_Face(face);
        LOG("Could not set font point size =%s", _fontSize);
        return;
    }
    delete [] buffer;

    */

    /*
    for (size_t n = 0 ; n< wcslen(pText); n++)
    {

        FT_Load_Glyph(face, FT_Get_Char_Index(face,  pText[n]), FT_LOAD_DEFAULT);
        error  =  FT_Get_Glyph(face->glyph,  &glyph);
        if (error)
        {
            FT_Done_Face(face);
            LOG("Could not get glyph.");
            return 0;
        }

        //  convert glyph to bitmap with 256 gray  
        FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal,  0 ,  1);  
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;  
        FT_Bitmap& bitmap  =  bitmap_glyph -> bitmap;
        std::string map;
        for ( int  i = 0 ; i < bitmap.rows;  ++ i)  
        {  
            for ( int  j = 0 ; j < bitmap.width;  ++ j)  
            {  
                //  if it has gray>0 we set show it as 1, o otherwise
                char ch = bitmap.buffer[i * bitmap.width + j] > 0 ? 'O' : ' ';
                map.append(&ch, 1);
            }  
            map.append( "\n" ); 
        }
        LOG("%s",map.c_str());

        //  free glyph  
        FT_Done_Glyph(glyph);  
        glyph  =  NULL;
    }
    */
}
