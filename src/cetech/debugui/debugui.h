#ifndef CETECH_DEBUGUI_H
#define CETECH_DEBUGUI_H

#include <cetech/macros.h>
#include <cstdarg>
#include <bgfx/bgfx.h>

#ifdef __cplusplus

extern "C" {
#endif

#include <stddef.h>

struct ct_texture;

typedef float _vec4[4];
typedef float _vec2[2];

struct ImGuiTextEditCallbackData;
struct ImGuiSizeConstraintCallbackData;

typedef unsigned int ImU32;
typedef unsigned int ImGuiID;
typedef unsigned short ImWchar;
typedef void *ImTextureID;
typedef int ImGuiColorEditMode;
typedef int ImGuiWindowFlags;
typedef int ImGuiSetCond;
typedef int ImGuiInputTextFlags;
typedef int ImGuiSelectableFlags;
typedef int ImGuiTreeNodeFlags;
typedef int (*ImGuiTextEditCallback)(ImGuiTextEditCallbackData *data);
typedef void (*ImGuiSizeConstraintCallback)(ImGuiSizeConstraintCallbackData *data);

enum DebugUIWindowFlags_ {
    DebugUIWindowFlags_Empty = 0,
    DebugUIWindowFlags_NoTitleBar = 1 << 0,   //!< Disable title-bar
    DebugUIWindowFlags_NoResize =
    1 << 1,   //!< Disable user resizing with the lower-right grip
    DebugUIWindowFlags_NoMove = 1 << 2,   //!< Disable user moving the window
    DebugUIWindowFlags_NoScrollbar = 1
            << 3,   //!< Disable scrollbars (window can still scroll with mouse or programatically)
    DebugUIWindowFlags_NoScrollWithMouse =
    1 << 4,   //!< Disable user vertically scrolling with mouse wheel
    DebugUIWindowFlags_NoCollapse =
    1 << 5,   //!< Disable user collapsing window by double-clicking on it
    DebugUIWindowFlags_AlwaysAutoResize =
    1 << 6,   //!< Resize every window to its content every frame
    DebugUIWindowFlags_ShowBorders =
    1 << 7,   //!< Show borders around windows and items
    DebugUIWindowFlags_NoSavedSettings =
    1 << 8,   //!< Never load/save settings in .ini file
    DebugUIWindowFlags_NoInputs =
    1 << 9,   //!< Disable catching mouse or keyboard inputs
    DebugUIWindowFlags_MenuBar = 1 << 10,  //!< Has a menu-bar
    DebugUIWindowFlags_HorizontalScrollbar = 1
            << 11,  //!< Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
    DebugUIWindowFlags_NoFocusOnAppearing = 1
            << 12,  //!< Disable taking focus when transitioning from hidden to visible state
    DebugUIWindowFlags_NoBringToFrontOnFocus = 1
            << 13,  //!< Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
    DebugUIWindowFlags_AlwaysVerticalScrollbar = 1
            << 14,  //!< Always show vertical scrollbar (even if ContentSize.y < Size.y)
    DebugUIWindowFlags_AlwaysHorizontalScrollbar = 1
            << 15,  //!< Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    DebugUIWindowFlags_AlwaysUseWindowPadding = 1
            << 16,  //!< Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)

    DebugUIWindowFlags_ChildWindow =
    1 << 20,  //!< Don't use! For internal use by BeginChild()
    DebugUIWindowFlags_ChildWindowAutoFitX =
    1 << 21,  //!< Don't use! For internal use by BeginChild()
    DebugUIWindowFlags_ChildWindowAutoFitY =
    1 << 22,  //!< Don't use! For internal use by BeginChild()
    DebugUIWindowFlags_ComboBox =
    1 << 23,  //!< Don't use! For internal use by ComboBox()
    DebugUIWindowFlags_Tooltip =
    1 << 24,  //!< Don't use! For internal use by BeginTooltip()
    DebugUIWindowFlags_Popup =
    1 << 25,  //!< Don't use! For internal use by BeginPopup()
    DebugUIWindowFlags_Modal =
    1 << 26,  //!< Don't use! For internal use by BeginPopupModal()
    DebugUIWindowFlags_ChildMenu =
    1 << 27   //!< Don't use! For internal use by BeginMenu()
};

//!< Flags for ImGui::InputText()
enum DebugInputTextFlags_ {
    DebugInputTextFlags_CharsDecimal = 1 << 0,   //!< Allow 0123456789.+-*/
    DebugInputTextFlags_CharsHexadecimal =
    1 << 1,   //!< Allow 0123456789ABCDEFabcdef
    DebugInputTextFlags_CharsUppercase = 1 << 2,   //!< Turn a..z into A..Z
    DebugInputTextFlags_CharsNoBlank = 1 << 3,   //!< Filter out spaces, tabs
    DebugInputTextFlags_AutoSelectAll =
    1 << 4,   //!< Select entire text when first taking mouse focus
    DebugInputTextFlags_EnterReturnsTrue = 1
            << 5,   //!< Return 'true' when Enter is pressed (as opposed to when the value was modified)
    DebugInputTextFlags_CallbackCompletion =
    1 << 6,   //!< Call user function on pressing TAB (for completion handling)
    DebugInputTextFlags_CallbackHistory = 1
            << 7,   //!< Call user function on pressing Up/Down arrows (for history handling)
    DebugInputTextFlags_CallbackAlways = 1
            << 8,   //!< Call user function every time. User code may query cursor position, modify text buffer.
    DebugInputTextFlags_CallbackCharFilter = 1
            << 9,   //!< Call user function to filter character. Modify data->EventChar to replace/filter input, or return 1 to discard character.
    DebugInputTextFlags_AllowTabInput =
    1 << 10,  //!< Pressing TAB input a '\t' character into the text field
    DebugInputTextFlags_CtrlEnterForNewLine = 1
            << 11,  //!< In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
    DebugInputTextFlags_NoHorizontalScroll =
    1 << 12,  //!< Disable following the cursor horizontally
    DebugInputTextFlags_AlwaysInsertMode = 1 << 13,  //!< Insert mode
    DebugInputTextFlags_ReadOnly = 1 << 14,  //!< Read-only mode
    DebugInputTextFlags_Password =
    1 << 15,  //!< Password mode, display all characters as '*'

    DebugInputTextFlags_Multiline =
    1 << 20   //!< For internal use by InputTextMultiline()
};

//!< Flags for ImGui::TreeNodeEx(), ImGui::CollapsingHeader*()
enum DebugUITreeNodeFlags_ {
    DebugUITreeNodeFlags_Selected = 1 << 0,   //!< Draw as selected
    DebugUITreeNodeFlags_Framed =
    1 << 1,   //!< Full colored frame (e.g. for CollapsingHeader)
    DebugUITreeNodeFlags_AllowOverlapMode =
    1 << 2,   //!< Hit testing to allow subsequent widgets to overlap this one
    DebugUITreeNodeFlags_NoTreePushOnOpen = 1
            << 3,   //!< Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
    DebugUITreeNodeFlags_NoAutoOpenOnLog = 1
            << 4,   //!< Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
    DebugUITreeNodeFlags_DefaultOpen = 1 << 5,   //!< Default node to be open
    DebugUITreeNodeFlags_OpenOnDoubleClick =
    1 << 6,   //!< Need double-click to open node
    DebugUITreeNodeFlags_OpenOnArrow = 1
            << 7,   //!< Only open when clicking on the arrow part. If DebugUITreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
    DebugUITreeNodeFlags_Leaf = 1
            << 8,   //!< No collapsing, no arrow (use as a convenience for leaf nodes).
    DebugUITreeNodeFlags_Bullet =
    1 << 9,   //!< Display a bullet instead of arrow
    DebugUITreeNodeFlags_CollapsingHeader =
    DebugUITreeNodeFlags_Framed | DebugUITreeNodeFlags_NoAutoOpenOnLog
};

//!< Flags for ImGui::Selectable()
enum DebugUISelectableFlags_ {

    DebugUISelectableFlags_DontClosePopups =
    1 << 0,   //!< Clicking this don't close parent popup window
    DebugUISelectableFlags_SpanAllColumns = 1
            << 1,   //!< Selectable frame can span all columns (text will still fit in current column)
    DebugUISelectableFlags_AllowDoubleClick =
    1 << 2    //!< Generate press events on double clicks too
};

//!< Enumeration for ColorEditMode()
//!< FIXME-OBSOLETE: Will be replaced by future color/picker api
enum DebugUIColorEditMode_ {
    DebugUIColorEditMode_UserSelect = -2,
    DebugUIColorEditMode_UserSelectShowButton = -1,
    DebugUIColorEditMode_RGB = 0,
    DebugUIColorEditMode_HSV = 1,
    DebugUIColorEditMode_HEX = 2
};


//!< Condition flags for ImGui::SetWindow***(), SetNextWindow***(), SetNextTreeNode***() functions
//!< All those functions treat 0 as a shortcut to DebugUISetCond_Always
enum DebugUISetCond_ {
    DebugUISetCond_Always = 1 << 0, //!< Set the variable
    DebugUISetCond_Once = 1
            << 1, //!< Set the variable once per runtime session (only the first call with succeed)
    DebugUISetCond_FirstUseEver = 1
            << 2, //!< Set the variable if the window has no saved data (if doesn't exist in the .ini file)
    DebugUISetCond_Appearing = 1
            << 3  //!< Set the variable if the window is appearing after being hidden/inactive (or the first time)
};

//!<! DebugUI API V0
struct ct_debugui_a0 {
    void (*render)(uint8_t viewid);
    void (*register_on_gui)(void (*on_gui)());
    void (*unregister_on_gui)(void (*on_gui)());

    void (*Text)(const char *fmt,
                 ...) CTECH_ATTR_FORMAT(1, 2);

    void (*TextV)(const char *fmt,
                  va_list args);

    void (*TextColored)(const _vec4 col,
                        const char *fmt,
                        ...) CTECH_ATTR_FORMAT(2, 3);

    void (*TextColoredV)(const _vec4 col,
                         const char *fmt,
                         va_list args);

    void (*TextDisabled)(const char *fmt,
                         ...) CTECH_ATTR_FORMAT(1, 2);

    void (*TextDisabledV)(const char *fmt,
                          va_list args);

    void (*TextWrapped)(const char *fmt,
                        ...) CTECH_ATTR_FORMAT(1, 2);

    void (*TextWrappedV)(const char *fmt,
                         va_list args);

    void (*TextUnformatted)(const char *text,
                            const char *text_end);

    void (*LabelText)(const char *label,
                      const char *fmt,
                      ...) CTECH_ATTR_FORMAT(2, 3);

    void (*LabelTextV)(const char *label,
                       const char *fmt,
                       va_list args);

    void (*Bullet)();

    void (*BulletText)(const char *fmt,
                       ...) CTECH_ATTR_FORMAT(1, 2);

    void (*BulletTextV)(const char *fmt,
                        va_list args);

    bool (*Button)(const char *label,
                   const _vec2 size);

    bool (*SmallButton)(const char *label);

    bool (*InvisibleButton)(const char *str_id,
                            const _vec2 size);

    void (*Image2)(struct ct_texture user_texture_id,
                   const _vec2 size,
                   const _vec2 uv0,
                   const _vec2 uv1,
                   const _vec4 tint_col,
                   const _vec4 border_col);

    bool (*ImageButton)(ImTextureID user_texture_id,
                        const _vec2 size,
                        const _vec2 uv0,
                        const _vec2 uv1,
                        int frame_padding,
                        const _vec4 bg_col,
                        const _vec4 tint_col);

    bool (*Checkbox)(const char *label,
                     bool *v);

    bool (*CheckboxFlags)(const char *label,
                          unsigned int *flags,
                          unsigned int flags_value);

    bool (*RadioButton)(const char *label,
                        bool active);

    bool (*RadioButton2)(const char *label,
                         int *v,
                         int v_button);

    bool (*Combo)(const char *label,
                  int *current_item,
                  const char *const *items,
                  int items_count,
                  int height_in_items);

    bool (*Combo2)(const char *label,
                   int *current_item,
                   const char *items_separated_by_zeros,
                   int height_in_items);

    bool (*Combo3)(const char *label,
                   int *current_item,
                   bool (*items_getter)(void *data,
                                        int idx,
                                        const char **out_text),
                   void *data,
                   int items_count,
                   int height_in_items);

    bool (*ColorButton)(const _vec4 col,
                        bool small_height,
                        bool outline_border);

    bool (*ColorEdit3)(const char *label,
                       float col[3]);

    bool (*ColorEdit4)(const char *label,
                       uint32_t* rgba,
                       bool show_alpha);

    void (*ColorEditMode)(ImGuiColorEditMode mode);

    void (*PlotLines)(const char *label,
                      const float *values,
                      int values_count,
                      int values_offset,
                      const char *overlay_text,
                      float scale_min,
                      float scale_max,
                      _vec2 graph_size,
                      int stride);

    void (*PlotLines2)(const char *label,
                       float (*values_getter)(void *data,
                                              int idx),
                       void *data,
                       int values_count,
                       int values_offset,
                       const char *overlay_text,
                       float scale_min,
                       float scale_max,
                       _vec2 graph_size);

    void (*PlotHistogram)(const char *label,
                          const float *values,
                          int values_count,
                          int values_offset,
                          const char *overlay_text,
                          float scale_min,
                          float scale_max,
                          _vec2 graph_size,
                          int stride);

    void (*PlotHistogram2)(const char *label,
                           float (*values_getter)(void *data,
                                                  int idx),
                           void *data,
                           int values_count,
                           int values_offset,
                           const char *overlay_text,
                           float scale_min,
                           float scale_max,
                           _vec2 graph_size);

    void (*ProgressBar)(float fraction,
                        const _vec2 size_arg,
                        const char *overlay);

    bool (*DragFloat)(const char *label,
                      float *v,
                      float v_speed,
                      float v_min,
                      float v_max,
                      const char *display_format,
                      float power);

    bool (*DragFloat2)(const char *label,
                       float v[2],
                       float v_speed,
                       float v_min,
                       float v_max,
                       const char *display_format,
                       float power);

    bool (*DragFloat3)(const char *label,
                       float v[3],
                       float v_speed,
                       float v_min,
                       float v_max,
                       const char *display_format,
                       float power);

    bool (*DragFloat4)(const char *label,
                       float v[4],
                       float v_speed,
                       float v_min,
                       float v_max,
                       const char *display_format,
                       float power);

    bool (*DragFloatRange2)(const char *label,
                            float *v_current_min,
                            float *v_current_max,
                            float v_speed,
                            float v_min,
                            float v_max,
                            const char *display_format,
                            const char *display_format_max,
                            float power);

    bool (*DragInt)(const char *label,
                    int *v,
                    float v_speed,
                    int v_min,
                    int v_max,
                    const char *display_format);

    bool (*DragInt2)(const char *label,
                     int v[2],
                     float v_speed,
                     int v_min,
                     int v_max,
                     const char *display_format);

    bool (*DragInt3)(const char *label,
                     int v[3],
                     float v_speed,
                     int v_min,
                     int v_max,
                     const char *display_format);

    bool (*DragInt4)(const char *label,
                     int v[4],
                     float v_speed,
                     int v_min,
                     int v_max,
                     const char *display_format);

    bool (*DragIntRange2)(const char *label,
                          int *v_current_min,
                          int *v_current_max,
                          float v_speed,
                          int v_min,
                          int v_max,
                          const char *display_format,
                          const char *display_format_max);

    bool (*InputText)(const char *label,
                      char *buf,
                      size_t buf_size,
                      ImGuiInputTextFlags flags,
                      ImGuiTextEditCallback callback,
                      void *user_data);

    bool (*InputTextMultiline)(const char *label,
                               char *buf,
                               size_t buf_size,
                               const _vec2 size,
                               ImGuiInputTextFlags flags,
                               ImGuiTextEditCallback callback,
                               void *user_data);

    bool (*InputFloat)(const char *label,
                       float *v,
                       float step,
                       float step_fast,
                       int decimal_precision,
                       ImGuiInputTextFlags extra_flags);

    bool (*InputFloat2)(const char *label,
                        float v[2],
                        int decimal_precision,
                        ImGuiInputTextFlags extra_flags);

    bool (*InputFloat3)(const char *label,
                        float v[3],
                        int decimal_precision,
                        ImGuiInputTextFlags extra_flags);

    bool (*InputFloat4)(const char *label,
                        float v[4],
                        int decimal_precision,
                        ImGuiInputTextFlags extra_flags);

    bool (*InputInt)(const char *label,
                     int *v,
                     int step,
                     int step_fast00,
                     ImGuiInputTextFlags extra_flags);

    bool (*InputInt2)(const char *label,
                      int v[2],
                      ImGuiInputTextFlags extra_flags);

    bool (*InputInt3)(const char *label,
                      int v[3],
                      ImGuiInputTextFlags extra_flags);

    bool (*InputInt4)(const char *label,
                      int v[4],
                      ImGuiInputTextFlags extra_flags);

    bool (*SliderFloat)(const char *label,
                        float *v,
                        float v_min,
                        float v_max,
                        const char *display_format,
                        float power);

    bool (*SliderFloat2)(const char *label,
                         float v[2],
                         float v_min,
                         float v_max,
                         const char *display_format,
                         float power);

    bool (*SliderFloat3)(const char *label,
                         float v[3],
                         float v_min,
                         float v_max,
                         const char *display_format,
                         float power);

    bool (*SliderFloat4)(const char *label,
                         float v[4],
                         float v_min,
                         float v_max,
                         const char *display_format,
                         float power);

    bool (*SliderAngle)(const char *label,
                        float *v_rad,
                        float v_degrees_min,
                        float v_degrees_max);

    bool (*SliderInt)(const char *label,
                      int *v,
                      int v_min,
                      int v_max,
                      const char *display_format);

    bool (*SliderInt2)(const char *label,
                       int v[2],
                       int v_min,
                       int v_max,
                       const char *display_format);

    bool (*SliderInt3)(const char *label,
                       int v[3],
                       int v_min,
                       int v_max,
                       const char *display_format);

    bool (*SliderInt4)(const char *label,
                       int v[4],
                       int v_min,
                       int v_max,
                       const char *display_format);

    bool (*VSliderFloat)(const char *label,
                         const _vec2 size,
                         float *v,
                         float v_min,
                         float v_max,
                         const char *display_format,
                         float power);

    bool (*VSliderInt)(const char *label,
                       const _vec2 size,
                       int *v,
                       int v_min,
                       int v_max,
                       const char *display_format);

    bool (*TreeNode)(const char *label);

    bool (*TreeNode2)(const char *str_id,
                      const char *fmt,
                      ...) CTECH_ATTR_FORMAT(2, 3);

    bool (*TreeNode3)(const void *ptr_id,
                      const char *fmt,
                      ...) CTECH_ATTR_FORMAT(2, 3);

    bool (*TreeNodeV)(const char *str_id,
                      const char *fmt,
                      va_list args);

    bool (*TreeNodeV2)(const void *ptr_id,
                       const char *fmt,
                       va_list args);

    bool (*TreeNodeEx)(const char *label,
                       ImGuiTreeNodeFlags flags);

    bool (*TreeNodeEx2)(const char *str_id,
                        ImGuiTreeNodeFlags flags,
                        const char *fmt,
                        ...) CTECH_ATTR_FORMAT(3, 4);

    bool (*TreeNodeEx3)(const void *ptr_id,
                        ImGuiTreeNodeFlags flags,
                        const char *fmt,
                        ...) CTECH_ATTR_FORMAT(3, 4);

    bool (*TreeNodeExV)(const char *str_id,
                        ImGuiTreeNodeFlags flags,
                        const char *fmt,
                        va_list args);

    bool (*TreeNodeExV2)(const void *ptr_id,
                         ImGuiTreeNodeFlags flags,
                         const char *fmt,
                         va_list args);

    void (*TreePush)(const char *str_id);

    void (*TreePush2)(const void *ptr_id);

    void (*TreePop)();

    void (*TreeAdvanceToLabelPos)();

    float (*GetTreeNodeToLabelSpacing)();

    void (*SetNextTreeNodeOpen)(bool is_open,
                                ImGuiSetCond cond);

    bool (*CollapsingHeader)(const char *label,
                             ImGuiTreeNodeFlags flags);

    bool (*CollapsingHeader2)(const char *label,
                              bool *p_open,
                              ImGuiTreeNodeFlags flags);

    bool (*Selectable)(const char *label,
                       bool selected,
                       ImGuiSelectableFlags flags,
                       const _vec2 size);

    bool (*Selectable2)(const char *label,
                        bool *p_selected,
                        ImGuiSelectableFlags flags,
                        const _vec2 size);

    bool (*ListBox)(const char *label,
                    int *current_item,
                    const char *const *items,
                    int items_count,
                    int height_in_items);

    bool (*ListBox2)(const char *label,
                     int *current_item,
                     bool (*items_getter)(void *data,
                                          int idx,
                                          const char **out_text),
                     void *data,
                     int items_count,
                     int height_in_items);

    bool (*ListBoxHeader)(const char *label,
                          const _vec2 size);

    bool (*ListBoxHeader2)(const char *label,
                           int items_count,
                           int height_in_items);

    void (*ListBoxFooter)();

    void (*Value)(const char *prefix,
                  bool b);

    void (*Value2)(const char *prefix,
                   int v);

    void (*Value3)(const char *prefix,
                   unsigned int v);

    void (*Value4)(const char *prefix,
                   float v,
                   const char *float_format);

    void (*ValueColor)(const char *prefix,
                       const _vec4 v);

    void (*ValueColor2)(const char *prefix,
                        ImU32 v);

    void (*SetTooltip)(const char *fmt,
                       ...) CTECH_ATTR_FORMAT(1, 2);

    void (*SetTooltipV)(const char *fmt,
                        va_list args);

    void (*BeginTooltip)();

    void (*EndTooltip)();

    bool (*BeginMainMenuBar)();

    void (*EndMainMenuBar)();

    bool (*BeginMenuBar)();

    void (*EndMenuBar)();

    bool (*BeginMenu)(const char *label,
                      bool enabled);

    void (*EndMenu)();

    bool (*MenuItem)(const char *label,
                     const char *shortcut,
                     bool selected,
                     bool enabled);

    bool (*MenuItem2)(const char *label,
                      const char *shortcut,
                      bool *p_selected,
                      bool enabled);

    void (*OpenPopup)(const char *str_id);

    bool (*BeginPopup)(const char *str_id);

    bool (*BeginPopupModal)(const char *name,
                            bool *p_open,
                            ImGuiWindowFlags extra_flags);

    bool (*BeginPopupContextItem)(const char *str_id,
                                  int mouse_button);

    bool (*BeginPopupContextWindow)(bool also_over_items,
                                    const char *str_id,
                                    int mouse_button);

    bool (*BeginPopupContextVoid)(const char *str_id,
                                  int mouse_button);

    void (*EndPopup)();

    void (*CloseCurrentPopup)();

    void (*ColorWheel)(const char *text,
                       float *rgba,
                       float size);

    void (*ColorWheel2)(const char *text,
                        uint32_t *rgba,
                        float size);

    bool (*IsWindowFocused)();
    bool (*IsMouseHoveringWindow)();

    void (*GetWindowSize)(float size[2]);

    bool (*BeginDock)(const char* label, bool* opened, DebugUIWindowFlags_ extra_flags);
    void (*EndDock)();
};

#ifdef __cplusplus
}
#endif

#endif //!<CETECH_DEBUGUI_H

//!<! |}