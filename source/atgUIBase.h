#ifndef _ATG_UI_BASE_H_
#define _ATG_UI_BASE_H_

#include "atgBase.h"

//////////////////////////////////////////////////////////////////////////
// USing ELGameEngine UI System
//////////////////////////////////////////////////////////////////////////

namespace ELGUI
{

    static const float DefaultNativeHorzRes = 640.0f;   // Default native horizontal resolution (for fonts and imagesets)
    static const float DefaultNativeVertRes = 480.0f;   // Default native vertical resolution (for fonts and imagesets)

#define CEGUI_ALIGN_ELEMENTS_TO_PIXELS 1

#if defined(CEGUI_ALIGN_ELEMENTS_TO_PIXELS)
# define PixelAligned(x) ((float)(int)((x) + ((x) > 0.0f ? 0.5f : -0.5f)))
#else
# define PixelAligned(x) (x)
#endif

    class Size
    {
    public:
        Size() {}
        Size(float width, float height) : d_width(width), d_height(height) {}

        bool operator==(const Size& other) const
        {
            return d_width == other.d_width && d_height == other.d_height;
        }
        bool operator!=(const Size& other) const
        {
            return !operator==(other);
        }

        float d_width, d_height;
    };

    class Vector2
    {
    public:
        Vector2() {}
        Vector2(float x, float y) : d_x(x), d_y(y) {}

        Vector2& operator*=(const Vector2& vec)
        {
            d_x *= vec.d_x;
            d_y *= vec.d_y;

            return *this;
        }

        Vector2& operator/=(const Vector2& vec)
        {
            d_x /= vec.d_x;
            d_y /= vec.d_y;

            return *this;
        }

        Vector2& operator+=(const Vector2& vec)
        {
            d_x += vec.d_x;
            d_y += vec.d_y;

            return *this;
        }

        Vector2& operator-=(const Vector2& vec)
        {
            d_x -= vec.d_x;
            d_y -= vec.d_y;

            return *this;
        }

        Vector2 operator+(const Vector2& vec) const
        {
            return Vector2(d_x + vec.d_x, d_y + vec.d_y);
        }

        Vector2 operator-(const Vector2& vec) const
        {
            return Vector2(d_x - vec.d_x, d_y - vec.d_y);
        }

        Vector2 operator*(const Vector2& vec) const
        {
            return Vector2(d_x * vec.d_x, d_y * vec.d_y);
        }

        bool    operator==(const Vector2& vec) const
        {
            return ((d_x == vec.d_x) && (d_y == vec.d_y));
        }

        bool    operator!=(const Vector2& vec) const
        {
            return !(operator==(vec));
        }

        Size asSize() const { return Size(d_x, d_y); }

        float d_x, d_y;
    };

    typedef Vector2 Point;

    class Vector3
    {
    public:
        Vector3() {}
        Vector3(float x, float y, float z) : d_x(x), d_y(y), d_z(z) {}

        float   d_x, d_y, d_z;
    };

    class Rect
    {
    public:
        Rect() {}
        Rect(float left, float top, float right, float bottom);
        Rect(Point pos, Size sz);

        Point getPosition() const
        {
            return Point(d_left, d_top);
        }

        float getWidth() const
        {
            return d_right - d_left;
        }

        float getHeight() const
        {
            return d_bottom - d_top;
        }

        Size getSize() const
        {
            return Size(getWidth(), getHeight());
        }

        void setPosition(const Point& pt);

        void setWidth(float width)
        {
            d_right = d_left + width;
        }

        void setHeight(float height)
        {
            d_bottom = d_top + height;
        }

        void setSize(const Size& sze)
        {
            setWidth(sze.d_width); setHeight(sze.d_height);
        }

        Rect getIntersection(const Rect& rect) const;
        bool isPointInRect(const Point& pt) const;
        Rect& offset(const Point& pt);

        Rect& constrainSizeMax(const Size& sz);
        Rect& constrainSizeMin(const Size& sz);
        Rect& constrainSize(const Size& max_sz, const Size& min_sz);

        bool operator==(const Rect& rhs) const
        {
            return ((d_left == rhs.d_left) && (d_right == rhs.d_right) && (d_top == rhs.d_top) && (d_bottom == rhs.d_bottom));
        }

        bool operator!=(const Rect& rhs) const
        {
            return !operator==(rhs);
        }

        Rect& operator=(const Rect& rhs);

        Rect operator*(float scalar) const
        {
            return Rect(d_left * scalar, d_top * scalar, d_right * scalar, d_bottom * scalar);
        }

        const Rect& operator*=(float scalar)
        {
            d_left *= scalar; d_top *= scalar; d_right *= scalar; d_bottom *= scalar; return *this;
        }

        // Data Fields
        float   d_top, d_bottom, d_left, d_right;
    };

#define elgui_absdim(x) ELGUI::UDim(0,(x))
#define elgui_reldim(x) ELGUI::UDim((x),0)

    class UDim
    {
    public:
        UDim() {}
        UDim(float scale, float offset) : d_scale(scale), d_offset(offset) {}
        ~UDim() {}

        float asAbsolute(float base) const    { return PixelAligned(base * d_scale) + d_offset; }
        float asRelative(float base) const    { return (base != 0.0f) ? d_offset / base + d_scale : 0.0f; }

        UDim operator+(const UDim& other) const     { return UDim(d_scale + other.d_scale, d_offset + other.d_offset); }
        UDim operator-(const UDim& other) const     { return UDim(d_scale - other.d_scale, d_offset - other.d_offset); }
        UDim operator/(const UDim& other) const     { return UDim(d_scale / other.d_scale, d_offset / other.d_offset); }
        UDim operator*(const UDim& other) const     { return UDim(d_scale * other.d_scale, d_offset * other.d_offset); }

        const UDim& operator+=(const UDim& other)   { d_scale += other.d_scale; d_offset += other.d_offset; return *this; }
        const UDim& operator-=(const UDim& other)   { d_scale -= other.d_scale; d_offset -= other.d_offset; return *this; }
        const UDim& operator/=(const UDim& other)   { d_scale /= other.d_scale; d_offset /= other.d_offset; return *this; }
        const UDim& operator*=(const UDim& other)   { d_scale *= other.d_scale; d_offset *= other.d_offset; return *this; }

        bool operator==(const UDim& other) const    { return d_scale == other.d_scale && d_offset == other.d_offset; }
        bool operator!=(const UDim& other) const    { return !operator==(other); }

        float d_scale, d_offset;
    };

    class UVector2
    {
    public:
        UVector2() {}
        UVector2(const UDim& x, const UDim& y) : d_x(x), d_y(y) {}
        ~UVector2() {}

        Vector2 asAbsolute(const Size& base) const    { return Vector2(d_x.asAbsolute(base.d_width), d_y.asAbsolute(base.d_height)); }
        Vector2 asRelative(const Size& base) const    { return Vector2(d_x.asRelative(base.d_width), d_y.asRelative(base.d_height)); }

        UVector2 operator+(const UVector2& other) const     { return UVector2(d_x + other.d_x, d_y + other.d_y); }
        UVector2 operator-(const UVector2& other) const     { return UVector2(d_x - other.d_x, d_y - other.d_y); }
        UVector2 operator/(const UVector2& other) const     { return UVector2(d_x / other.d_x, d_y / other.d_y); }
        UVector2 operator*(const UVector2& other) const     { return UVector2(d_x * other.d_x, d_y * other.d_y); }

        const UVector2& operator+=(const UVector2& other)   { d_x += other.d_x; d_y += other.d_y; return *this; }
        const UVector2& operator-=(const UVector2& other)   { d_x -= other.d_x; d_y -= other.d_y; return *this; }
        const UVector2& operator/=(const UVector2& other)   { d_x /= other.d_x; d_y /= other.d_y; return *this; }
        const UVector2& operator*=(const UVector2& other)   { d_x *= other.d_x; d_y *= other.d_y; return *this; }

        bool operator==(const UVector2& other) const    { return d_x == other.d_x && d_y == other.d_y; }
        bool operator!=(const UVector2& other) const    { return !operator==(other); }

        UDim d_x, d_y;
    };

    class URect
    {
    public:
        URect() {}

        URect(const UVector2& min, const UVector2& max) : d_min(min), d_max(max) {}

        URect(const UDim& left, const UDim& top, const UDim& right, const UDim& bottom)
        {
            d_min.d_x = left;
            d_min.d_y = top;
            d_max.d_x = right;
            d_max.d_y = bottom;
        }

        ~URect() {}

        Rect asAbsolute(const Size& base) const
        {
            return Rect(
                d_min.d_x.asAbsolute(base.d_width),
                d_min.d_y.asAbsolute(base.d_height),
                d_max.d_x.asAbsolute(base.d_width),
                d_max.d_y.asAbsolute(base.d_height)
                );
        }

        Rect asRelative(const Size& base) const
        {
            return Rect(
                d_min.d_x.asRelative(base.d_width),
                d_min.d_y.asRelative(base.d_height),
                d_max.d_x.asRelative(base.d_width),
                d_max.d_y.asRelative(base.d_height)
                );
        }

        const UVector2& getPosition() const     { return d_min; }
        UVector2 getSize() const                { return d_max - d_min; }
        UDim getWidth() const                   { return d_max.d_x - d_min.d_x; }
        UDim getHeight() const                  { return d_max.d_y - d_min.d_y; }

        void setPosition(const UVector2& pos)
        {
            UVector2 sz(d_max - d_min);
            d_min = pos;
            d_max = d_min + sz;
        }

        void setSize(const UVector2& sz)
        {
            d_max = d_min + sz;
        }

        void setWidth(const UDim& w)        { d_max.d_x = d_min.d_x + w; }
        void setHeight(const UDim& h)       { d_max.d_y = d_min.d_y + h; }

        void offset(const UVector2& sz)
        {
            d_min += sz;
            d_max += sz;
        }

        UVector2 d_min, d_max;
    };


    typedef unsigned int argb_t;

    class Colour
    {
    public:
        Colour();
        Colour(const Colour& val);
        Colour(float red, float green, float blue, float alpha = 1.0f);
        Colour(argb_t argb);

        argb_t  getARGB() const
        {
            argb_t d_argb = 0xff000000;
            //if (!d_argbValid)
            {
                d_argb = calculateARGB();
                //d_argbValid = true;
            }

            return d_argb;
        }

        float   getAlpha() const
        {
            return d_alpha;
        }
        float   getRed() const
        {
            return d_red;
        }
        float   getGreen() const
        {
            return d_green;
        }
        float   getBlue() const
        {
            return d_blue;
        }

        float   getHue() const;
        float   getSaturation() const;
        float   getLumination() const;

        void    setARGB(argb_t argb);
        inline void setAlpha(float alpha)
        {
            d_argbValid = false;
            d_alpha = alpha;
        }

        inline void setRed(float red)
        {   
            d_argbValid = false;
            d_red = red;
        }

        inline void setGreen(float green)
        {
            d_argbValid = false;
            d_green = green;
        }

        inline void setBlue(float blue)
        {
            d_argbValid = false;
            d_blue = blue;
        }

        inline void set(float red, float green, float blue, float alpha = 1.0f)
        {
            d_argbValid = false;
            d_alpha = alpha;
            d_red = red;
            d_green = green;
            d_blue = blue;
        }

        inline void setRGB(float red, float green, float blue)
        {
            d_argbValid = false;
            d_red = red;
            d_green = green;
            d_blue = blue;
        }

        inline void setRGB(const Colour& val)
        {
            d_red = val.d_red;
            d_green = val.d_green;
            d_blue = val.d_blue;
            if (d_argbValid)
            {
                d_argbValid = val.d_argbValid;
                //if (d_argbValid)
                //d_argb = (d_argb & 0xFF000000) | (val.d_argb & 0x00FFFFFF);
            }
        }

        void    setHSL(float hue, float saturation, float luminance, float alpha = 1.0f);

        void    invertColour();
        void    invertColourWithAlpha();

        inline Colour& operator=(argb_t val)
        {
            setARGB(val);
            return *this;
        }

        inline Colour& operator=(const Colour& val)
        {
            d_alpha = val.d_alpha;
            d_red   = val.d_red;
            d_green = val.d_green;
            d_blue  = val.d_blue;
            //d_argb  = val.d_argb;
            d_argbValid = val.d_argbValid;

            return *this;
        }

        inline Colour& operator&=(argb_t val)
        {
            setARGB(getARGB() & val);
            return *this;
        }

        inline Colour& operator&=(const Colour& val)
        {
            setARGB(getARGB() & val.getARGB());
            return *this;
        }

        inline Colour& operator|=(argb_t val)
        {
            setARGB(getARGB() | val);
            return *this;
        }

        inline Colour& operator|=(const Colour& val)
        {
            setARGB(getARGB() | val.getARGB());
            return *this;
        }

        inline Colour& operator<<=(int val)
        {
            setARGB(getARGB() << val);
            return *this;
        }

        inline Colour& operator>>=(int val)
        {
            setARGB(getARGB() >> val);
            return *this;
        }

        inline Colour operator+(const Colour& val) const
        {
            return Colour(
                d_red   + val.d_red, 
                d_green + val.d_green, 
                d_blue  + val.d_blue,
                d_alpha + val.d_alpha
                );
        }

        inline Colour operator-(const Colour& val) const
        {
            return Colour(
                d_red   - val.d_red,
                d_green - val.d_green,
                d_blue  - val.d_blue,
                d_alpha - val.d_alpha
                );
        }

        inline Colour operator*(const float val) const
        {       
            return Colour(
                d_red   * val, 
                d_green * val, 
                d_blue  * val,
                d_alpha * val 
                );  
        }

        inline Colour& operator*=(const Colour& val)
        {
            d_red *= val.d_red;
            d_blue *= val.d_blue;
            d_green *= val.d_green;
            d_alpha *= val.d_alpha;

            d_argbValid = false;

            return *this;
        }

        inline bool operator==(const Colour& rhs) const
        {
            return d_red   == rhs.d_red   &&
                d_green == rhs.d_green &&
                d_blue  == rhs.d_blue  &&
                d_alpha == rhs.d_alpha;
        }

        inline bool operator!=(const Colour& rhs) const
        {
            return !(*this == rhs);
        }

        operator argb_t() const
        {
            return getARGB();
        }

    protected:
        argb_t calculateARGB() const;

    protected:
        float d_alpha, d_red, d_green, d_blue;
        mutable bool d_argbValid;
    };

    class ColourRect
    {
    public:
        ColourRect();
        ColourRect(const Colour& col);
        ColourRect(const Colour& top_left, const Colour& top_right, const Colour& bottom_left, const Colour& bottom_right);

        void setAlpha(float alpha);
        void setTopAlpha(float alpha);
        void setBottomAlpha(float alpha);
        void setLeftAlpha(float alpha);
        void setRightAlpha(float alpha);

        void setColours(const Colour& col);

        ColourRect getSubRectangle( float left, float right, float top, float bottom ) const;
        Colour getColourAtPoint( float x, float y ) const;

        bool isMonochromatic() const;

        void modulateAlpha(float alpha);

        ColourRect& operator*=(const ColourRect& other);

        Colour  d_top_left, d_top_right, d_bottom_left, d_bottom_right;
    };

    //////////////////////////////////////////////////////////////////////////
    template <typename T> class Singleton
    {
    protected:
        static T* ms_Singleton;

    public:
        Singleton()
        {
            assert(!ms_Singleton);
            ms_Singleton = static_cast<T*>(this);
        }

        virtual ~Singleton()
        {
            assert( ms_Singleton );
            ms_Singleton = 0;
        }

        static T& getSingleton()
        {
            assert( ms_Singleton );
            return (*ms_Singleton);
        }

        static T* getSingletonPtr()
        {
            return (ms_Singleton);
        }
    };
}

#include "atgUI.h"
#include "atgUIElements.h"

#endif // _ATG_UI_BASE_H_
