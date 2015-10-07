#include "atgUIBase.h"
#include "atgMath.h"

namespace ELGUI
{
    //////////////////////////////////////////////////////////////////////////
    Rect::Rect(float left, float top, float right, float bottom)
        : d_top(top)
        , d_bottom(bottom)
        , d_left(left)
        , d_right(right)
    {
    }

    Rect::Rect(Point pos, Size sz)
        : d_top(pos.d_y)
        , d_bottom(pos.d_y + sz.d_height)
        , d_left(pos.d_x)
        , d_right(pos.d_x + sz.d_width)
    {
    }

    Rect Rect::getIntersection(const Rect& rect) const
    {
        // check for total exclusion
        if ((d_right > rect.d_left) &&
            (d_left < rect.d_right) &&
            (d_bottom > rect.d_top) &&
            (d_top < rect.d_bottom))
        {
            Rect temp;

            // fill in temp with the intersection
            temp.d_left = (d_left > rect.d_left) ? d_left : rect.d_left;
            temp.d_right = (d_right < rect.d_right) ? d_right : rect.d_right;
            temp.d_top = (d_top > rect.d_top) ? d_top : rect.d_top;
            temp.d_bottom = (d_bottom < rect.d_bottom) ? d_bottom : rect.d_bottom;

            return temp;
        }
        else
        {
            return Rect(0.0f, 0.0f, 0.0f, 0.0f);
        }

    }

    bool Rect::isPointInRect(const Point& pt) const
    {
        if ((d_left > pt.d_x) ||
            (d_right <= pt.d_x) ||
            (d_top > pt.d_y) ||
            (d_bottom <= pt.d_y))
        {
            return false;
        }

        return true;
    }

    Rect& Rect::offset(const Point& pt)
    {
        d_left      += pt.d_x;
        d_right     += pt.d_x;
        d_top       += pt.d_y;
        d_bottom    += pt.d_y;
        return *this;
    }

    void Rect::setPosition(const Point& pt)
    {
        Size sz(getSize());

        d_left = pt.d_x;
        d_top  = pt.d_y;
        setSize(sz);
    }

    Rect& Rect::constrainSizeMax(const Size& sz)
    {
        if (getWidth() > sz.d_width)
        {
            setWidth(sz.d_width);
        }

        if (getHeight() > sz.d_height)
        {
            setHeight(sz.d_height);
        }

        return *this;
    }

    Rect& Rect::constrainSizeMin(const Size& sz)
    {
        if (getWidth() < sz.d_width)
        {
            setWidth(sz.d_width);
        }

        if (getHeight() < sz.d_height)
        {
            setHeight(sz.d_height);
        }

        return *this;
    }

    Rect& Rect::constrainSize(const Size& max_sz, const Size& min_sz)
    {
        Size curr_sz(getSize());

        if (curr_sz.d_width > max_sz.d_width)
        {
            setWidth(max_sz.d_width);
        }
        else if (curr_sz.d_width < min_sz.d_width)
        {
            setWidth(min_sz.d_width);
        }

        if (curr_sz.d_height > max_sz.d_height)
        {
            setHeight(max_sz.d_height);
        }
        else if (curr_sz.d_height < min_sz.d_height)
        {
            setHeight(min_sz.d_height);
        }

        return *this;
    }

    Rect& Rect::operator=(const Rect& rhs)
    {
        d_left = rhs.d_left;
        d_top = rhs.d_top;
        d_right = rhs.d_right;
        d_bottom = rhs.d_bottom;

        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    Colour::Colour()
        : d_alpha(1.0f)
        , d_red(0.0f)
        , d_green(0.0f)
        , d_blue(0.0f)
        //, d_argb(0xFF000000)
        , d_argbValid(true)
    {
    }

    Colour::Colour(const Colour& val)
    {
        this->operator=(val);
    }

    Colour::Colour(float red, float green, float blue, float alpha) :
    d_alpha(alpha),
        d_red(red),
        d_green(green),
        d_blue(blue),
        d_argbValid(false)
    {
    }

    Colour::Colour(argb_t argb)
    {
        setARGB(argb);
    }

    float Colour::getHue() const
    {
        float pRed = d_red;
        float pGreen = d_green;
        float pBlue = d_blue;

        float pMax = Max(Max(d_red, d_green), d_blue);
        float pMin = Min(Min(d_red, d_green), d_blue);

        float pHue;

        if( pMax == pMin )
        {
            pHue = 0;
        }
        else
        {
            if( pMax == pRed )
            {
                pHue = (pGreen - pBlue) / (pMax - pMin);
            }
            else if( pMax == pGreen )
            {
                pHue = 2 + (pBlue - pRed) / (pMax - pMin);
            }
            else
            {
                pHue = 4 + (pRed - pGreen) / (pMax - pMin);
            }
        }

        float Hue = pHue / 6;
        if( Hue < 0 )
            Hue += 1;

        return Hue;
    }

    float Colour::getSaturation() const
    {
        float pMax = Max(Max(d_red, d_green), d_blue);
        float pMin = Min(Min(d_red, d_green), d_blue);

        float pLum = (pMax + pMin) / 2;
        float pSat;

        if( pMax == pMin )
        {
            pSat = 0;
        }
        else
        {
            if( pLum < 0.5 )
                pSat = (pMax - pMin) / (pMax + pMin);
            else
                pSat = (pMax - pMin) / (2 - pMax - pMin);
        }

        return pSat;
    }

    float Colour::getLumination() const
    {
        float pMax = Max(Max(d_red, d_green), d_blue);
        float pMin = Min(Min(d_red, d_green), d_blue);

        float pLum = (pMax + pMin) / 2;
        return pLum;
    }

    void Colour::setARGB(argb_t argb)
    {
        //d_argb = argb;

#ifdef __BIG_ENDIAN__
        d_alpha = static_cast<float>(argb & 0xFF) / 255.0f;
        argb >>= 8;
        d_red   = static_cast<float>(argb & 0xFF) / 255.0f;
        argb >>= 8;
        d_green = static_cast<float>(argb & 0xFF) / 255.0f;
        argb >>= 8;
        d_blue  = static_cast<float>(argb & 0xFF) / 255.0f;
#else
        d_blue  = static_cast<float>(argb & 0xFF) / 255.0f;
        argb >>= 8;
        d_green = static_cast<float>(argb & 0xFF) / 255.0f;
        argb >>= 8;
        d_red   = static_cast<float>(argb & 0xFF) / 255.0f;
        argb >>= 8;
        d_alpha = static_cast<float>(argb & 0xFF) / 255.0f;
#endif

        d_argbValid = true;
    }

    void Colour::setHSL(float hue, float saturation, float luminance, float alpha)
    {
        d_alpha = alpha;

        float temp3[3];

        float pHue = hue;
        float pSat = saturation;
        float pLum = luminance;

        if( pSat == 0 )
        {
            d_red = pLum;
            d_green = pLum;
            d_blue = pLum;
        }
        else
        {
            float temp2;
            if( pLum < 0.5f )
            {
                temp2 = pLum * (1 + pSat);
            }
            else
            {
                temp2 = pLum + pSat - pLum * pSat;
            }

            float temp1 = 2 * pLum - temp2;

            temp3[0] = pHue + (1.0f / 3);
            temp3[1] = pHue;
            temp3[2] = pHue - (1.0f / 3);

            for( int n = 0; n < 3; n ++ )
            {
                if( temp3[n] < 0 )
                    temp3[n] ++;
                if( temp3[n] > 1 )
                    temp3[n] --;

                if( (temp3[n] * 6) < 1 )
                {
                    temp3[n] = temp1 + (temp2 - temp1) * 6 * temp3[n];
                }
                else
                {
                    if( (temp3[n] * 2) < 1 )
                    {
                        temp3[n] = temp2;
                    }
                    else
                    {
                        if( (temp3[n] * 3) < 2 )
                        {
                            temp3[n] = temp1 + (temp2 - temp1) * ((2.0f / 3) - temp3[n]) * 6;
                        }
                        else
                        {
                            temp3[n] = temp1;
                        }
                    }
                }
            }

            d_red = temp3[0];
            d_green = temp3[1];
            d_blue = temp3[2];
        }

        d_argbValid = false;
    }

    argb_t Colour::calculateARGB() const
    {
#ifdef __BIG_ENDIAN__
        return (
            static_cast<argb_t>(d_blue * 255) << 24 |
            static_cast<argb_t>(d_green * 255) << 16 |
            static_cast<argb_t>(d_red * 255) << 8 |
            static_cast<argb_t>(d_alpha * 255)
            );
#else
        return (
            static_cast<argb_t>(d_alpha * 255) << 24 |
            static_cast<argb_t>(d_red * 255) << 16 |
            static_cast<argb_t>(d_green * 255) << 8 |
            static_cast<argb_t>(d_blue * 255)
            );
#endif
    }

    void Colour::invertColour()
    {
        d_red   = 1.0f - d_red;
        d_green = 1.0f - d_green;
        d_blue  = 1.0f - d_blue;
    }

    void Colour::invertColourWithAlpha()
    {
        d_alpha = 1.0f - d_alpha;
        d_red   = 1.0f - d_red;
        d_green = 1.0f - d_green;
        d_blue  = 1.0f - d_blue;
    }

    //////////////////////////////////////////////////////////////////////////
    ColourRect::ColourRect(const Colour& top_left, const Colour& top_right, const Colour& bottom_left, const Colour& bottom_right)
        : d_top_left(top_left)
        , d_top_right(top_right)
        , d_bottom_left(bottom_left)
        , d_bottom_right(bottom_right)
    {
    }

    ColourRect::ColourRect(const Colour& col)
        : d_top_left(col)
        , d_top_right(col)
        , d_bottom_left(col)
        , d_bottom_right(col)
    {
    }

    ColourRect::ColourRect()
        : d_top_left()
        , d_top_right()
        , d_bottom_left()
        , d_bottom_right()
    {
    }

    void ColourRect::setAlpha(float alpha)
    {
        d_top_left.setAlpha(alpha);
        d_top_right.setAlpha(alpha);
        d_bottom_left.setAlpha(alpha);
        d_bottom_right.setAlpha(alpha);
    }

    void ColourRect::setTopAlpha(float alpha)
    {
        d_top_left.setAlpha(alpha);
        d_top_right.setAlpha(alpha);
    }

    void ColourRect::setBottomAlpha(float alpha)
    {
        d_bottom_left.setAlpha(alpha);
        d_bottom_right.setAlpha(alpha);
    }

    void ColourRect::setLeftAlpha(float alpha)
    {
        d_top_left.setAlpha(alpha);
        d_bottom_left.setAlpha(alpha);
    }

    void ColourRect::setRightAlpha(float alpha)
    {
        d_top_right.setAlpha(alpha);
        d_bottom_right.setAlpha(alpha);
    }

    void ColourRect::setColours(const Colour& col)
    {
        d_top_left = d_top_right = d_bottom_left = d_bottom_right = col;
    }

    Colour ColourRect::getColourAtPoint( float x, float y ) const
    {
        Colour h1((d_top_right - d_top_left) * x + d_top_left);
        Colour h2((d_bottom_right - d_bottom_left) * x + d_bottom_left);
        return Colour((h2 - h1) * y + h1);
    }

    ColourRect ColourRect::getSubRectangle( float left, float right, float top, float bottom ) const
    {
        return ColourRect(
            getColourAtPoint(left, top),
            getColourAtPoint(right, top),
            getColourAtPoint(left, bottom),
            getColourAtPoint(right, bottom)
            );
    }

    bool ColourRect::isMonochromatic() const
    {
        return d_top_left == d_top_right &&
            d_top_left == d_bottom_left &&
            d_top_left == d_bottom_right;
    }

    void ColourRect::modulateAlpha(float alpha)
    {
        d_top_left.setAlpha(d_top_left.getAlpha()*alpha);
        d_top_right.setAlpha(d_top_right.getAlpha()*alpha);
        d_bottom_left.setAlpha(d_bottom_left.getAlpha()*alpha);
        d_bottom_right.setAlpha(d_bottom_right.getAlpha()*alpha);
    }

    ColourRect& ColourRect::operator *=(const ColourRect& other)
    {
        d_top_left *= other.d_top_left;
        d_top_right *= other.d_top_right;
        d_bottom_left *= other.d_bottom_left;
        d_bottom_right *= other.d_bottom_right;

        return *this;
    }

}
