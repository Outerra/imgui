#pragma once

#include "comm/commtypes.h"
#include "comm/str.h"
#include "imgui.h"

static constexpr ImU32 IM_FORCED_COLOR = IM_COL32(255, 130, 40, 255);
static constexpr ImU32 IM_ERROR_COLOR = IM_COL32(231, 76, 60, 255);
static constexpr ImU32 IM_WARN_COLOR = IM_COL32(241, 196, 15, 255);
static constexpr ImU32 IM_INFO_COLOR = IM_COL32(52, 152, 219, 255);
static constexpr ImU32 IM_OK_COLOR = IM_COL32(91, 193, 0, 255);

static const ImU32 IM_OK_BUTTON_COLOR = (ImU32)ImColor::HSV(0.33f, 0.5f, 0.5f);
static const ImU32 IM_OK_BUTTON_COLOR_HOVERED = (ImU32)ImColor::HSV(0.33f, 0.6f, 0.6f);
static const ImU32 IM_OK_BUTTON_COLOR_ACTIVE = (ImU32)ImColor::HSV(0.33f, 0.7f, 0.7f);

static const ImU32 IM_ERROR_BUTTON_COLOR = (ImU32)ImColor::HSV(0.02f, 0.6f, 0.6f);
static const ImU32 IM_ERROR_BUTTON_COLOR_HOVERED = (ImU32)ImColor::HSV(0.02f, 0.7f, 0.7f);
static const ImU32 IM_ERROR_BUTTON_COLOR_ACTIVE = (ImU32)ImColor::HSV(0.02f, 0.8f, 0.8f);

typedef int ImGuiExTextClippedFlags;              // -> enum ImGuiExTextClippedFlags_       // Flags: for TextClipped()
typedef int ImGuiExInputBitfieldFlags;            // -> enum ImGuiExInputBitfieldFlags_     // Flags: for InputBitfield()
typedef int ImGuiExSliderFlags;                   // -> enum ImGuiExSliderFlags_            // Flags: for SliderWithArrows()

// Flags for TextClipped()
enum ImGuiExTextClippedFlags_
{
    ImGuiExTextClippedFlags_None = 0,
    ImGuiExTextClippedFlags_Center = 1 << 0,      // Center text if shorter than max_width
    ImGuiExTextClippedFlags_UseTooltip = 1 << 1,  // Show tooltip with whole text if clipped
};

// Flags for InputBitfield()
enum ImGuiExInputBitfieldFlags_
{
    ImGuiExInputBitfieldFlags_None = 0,
    ImGuiExInputBitfieldFlags_ReadOnly = 1 << 0,      // Read-only mode
};

enum ImGuiExSliderFlags_
{
    ImGuiExSliderFlags_None                   = 0,
    ImGuiExSliderFlags_AlwaysClamp            = 1 << 4,       // Clamp value to min/max bounds when input manually with CTRL+Click. By default CTRL+Click allows going out of bounds.
    ImGuiExSliderFlags_Logarithmic            = 1 << 5,       // Make the widget logarithmic (linear otherwise). Consider using ImGuiSliderFlags_NoRoundToFormat with this if using a format-string with small amount of digits.
    ImGuiExSliderFlags_NoRoundToFormat        = 1 << 6,       // Disable rounding underlying value to match precision of the display format string (e.g. %.3f values are rounded to those 3 digits)
    ImGuiExSliderFlags_NoInput                = 1 << 7,       // Disable CTRL+Click or Enter key allowing to input text directly into the widget

    ImGuiExSliderFlags_NoZoomPopup            = 1 << 16,      // Disable Shift+Click to open enlarged slider in popup for easier manipulation in narrow UI
    ImGuiExSliderFlags_UseSmallArrows         = 1 << 17,      // Tight fit of arrows, better for really narrow UI

    // [Internal]
    ImGuiExSliderFlags_CoreFlagsMask_         = (1 << 16) - 1 // Mask for filtering flags to send to core imgui widget
};


namespace ImGui
{

// only handles coplexity on top level, if you have a node with 1M children, you are out of luck
// changes in clipped parts are not detected, so if somebody deleted nodes in clipped, scrollbar is not updated until user actually scrolls
// scrolling and changes refresh the clipper == that part is as slow as no clipping at all
// only single list in BeginChild/EndChild is tested
struct TreeViewClipper {
    // persist
    float cursor_end = 0;
    float cursor_visible_start = 0;
    uint first_visible_index = 0;
    float last_scroll = 0;
    float cursor_visible_end = 0;
    uint visible_end_index = 0;
    bool full_pass = true;

    // valid only between begin end
    bool scrolled;
    bool met_visible;
    bool last_is_visible;
    uint idx;
    float y;
    bool finished;
    uint count;

    // returns index of first visible top-level node
    uint Begin(uint _count) {
        if (_count != count) full_pass = true;
        count = _count;
        scrolled = ImGui::GetScrollY() != last_scroll;
        if (scrolled) full_pass = true;
        if (full_pass) Refresh();

        // skip invisible space
        ImGui::SetCursorPosY(cursor_visible_start);

        // init runtime data
        met_visible = false;
        last_is_visible = true;
        idx = first_visible_index;
        finished = idx >= count;

        return idx;
    }

    void Refresh() {
        full_pass = false;
        last_scroll = ImGui::GetScrollY();
        first_visible_index = 0;
        cursor_visible_start = 0;
        cursor_end = 0;
    }

    bool BeginNode() {
        y = ImGui::GetCursorPosY();
        return !finished;
    }

    void EndNode() {
        const bool visible = ImGui::IsItemVisible();
        const bool is_first_visible = visible && !met_visible;
        if (is_first_visible) {
            met_visible = true;
            first_visible_index = idx;
            cursor_visible_start = y;
        }
        if (met_visible && !visible) {
            last_is_visible = false;
            y = ImGui::GetCursorPosY();
            if (cursor_end != 0) {
                // something has expended or collapsed
                if (y != cursor_visible_end && cursor_visible_end != 0) full_pass = true;
                if (idx != visible_end_index && visible_end_index != 0) full_pass = true;
                finished = true;
                cursor_visible_end = y;
                visible_end_index = idx;
            }
        }
        ++idx;
        if (idx == count) finished = true;
    }

    void End() {
        if (cursor_end == 0 || last_is_visible) {
            cursor_end = ImGui::GetCursorPosY();
        }
        else {
            ImGui::SetCursorPosY(cursor_end - 2); // TODO why -2
        }
    }
};

IMGUI_API void StyleColorsOuterra(ImGuiStyle* dst = NULL);    // Outerra, recommended style (default)

IMGUI_API void PushDragDropStyle();
IMGUI_API void PopDragDropStyle();

template<typename T>
inline bool SliderScalarT(ImStrv label, T& p_data, const T p_min, const T p_max, const char* format = NULL, ImGuiSliderFlags flags = 0) {
    if constexpr (std::is_same_v<T, int8>)
        return ImGui::SliderScalar(label, ImGuiDataType_S8, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v < T, uint8>)
        return ImGui::SliderScalar(label, ImGuiDataType_U8, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v < T, int16>)
        return ImGui::SliderScalar(label, ImGuiDataType_S16, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v < T, uint16>)
        return ImGui::SliderScalar(label, ImGuiDataType_U16, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v < T, int32>)
        return ImGui::SliderScalar(label, ImGuiDataType_S32, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v < T, uint32>)
        return ImGui::SliderScalar(label, ImGuiDataType_U32, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v < T, int64>)
        return ImGui::SliderScalar(label, ImGuiDataType_S64, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v < T, uint64>)
        return ImGui::SliderScalar(label, ImGuiDataType_U64, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v<T, float>)
        return ImGui::SliderScalar(label, ImGuiDataType_Float, &p_data, &p_min, &p_max, format, flags);
    else if constexpr (std::is_same_v<T, double>)
        return ImGui::SliderScalar(label, ImGuiDataType_Double, &p_data, &p_min, &p_max, format, flags);
    else {
        static_assert(!std::is_same_v<T, void>, "Unknown type");
        return false;
    }
}
IMGUI_API bool Combo(ImStrv label, uint8* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
IMGUI_API bool CheckBoxTristate(ImStrv label, int* v_tristate);


IMGUI_API void TextClipped(ImStrv text, float max_width, ImGuiExTextClippedFlags flags);
IMGUI_API bool TextFilter(ImStrv hint, char* buf, size_t buf_size);

IMGUI_API bool SliderFloatForced(ImStrv label, bool& forced, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool InputFloatForced(ImStrv label, bool& forced, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

IMGUI_API bool InputIntForced(ImStrv label, bool& forced, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt64Forced(ImStrv label, bool& forced, int64* v, int64 step = 1, int64 step_fast = 100, ImGuiInputTextFlags flags = 0);

IMGUI_API bool DragFloatForced(ImStrv label, bool& forced, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat2Forced(ImStrv label, bool& forced, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat3Forced(ImStrv label, bool& forced, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat4Forced(ImStrv label, bool& forced, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

IMGUI_API bool CheckboxForced(ImStrv label, bool& forced, bool* v);

///////////////////////////////////////////////////////////////////////////////

//IMGUI_API void             DockWindow(const char* window_name, ImGuiID node_id);
//IMGUI_API bool             IsWindowDocked(const char* window_name);
//IMGUI_API ImGuiID          GetWindowDockID(const char* window_name);
//IMGUI_API ImGuiDockNode* DockBuilderGetNode(ImGuiID node_id);
//inline ImGuiDockNode*    DockBuilderGetCentralNode(ImGuiID node_id) { ImGuiDockNode* node = DockBuilderGetNode(node_id); if (!node) return NULL; return DockNodeGetRootNode(node)->CentralNode; }
IMGUI_API ImVec2           DockSpaceGetCentralNodeMin(ImGuiID dockspace_id);
IMGUI_API ImVec2           DockSpaceGetCentralNodeMax(ImGuiID dockspace_id);
//IMGUI_API ImGuiID          AddDockNode(ImGuiID node_id = 0, ImGuiDockNodeFlags flags = 0);
//IMGUI_API void           DockBuilderRemoveNode(ImGuiID node_id);                 // Remove node and all its child, undock all windows
//IMGUI_API void           DockBuilderRemoveNodeDockedWindows(ImGuiID node_id, bool clear_settings_refs = true);
//IMGUI_API void           DockBuilderRemoveNodeChildNodes(ImGuiID node_id);       // Remove all split/hierarchy. All remaining docked windows will be re-docked to the remaining root node (node_id).
//IMGUI_API void             SetDockNodePos(ImGuiID node_id, ImVec2 pos);
//IMGUI_API void             SetDockNodeSize(ImGuiID node_id, ImVec2 size);
//IMGUI_API ImGuiID          SplitDockNode(ImGuiID node_id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_at_dir, ImGuiID* out_id_at_opposite_dir); // Create 2 child nodes in this parent node.
//IMGUI_API void           DockBuilderCopyDockSpace(ImGuiID src_dockspace_id, ImGuiID dst_dockspace_id, ImVector<const char*>* in_window_remap_pairs);
//IMGUI_API void           DockBuilderCopyNode(ImGuiID src_node_id, ImGuiID dst_node_id, ImVector<ImGuiID>* out_node_remap_pairs);
//IMGUI_API void           DockBuilderCopyWindowSettings(const char* src_name, const char* dst_name);
//IMGUI_API void             DockChangesFinish(ImGuiID node_id);


// ClearAllFn - Clear all settings data
// ReadInitFn - Read : Called before reading(in registration order)
// ReadOpenFn - Read : Called when entering into a new ini entry e.g. "[Window][Name]"
// ReadLineFn - Read : Called for every line of text within an ini entry
// ApplyAllFn - Read : Called after reading(in registration order)
// WriteAllFn - Write : Output every entries into 'out_buf'
// void  (*ClearAllFn)(ImGuiContext* ctx, ImGuiSettingsHandler* handler);
// void  (*ReadInitFn)(ImGuiContext* ctx, ImGuiSettingsHandler* handler);
// void* (*ReadOpenFn)(ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name);
// void  (*ReadLineFn)(ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line);
// void  (*ApplyAllFn)(ImGuiContext* ctx, ImGuiSettingsHandler* handler);
// void  (*WriteAllFn)(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf);
IMGUI_API void RegisterSettingsHandler(
    bool overwrite_if_exists,
    ImGuiContext* context,
    const char* name,
    void* ClearAllFn,
    void* ReadInitFn,
    void* ReadOpenFn,
    void* ReadLineFn,
    void* ApplyAllFn,
    void* WriteAllFn,
    void* UserData
);

IMGUI_API void* GetSettingsHandlerUserData(
    void* SettingsHandler
);


IMGUI_API void ItemSeparator();

IMGUI_API void SetTooltipUnformatted(ImStrv text);

}

namespace ImGuiEx
{

IMGUI_API void Label(ImStrv label);
IMGUI_API bool LabelCheck(ImStrv label, bool* checkbox); //label with right aligned checkbox for enabling/disabling

IMGUI_API bool Checkbox(ImStrv label, bool* v);
IMGUI_API bool CheckboxRightAlign(ImStrv label, bool* v); // label+checkbox aligned to the right window edge

IMGUI_API bool DragFloat(ImStrv label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // If v_min >= v_max we have no bound
IMGUI_API bool DragFloat2(ImStrv label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat3(ImStrv label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat4(ImStrv label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloatRange2(ImStrv label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool DragInt(ImStrv label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);  // If v_min >= v_max we have no bound
IMGUI_API bool DragInt2(ImStrv label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragInt3(ImStrv label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragInt4(ImStrv label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragIntRange2(ImStrv label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool DragScalar(ImStrv label, ImGuiDataType data_type, void* p_data, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool DragScalarN(ImStrv label, ImGuiDataType data_type, void* p_data, int components, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0);

IMGUI_API bool SliderFloat(ImStrv label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display.
IMGUI_API bool SliderFloat2(ImStrv label, float v[2], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloat3(ImStrv label, float v[3], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloat4(ImStrv label, float v[4], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloatN(ImStrv label, float* p_data, int components, float v_min, float v_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloatStateN(ImStrv label, bool* p_state, float* p_data, int components, float v_min, float v_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderAngle(ImStrv label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt(ImStrv label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt2(ImStrv label, int v[2], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt3(ImStrv label, int v[3], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt4(ImStrv label, int v[4], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderU8(ImStrv label, uint8* v, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderU8Vec2(ImStrv label, uint8 v[2], const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderU8Vec3(ImStrv label, uint8 v[3], const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderU8Vec4(ImStrv label, uint8 v[4], const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderU8Norm(ImStrv label, uint8* v, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderU8Vec2Norm(ImStrv label, uint8 v[2], const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderU8Vec3Norm(ImStrv label, uint8 v[3], const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderU8Vec4Norm(ImStrv label, uint8 v[4], const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderScalar(ImStrv label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderScalarN(ImStrv label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderScalarStateN(ImStrv label, bool* p_state, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool VSliderFloat(ImStrv label, const ImVec2& size, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool VSliderInt(ImStrv label, const ImVec2& size, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool VSliderScalar(ImStrv label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);

IMGUI_API bool InputText(ImStrv label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
IMGUI_API bool InputTextMultiline(ImStrv label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
IMGUI_API bool InputTextWithHint(ImStrv label, ImStrv hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
IMGUI_API bool InputFloat(ImStrv label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputFloat2(ImStrv label, float v[2], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputFloat3(ImStrv label, float v[3], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputFloat4(ImStrv label, float v[4], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt(ImStrv label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt2(ImStrv label, int v[2], ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt3(ImStrv label, int v[3], ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt4(ImStrv label, int v[4], ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputDouble(ImStrv label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputDouble3(ImStrv label, double v[4], const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputDouble4(ImStrv label, double v[4], const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputScalar(ImStrv label, ImGuiDataType data_type, void* p_data, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputScalarN(ImStrv label, ImGuiDataType data_type, void* p_data, int components, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);

IMGUI_API bool SliderIntCheckbox(ImStrv label, bool* s, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloatCheckbox(ImStrv label, bool* s, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display.
IMGUI_API bool InputFloatCheckbox(ImStrv label, bool* s, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

IMGUI_API bool ColorEdit3(ImStrv label, float col[3], ImGuiColorEditFlags flags = 0);
IMGUI_API bool ColorEdit4(ImStrv label, float col[4], ImGuiColorEditFlags flags = 0);
IMGUI_API bool ColorPicker3(ImStrv label, float col[3], ImGuiColorEditFlags flags = 0);
IMGUI_API bool ColorPicker4(ImStrv label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);

IMGUI_API bool BeginStatusBar();                                                 // create and append to a full screen status-bar.
IMGUI_API void EndStatusBar();                                                   // only call EndStatusBar() if BeginStatusBar() returns true!

// custom ones
IMGUI_API bool SliderFloatForced(ImStrv label, bool& forced, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool InputFloatForced(ImStrv label, bool& forced, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

IMGUI_API bool InputIntForced(ImStrv label, bool& forced, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt64Forced(ImStrv label, bool& forced, int64* v, int64 step = 1, int64 step_fast = 100, ImGuiInputTextFlags flags = 0);

IMGUI_API bool DragFloatForced(ImStrv label, bool& forced, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat2Forced(ImStrv label, bool& forced, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat3Forced(ImStrv label, bool& forced, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat4Forced(ImStrv label, bool& forced, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

IMGUI_API bool CheckboxForced(ImStrv label, bool& forced, bool* v);

IMGUI_API bool SliderUInt(ImStrv label, uint* v, uint v_min, uint v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool BeginCombo(ImStrv label, ImStrv preview_value, ImGuiComboFlags flags = 0);
IMGUI_API void EndCombo();
IMGUI_API bool Combo(ImStrv label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
IMGUI_API bool Combo(ImStrv label, int* current_item, const char* const* items_terinated_by_zero, int popup_max_height_in_items = -1);
IMGUI_API bool CheckBoxTristate(ImStrv label, int* v_tristate);

IMGUI_API bool BeginListBox(ImStrv label, const ImVec2& size_arg = ImVec2(0, 0));
IMGUI_API void EndListBox();

IMGUI_API bool MultistateToggleButton(ImStrv label, int* current_item, const char* items_separated_by_zeros);
IMGUI_API bool MultistateToggleButton(ImStrv label, int* current_item, const char** items_terminated_by_zero, const char** tooltips = 0, uint inactive_mask = 0, bool reverse = false);
IMGUI_API bool InputBitfield(ImStrv label, uint* bits, const char* items_separated_by_zeros, ImGuiExInputBitfieldFlags flags = 0);

IMGUI_API bool ActiveButton(ImStrv label, bool active, const ImVec2& size_arg = ImVec2(0, 0));

IMGUI_API void TextFramed(ImStrv label, const char* fmt, ...) IM_FMTARGS(1);

IMGUI_API bool InputTextCharstr(ImStrv label, coid::charstr& buf, size_t max_size = 0, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputTextWithHintCharstr(ImStrv label, ImStrv hint, coid::charstr& buf, ImGuiInputTextFlags flags = 0);

IMGUI_API bool SliderStepScalar(ImStrv label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const void* p_step, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderWithArrows(ImStrv label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const void* p_steps, const char* format = NULL, ImGuiExSliderFlags flags = 0);
IMGUI_API bool SliderWithArrowsFloat(ImStrv label, float* v, float v_min, float v_max, float v_step, const char* format = "%.3f", ImGuiExSliderFlags flags = 0);
IMGUI_API bool SliderWithArrowsInt(ImStrv label, int* v, int v_min, int v_max, int v_step = 1, const char* format = "%d", ImGuiExSliderFlags flags = 0);
IMGUI_API bool SliderWithArrowsUInt(ImStrv label, uint* v, uint v_min, uint v_max, uint v_step = 1, const char* format = "%u", ImGuiExSliderFlags flags = 0);

/// @brief Sets tooltip for hovered item
/// @param additional_hovered_flags - additional flags added to default(see ImGuiHoveredFlags_Tooltip flag)
/// @param override_previous - override previous tooltips for hovered item (concate otherwise)
/// @param fmt, ... - printf params
IMGUI_API void SetItemTooltip(ImGuiHoveredFlags additional_hovered_flags, bool override_previous, const char* fmt, ...);
}

struct ImGuiTextureExt
{
    enum Etype {
        rgb = 0,
        env = 1,
        ycocg = 2,
        recolor3 = 3,
        recolor6 = 6,
        recolor7 = 7,
        recolor8 = 8,
    };

    union {
        struct {
            Etype type : 8;
            unsigned int recolor_id : 24;
            unsigned int id;
        };
        unsigned int type_recolor_id;
        void* tex_id = 0;
    };

    ImGuiTextureExt() {}

    ImGuiTextureExt(ImTextureID imTexId) {
        tex_id = imTexId;
    }

    ImGuiTextureExt(uint handle, Etype type, uint recolor_id = 0) {
        this->id = handle;
        this->type = type;
        this->recolor_id = recolor_id;
    }

    ImTextureID getImTexID() { return tex_id; }
};

static_assert(sizeof(ImGuiTextureExt) == sizeof(intptr_t), "mismatch");
