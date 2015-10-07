#pragma once

#include "atgUIBase.h"

class FontFreeType : public ELGUI::Font
{
public:
    FontFreeType(const std::string& fontFile, int size);
    virtual ~FontFreeType(void);

    virtual void load ();

    //virtual const ELGUI::FontGlyph* getGlyphData(wchar_t codepoint);
    //virtual void rasterize(wchar_t start_codepoint, wchar_t end_codepoint);
    //virtual void rasterize(wchar_t codepoint);


    FT_Library GetLibrary() const { return library_; }

protected:
    std::string     _fontFile;
    int             _fontSize;

    /// FreeType library.
    FT_Library library_;
};

