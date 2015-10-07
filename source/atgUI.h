#ifndef _ATG_UI_H_
#define _ATG_UI_H_

//////////////////////////////////////////////////////////////////////////
// USing ELGameEngine UI System
//////////////////////////////////////////////////////////////////////////
#include "atgBase.h"
#include "atgRenderer.h"
#include "atgMesh.h"

namespace ELGUI
{
    class EventArgs
    {
    public:
        EventArgs() : handled(false) {}
        virtual ~EventArgs() {}

        // Data Members
        bool    handled;
    };

    class SlotFunctorBase
    {
    public:
        virtual ~SlotFunctorBase() {}
        virtual bool operator()(const EventArgs& args) = 0;
    };

    template<typename T>
    class MemberFunctionSlot : public SlotFunctorBase
    {
    public:
        typedef bool(T::*MemberFunctionType)(const EventArgs&);

        MemberFunctionSlot(MemberFunctionType func, T* obj)
            : d_function(func),
            d_object(obj)
        {

        }

        virtual bool operator() (const EventArgs& args)
        {
            return (d_object->*d_function)(args);
        }

    private:
        MemberFunctionType  d_function;
        T*  d_object;
    };

    class Event
    {
    public:
        Event(const std::string& name);
        virtual ~Event();

        bool subscribe(SlotFunctorBase* slot);
        const std::string& getName() const;
        void operator()(EventArgs& args);

    protected:
        typedef std::vector<SlotFunctorBase*>   SlotContainer;
        SlotContainer   d_slots;

        const std::string   d_name;
    };

    class EventSet
    {
    public:
        EventSet();
        virtual ~EventSet();

        void addEvent(const std::string& name);
        void removeEvent(const std::string& name);
        void removeAllEvents();
        bool isEventPresent(const std::string& name);

        virtual bool subscribeEvent(const std::string& name, SlotFunctorBase* slot);
        virtual void fireEvent(const std::string& name, EventArgs& args);

        bool isMuted() const;
        void setMutedState(bool setting);

    protected:
        Event* getEventObject(const std::string& name, bool autoAdd = false);
        void fireEvent_impl(const std::string& name, EventArgs& args);

    protected:
        typedef std::map<std::string, Event*>   EventMap;
        EventMap    d_events;

        bool    d_muted;
    };

    /*!
    /brief
        Enumeration of mouse buttons
    */
    enum MouseButton
    {
        LeftButton,
        RightButton,
        MiddleButton,
        X1Button,
        X2Button,
        MouseButtonCount,       // Dummy value that is == to the maximum number of mouse buttons supported.
        NoButton                // Value set for no mouse button.  NB: This is not 0, do not assume! 
    };

    /*!
    \brief
        System key flag values
    */
    enum SystemKey
    {
        LeftMouse       = 0x0001,           // The left mouse button.
        RightMouse      = 0x0002,           // The right mouse button.
        Shift           = 0x0004,           // Either shift key.
        Control         = 0x0008,           // Either control key.
        MiddleMouse     = 0x0010,           // The middle mouse button.
        X1Mouse         = 0x0020,           // The first 'extra' mouse button
        X2Mouse         = 0x0040,           // The second 'extra' mouse button.
        Alt             = 0x0080            // Either alt key.
    };

    class Window;
    class WindowEventArgs : public EventArgs
    {
    public:
        WindowEventArgs(Window* wnd) : window(wnd) {}

        Window*         window;
    };

    class MouseEventArgs : public WindowEventArgs
    {
    public:
        MouseEventArgs(Window* wnd) : WindowEventArgs(wnd) {}

        Point           position;
        Vector2         moveDelta;
        MouseButton     button;
        unsigned int    sysKeys;
        float           wheelChange;
        unsigned int    clickCount; 
    };

    class KeyEventArgs : public WindowEventArgs
    {
    public:
        KeyEventArgs(Window* wnd) : WindowEventArgs(wnd) {}

        unsigned int        codepoint;
        Key::Scan           scancode;
        unsigned int        sysKeys;
    };

    class ActivationEventArgs : public WindowEventArgs
    {
    public:
        ActivationEventArgs(Window* wnd) : WindowEventArgs(wnd) {}

        Window* otherWindow;
    };

    class MouseCursor : public Singleton<MouseCursor>
    {
    public:
        MouseCursor();
        virtual ~MouseCursor();

        static MouseCursor& getSingleton();
        static MouseCursor* getSingletonPtr();

        void setPosition(const Point& position);
        void offsetPosition(const Point& offset);
        Point getPosition() const;

        void setSize(const Size& sz);
        Size getSize() const;

    protected:
        Point       d_position;
        Size        d_size;
    };

    class Texture
    {
    public:
        enum PixelFormat
        {
            PF_RGB,
            PF_RGBA
        };

        Texture();
        virtual ~Texture();

        virtual void loadFromFile(const std::string& filename) = 0;
        virtual void loadFromMemory(const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, PixelFormat pixelFormat) = 0;

        virtual unsigned short getWidth() const = 0;
        virtual unsigned short getHeight() const = 0;

        virtual unsigned short getOriginalWidth() const
        {
            return getWidth();
        }

        virtual float getXScale() const
        {
            return 1.0f / static_cast<float>(getOriginalWidth());
        }

        virtual unsigned short getOriginalHeight() const
        {
            return getHeight();
        }

        virtual float getYScale() const
        {
            return 1.0f / static_cast<float>(getOriginalHeight());
        }
    };

    class Renderer
    {
    public:
        Renderer();
        virtual ~Renderer();

        virtual void addQuad(const Rect& dest_rect, const Texture* tex, const Rect& texture_rect, const ColourRect& colours) = 0;
        virtual void clearRenderList() = 0;
        virtual void doRender() = 0;

        virtual Texture* createTexture(const std::string& filename) = 0;
        virtual Texture* createTexture() = 0;
        virtual void destroyTexture(Texture* texture) = 0;

        virtual float getWidth() const = 0;
        virtual float getHeight() const = 0;
        virtual Size getSize() const = 0;
        virtual Rect getRect() const = 0;
    };

    class Imageset;
    class Image
    {
    public:
        Image() {}
        Image(const Imageset* owner, const std::string& name, const Rect& area, const Point& render_offset, float horzScaling = 1.0f, float vertScaling = 1.0f);
        Image(const Image& image);
        virtual ~Image();

        void setArea(Rect area);
        void setOffset(Point render_offset);

        Size getSize() const
        {
            return Size(d_scaledWidth, d_scaledHeight);
        }

        float getWidth() const
        {
            return d_scaledWidth;
        }

        float getHeight() const
        {
            return d_scaledHeight;
        }

        Point getOffsets() const
        {
            return d_scaledOffset;
        }

        float getOffsetX() const
        {
            return d_scaledOffset.d_x;
        }

        float getOffsetY() const
        {
            return d_scaledOffset.d_y;
        }

        void draw(const Rect& dest_rect, const Rect& clip_rect, const ColourRect& colours) const;
        void draw(const Point& position, const Size& size, const Rect& clip_rect, const ColourRect& colours) const
        {
            draw(Rect(position.d_x, position.d_y, position.d_x + size.d_width, position.d_y + size.d_height), clip_rect, colours);
        }

        const std::string& getName() const;
        const std::string& getImagesetName() const;

        const Imageset* getImageset() const
        {
            return d_owner;
        }

        const Rect& getSourceTextureArea() const;

        friend class std::map<std::string, Image>;
        friend struct std::pair<const std::string, Image>;

    private:
        friend class Imageset;

        void setHorzScaling(float factor);
        void setVertScaling(float factor);

    protected:
        const Imageset* d_owner;
        Rect            d_area;
        Point           d_offset;

        // image auto-scaling fields.
        float           d_scaledWidth;
        float           d_scaledHeight;
        Point           d_scaledOffset;
        std::string     d_name;
    };

    class ImagesetManager : public Singleton<ImagesetManager>
    {
    public:
        ImagesetManager();
        virtual ~ImagesetManager();

        static ImagesetManager& getSingleton();
        static ImagesetManager* getSingletonPtr();

        Imageset* createImageset(const std::string& name, Texture* texture);

        void destroyImageset(const std::string& name);
        void destroyImageset(Imageset* imageset);
        void destroyAllImagesets();

        Imageset* getImageset(const std::string& name) const;
        bool isImagesetPresent(const std::string& name) const
        {
            return d_imagesets.find(name.c_str()) != d_imagesets.end();
        }

        void notifyScreenResolution(const Size& size);

    protected:
        typedef std::map<std::string, Imageset*> ImagesetRegistry;
        ImagesetRegistry    d_imagesets;
    };

    class Imageset
    {
    private:
        typedef std::map<std::string, Image> ImageRegistry;

        friend Imageset* ImagesetManager::createImageset(const std::string& name, Texture* texture);
        friend void ImagesetManager::destroyImageset(const std::string& name);

        Imageset(const std::string& name, Texture* texture);

    public:
        virtual ~Imageset();

    public:
        Texture* getTexture() const
        {
            return d_texture;
        }

        const std::string& getName() const
        {
            return d_name;
        }

        unsigned int getImageCount() const
        {
            return (unsigned int)d_images.size();
        }

        bool isImageDefined(const std::string& name) const
        {
            return d_images.find(name) != d_images.end();
        }

        Image& getImage(const std::string& name) const;

        void undefineImage(const std::string& name);

        void undefineAllImages();

        Size getImageSize(const std::string& name) const
        {
            return getImage(name).getSize();
        }

        float getImageWidth(const std::string& name) const
        {
            return getImage(name).getWidth();
        }

        float getImageHeight(const std::string& name) const
        {
            return getImage(name).getHeight();
        }

        Point getImageOffset(const std::string& name) const
        {
            return getImage(name).getOffsets();
        }

        float getImageOffsetX(const std::string& name) const
        {
            return getImage(name).getOffsetX();
        }

        float getImageOffsetY(const std::string& name) const
        {
            return getImage(name).getOffsetY();
        }

        void defineImage(const std::string& name, const Point& position, const Size& size, const Point& render_offset)
        {
            defineImage(name, Rect(position.d_x, position.d_y, position.d_x + size.d_width, position.d_y + size.d_height), render_offset);
        }

        void defineImage(const std::string& name, const Rect& image_rect, const Point& render_offset);

        void draw(const Rect& source_rect, const Rect& dest_rect, const Rect& clip_rect, const ColourRect& colours) const;

        bool isAutoScaled() const
        {
            return d_autoScale;
        }

        Size getNativeResolution() const
        {
            return Size(d_nativeHorzRes, d_nativeVertRes);
        }

        void setAutoScalingEnabled(bool setting);

        void setNativeResolution(const Size& size);

        void notifyScreenResolution(const Size& size);

    protected:
        void setTexture(Texture* texture);
        void unload();
        void updateImageScalingFactors();

    protected:
        std::string             d_name;
        ImageRegistry           d_images;
        Texture*                d_texture;
        std::string             d_textureFilename;

        // auto-scaling fields
        bool                    d_autoScale;
        float                   d_horzScaling;
        float                   d_vertScaling;
        float                   d_nativeHorzRes;
        float                   d_nativeVertRes;
    };

    // Enumerated type that contains valid formatting types that can be specified when rendering text into a Rect area (the formatting Rect).
    enum TextFormatting
    {
        LeftAligned,
        RightAligned,
        Centred,
        Justified,
        WordWrapLeftAligned,
        WordWrapRightAligned,
        WordWrapCentred,
        WordWrapJustified
    };

    class FontGlyph
    {
    public:
        FontGlyph() : d_image(0), d_advance_a(0.0f), d_advance_b(0.0f), d_advance_c(0.0f), d_advance(0.0f) {}
        FontGlyph(const Image *image, float advance_a, float advance_b, float advance_c) : d_image(image), d_advance_a(advance_a), d_advance_b(advance_b), d_advance_c(advance_c)
        {
            d_advance = d_advance_a + d_advance_b + d_advance_c;
        }

        const Image* getImage() const
        {
            return d_image;
        }

        const Imageset* getImageset() const
        {
            return d_image ? d_image->getImageset() : 0;
        }

        Size getSize(float x_scale, float y_scale) const
        {
            return Size (getWidth (x_scale), getHeight (y_scale));
        }

        float getWidth(float x_scale) const
        {
            return d_image ? d_image->getWidth () * x_scale : 0.0f;
        }

        float getHeight(float y_scale) const
        {
            return d_image ? d_image->getHeight () * y_scale : 0.0f;
        }

        float getAdvance(float x_scale = 1.0) const
        {
            return d_advance * x_scale;
        }

        float getAdvanceA(float x_scale = 1.0) const
        {
            return d_advance_a * x_scale;
        }

        float getAdvanceB(float x_scale = 1.0) const
        {
            return d_advance_b * x_scale;
        }

        float getAdvanceC(float x_scale = 1.0) const
        {
            return d_advance_c * x_scale;
        }

    protected:
        const Image*    d_image;

        float           d_advance_a;
        float           d_advance_b;
        float           d_advance_c;

        float           d_advance;
    };

    class Font
    {
    public:
        Font() : d_height(0.0f) {}
        virtual ~Font() {}

        virtual void load () = 0;

        virtual const FontGlyph* getGlyphData(wchar_t codepoint);
        virtual void rasterize(wchar_t start_codepoint, wchar_t end_codepoint);
        virtual void rasterize(wchar_t codepoint);

        float getLineSpacing(float y_scale = 1.0f) const;
        size_t getCharAtPixel(const std::wstring& text, float pixel, float x_scale = 1.0f);
        size_t getCharAtPixel(const std::wstring& text, size_t start_char, float pixel, float x_scale = 1.0f);
        float getWrappedTextExtent(const std::wstring& text, float wrapWidth, float x_scale = 1.0f);
        float getTextExtent(const std::wstring& text, float x_scale);
        size_t getFormattedLineCount(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale = 1.0f);
        float getFormattedTextExtent(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale = 1.0f);

        size_t getNextWord(const std::wstring& in_string, size_t start_idx, std::wstring& out_string) const;

        size_t drawText(const std::wstring& text, const Rect& draw_area, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

        void drawTextLine(const std::wstring& text, const Vector2& position, const Rect& clip_rect, const ColourRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);
        void drawTextLineJustified(const std::wstring& text, const Rect& draw_area, const Vector2& position, const Rect& clip_rect, const ColourRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);
        size_t drawWrappedText(const std::wstring& text, const Rect& draw_area, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

    protected:
        typedef std::map<wchar_t, FontGlyph> CodepointMap;
        CodepointMap    d_cp_map;

        float           d_height;
    };

    class FontManager : public Singleton<FontManager>
    {
    public:
        FontManager();
        virtual ~FontManager();

        static FontManager& getSingleton();
        static FontManager* getSingletonPtr();

        void registFont(const std::string& name, Font* font);

        void destroyAllFonts();
        void destroyFont(const std::string& name);

        bool isFontPresent(const std::string& name) const;
        Font* getFont(const std::string& name) const;

    protected:
        typedef std::map<std::string, Font*> FontRegistry;
        FontRegistry    d_fonts;
    };

    class SimpleTimer
    {
    public:
        static double currentTime();

        SimpleTimer() : d_baseTime(currentTime()) {}

        void restart() { d_baseTime = currentTime(); }
        double elapsed() { return currentTime() - d_baseTime; }

    public:
        double  d_baseTime;
    };

    class Window;
    struct MouseClickTracker
    {
        MouseClickTracker(void) : d_click_count(0), d_click_area(0, 0, 0, 0) {}

        SimpleTimer     d_timer;            // Timer used to track clicks for this button.
        int             d_click_count;      //  count of clicks made so far.
        Rect            d_click_area;       // area used to detect multi-clicks
        Window*         d_target_window;    // target window for any events generated.
    };

    struct MouseClickTrackerImpl
    {
        MouseClickTracker   click_trackers[MouseButtonCount];
    };

#define ELGUI_DECLARE_WINDOW_FACTORY(T) \
    class T ## Factory : public WindowFactory \
    { \
    public: \
    T ## Factory(const std::string& type) : WindowFactory(type) {} \
    Window* createWindow(const std::string& name) \
    { \
    return new T(d_type, name); \
    } \
    void destroyWindow(Window* window) \
    { \
    delete window; \
    } \
    };

    #define ELGUI_CREATE_WINDOW_FACTORY(T) new T ## Factory(#T)

    class WindowFactory
    {
    public:
        WindowFactory(const std::string& type) : d_type(type) {}
        virtual ~WindowFactory() {}

        virtual Window* createWindow(const std::string& name) = 0;
        virtual void destroyWindow(Window* window) = 0;

        const std::string& getTypeName() const { return d_type; }

    protected:
        std::string     d_type;
    };

    class WindowFactoryManager : public Singleton<WindowFactoryManager>
    {
    public:
        WindowFactoryManager();
        virtual ~WindowFactoryManager();

        static WindowFactoryManager& getSingleton();
        static WindowFactoryManager* getSingletonPtr();

        void addFactory(WindowFactory* factory);
        WindowFactory* getFactory(const std::string& type) const;
        void destroyAllFactory();

    protected:
        typedef std::map<std::string, WindowFactory*> WindowFactoryRegistry;
        WindowFactoryRegistry   d_factoryRegistry;
    };

    class WindowManager : public Singleton<WindowManager>
    {
    public:
        WindowManager();
        virtual ~WindowManager();

        static WindowManager& getSingleton();
        static WindowManager* getSingletonPtr();

        Window* createWindow(const std::string type, const std::string& name = "");

        void destroyWindow(Window* window);
        void destroyWindow(const std::string& window);
        void destroyAllWindows();

        Window* getWindow(const std::string& name) const;
        bool isWindowPresent(const std::string& name) const;

        std::string generateUniqueWindowName();

    protected:
        typedef std::map<std::string, Window*> WindowRegistry;
        WindowRegistry          d_windowRegistry;

        unsigned long           d_uid_counter;
    };

    class System : public Singleton<System>
    {
    public:
        System(Renderer* renderer);
        virtual ~System();

        Window* setGUISheet(Window* sheet);
        Window* getGUISheet() const;

        void renderGUI();

        bool injectMouseMove(float delta_x, float delta_y);
        bool injectMousePosition(float x_pos, float y_pos);
        bool injectMouseButtonDown(MouseButton button);
        bool injectMouseButtonUp(MouseButton button);
        bool injectMouseWheelChange(float delta);
        bool injectMouseSize(float width, float hight);

        bool injectKeyDown(unsigned int key_code);
        bool injectKeyUp(unsigned int key_code);
        bool injectChar(unsigned int code_point);

        SystemKey mouseButtonToSyskey(MouseButton btn) const;
        SystemKey keyCodeToSyskey(Key::Scan key, bool direction);
        unsigned int getSystemKeys() const;

        void notifyWindowDestroyed(const Window* window);

        void setModalTarget(Window* target);

        Window* getWindowContainingMouse() const;
        Window* getModalTarget(void) const;
        Window* getFocusWindow( void );
        Window* getTargetWindow(const Point& pt) const;
        Window* getKeyboardTargetWindow() const;

        Renderer* getRenderer() const;
        bool handleDisplaySizeChange();

        void signalRedraw();

        static System& getSingleton();
        static System* getSingletonPtr();

    protected:
        void createSingletons();
        void destroySingletons();

        void addWindowFactories();

        Window* getNextTargetWindow(Window* w) const;

    protected:
        Renderer*       d_renderer;
        Window*         d_wndWithMouse;
        Window*         d_activeSheet;
        Window*         d_modalTarget;

        bool            d_gui_redraw;

        unsigned int    d_sysKeys;              // Current set of system keys pressed (in mk1 these were passed in, here we track these ourself).
        bool            d_lshift;               // Tracks state of left shift.
        bool            d_rshift;               // Tracks state of right shift.
        bool            d_lctrl;                // Tracks state of left control.
        bool            d_rctrl;                // Tracks state of right control.
        bool            d_lalt;                 // Tracks state of left alt.
        bool            d_ralt;                 // Tracks state of right alt.

        double          d_click_timeout;        // Timeout value, in seconds, used to generate a single-click (button down then up)
        double          d_dblclick_timeout;     // Timeout value, in seconds, used to generate multi-click events (botton down, then up, then down, and so on).
        Size            d_dblclick_size;        // Size of area the mouse can move and still make multi-clicks.

        MouseClickTrackerImpl* const    d_clickTrackerPimpl;

        float           d_mouseScalingFactor;   // Scaling applied to mouse movement inputs.

    public:
        static const double     DefaultSingleClickTimeout;      // Default timeout for generation of single click events.
        static const double     DefaultMultiClickTimeout;       // Default timeout for generation of multi-click events.
        static const Size       DefaultMultiClickAreaSize;      // Default allowable mouse movement for multi-click event generation.
    };

    class Helper
    {
    public:
        static const std::wstring DefaultWhitespace;
        static const std::wstring DefaultAlphanumerical;
        static const std::wstring DefaultWrapDelimiters;

        static std::wstring getNextWord(const std::wstring& str, std::wstring::size_type start_idx = 0, const std::wstring& delimiters = DefaultWhitespace);

        static const Image* stringToImage(const std::string& str);
    };
}

class atgTexture;
class atgUITexture : public ELGUI::Texture
{
public:
    atgUITexture();
    virtual ~atgUITexture();

    virtual void loadFromFile(const std::string& filename);
    virtual void loadFromMemory(const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, ELGUI::Texture::PixelFormat pixelFormat);
   
    virtual unsigned short getWidth() const;
    virtual unsigned short getHeight() const;

    virtual void setTexture(atgTexture* tex);
    virtual atgTexture* getTexture() const;
protected:
    atgTexture* _texture;
};


class atgUIPass : public atgPass
{
public:
    ~atgUIPass();
    static atgPass*         Create(const char* VSFilePath, const char* FSFilePath);
protected:
    atgUIPass();
    virtual void            BeginContext(void* data);
    virtual void            EndContext(void* data);

    BlendFunction _oldSrcBlend;
    BlendFunction _oldDestBlend;
};

class atgQuadMesh : public atgMesh
{
public:
    atgQuadMesh();
    virtual ~atgQuadMesh();

    void Setup(const ELGUI::Rect& dest_rect, const ELGUI::Texture* tex, const ELGUI::Rect& texture_rect, const ELGUI::ColourRect& colours);

    virtual uint32          GetNumberOfVertexs() const;
    virtual void*           GetVertexs();
    virtual atgVertexDecl*  GetVertexDeclaration() const;

    virtual uint32          GetNumberOfIndices() const;
    virtual void*           GetIndices();
    virtual uint32          GetIndexFormat() const;

    virtual void            Render();
protected:

    atgVertexDecl* _VertexDecl;

    enum PositionComponent { x, y, z };
    enum ColorComponent { r, g, b, a };
    enum TextureCoord { u, v };

    typedef struct VertexData_t 
    {
        float position[3];
        float color[3];
        float texCoord[2];
    }VertexData;

    static const uint32 NumberOfVertexs = 4;
    static const uint32 NumberOfIndices = 6;
    static const atgIndexBuffer::IndexFormat IndexFormat = atgIndexBuffer::IFB_Index16;

    VertexData _vertexs[NumberOfVertexs];
    unsigned short _indices[NumberOfIndices];
};

class atgUIRenderer : public ELGUI::Renderer
{
public:
    atgUIRenderer(atgGame* game);
    virtual ~atgUIRenderer();

    virtual void addQuad(const ELGUI::Rect& dest_rect, const ELGUI::Texture* tex, const ELGUI::Rect& texture_rect, const ELGUI::ColourRect& colours);
    virtual void clearRenderList();
    virtual void doRender();

    virtual ELGUI::Texture* createTexture(const std::string& filename);
    virtual ELGUI::Texture* createTexture();
    virtual void destroyTexture(ELGUI::Texture* texture);

    virtual float getWidth() const;
    virtual float getHeight() const;
    virtual ELGUI::Size getSize() const;
    virtual ELGUI::Rect getRect() const;

    ACCESSOR(atgPass*, UIMaterialPass);

protected:
    typedef std::vector<atgQuadMesh*> DrawQuadList;

    atgGame*                _game;
    DrawQuadList            _quads;
    uint32                  _drawQuadCount;
};

#endif // _ATG_UI_H_