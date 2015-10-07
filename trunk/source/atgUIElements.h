#ifndef _ATG_UI_ELEMENTS_H_
#define _ATG_UI_ELEMENTS_H_

//////////////////////////////////////////////////////////////////////////
// USing ELGameEngine UI System
//////////////////////////////////////////////////////////////////////////

namespace ELGUI
{
    class RenderCache
    {
    public:
        bool hasCachedImagery() const;
        void render(const Point& basePos, const Rect& clipper);
        void clearCachedImagery();

        void cacheImage(const Image& image, const Rect& destArea, const ColourRect& cols, const Rect* clipper = 0, bool clipToDisplay = false);
        void cacheText(const std::wstring& text, Font* font, TextFormatting format, const Rect& destArea, const ColourRect& cols, const Rect* clipper = 0, bool clipToDisplay = false);

    protected:
        struct ImageInfo
        {
            const Image* source_image;
            Rect target_area;
            ColourRect colours;
            Rect customClipper;
            bool usingCustomClipper;
            bool clipToDisplay;
        };

        struct TextInfo
        {
            std::wstring text;
            Font* source_font;
            TextFormatting formatting;
            Rect target_area;
            ColourRect colours;
            Rect customClipper;
            bool usingCustomClipper;
            bool clipToDisplay;
        };

        typedef std::vector<ImageInfo>  ImageryList;
        typedef std::vector<TextInfo>   TextList;

        ImageryList d_cachedImages;
        TextList d_cachedTexts;
    };

    class Window : public EventSet
    {
    public:
        static const std::string EventParentSized;
        static const std::string EventSized;
        static const std::string EventMoved;
        static const std::string EventAlphaChanged;
        static const std::string EventActivated;
        static const std::string EventDeactivated;
        static const std::string EventEnabled;
        static const std::string EventDisabled;
        static const std::string EventShown;
        static const std::string EventHidden;
        static const std::string EventInputCaptureGained;
        static const std::string EventInputCaptureLost;
        static const std::string EventChildAdded;
        static const std::string EventChildRemoved;
        static const std::string EventZOrderChanged;
        static const std::string EventMouseEnters;
        static const std::string EventMouseLeaves;
        static const std::string EventMouseMove;
        static const std::string EventMouseWheel;
        static const std::string EventMouseButtonDown;
        static const std::string EventMouseButtonUp;
        static const std::string EventMouseClick;
        static const std::string EventMouseDoubleClick;
        static const std::string EventMouseTripleClick;
        static const std::string EventKeyDown;
        static const std::string EventKeyUp;
        static const std::string EventCharacterKey;

    public:
        Window(const std::string& type, const std::string& name);
        virtual ~Window();

        void destroy();

        virtual void onMoved(WindowEventArgs& e);
        virtual void onSized(WindowEventArgs& e);
        virtual void onParentSized(WindowEventArgs& e);
        virtual void onActivated(ActivationEventArgs& e);
        virtual void onDeactivated(ActivationEventArgs& e);
        virtual void onAlphaChanged(WindowEventArgs& e);
        virtual void onZChanged(WindowEventArgs& e);
        virtual void onChildAdded(WindowEventArgs& e);
        virtual void onChildRemoved(WindowEventArgs& e);
        virtual void onCaptureGained(WindowEventArgs& e);
        virtual void onCaptureLost(WindowEventArgs& e);
        virtual void onEnabled(WindowEventArgs& e);
        virtual void onDisabled(WindowEventArgs& e);
        virtual void onShown(WindowEventArgs& e);
        virtual void onHidden(WindowEventArgs& e);

        virtual void onMouseEnters(MouseEventArgs& e);
        virtual void onMouseLeaves(MouseEventArgs& e);
        virtual void onMouseMove(MouseEventArgs& e);
        virtual void onMouseWheel(MouseEventArgs& e);

        virtual void onMouseButtonDown(MouseEventArgs& e);
        virtual void onMouseButtonUp(MouseEventArgs& e);
        virtual void onMouseClicked(MouseEventArgs& e);
        virtual void onMouseDoubleClicked(MouseEventArgs& e);
        virtual void onMouseTripleClicked(MouseEventArgs& e);

        virtual void onKeyDown(KeyEventArgs& e);
        virtual void onKeyUp(KeyEventArgs& e);
        virtual void onCharacter(KeyEventArgs& e);

        void setArea(const UDim& xpos, const UDim& ypos, const UDim& width, const UDim& height);
        void setArea(const UVector2& pos, const UVector2& size);
        void setArea(const URect& area);
        void setPosition(const UVector2& pos);
        void setXPosition(const UDim& x);
        void setYPosition(const UDim& y);
        void setSize(const UVector2& size);
        void setWidth(const UDim& width);
        void setHeight(const UDim& height);

        const std::string& getName() const;
        const std::string& getType() const;

        Window* getParent() const;
        Window* getTargetChildAtPosition(const Vector2& position) const;

        void setParent(Window* parent);

        Rect getPixelRect() const;
        Rect getInnerRect() const;
        Rect getUnclippedPixelRect() const;
        Rect getUnclippedInnerRect() const;

        const URect& getArea() const;
        Size getPixelSize() const;

        float getParentPixelWidth() const;
        float getParentPixelHeight() const;
        Size getParentPixelSize() const;

        bool isCapturedByThis(void) const;
        static Window* getCaptureWindow();

        void notifyScreenAreaChanged();

        void activate();
        void deactivate();

        bool isVisible(bool localOnly = false) const;
        bool isDisabled(bool localOnly = false) const;
        bool isClippedByParent() const;
        bool isAlwaysOnTop() const;
        bool isMousePassThroughEnabled() const;
        bool isDestroyedByParent() const;
        bool wantsMultiClickEvents() const;
        bool distributesCapturedInputs() const;

        void setMousePassThroughEnabled(bool setting);

        bool inheritsAlpha() const;
        float getAlpha() const;
        float getEffectiveAlpha() const;
        void setAlpha(float alpha);

        void setEnabled(bool setting);
        void setVisible(bool setting);

        void requestRedraw() const;

        bool isActive() const;
        bool isTopOfZOrder() const;
        bool isHit(const Vector2& position) const;

        void addChildWindow(const std::string& name);
        void addChildWindow(Window* window);
        void removeChildWindow(Window* window);

        size_t getChildCount() const;
        size_t getChildRecursiveCount();

        bool isChild(const std::string& name) const;
        bool isChild(const Window* window) const;

        Window* getChild(const std::string& name) const;
        Window* recursiveChildSearch(const std::string& name) const;
        Window* getChildAtIdx(size_t idx) const;

        Window* getActiveChild();
        const Window* getActiveChild() const;
        Window* getActiveSibling();

        bool isAncestor(const Window* window) const;

        void addWindowToDrawList(Window& wnd, bool at_back = false);
        void removeWindowFromDrawList(const Window& wnd);

        bool doRiseOnClick();
        void moveToFront();
        void moveToBack();

        bool captureInput();
        void releaseInput();

        void render();

    protected:
        void setArea_impl(const UVector2& pos, const UVector2& size, bool topLeftSizing = false, bool fireEvents = true);

        Rect getPixelRect_impl() const;
        Rect getUnclippedInnerRect_impl() const;
        Size getSize_impl(const Window* window) const;

        void cleanupChildren();
        void addChild_impl(Window* wnd);
        void removeChild_impl(Window* wnd);

        virtual void onZChange_impl();

        bool moveToFront_impl(bool wasClicked);

        virtual void drawSelf();
        virtual void populateRenderCache();
        RenderCache& getRenderCache();

    protected:
        typedef std::vector<Window*> ChildList;
        // The list of child Window objects attached to this.
        ChildList           d_children;
        // Child window objects arranged in rendering order.
        ChildList           d_drawList;

        const std::string   d_type;
        std::string         d_name;

        static Window*      d_captureWindow;
        Window*             d_oldCapture;

        Window*             d_parent;

        bool                d_visible;
        bool                d_enabled;
        bool                d_clippedByParent;
        bool                d_alwaysOnTop;
        bool                d_mousePassThroughEnabled;
        bool                d_riseOnClick;
        bool                d_destroyedByParent;
        bool                d_wantsMultiClicks;
        bool                d_distCapturedInputs;
        bool                d_inheritsAlpha;
        bool                d_zOrderingEnabled;
        float               d_alpha;
        bool                d_restoreOldCapture;

        bool                d_active;

        URect               d_area;
        Size                d_pixelSize;

        mutable Rect        d_screenUnclippedRect;
        mutable bool        d_screenUnclippedRectValid;
        mutable Rect        d_screenUnclippedInnerRect;
        mutable bool        d_screenUnclippedInnerRectValid;
        mutable Rect        d_screenRect;
        mutable bool        d_screenRectValid;
        mutable Rect        d_screenInnerRect;
        mutable bool        d_screenInnerRectValid;

        RenderCache         d_renderCache;
        mutable bool        d_needsRedraw;
    };

    class DefaultWindow : public Window
    {
    public:
        DefaultWindow(const std::string& type, const std::string& name) : Window(type, name) {}
    };

    ELGUI_DECLARE_WINDOW_FACTORY(DefaultWindow);

    // window with nine imagery sections
    class FrameWindow : public Window
    {
    public:
        enum Section
        {
            TopLeft     = 0,
            TopMiddle,
            TopRight,
            MiddleLeft,
            MiddleMiddle,
            MiddleRight,
            BottomLeft,
            BottomMiddle,
            BottomRight,
            SectionCount
        };

        FrameWindow(const std::string& type, const std::string& name);
        virtual ~FrameWindow();

        void setSectionImage(Section st, const Image* img);

    protected:
        virtual void populateRenderCache();

        virtual void updateSectionSizes();

        float getSectionImageWidth(Section st) const;
        float getSectionImageHeight(Section st) const;

    protected:
        const Image*    d_section_images[SectionCount];

        float           d_width_left;
        float           d_width_middle;
        float           d_width_right;
        float           d_height_top;
        float           d_height_middle;
        float           d_height_bottom;
    };

    ELGUI_DECLARE_WINDOW_FACTORY(FrameWindow);

    class ButtonBase : public Window
    {
    public:
        ButtonBase(const std::string& type, const std::string& name) : d_pushed(false), d_hovering(false), Window(type, name) {}

        bool isHovering() const;
        bool isPushed() const;

        virtual void onMouseMove(MouseEventArgs& e);
        virtual void onMouseButtonDown(MouseEventArgs& e);
        virtual void onMouseButtonUp(MouseEventArgs& e);
        virtual void onCaptureLost(WindowEventArgs& e);
        virtual void onMouseLeaves(MouseEventArgs& e);

        void updateInternalState(const Point& mouse_pos);

    protected:
        bool    d_pushed;
        bool    d_hovering;
    };

    ELGUI_DECLARE_WINDOW_FACTORY(ButtonBase);

    class ImageButton : public ButtonBase
    {
    public:
        ImageButton(const std::string& type, const std::string& name);

        void setNormalImage(const Image* img);
        void setHoverImage(const Image* img);
        void setPushedImage(const Image* img);
        void setDisabledImage(const Image* img);

    protected:
        virtual void populateRenderCache();

    protected:
        const Image* d_image_normal;
        const Image* d_image_hover;
        const Image* d_image_pushed;
        const Image* d_image_disabled;
    };

    ELGUI_DECLARE_WINDOW_FACTORY(ImageButton);

    class StaticImage : public Window
    {
    public:
        StaticImage(const std::string& type, const std::string& name) : d_image (0), Window(type, name) {}

        const Image* getImage() const;
        void setImage(const Image* img);

    protected:
        virtual void populateRenderCache();

    protected:
        const Image* d_image;
    };

    ELGUI_DECLARE_WINDOW_FACTORY(StaticImage);

    class StaticText : public Window
    {
    public:
        StaticText(const std::string& type, const std::string& name);

        const std::wstring& getText() const;
        void setText(const std::wstring& text);

        void setFont(const std::string& name);
        void setFont(Font* font);
        Font* getFont() const;

        void setTextColours(const ColourRect& colours);
        ColourRect getTextColours() const;

        void setTextFormatting(TextFormatting formatting);
        TextFormatting getTextFormatting() const;

    protected:
        virtual void populateRenderCache();

    protected:
        std::wstring    d_text;

        Font*           d_font;
        ColourRect      d_textCols;
        TextFormatting  d_formatting;
    };

    ELGUI_DECLARE_WINDOW_FACTORY(StaticText);

    class Tooltip : public FrameWindow
    {
    public:
        Tooltip(const std::string& type, const std::string& name);
        virtual ~Tooltip();

        const std::wstring& getText() const;
        void setText(const std::wstring& text);

        void setFont(const std::string& name);
        void setFont(Font* font);
        Font* getFont() const;

        void setTextColours(const ColourRect& colours);
        ColourRect getTextColours() const;

        void setTextFormatting(TextFormatting formatting);
        TextFormatting getTextFormatting() const;

        Size getTextSize() const;

        void sizeSelf();
        void positionSelf();

    protected:
        virtual void populateRenderCache();

    protected:
        std::wstring    d_text;

        Font*           d_font;
        ColourRect      d_textCols;
        TextFormatting  d_formatting;

        float           d_maxWidth;
    };

    ELGUI_DECLARE_WINDOW_FACTORY(Tooltip);

    class CoordConverter
    {
    public:
        static float windowToScreenX(const Window& window, const UDim& x);
        static float windowToScreenX(const Window& window, const float x);

        static float windowToScreenY(const Window& window, const UDim& y);
        static float windowToScreenY(const Window& window, const float y);

        static Vector2 windowToScreen(const Window& window, const UVector2& vec);
        static Vector2 windowToScreen(const Window& window, const Vector2& vec);
        static Rect windowToScreen(const Window& window, const URect& rect);
        static Rect windowToScreen(const Window& window, const Rect& rect);

        static float screenToWindowX(const Window& window, const UDim& x);
        static float screenToWindowX(const Window& window, const float x);

        static float screenToWindowY(const Window& window, const UDim& y);
        static float screenToWindowY(const Window& window, const float y);

        static Vector2 screenToWindow(const Window& window, const UVector2& vec);
        static Vector2 screenToWindow(const Window& window, const Vector2& vec);
        static Rect screenToWindow(const Window& window, const URect& rect);
        static Rect screenToWindow(const Window& window, const Rect& rect);

    protected:
        static float getBaseXValue(const Window& window);
        static float getBaseYValue(const Window& window);
        static Vector2 getBaseValue(const Window& window);
    };
}


#endif // _ATG_UI_ELEMENTS_H_