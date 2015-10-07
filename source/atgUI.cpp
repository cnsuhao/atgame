#include "atgBase.h"
#include "atgUIBase.h"
#include "atgUI.h"
#include "atgMaterial.h"
#include "atgMisc.h"
#include "atgProfile.h"

#include <algorithm>
#include <sstream>

namespace ELGUI
{
    Event::Event(const std::string& name)
        : d_name(name)
    {

    }

    Event::~Event()
    {
        SlotContainer::iterator iter(d_slots.begin());
        SlotContainer::iterator end_iter(d_slots.end());

        for (; iter != end_iter; ++iter)
        {
            delete *iter;
        }
        d_slots.clear();
    }

    bool Event::subscribe(SlotFunctorBase* slot)
    {
        d_slots.push_back(slot);
        return true;
    }

    const std::string& Event::getName() const
    {
        return d_name;
    }

    void Event::operator()(EventArgs& args)
    {
        SlotContainer::iterator iter(d_slots.begin());
        SlotContainer::iterator end_iter(d_slots.end());

        // execute all subscribers, updating the 'handled' state as we go
        for (; iter != end_iter; ++iter)
        {
            args.handled |= (*(*iter))(args);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    EventSet::EventSet()
        : d_muted(false)
    {

    }

    EventSet::~EventSet()
    {
        removeAllEvents();
    }

    void EventSet::addEvent(const std::string& name)
    {
        if (isEventPresent(name))
        {
            assert(0 && std::string("An event named '" + name + "' already exists in the EventSet.").c_str());
        }

        d_events[name.c_str()] = new Event(name);
    }

    void EventSet::removeEvent(const std::string& name)
    {
        EventMap::iterator pos = d_events.find(name.c_str());

        if (pos != d_events.end())
        {
            delete pos->second;
            d_events.erase(pos);
        }
    }

    void EventSet::removeAllEvents()
    {
        EventMap::iterator pos = d_events.begin();
        EventMap::iterator end = d_events.end() ;

        for (; pos != end; ++pos)
        {
            delete pos->second;
        }

        d_events.clear();
    }

    bool EventSet::isEventPresent(const std::string& name)
    {
        return (d_events.find(name.c_str()) != d_events.end());
    }

    bool EventSet::subscribeEvent(const std::string& name, SlotFunctorBase* slot)
    {
        Event* event = getEventObject(name, true);
        if (event)
            return event->subscribe(slot);
        else
            return false;
    }

    void EventSet::fireEvent(const std::string& name, EventArgs& args)
    {
        fireEvent_impl(name, args);
    }

    bool EventSet::isMuted() const
    {
        return d_muted;
    }

    void EventSet::setMutedState(bool setting)
    {
        d_muted = setting;
    }

    Event* EventSet::getEventObject(const std::string& name, bool autoAdd /* = false */)
    {
        EventMap::iterator pos = d_events.find(name.c_str());

        // if event did not exist, add it and then find it.
        if (pos == d_events.end())
        {
            if (autoAdd)
            {
                addEvent(name);
                return d_events.find(name.c_str())->second;
            }
            else
            {
                return 0;
            }
        }

        return pos->second;
    }

    void EventSet::fireEvent_impl(const std::string& name, EventArgs& args)
    {
        // find event object
        Event* ev = getEventObject(name);

        // fire the event if present and set is not muted
        if ((ev != 0) && !d_muted)
            (*ev)(args);
    }

    Image::Image(const Imageset* owner, const std::string& name, const Rect& area, const Point& render_offset, float horzScaling /* = 1.0f */, float vertScaling /* = 1.0f */)
        : d_owner(owner)
        , d_area(area)
        , d_offset(render_offset)
        , d_name(name)
    {
        assert(d_owner && "Image::Image - Imageset pointer passed to Image constructor must be valid.");

        // setup initial image scaling
        setHorzScaling(horzScaling);
        setVertScaling(vertScaling);
    }

    Image::Image(const Image& image)
        : d_owner(image.d_owner)
        , d_area(image.d_area)
        , d_offset(image.d_offset)
        , d_scaledWidth(image.d_scaledWidth)
        , d_scaledHeight(image.d_scaledHeight)
        , d_scaledOffset(image.d_scaledOffset)
        , d_name(image.d_name)
    {
    }

    Image::~Image()
    {
    }

    void Image::setArea(Rect area)
    {
        d_area = area;
    }

    void Image::setOffset(Point render_offset)
    {
        d_offset = render_offset;
    }

    void Image::setHorzScaling(float factor)
    {
        d_scaledWidth       = PixelAligned(d_area.getWidth() * factor);
        d_scaledOffset.d_x  = PixelAligned(d_offset.d_x * factor);
    }

    void Image::setVertScaling(float factor)
    {
        d_scaledHeight      = PixelAligned(d_area.getHeight() * factor);
        d_scaledOffset.d_y  = PixelAligned(d_offset.d_y * factor);
    }

    void Image::draw(const Rect& dest_rect, const Rect& clip_rect, const ColourRect& colours) const
    {
        Rect dest(dest_rect);

        // apply rendering offset to the destination Rect
        dest.offset(d_scaledOffset);

        // draw
        d_owner->draw(d_area, dest, clip_rect, colours);
    }

    const std::string& Image::getName() const
    {
        return d_name;
    }

    const std::string& Image::getImagesetName() const
    {
        return d_owner->getName();
    }

    const Rect& Image::getSourceTextureArea() const
    {
        return d_area;
    }

    //////////////////////////////////////////////////////////////////////////
    const FontGlyph* Font::getGlyphData(wchar_t codepoint)
    {
        rasterize(codepoint, codepoint);

        CodepointMap::const_iterator pos = d_cp_map.find(codepoint);
        return (pos != d_cp_map.end()) ? &pos->second : 0;
    }

    void Font::rasterize(wchar_t start_codepoint, wchar_t end_codepoint)
    {
        for (wchar_t c = start_codepoint; c <= end_codepoint; ++c)
        {
            rasterize(c);
        }
    }

    void Font::rasterize(wchar_t codepoint)
    {

    }

    float Font::getLineSpacing(float y_scale /* = 1.0f */) const
    {
        return d_height * y_scale;
    }

    size_t Font::getCharAtPixel(const std::wstring& text, float pixel, float x_scale /* = 1.0f */)
    {
        return getCharAtPixel(text, 0, pixel, x_scale);
    }

    size_t Font::getCharAtPixel(const std::wstring& text, size_t start_char, float pixel, float x_scale /* = 1.0f */)
    {
        const FontGlyph* glyph;
        float cur_extent = 0;
        size_t char_count = text.length();

        // handle simple cases
        if ((pixel <= 0) || (char_count <= start_char))
            return start_char;

        for (size_t c = start_char; c < char_count; ++c)
        {
            glyph = getGlyphData(text[c]);

            if (glyph)
            {
                cur_extent += glyph->getAdvance(x_scale);

                if (pixel < cur_extent)
                    return c;
            }
        }

        return char_count;
    }

    float Font::getWrappedTextExtent(const std::wstring& text, float wrapWidth, float x_scale /* = 1.0f */)
    {
        std::wstring  whitespace = Helper::DefaultWhitespace;
        std::wstring    thisWord;
        size_t  currpos;
        float   lineWidth, wordWidth;
        float   widest = 0;

        // get first word.
        currpos = getNextWord(text, 0, thisWord);
        lineWidth = getTextExtent(thisWord, x_scale);

        // while there are words left in the string...
        while (std::wstring::npos != text.find_first_not_of(whitespace, currpos))
        {
            // get next word of the string...
            currpos += getNextWord(text, currpos, thisWord);
            wordWidth = getTextExtent(thisWord, x_scale);

            // if the new word would make the string too long
            if ((lineWidth + wordWidth) > wrapWidth)
            {
                if (lineWidth > widest)
                    widest = lineWidth;

                // remove whitespace from next word - it will form start of next line
                //thisWord = thisWord.substr (thisWord.find_first_not_of (whitespace));
                std::wstring::size_type cutpos = thisWord.find_first_not_of(whitespace);
                if (cutpos != std::wstring::npos)
                {
                    thisWord = thisWord.substr(cutpos);
                }

                wordWidth = getTextExtent(thisWord, x_scale);

                // reset for a new line.
                lineWidth = 0;
            }

            // add the next word to the line
            lineWidth += wordWidth;
        }

        if (lineWidth > widest)
            widest = lineWidth;

        return widest;
    }

    float Font::getTextExtent(const std::wstring& text, float x_scale)
    {
        const FontGlyph* glyph;
        float cur_extent = 0.0f, width;

        for (size_t c = 0; c < text.length(); ++c)
        {
            glyph = getGlyphData(text[c]);

            if (glyph)
            {
                width = glyph->getAdvance(x_scale);
                cur_extent += width;
            }
        }

        return cur_extent;
    }

    size_t Font::getFormattedLineCount(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale /* = 1.0f */)
    {
        // handle simple non-wrapped cases.
        if ((fmt == LeftAligned) || (fmt == Centred) || (fmt == RightAligned) || (fmt == Justified))
        {
            return std::count(text.begin(), text.end(), L'\n') + 1;
        }

        // handle wraping cases
        size_t lineStart = 0, lineEnd = 0;
        std::wstring    sourceLine;

        float   wrap_width = format_area.getWidth();
        std::wstring  whitespace = Helper::DefaultWhitespace;
        std::wstring    thisLine, thisWord;
        size_t  line_count = 0, currpos = 0;

        while (lineEnd < text.length())
        {
            if ((lineEnd = text.find_first_of('\n', lineStart)) == std::string::npos)
            {
                lineEnd = text.length();
            }

            sourceLine = text.substr(lineStart, lineEnd - lineStart);
            lineStart = lineEnd + 1;

            // get first word.
            currpos = getNextWord(sourceLine, 0, thisLine);

            // while there are words left in the string...
            while (std::wstring::npos != sourceLine.find_first_not_of(whitespace, currpos))
            {
                // get next word of the string...
                currpos += getNextWord(sourceLine, currpos, thisWord);

                // if the new word would make the string too long
                if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
                {
                    // too long, so that's another line of text
                    line_count++;

                    // remove whitespace from next word - it will form start of next line
                    //thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));
                    std::wstring::size_type cutpos = thisWord.find_first_not_of(whitespace);
                    if (cutpos != std::wstring::npos)
                    {
                        thisWord = thisWord.substr(cutpos);
                    }

                    // reset for a new line.
                    thisLine.clear();
                }

                // add the next word to the line
                thisLine += thisWord;
            }

            // plus one for final line
            line_count++;
        }

        return line_count;
    }

    float Font::getFormattedTextExtent(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale /* = 1.0f */)
    {
        float lineWidth;
        float widest = 0;

        size_t lineStart = 0, lineEnd = 0;
        std::wstring    currLine;

        while (lineEnd < text.length())
        {
            if ((lineEnd = text.find_first_of('\n', lineStart)) == std::wstring::npos)
            {
                lineEnd = text.length();
            }

            currLine = text.substr(lineStart, lineEnd - lineStart);
            lineStart = lineEnd + 1;    // +1 to skip \n char

            switch(fmt)
            {
            case Centred:
            case RightAligned:
            case LeftAligned:
                lineWidth = getTextExtent(currLine, x_scale);
                break;

            case Justified:
                // usually we use the width of the rect but we have to ensure the current line is not wider than that
                lineWidth = Max(format_area.getWidth(), getTextExtent(currLine, x_scale));
                break;

            case WordWrapLeftAligned:
            case WordWrapRightAligned:
            case WordWrapCentred:
                lineWidth = getWrappedTextExtent(currLine, format_area.getWidth(), x_scale);
                break;

            case WordWrapJustified:
                // same as above
                lineWidth = Max(format_area.getWidth(), getWrappedTextExtent(currLine, format_area.getWidth(), x_scale));
                break;

            default:
                assert(0 && "Font::getFormattedTextExtent - Unknown or unsupported TextFormatting value specified.");
            }

            if (lineWidth > widest)
            {
                widest = lineWidth;
            }

        }

        return widest;
    }

    size_t Font::getNextWord(const std::wstring& in_string, size_t start_idx, std::wstring& out_string) const
    {
        out_string = Helper::getNextWord(in_string, start_idx, Helper::DefaultWrapDelimiters);

        return out_string.length();
    }

    size_t Font::drawText(const std::wstring& text, const Rect& draw_area, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale /* = 1.0f */, float y_scale /* = 1.0f */)
    {
        size_t thisCount;
        size_t lineCount = 0;

        float y_base = draw_area.d_top;

        Rect tmpDrawArea(
            PixelAligned(draw_area.d_left),
            PixelAligned(draw_area.d_top),
            PixelAligned(draw_area.d_right),
            PixelAligned(draw_area.d_bottom)
            );

        size_t lineStart = 0, lineEnd = 0;
        std::wstring currLine;

        while (lineEnd < text.length())
        {
            if ((lineEnd = text.find_first_of('\n', lineStart)) == std::wstring::npos)
                lineEnd = text.length();

            currLine = text.substr(lineStart, lineEnd - lineStart);
            lineStart = lineEnd + 1;    // +1 to skip \n char

            switch (fmt)
            {
            case LeftAligned:
                drawTextLine(currLine, Vector2(tmpDrawArea.d_left, y_base), clip_rect, colours, x_scale, y_scale);
                thisCount = 1;
                y_base += getLineSpacing(y_scale);
                break;

            case RightAligned:
                drawTextLine(currLine, Vector2(tmpDrawArea.d_right - getTextExtent(currLine, x_scale), y_base), clip_rect, colours, x_scale, y_scale);
                thisCount = 1;
                y_base += getLineSpacing(y_scale);
                break;

            case Centred:
                drawTextLine(currLine, Vector2(PixelAligned(tmpDrawArea.d_left + ((tmpDrawArea.getWidth() - getTextExtent(currLine, x_scale)) / 2.0f)), y_base), clip_rect, colours, x_scale, y_scale);
                thisCount = 1;
                y_base += getLineSpacing(y_scale);
                break;

            case Justified:
                // new function in order to keep drawTextLine's signature unchanged
                drawTextLineJustified(currLine, draw_area, Vector2(tmpDrawArea.d_left, y_base), clip_rect, colours, x_scale, y_scale);
                thisCount = 1;
                y_base += getLineSpacing(y_scale);
                break;

            case WordWrapLeftAligned:
                thisCount = drawWrappedText(currLine, tmpDrawArea, clip_rect, LeftAligned, colours, x_scale, y_scale);
                tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
                break;

            case WordWrapRightAligned:
                thisCount = drawWrappedText(currLine, tmpDrawArea, clip_rect, RightAligned, colours, x_scale, y_scale);
                tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
                break;

            case WordWrapCentred:
                thisCount = drawWrappedText(currLine, tmpDrawArea, clip_rect, Centred, colours, x_scale, y_scale);
                tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
                break;

            case WordWrapJustified:
                // no change needed
                thisCount = drawWrappedText(currLine, tmpDrawArea, clip_rect, Justified, colours, x_scale, y_scale);
                tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
                break;

            default:
                assert(0 && "Font::drawText - Unknown or unsupported TextFormatting value specified.");
            }

            lineCount += thisCount;

        }

        // should not return 0
        return Max(lineCount, (size_t)1);
    }

    void Font::drawTextLine(const std::wstring& text, const Vector2& position, const Rect& clip_rect, const ColourRect& colours, float x_scale /* = 1.0f */, float y_scale /* = 1.0f */)
    {
        Point cur_pos(position);

        const FontGlyph* glyph;
        float base_y = position.d_y;

        for (size_t c = 0; c < text.length(); ++c)
        {
            glyph = getGlyphData(text[c]);

            if (glyph)
            {
                cur_pos.d_x += glyph->getAdvanceA(x_scale);

                const Image* img = glyph->getImage();
                if (img)
                {
                    cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY() * y_scale);

                    // temporarily, we just give each character a one-pixel shadow
                    img->draw(Point(cur_pos.d_x + 1.0f, cur_pos.d_y + 1.0f),
                        glyph->getSize(x_scale, y_scale), clip_rect,
                        ColourRect(Colour(0.0f, 0.0f, 0.0f, colours.d_top_left.getAlpha()),
                        Colour(0.0f, 0.0f, 0.0f, colours.d_top_right.getAlpha()),
                        Colour(0.0f, 0.0f, 0.0f, colours.d_bottom_left.getAlpha()),
                        Colour(0.0f, 0.0f, 0.0f, colours.d_bottom_right.getAlpha())));

                    img->draw(cur_pos, glyph->getSize(x_scale, y_scale), clip_rect, colours);
                }

                cur_pos.d_x += glyph->getAdvanceB(x_scale) + glyph->getAdvanceC(x_scale);
            }
        }
    }

    void Font::drawTextLineJustified(const std::wstring& text, const Rect& draw_area, const Vector2& position, const Rect& clip_rect, const ColourRect& colours, float x_scale /* = 1.0f */, float y_scale /* = 1.0f */)
    {
        Vector2 cur_pos(position);

        const FontGlyph* glyph;
        float base_y = position.d_y;
        size_t char_count = text.length();

        // Calculate the length difference between the justified text and the same text, left aligned
        // This space has to be shared between the space characters of the line
        float lost_space = getFormattedTextExtent(text, draw_area, Justified, x_scale) - getTextExtent(text, x_scale);

        // The number of spaces and tabs in the current line
        unsigned int space_count = 0;
        size_t c;
        for (c = 0; c < char_count; ++c)
            if ((text[c] == ' ') || (text[c] == '\t'))
                ++space_count;

        // The width that must be added to each space character in order to transform the left aligned text in justified text
        float shared_lost_space = 0.0;
        if (space_count > 0)
            shared_lost_space = lost_space / (float)space_count;

        for (c = 0; c < char_count; ++c)
        {
            glyph = getGlyphData(text[c]);

            if (glyph)
            {
                cur_pos.d_x += glyph->getAdvanceA(x_scale);

                const Image* img = glyph->getImage();
                if (img)
                {
                    cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY() * y_scale);

                    // temporarily, we just give each character a one-pixel shadow
                    img->draw(Point(cur_pos.d_x + 1.0f, cur_pos.d_y + 1.0f),
                        glyph->getSize(x_scale, y_scale), clip_rect,
                        ColourRect(Colour(0.0f, 0.0f, 0.0f, colours.d_top_left.getAlpha()),
                        Colour(0.0f, 0.0f, 0.0f, colours.d_top_right.getAlpha()),
                        Colour(0.0f, 0.0f, 0.0f, colours.d_bottom_left.getAlpha()),
                        Colour(0.0f, 0.0f, 0.0f, colours.d_bottom_right.getAlpha())));

                    img->draw(cur_pos, glyph->getSize(x_scale, y_scale), clip_rect, colours);
                }

                cur_pos.d_x += glyph->getAdvanceB(x_scale) + glyph->getAdvanceC(x_scale);

                // That's where we adjust the size of each space character
                if ((text[c] == ' ') || (text[c] == '\t'))
                    cur_pos.d_x += shared_lost_space;
            }
        }
    }

    size_t Font::drawWrappedText(const std::wstring& text, const Rect& draw_area, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale /* = 1.0f */, float y_scale /* = 1.0f */)
    {
        size_t  line_count = 0;
        Rect    dest_area(draw_area);
        float   wrap_width = draw_area.getWidth();

        std::wstring  whitespace = Helper::DefaultWhitespace;
        std::wstring    thisLine, thisWord;
        size_t  currpos = 0;

        // get first word.
        currpos += getNextWord(text, currpos, thisLine);

        // while there are words left in the string...
        while (std::wstring::npos != text.find_first_not_of(whitespace, currpos))
        {
            // get next word of the string...
            currpos += getNextWord(text, currpos, thisWord);

            // if the new word would make the string too long
            if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
            {
                // output what we had until this new word
                line_count += drawText(thisLine, dest_area, clip_rect, fmt, colours, x_scale, y_scale);

                // remove whitespace from next word - it will form start of next line
                //thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));
                std::wstring::size_type cutpos = thisWord.find_first_not_of(whitespace);
                if (cutpos != std::wstring::npos)
                {
                    thisWord = thisWord.substr(cutpos);
                }

                // reset for a new line.
                thisLine.clear();

                // update y co-ordinate for next line
                dest_area.d_top += getLineSpacing(y_scale);
            }

            // add the next word to the line
            thisLine += thisWord;
        }

        // Last line is left aligned
        TextFormatting last_fmt = (fmt == Justified ? LeftAligned : fmt);
        // output last bit of string
        line_count += drawText(thisLine, dest_area, clip_rect, last_fmt, colours, x_scale, y_scale);

        return line_count;
    }

    //////////////////////////////////////////////////////////////////////////
    template<> FontManager* Singleton<FontManager>::ms_Singleton = 0;

    FontManager::FontManager()
    {

    }

    FontManager::~FontManager()
    {
        destroyAllFonts();
    }

    FontManager& FontManager::getSingleton()
    {
        return Singleton<FontManager>::getSingleton();
    }

    FontManager* FontManager::getSingletonPtr()
    {
        return Singleton<FontManager>::getSingletonPtr();
    }

    void FontManager::registFont(const std::string& name, Font* font)
    {
        if (isFontPresent(name))
            assert(0 && std::string("FontManager::createFont - A font named '" + name + "' already exists.").c_str());
        else
            d_fonts[name] = font;
    }

    void FontManager::destroyAllFonts()
    {
        for (FontRegistry::iterator i = d_fonts.begin(); i != d_fonts.end(); ++i)
        {
            delete i->second;
        }
        d_fonts.clear();
    }

    void FontManager::destroyFont(const std::string& name)
    {
        FontRegistry::iterator pos = d_fonts.find(name.c_str());

        if (pos != d_fonts.end())
        {
            delete pos->second;
            d_fonts.erase(pos);
        }
    }

    bool FontManager::isFontPresent(const std::string& name) const
    {
        return (d_fonts.find(name.c_str()) != d_fonts.end());
    }

    Font* FontManager::getFont(const std::string& name) const
    {
        FontRegistry::const_iterator pos = d_fonts.find(name.c_str());

        if (pos == d_fonts.end())
        {
            assert(0 && std::string("FontManager::getFont - A Font object with the specified name '" + name +"' does not exist within the system").c_str());
        }

        return pos->second;
    }

    //////////////////////////////////////////////////////////////////////////
    template<> ImagesetManager* Singleton<ImagesetManager>::ms_Singleton = 0;

    ImagesetManager::ImagesetManager()
    {

    }

    ImagesetManager::~ImagesetManager()
    {
        destroyAllImagesets();
    }

    ImagesetManager& ImagesetManager::getSingleton()
    {
        return Singleton<ImagesetManager>::getSingleton();
    }

    ImagesetManager* ImagesetManager::getSingletonPtr()
    {
        return Singleton<ImagesetManager>::getSingletonPtr();
    }

    Imageset* ImagesetManager::createImageset(const std::string& name, Texture* texture)
    {
        if (isImagesetPresent(name))
        {
            assert(0 && std::string("ImagesetManager::createImageset - An Imageset object named '" + name + "' already exists.").c_str());
        }

        Imageset* temp = new Imageset(name, texture);
        d_imagesets[name.c_str()] = temp;

        return temp;
    }

    void ImagesetManager::destroyImageset(const std::string& name)
    {
        ImagesetRegistry::iterator  pos = d_imagesets.find(name.c_str());

        if (pos != d_imagesets.end())
        {
            delete pos->second;
            d_imagesets.erase(pos);
        }

    }

    void ImagesetManager::destroyImageset(Imageset* imageset)
    {
        if (imageset)
        {
            destroyImageset(imageset->getName());
        }
    }

    void ImagesetManager::destroyAllImagesets()
    {
        while (!d_imagesets.empty())
        {
            destroyImageset(d_imagesets.begin()->first);
        }
    }

    Imageset* ImagesetManager::getImageset(const std::string& name) const
    {
        ImagesetRegistry::const_iterator    pos = d_imagesets.find(name.c_str());

        if (pos == d_imagesets.end())
        {
            assert(0 && std::string("ImagesetManager::getImageset - No Imageset named '" + name + "' is present in the system.").c_str());
        }

        return pos->second;
    }

    void ImagesetManager::notifyScreenResolution(const Size& size)
    {
        // notify all attached Imageset objects of the change in resolution
        ImagesetRegistry::iterator pos = d_imagesets.begin(), end = d_imagesets.end();

        for (; pos != end; ++pos)
        {
            pos->second->notifyScreenResolution(size);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    Imageset::Imageset(const std::string& name, Texture* texture)
        : d_name(name)
        , d_texture(texture)
    {
        assert(d_texture && "Imageset::Imageset - Texture object supplied for Imageset creation must be valid.");

        // defaults for scaling options
        d_autoScale = false;
        setNativeResolution(Size(DefaultNativeHorzRes, DefaultNativeVertRes));
    }

    Imageset::~Imageset()
    {
        unload();
    }

    void Imageset::setTexture(Texture* texture)
    {
        assert(d_texture && "Imageset::setTexture - Texture object supplied for Imageset creation must be valid.");

        d_texture = texture;
    }

    Image& Imageset::getImage(const std::string& name) const
    {
        ImageRegistry::const_iterator pos = d_images.find(name);

        if (pos == d_images.end())
        {
            assert(0 && std::string("Imageset::getImage - The Image named '" + name + "' could not be found in Imageset '" + d_name + "'.").c_str());
        }

        return *(Image*)&pos->second;   // anyway else...?
    }

    void Imageset::defineImage(const std::string& name, const Rect& image_rect, const Point& render_offset)
    {
        //if (isImageDefined(name))
        //{
        //  assert(0 && std::string("Imageset::defineImage - An image with the name '" + name + "' already exists in Imageset '" + d_name + "'.").c_str());
        //}

        // get scaling factors
        float hscale = d_autoScale ? d_horzScaling : 1.0f;
        float vscale = d_autoScale ? d_vertScaling : 1.0f;

        // add the Image definition
        d_images[name.c_str()] = Image(this, name, image_rect, render_offset, hscale, vscale);
    }

    void Imageset::draw(const Rect& source_rect, const Rect& dest_rect, const Rect& clip_rect, const ColourRect& colours) const
    {
        // get the rect area that we will actually draw to (i.e. perform clipping)
        Rect final_rect(dest_rect.getIntersection(clip_rect));

        // check if rect was totally clipped
        if (final_rect.getWidth() != 0)
        {
            // Obtain correct scale vlaue from the texture
            const float x_scale = d_texture->getXScale();
            const float y_scale = d_texture->getYScale();

            float tex_per_pix_x = source_rect.getWidth() / dest_rect.getWidth();
            float tex_per_pix_y = source_rect.getHeight() / dest_rect.getHeight();

            // calculate final, clipped, texture co-ordinates

            // This makes use of some tricks that we offset 0.5-pixel to each source image rect so that
            // some pixel blending at the edge will be avoid
            bool isOpengl = IsOpenGLGraph();
            float left = (source_rect.d_left + 0.5f + ((final_rect.d_left - dest_rect.d_left) * tex_per_pix_x)) * x_scale;
            float top = (source_rect.d_top + 0.5f + ((final_rect.d_top - dest_rect.d_top) * tex_per_pix_y)) * y_scale;
            float right = (source_rect.d_right + ((final_rect.d_right - dest_rect.d_right) * tex_per_pix_x)) * x_scale;
            float bottom = (source_rect.d_bottom + ((final_rect.d_bottom - dest_rect.d_bottom) * tex_per_pix_y)) * y_scale;

            Rect tex_rect(left,top,right,bottom);

            final_rect.d_left   = PixelAligned(final_rect.d_left);
            final_rect.d_right  = PixelAligned(final_rect.d_right);
            final_rect.d_top    = PixelAligned(final_rect.d_top);
            final_rect.d_bottom = PixelAligned(final_rect.d_bottom);

            // queue a quad to be rendered
            System::getSingleton().getRenderer()->addQuad(final_rect, d_texture, tex_rect, colours);
        }
    }

    void Imageset::unload()
    {
        undefineAllImages();

        // cleanup texture
        System::getSingleton().getRenderer()->destroyTexture(d_texture);
        d_texture = 0;
    }

    void Imageset::updateImageScalingFactors()
    {
        float hscale, vscale;

        if (d_autoScale)
        {
            hscale = d_horzScaling;
            vscale = d_vertScaling;
        }
        else
        {
            hscale = vscale = 1.0f;
        }

        ImageRegistry::iterator pos = d_images.begin(), end = d_images.end();
        for(; pos != end; ++pos)
        {
            pos->second.setHorzScaling(hscale);
            pos->second.setVertScaling(vscale);
        }
    }

    void Imageset::setAutoScalingEnabled(bool setting)
    {
        if (setting != d_autoScale)
        {
            d_autoScale = setting;
            updateImageScalingFactors();
        }
    }

    void Imageset::setNativeResolution(const Size& size)
    {
        d_nativeHorzRes = size.d_width;
        d_nativeVertRes = size.d_height;

        // re-calculate scaling factors & notify images as required
        notifyScreenResolution(System::getSingleton().getRenderer()->getSize());
    }

    void Imageset::notifyScreenResolution(const Size& size)
    {
        d_horzScaling = size.d_width / d_nativeHorzRes;
        d_vertScaling = size.d_height / d_nativeVertRes;

        if (d_autoScale)
        {
            updateImageScalingFactors();
        }
    }

    void Imageset::undefineImage(const std::string& name)
    {
        d_images.erase(name.c_str());
    }

    void Imageset::undefineAllImages()
    {
        d_images.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    template<> MouseCursor* Singleton<MouseCursor>::ms_Singleton    = 0;

    MouseCursor::MouseCursor()
        : d_position(0.0f, 0.0f)
        , d_size(0.0f, 0.0f)
    {

    }

    MouseCursor::~MouseCursor()
    {

    }

    MouseCursor& MouseCursor::getSingleton()
    {
        return Singleton<MouseCursor>::getSingleton();
    }

    MouseCursor* MouseCursor::getSingletonPtr()
    {
        return Singleton<MouseCursor>::getSingletonPtr();
    }

    void MouseCursor::setPosition(const Point& position)
    {
        d_position.d_x = position.d_x;
        d_position.d_y = position.d_y;
    }

    void MouseCursor::offsetPosition(const Point& offset)
    {
        d_position.d_x += offset.d_x;
        d_position.d_y += offset.d_y;
    }

    Point MouseCursor::getPosition() const
    {
        return Point(d_position.d_x, d_position.d_y);
    }

    void MouseCursor::setSize(const Size& sz)
    {
        d_size = sz;
    }

    Size MouseCursor::getSize() const
    {
        return d_size;
    }

    template<> WindowFactoryManager* Singleton<WindowFactoryManager>::ms_Singleton = 0;

    WindowFactoryManager::WindowFactoryManager()
    {

    }

    WindowFactoryManager::~WindowFactoryManager()
    {
        destroyAllFactory();
    }

    WindowFactoryManager& WindowFactoryManager::getSingleton()
    {
        return Singleton<WindowFactoryManager>::getSingleton();
    }

    WindowFactoryManager* WindowFactoryManager::getSingletonPtr()
    {
        return Singleton<WindowFactoryManager>::getSingletonPtr();
    }

    void WindowFactoryManager::addFactory(WindowFactory* factory)
    {
        // assert if passed factory is null.
        assert(factory);

        // assert if type name for factory is already in use.
        if (d_factoryRegistry.find(factory->getTypeName().c_str()) != d_factoryRegistry.end())
        {
            assert(0);
        }

        // add the factory to the registry
        d_factoryRegistry[factory->getTypeName().c_str()] = factory;
    }

    WindowFactory* WindowFactoryManager::getFactory(const std::string& type) const
    {
        WindowFactoryRegistry::const_iterator pos = d_factoryRegistry.find(type);

        // found an actual factory for this type
        if (pos != d_factoryRegistry.end())
        {
            return pos->second;
        }

        return 0;
    }

    void WindowFactoryManager::destroyAllFactory()
    {
        for (WindowFactoryRegistry::iterator i = d_factoryRegistry.begin(); i != d_factoryRegistry.end(); ++i)
        {
            delete i->second;
        }
        d_factoryRegistry.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    template<> WindowManager* Singleton<WindowManager>::ms_Singleton = 0;

    WindowManager::WindowManager()
    : d_uid_counter(0)
    {

    }

    WindowManager::~WindowManager()
    {
        destroyAllWindows();
    }

    WindowManager& WindowManager::getSingleton()
    {
        return Singleton<WindowManager>::getSingleton();
    }

    WindowManager* WindowManager::getSingletonPtr()
    {
        return Singleton<WindowManager>::getSingletonPtr();
    }

    Window* WindowManager::createWindow(const std::string type, const std::string& name /* = "" */ )
    {
        std::string finalName(name);
        if (finalName.empty())
        {
            finalName = generateUniqueWindowName();
        }

        if (isWindowPresent(finalName))
        {
            assert(0);
            return getWindow(finalName);
        }

        WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
        WindowFactory* factory = wfMgr.getFactory(type);
        assert(factory);
        
        Window* newWindow = factory->createWindow(finalName);
        d_windowRegistry[finalName.c_str()] = newWindow;

        return newWindow;
    }

    void WindowManager::destroyWindow(Window* window)
    {
        if (window)
        {
            std::string name = window->getName();
            destroyWindow(name);
        }
    }

    void WindowManager::destroyWindow(const std::string& window)
    {
        WindowRegistry::iterator wndpos = d_windowRegistry.find(window.c_str());

        if (wndpos != d_windowRegistry.end())
        {
            Window* wnd = wndpos->second;

            // remove entry from the WindowRegistry.
            d_windowRegistry.erase(wndpos);

            // do 'safe' part of cleanup
            wnd->destroy();

            // delete wnd instance
            WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
            WindowFactory* factory = wfMgr.getFactory(wnd->getType());
            assert(factory);
            factory->destroyWindow(wnd);

            // notify system object of the window destruction
            System::getSingleton().notifyWindowDestroyed(wnd);
        }
    }

    void WindowManager::destroyAllWindows()
    {
        std::string window_name;
        while (!d_windowRegistry.empty())
        {
            window_name = d_windowRegistry.begin()->first;
            destroyWindow(window_name);
        }
    }
    
    Window* WindowManager::getWindow(const std::string& name) const
    {
        WindowRegistry::const_iterator pos = d_windowRegistry.find(name.c_str());

        if (pos == d_windowRegistry.end())
        {
            assert(0);
            return 0;
        }

        return pos->second;
    }

    bool WindowManager::isWindowPresent(const std::string& name) const
    {
        return (d_windowRegistry.find(name.c_str()) != d_windowRegistry.end());
    }

    std::string WindowManager::generateUniqueWindowName()
    {
        // build name
        std::ostringstream uidname;
        uidname << "__elgui_uid_" << d_uid_counter;

        // update counter for next time
        unsigned long old_uid = d_uid_counter;
        ++d_uid_counter;

        // log if we ever wrap-around (which should be pretty unlikely)
        if (d_uid_counter < old_uid)
            assert(0 && "UID counter for generated window names has wrapped around - the fun shall now commence!");

        // return generated name as a CEGUI::String.
        return uidname.str();
    }


    //////////////////////////////////////////////////////////////////////////
    template<> System* Singleton<System>::ms_Singleton = 0;

    const double System::DefaultSingleClickTimeout = 0.2;
    const double System::DefaultMultiClickTimeout = 0.33;
    const Size System::DefaultMultiClickAreaSize(12, 12);

#if defined(__WIN32__) || defined(_WIN32)
#include "mmsystem.h"
    double SimpleTimer::currentTime()
    {
        return timeGetTime() / 1000.0;
    }

#elif defined(__linux__)
#include <sys/time.h>
    double SimpleTimer::currentTime()
    {
        timeval timeStructure;
        gettimeofday(&timeStructure, 0);
        return timeStructure.tv_sec + timeStructure.tv_usec / 1000000.0;
    }
#endif

    System::System(Renderer* renderer)
        : d_renderer(renderer)
        , d_wndWithMouse(0)
        , d_activeSheet(0)
        , d_modalTarget(0)
        , d_gui_redraw(true)
        , d_sysKeys(0)
        , d_lshift(false)
        , d_rshift(false)
        , d_lctrl(false)
        , d_rctrl(false)
        , d_lalt(false)
        , d_ralt(false)
        , d_click_timeout(DefaultSingleClickTimeout)
        , d_dblclick_timeout(DefaultMultiClickTimeout)
        , d_dblclick_size(DefaultMultiClickAreaSize)
        , d_clickTrackerPimpl(new MouseClickTrackerImpl)
        , d_mouseScalingFactor(1.0f)
    {
        // create the core system singleton objects
        createSingletons();

        // add the window factories for the core window types
        addWindowFactories();
    }

    System::~System()
    {
        // cleanup singletons
        destroySingletons();

        if (d_clickTrackerPimpl)
            delete d_clickTrackerPimpl;
    }

    Window* System::setGUISheet(Window* sheet)
    {
        Window* old = d_activeSheet;
        d_activeSheet = sheet;

        // Force and update for the area Rects for 'sheet' so they're correct according
        // to the screen size.
        if (sheet)
        {
            WindowEventArgs sheetargs(0);
            sheet->onParentSized(sheetargs);
        }

        return old;
    }

    Window* System::getGUISheet() const
    {
        return d_activeSheet;
    }

    void System::renderGUI()
    {
		ATG_PROFILE("System::renderGUI");
        // This makes use of some tricks the Renderer can do so that we do not
        // need to do a full redraw every frame - only when some UI element has
        // changed.
        // Since the mouse is likely to move very often, and in order not to
        // short-circuit the above optimisation, the mouse is not queued, but is
        // drawn directly to the display every frame.

        if (d_gui_redraw)
        {
            d_renderer->clearRenderList();

            if (d_activeSheet)
            {
                d_activeSheet->render();
            }
            d_gui_redraw = false;
        }

        d_renderer->doRender();
    }

    bool System::injectMouseMove(float delta_x, float delta_y)
    {
        MouseEventArgs ma(0);
        MouseCursor& mouse = MouseCursor::getSingleton();

        ma.moveDelta.d_x = delta_x * d_mouseScalingFactor;
        ma.moveDelta.d_y = delta_y * d_mouseScalingFactor;
        ma.sysKeys = d_sysKeys;
        ma.wheelChange = 0;
        ma.clickCount = 0;
        ma.button = NoButton;

        // move the mouse cursor & update position in args.
        mouse.offsetPosition(ma.moveDelta);
        ma.position = mouse.getPosition();

        Window* dest_window = getTargetWindow(ma.position);

        // has window containing mouse changed?
        if (dest_window != d_wndWithMouse)
        {
            // store previous window that contained mouse
            Window* oldWindow = d_wndWithMouse;
            // set the new window that contains the mouse.
            d_wndWithMouse = dest_window;

            // inform previous window the mouse has left it
            if (oldWindow)
            {
                ma.window = oldWindow;
                oldWindow->onMouseLeaves(ma);
            }

            // inform window containing mouse that mouse has entered it
            if (d_wndWithMouse)
            {
                ma.window = d_wndWithMouse;
                ma.handled = false;
                d_wndWithMouse->onMouseEnters(ma);
            }
        }

        // inform appropriate window of the mouse movement event
        if (dest_window)
        {
            // ensure event starts as 'not handled'
            ma.handled = false;

            // loop backwards until event is handled or we run out of windows.
            while ((!ma.handled) && (dest_window != 0))
            {
                ma.window = dest_window;
                dest_window->onMouseMove(ma);
                dest_window = getNextTargetWindow(dest_window);
            }
        }

        return ma.handled;
    }

    bool System::injectMousePosition(float x_pos, float y_pos)
    {
        // set new mouse position
        MouseCursor::getSingleton().setPosition(Point(x_pos, y_pos));

        // do the real work
        return injectMouseMove(0, 0);
    }

    bool System::injectMouseButtonDown(MouseButton button)
    {
        // update system keys
        d_sysKeys |= mouseButtonToSyskey(button);

        MouseEventArgs ma(0);
        ma.position = MouseCursor::getSingleton().getPosition();
        ma.moveDelta = Vector2(0.0f, 0.0f);
        ma.button = button;
        ma.sysKeys = d_sysKeys;
        ma.wheelChange = 0;

        // find the likely destination for generated events.
        Window* dest_window = getTargetWindow(ma.position);

        //
        // Handling for multi-click generation
        //
        MouseClickTracker& tkr = d_clickTrackerPimpl->click_trackers[button];

        tkr.d_click_count++;

        // if multi-click requirements are not met
        if ((tkr.d_timer.elapsed() > d_dblclick_timeout) ||
            (!tkr.d_click_area.isPointInRect(ma.position)) ||
            (tkr.d_target_window != dest_window) ||
            (tkr.d_click_count > 3))
        {
            // reset to single down event.
            tkr.d_click_count = 1;

            // build new allowable area for multi-clicks
            tkr.d_click_area.setPosition(ma.position);
            tkr.d_click_area.setSize(d_dblclick_size);
            tkr.d_click_area.offset(Point(-(d_dblclick_size.d_width / 2), -(d_dblclick_size.d_height / 2)));

            // set target window for click events on this tracker
            tkr.d_target_window = dest_window;
        }

        // set click count in the event args
        ma.clickCount = tkr.d_click_count;

        // loop backwards until event is handled or we run out of windows.
        while ((!ma.handled) && (dest_window != 0))
        {
            ma.window = dest_window;

            if (dest_window->wantsMultiClickEvents())
            {
                switch (tkr.d_click_count)
                {
                case 1:
                    dest_window->onMouseButtonDown(ma);
                    break;

                case 2:
                    dest_window->onMouseDoubleClicked(ma);
                    break;

                case 3:
                    dest_window->onMouseTripleClicked(ma);
                    break;
                }
            }
            // current target window does not want multi-clicks,
            // so just send a mouse down event instead.
            else
            {
                dest_window->onMouseButtonDown(ma);
            }

            dest_window = getNextTargetWindow(dest_window);
        }

        // reset timer for this tracker.
        tkr.d_timer.restart();

        return ma.handled;
    }

    bool System::injectMouseButtonUp(MouseButton button)
    {
        // update system keys
        d_sysKeys &= ~mouseButtonToSyskey(button);

        MouseEventArgs ma(0);
        ma.position = MouseCursor::getSingleton().getPosition();
        ma.moveDelta = Vector2(0.0f, 0.0f);
        ma.button = button;
        ma.sysKeys = d_sysKeys;
        ma.wheelChange = 0;

        // get the tracker that holds the number of down events seen so far for this button
        MouseClickTracker& tkr = d_clickTrackerPimpl->click_trackers[button];
        // set click count in the event args
        ma.clickCount = tkr.d_click_count;

        Window* const initial_dest_window = getTargetWindow(ma.position);
        Window* dest_window = initial_dest_window;

        // loop backwards until event is handled or we run out of windows.
        while ((!ma.handled) && (dest_window != 0))
        {
            ma.window = dest_window;
            dest_window->onMouseButtonUp(ma);
            dest_window = getNextTargetWindow(dest_window);
        }

        bool wasUpHandled = ma.handled;

        // if requirements for click events are met
        if ((tkr.d_timer.elapsed() <= d_click_timeout) &&
            (tkr.d_click_area.isPointInRect(ma.position)) &&
            (tkr.d_target_window == initial_dest_window))
        {
            ma.handled = false;
            dest_window = initial_dest_window;

            // loop backwards until event is handled or we run out of windows.
            while ((!ma.handled) && (dest_window != 0))
            {
                ma.window = dest_window;
                dest_window->onMouseClicked(ma);
                dest_window = getNextTargetWindow(dest_window);
            }

        }

        return (ma.handled | wasUpHandled);
    }

    bool System::injectMouseWheelChange(float delta)
    {
        MouseEventArgs ma(0);
        ma.position = MouseCursor::getSingleton().getPosition();
        ma.moveDelta = Vector2(0.0f, 0.0f);
        ma.button = NoButton;
        ma.sysKeys = d_sysKeys;
        ma.wheelChange = delta;
        ma.clickCount = 0;

        Window* dest_window = getTargetWindow(ma.position);

        // loop backwards until event is handled or we run out of windows.
        while ((!ma.handled) && (dest_window != 0))
        {
            ma.window = dest_window;
            dest_window->onMouseWheel(ma);
            dest_window = getNextTargetWindow(dest_window);
        }

        return ma.handled;
    }

    bool System::injectMouseSize(float width, float hight)
    {
        // set new mouse cursor size
        MouseCursor::getSingleton().setSize(Size(width, hight));
        return true;
    }

    bool System::injectKeyDown(unsigned int key_code)
    {
        // update system keys
        d_sysKeys |= keyCodeToSyskey((Key::Scan)key_code, true);

        KeyEventArgs args(0);

        if (d_activeSheet)
        {
            args.scancode = (Key::Scan)key_code;
            args.sysKeys = d_sysKeys;

            Window* dest = getKeyboardTargetWindow();

            // loop backwards until event is handled or we run out of windows.
            while ((dest != 0) && (!args.handled))
            {
                args.window = dest;
                dest->onKeyDown(args);
                dest = getNextTargetWindow(dest);
            }

        }

        return args.handled;
    }

    bool System::injectKeyUp(unsigned int key_code)
    {
        // update system keys
        d_sysKeys &= ~keyCodeToSyskey((Key::Scan)key_code, false);

        KeyEventArgs args(0);

        if (d_activeSheet)
        {
            args.scancode = (Key::Scan)key_code;
            args.sysKeys = d_sysKeys;

            Window* dest = getKeyboardTargetWindow();

            // loop backwards until event is handled or we run out of windows.
            while ((dest != 0) && (!args.handled))
            {
                args.window = dest;
                dest->onKeyUp(args);
                dest = getNextTargetWindow(dest);
            }

        }

        return args.handled;
    }

    bool System::injectChar(unsigned int code_point)
    {
        // whether is a normal character
        if (d_sysKeys & Control)
        {
            return false;
        }

        KeyEventArgs args(0);

        if (d_activeSheet)
        {
            args.codepoint = code_point;
            args.sysKeys = d_sysKeys;

            Window* dest = getKeyboardTargetWindow();

            // loop backwards until event is handled or we run out of windows.
            while ((dest != 0) && (!args.handled))
            {
                args.window = dest;
                dest->onCharacter(args);
                dest = getNextTargetWindow(dest);
            }

        }

        return args.handled;
    }

    SystemKey System::mouseButtonToSyskey(MouseButton btn) const
    {
        switch (btn)
        {
        case LeftButton:
            return LeftMouse;

        case RightButton:
            return RightMouse;

        case MiddleButton:
            return MiddleMouse;

        case X1Button:
            return X1Mouse;

        case X2Button:
            return X2Mouse;

        default:
            assert(0 && "System::mouseButtonToSyskey - the parameter 'btn' is not a valid MouseButton value.");
            return (SystemKey)0;
        }
    }

    SystemKey System::keyCodeToSyskey(Key::Scan key, bool direction)
    {
        switch (key)
        {
        case Key::LeftShift:
            d_lshift = direction;

            if (!d_rshift)
            {
                return Shift;
            }
            break;

        case Key::RightShift:
            d_rshift = direction;

            if (!d_lshift)
            {
                return Shift;
            }
            break;


        case Key::LeftControl:
            d_lctrl = direction;

            if (!d_rctrl)
            {
                return Control;
            }
            break;

        case Key::RightControl:
            d_rctrl = direction;

            if (!d_lctrl)
            {
                return Control;
            }
            break;

        case Key::LeftAlt:
            d_lalt = direction;

            if (!d_ralt)
            {
                return Alt;
            }
            break;

        case Key::RightAlt:
            d_ralt = direction;

            if (!d_lalt)
            {
                return Alt;
            }
            break;

        default:
            break;
        }

        // if not a system key or overall state unchanged, return 0.
        return (SystemKey)0;
    }

    unsigned int System::getSystemKeys() const
    {
        return d_sysKeys;
    }

    void System::notifyWindowDestroyed(const Window* window)
    {
        if (d_wndWithMouse == window)
        {
            d_wndWithMouse = 0;
        }

        if (d_activeSheet == window)
        {
            d_activeSheet = 0;
        }

        if (d_modalTarget == window)
        {
            d_modalTarget = 0;
        }
    }

    void System::setModalTarget(Window* target)
    {
        d_modalTarget = target;
    }

    Window* System::getWindowContainingMouse() const
    {
        return d_wndWithMouse;
    }

    Window* System::getModalTarget() const
    {
        return d_modalTarget;
    }

    Window* System::getFocusWindow()
    {
        return getKeyboardTargetWindow();
    }

    Window* System::getTargetWindow(const Point& pt) const
    {
        Window* dest_window = 0;

        // if there is no GUI sheet, then there is nowhere to send input
        if (d_activeSheet)
        {
            dest_window = Window::getCaptureWindow();

            if (!dest_window)
            {
                dest_window = d_activeSheet->getTargetChildAtPosition(pt);

                if (!dest_window)
                {
                    dest_window = d_activeSheet;
                }

            }
            else
            {
                if (dest_window->distributesCapturedInputs())
                {
                    Window* child_window = dest_window->getTargetChildAtPosition(pt);

                    if (child_window)
                    {
                        dest_window = child_window;
                    }

                }

            }

            // modal target overrules
            if (d_modalTarget != 0 && dest_window != d_modalTarget)
            {
                if (!dest_window->isAncestor(d_modalTarget))
                {
                    dest_window = d_modalTarget;
                }

            }

        }

        return dest_window;
    }

    Window* System::getKeyboardTargetWindow() const
    {
        Window* target = 0;

        if (!d_modalTarget)
        {
            target = d_activeSheet->getActiveChild();
        }
        else
        {
            target = d_modalTarget->getActiveChild();
            if (!target)
            {
                target = d_modalTarget;
            }
        }

        return target;
    }

    Renderer* System::getRenderer() const
    {
        return d_renderer;
    }

    bool System::handleDisplaySizeChange()
    {
        // notify the imageset manager of the size change
        Size new_sz = getRenderer()->getSize();
        ImagesetManager::getSingleton().notifyScreenResolution(new_sz);

        // notify gui sheet / root if size change, event propagation will ensure everything else
        // gets updated as required.
        if (d_activeSheet)
        {
            WindowEventArgs args(0);
            d_activeSheet->onParentSized(args);
        }

        return true;
    }

    void System::signalRedraw()
    {
        d_gui_redraw = true;
    }

    System& System::getSingleton()
    {
        return Singleton<System>::getSingleton();
    }

    System* System::getSingletonPtr()
    {
        return Singleton<System>::getSingletonPtr();
    }

    void System::createSingletons()
    {
        // cause creation of other singleton objects
        new ImagesetManager();
        new FontManager();
        new WindowFactoryManager();
        new WindowManager();
        new MouseCursor();
    }

    void System::destroySingletons()
    {
        delete WindowManager::getSingletonPtr();
        delete WindowFactoryManager::getSingletonPtr();
        delete FontManager::getSingletonPtr();
        delete MouseCursor::getSingletonPtr();
        delete ImagesetManager::getSingletonPtr();
    }

    void System::addWindowFactories()
    {
        // Add factories for types all windows
        WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
        wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(DefaultWindow));
        wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(StaticImage));
        wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(StaticText));
        wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(ButtonBase));
        wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(ImageButton));
        wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(FrameWindow));
        wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(Tooltip));
    }

    Window* System::getNextTargetWindow(Window* w) const
    {
        // if we have not reached the modal target, return the parent
        if (w != d_modalTarget)
        {
            return w->getParent();
        }

        // otherwise stop now
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    const std::wstring Helper::DefaultWhitespace(L" \n\t\r");
    const std::wstring Helper::DefaultAlphanumerical(L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
    const std::wstring Helper::DefaultWrapDelimiters(L" \n\t\r");

    std::wstring Helper::getNextWord(const std::wstring& str, std::wstring::size_type start_idx /*= 0*/, const std::wstring& delimiters /*= DefaultWhitespace*/)
    {
        std::wstring::size_type word_len = 0;

        std::wstring::size_type word_end = str.find_first_not_of(DefaultAlphanumerical, start_idx);

        if (word_end == std::wstring::npos)
        {
            word_len = str.length();
        }
        else if (word_end == start_idx)
        {
            word_len = 1;
        }
        else
        {
            word_len = word_end - start_idx;
        }

        return str.substr(start_idx, word_len);
    }

    const Image* Helper::stringToImage(const std::string& str)
    {
        using namespace std;

        // handle empty string case
        if (str.empty())
            return 0;

        char imageSet[128];
        char imageName[128];

        sscanf(str.c_str(), " set:%127s image:%127s", imageSet, imageName);

        const Image* image;

        try
        {
            image = &ImagesetManager::getSingleton().getImageset(imageSet)->getImage(imageName);
        }
        catch (...)
        {
            image = 0;
        }

        return image;
    }

    //////////////////////////////////////////////////////////////////////////
    Texture::Texture()
    {

    }

    Texture::~Texture()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    Renderer::Renderer()
    {

    }

    Renderer::~Renderer()
    {

    }
}

//////////////////////////////////////////////////////////////////////////
atgUITexture::atgUITexture():_texture(NULL)
{

}

atgUITexture::~atgUITexture()
{
    SAFE_DELETE(_texture);
}

void atgUITexture::loadFromFile(const std::string& filename)
{
    SAFE_DELETE(_texture);

    static std::string jpg("jpg");
    static std::string tga("tga");
    static std::string png("png");

    bool isOpenGL = IsOpenGLGraph();
    size_t d = filename.rfind('.');
    if (d != std::string::npos)
    {
        if (0 == filename.compare(d+1, filename.length(), tga))
        {
            TGA_Image image;
            if (TGA_Loader::Load(&image, filename.c_str(), false, isOpenGL ? CO_RGBA: CO_ARGB))
            {
                _texture = atgTexture::Create(image.width, image.height, image.bpp, image.imageData);
            }
        }
        else if(0 == filename.compare(d+1, filename.length(), jpg))
        {
            JPEG_Image image;
            if (JPEG_Loader::Load(&image, filename.c_str(), false, isOpenGL ? CO_RGBA: CO_ARGB))
            {
                _texture = atgTexture::Create(image.width, image.height, image.bpp, image.imageData);
            }
        }
        else if (0 == filename.compare(d+1, filename.length(), png))
        {
            PNG_Image image;
            if (PNG_Loader::Load(&image, filename.c_str(), false, isOpenGL ? CO_RGBA: CO_ARGB))
            {
                _texture = atgTexture::Create(image.width, image.height, image.bpp, image.imageData);
            }
        }
    }
}

void atgUITexture::loadFromMemory(const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, ELGUI::Texture::PixelFormat pixelFormat)
{

}

unsigned short atgUITexture::getWidth() const
{
    if (_texture)
    {
        return _texture->GetWidth();
    }
    return 0;
}

unsigned short atgUITexture::getHeight() const
{
    if (_texture)
    {
        return _texture->GetHeight();
    }
    return 0;
}

void atgUITexture::setTexture(atgTexture* tex)
{
    SAFE_DELETE(_texture);
    _texture = tex;
}

atgTexture* atgUITexture::getTexture() const
{
    return _texture;
}

atgUIPass::atgUIPass()
{
}

atgUIPass::~atgUIPass()
{
}
     
atgPass* atgUIPass::Create(const char* VSFilePath, const char* FSFilePath)
{
    atgUIPass* pPass = new atgUIPass();
    if (!pPass->Load(VSFilePath, FSFilePath))
    {
        delete pPass;
        pPass = NULL;
    }
    if(!pPass->Link())
    {
        SAFE_DELETE(pPass);
        return NULL;
    }
    LOG("create pass(%s,%s) success.\n", VSFilePath, FSFilePath);
    return pPass;
}

void atgUIPass::BeginContext(void* data)
{
    float screenWidth = ELGUI::System::getSingleton().getRenderer()->getWidth();
    float screenHeight = ELGUI::System::getSingleton().getRenderer()->getHeight();

    Matrix WVP;
    WVP.Identity();
    OrthoProject(screenWidth, screenHeight, -1.0f, 1.0f, WVP.m);
    MatTranspose(WVP.m, WVP.m);
    SetMatrix4x4("world_view_projection_matrix", WVP.m);


    g_Renderer->GetBlendFunction(_oldSrcBlend, _oldDestBlend);
    g_Renderer->SetBlendFunction(BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA);

    g_Renderer->SetFaceCull(FCM_CCW);
}

void atgUIPass::EndContext(void* data)
{
    g_Renderer->SetBlendFunction(_oldSrcBlend, _oldDestBlend);
    g_Renderer->SetFaceCull(FCM_CW);
}

atgQuadMesh::atgQuadMesh()
{
    _VertexDecl = new atgVertexDecl();
    _VertexDecl->AppendElement(0, atgVertexDecl::VA_Pos3);
    _VertexDecl->AppendElement(0, atgVertexDecl::VA_Diffuse);
    _VertexDecl->AppendElement(0, atgVertexDecl::VA_Texture0);

    _indices[0] = 0;
    _indices[1] = 1;
    _indices[2] = 2;
    _indices[3] = 2;
    _indices[4] = 1;
    _indices[5] = 3;

    _material = new atgMaterial();
    atgUIRenderer* renderer = static_cast<atgUIRenderer*>(ELGUI::System::getSingleton().getRenderer());
    atgPass* pass = renderer->GetUIMaterialPass();
    if (NULL == pass)
    {
#ifdef USE_OPENGL
        pass = atgUIPass::Create("shaders/ui_element.glvs", "shaders/ui_element.glfs");
#else
        pass = atgUIPass::Create("shaders/ui_element.dxvs", "shaders/ui_element.dxps");
#endif
        renderer->SetUIMaterialPass(pass);
    }
    _material->SetPass(pass);
}

atgQuadMesh::~atgQuadMesh()
{

}

void atgQuadMesh::Setup(const ELGUI::Rect& dest_rect, const ELGUI::Texture* tex, const ELGUI::Rect& texture_rect, const ELGUI::ColourRect& colours)
{
    float screenWidth = ELGUI::System::getSingleton().getRenderer()->getWidth();
    float screenHeight = ELGUI::System::getSingleton().getRenderer()->getHeight();

    ELGUI::Rect final_dest_rect;
    final_dest_rect.d_left = ((dest_rect.d_left / screenWidth) - 0.5f) * screenWidth;
    final_dest_rect.d_right = ((dest_rect.d_right / screenWidth) - 0.5f) * screenWidth;

    final_dest_rect.d_top = (0.5f - (dest_rect.d_top / screenHeight)) * screenHeight;
    final_dest_rect.d_bottom = (0.5f - (dest_rect.d_bottom / screenHeight)) * screenHeight;

    _vertexs[0].position[x] = final_dest_rect.d_left;
    _vertexs[0].position[y] = final_dest_rect.d_top;
    _vertexs[0].position[z] = 0.0f;
    _vertexs[0].color[r] = colours.d_top_left.getRed();
    _vertexs[0].color[g] = colours.d_top_left.getGreen();
    _vertexs[0].color[b] = colours.d_top_left.getBlue();
    _vertexs[0].texCoord[u] = texture_rect.d_left;
    _vertexs[0].texCoord[v] = texture_rect.d_top;

    _vertexs[1].position[x] = final_dest_rect.d_right;
    _vertexs[1].position[y] = final_dest_rect.d_top;
    _vertexs[1].position[z] = 0.0f;
    _vertexs[1].color[r] = colours.d_top_right.getRed();
    _vertexs[1].color[g] = colours.d_top_right.getGreen();
    _vertexs[1].color[b] = colours.d_top_right.getBlue();
    _vertexs[1].texCoord[u] = texture_rect.d_right;
    _vertexs[1].texCoord[v] = texture_rect.d_top;

    _vertexs[2].position[x] = final_dest_rect.d_left;
    _vertexs[2].position[y] = final_dest_rect.d_bottom;
    _vertexs[2].position[z] = 0.0f;
    _vertexs[2].color[r] = colours.d_bottom_left.getRed();
    _vertexs[2].color[g] = colours.d_bottom_left.getGreen();
    _vertexs[2].color[b] = colours.d_bottom_left.getBlue();
    _vertexs[2].texCoord[u] = texture_rect.d_left;
    _vertexs[2].texCoord[v] = texture_rect.d_bottom;

    _vertexs[3].position[x] = final_dest_rect.d_right;
    _vertexs[3].position[y] = final_dest_rect.d_bottom;
    _vertexs[3].position[z] = 0.0f;
    _vertexs[3].color[r] = colours.d_bottom_right.getRed();
    _vertexs[3].color[g] = colours.d_bottom_right.getGreen();
    _vertexs[3].color[b] = colours.d_bottom_right.getBlue();
    _vertexs[3].texCoord[u] = texture_rect.d_right;
    _vertexs[3].texCoord[v] = texture_rect.d_bottom;

    _material->ClearTexture();
    ELGUI::Texture* texture = const_cast<ELGUI::Texture*>(tex);
    _material->AddTexture(static_cast<atgUITexture*>(texture)->getTexture());

    SetVertexsDataDirty();
}

uint32 atgQuadMesh::GetNumberOfVertexs() const
{
    return NumberOfVertexs;
}

void* atgQuadMesh::GetVertexs()
{
    return _vertexs;
}
     
atgVertexDecl* atgQuadMesh::GetVertexDeclaration() const
{
    return _VertexDecl;
}

uint32 atgQuadMesh::GetNumberOfIndices() const
{
    return NumberOfIndices;
}

void* atgQuadMesh::GetIndices()
{
    return _indices;
}

uint32 atgQuadMesh::GetIndexFormat() const
{
    return (uint32)IndexFormat;
}

void atgQuadMesh::Render()
{
    atgMesh::Render();
}

atgUIRenderer* g_UIRenderer = NULL;

atgUIRenderer::atgUIRenderer(atgGame* game):_game(game),_drawQuadCount(0)
{
    _UIMaterialPass = NULL;
}

atgUIRenderer::~atgUIRenderer()
{
    clearRenderList();
}

void atgUIRenderer::addQuad(const ELGUI::Rect& dest_rect, const ELGUI::Texture* tex, const ELGUI::Rect& texture_rect, const ELGUI::ColourRect& colours)
{
    atgQuadMesh* quadMesh = NULL;
    if (_drawQuadCount >= _quads.size())
    {
        _quads.push_back(new atgQuadMesh());
    }    
        
    quadMesh = _quads[_drawQuadCount];
    quadMesh->Setup(dest_rect, tex, texture_rect, colours);
    ++_drawQuadCount;
}

void atgUIRenderer::clearRenderList()
{
    DrawQuadList::iterator itEnd = _quads.end();
    for (DrawQuadList::iterator it = _quads.begin();
         it != itEnd; ++it)
    {
        delete *it;
    }
    _quads.clear();
    _drawQuadCount = 0;
}

void atgUIRenderer::doRender()
{
    DrawQuadList::iterator itEnd = _quads.end();
    for (DrawQuadList::iterator it = _quads.begin();
        it != itEnd; ++it)
    {
        (*it)->Render();
    }

    _drawQuadCount = 0;
}

ELGUI::Texture* atgUIRenderer::createTexture(const std::string& filename)
{
    atgUITexture* t = new atgUITexture();
    if (t)
    {
        t->loadFromFile(filename);
        if (t->getWidth() == 0)
        {
            SAFE_DELETE(t);
        }
        return t;
    }
    return NULL;
}

ELGUI::Texture* atgUIRenderer::createTexture()
{
    return new atgUITexture();
}

void atgUIRenderer::destroyTexture(ELGUI::Texture* texture)
{
    SAFE_DELETE(texture);
}

float atgUIRenderer::getWidth() const
{
    uint32 port[4];
    g_Renderer->GetViewPort(port[0], port[1], port[2], port[3]);
    return (float)port[2];
}

float atgUIRenderer::getHeight() const
{
    uint32 port[4];
    g_Renderer->GetViewPort(port[0], port[1], port[2], port[3]);
    return (float)port[3];
}

ELGUI::Size atgUIRenderer::getSize() const
{
    uint32 port[4];
    g_Renderer->GetViewPort(port[0], port[1], port[2], port[3]);
    return ELGUI::Size((float)port[2],(float)port[3]);
}

ELGUI::Rect atgUIRenderer::getRect() const
{
    uint32 port[4];
    g_Renderer->GetViewPort(port[0], port[1], port[2], port[3]);
    return ELGUI::Rect(0,0,(float)port[2], (float)port[3]);
}