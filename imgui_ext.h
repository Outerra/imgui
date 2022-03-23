#pragma once

#include "comm/commtypes.h"
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui.h"
#undef IMGUI_DISABLE_OBSOLETE_FUNCTIONS

static constexpr ImU32 IM_FORCED_COLOR = IM_COL32(255, 130, 40, 255);
static constexpr ImU32 IM_ERROR_COLOR = IM_COL32(255, 130, 120, 255);

typedef int ImGuiTextClippedFlags;               // -> enum ImGuiTextClippedFlags_     // Flags: for TextClipped()

// Flags for TextClipped()
enum ImGuiTextClippedFlags_
{
    ImGuiTextClippedFlags_None = 0,
    ImGuiTextClippedFlags_Center = 1 << 0,      // Center text if shorter than max_width
    ImGuiTextClippedFlags_UseTooltip = 1 << 1,  // Show tooltip with whole text if clipped
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

IMGUI_API void PushDragDropStyle();
IMGUI_API void PopDragDropStyle();

IMGUI_API void PushDisable();
IMGUI_API void PopDisable();

IMGUI_API bool SliderUInt(const char* label, uint* v, uint v_min, uint v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool Combo(const char* label, uint8* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
IMGUI_API bool CheckBoxTristate(const char* label, int* v_tristate);


IMGUI_API void TextClipped(const char* text, float max_width, ImGuiTextClippedFlags flags);
IMGUI_API bool TextFilter(const char* hint, char* buf, size_t buf_size, float width = -1.0f); //for more about width, see ImGui::PushItemWidth(float)

IMGUI_API bool SliderFloatForced(const char* label, bool& forced, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool InputFloatForced(const char* label, bool& forced, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

IMGUI_API bool InputIntForced(const char* label, bool& forced, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt64Forced(const char* label, bool& forced, int64* v, int64 step = 1, int64 step_fast = 100, ImGuiInputTextFlags flags = 0);

IMGUI_API bool DragFloatForced(const char* label, bool& forced, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat2Forced(const char* label, bool& forced, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat3Forced(const char* label, bool& forced, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat4Forced(const char* label, bool& forced, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

IMGUI_API bool CheckboxForced(const char* label, bool& forced, bool* v);

///////////////////////////////////////////////////////////////////////////////

IMGUI_API void             DockWindow(const char* window_name, ImGuiID node_id);
IMGUI_API bool             IsWindowDocked(const char* window_name);
IMGUI_API ImGuiID          GetWindowDockID(const char* window_name);
//IMGUI_API ImGuiDockNode* DockBuilderGetNode(ImGuiID node_id);
//inline ImGuiDockNode*    DockBuilderGetCentralNode(ImGuiID node_id) { ImGuiDockNode* node = DockBuilderGetNode(node_id); if (!node) return NULL; return DockNodeGetRootNode(node)->CentralNode; }
IMGUI_API ImGuiID          AddDockNode(ImGuiID node_id = 0, ImGuiDockNodeFlags flags = 0);
//IMGUI_API void           DockBuilderRemoveNode(ImGuiID node_id);                 // Remove node and all its child, undock all windows
//IMGUI_API void           DockBuilderRemoveNodeDockedWindows(ImGuiID node_id, bool clear_settings_refs = true);
//IMGUI_API void           DockBuilderRemoveNodeChildNodes(ImGuiID node_id);       // Remove all split/hierarchy. All remaining docked windows will be re-docked to the remaining root node (node_id).
IMGUI_API void             SetDockNodePos(ImGuiID node_id, ImVec2 pos);
IMGUI_API void             SetDockNodeSize(ImGuiID node_id, ImVec2 size);
IMGUI_API ImGuiID          SplitDockNode(ImGuiID node_id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_at_dir, ImGuiID* out_id_at_opposite_dir); // Create 2 child nodes in this parent node.
//IMGUI_API void           DockBuilderCopyDockSpace(ImGuiID src_dockspace_id, ImGuiID dst_dockspace_id, ImVector<const char*>* in_window_remap_pairs);
//IMGUI_API void           DockBuilderCopyNode(ImGuiID src_node_id, ImGuiID dst_node_id, ImVector<ImGuiID>* out_node_remap_pairs);
//IMGUI_API void           DockBuilderCopyWindowSettings(const char* src_name, const char* dst_name);
IMGUI_API void             DockChangesFinish(ImGuiID node_id);

}

namespace ImGuiEx
{

IMGUI_API void Label(const char* label);

IMGUI_API bool Checkbox(const char* label, bool* v);

IMGUI_API bool DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // If v_min >= v_max we have no bound
IMGUI_API bool DragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);  // If v_min >= v_max we have no bound
IMGUI_API bool DragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0);

IMGUI_API bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display.
IMGUI_API bool SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
IMGUI_API bool VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool VSliderScalar(const char* label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);

IMGUI_API bool InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
IMGUI_API bool InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
IMGUI_API bool InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
IMGUI_API bool InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputFloat2(const char* label, float v[2], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputFloat3(const char* label, float v[3], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputFloat4(const char* label, float v[4], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt(const char* label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt2(const char* label, int v[2], ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt3(const char* label, int v[3], ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt4(const char* label, int v[4], ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputDouble(const char* label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);

IMGUI_API bool ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
IMGUI_API bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
IMGUI_API bool ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
IMGUI_API bool ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);

// custom ones
IMGUI_API bool SliderFloatForced(const char* label, bool& forced, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool InputFloatForced(const char* label, bool& forced, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

IMGUI_API bool InputIntForced(const char* label, bool& forced, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
IMGUI_API bool InputInt64Forced(const char* label, bool& forced, int64* v, int64 step = 1, int64 step_fast = 100, ImGuiInputTextFlags flags = 0);

IMGUI_API bool DragFloatForced(const char* label, bool& forced, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat2Forced(const char* label, bool& forced, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat3Forced(const char* label, bool& forced, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragFloat4Forced(const char* label, bool& forced, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

IMGUI_API bool CheckboxForced(const char* label, bool& forced, bool* v);

IMGUI_API bool SliderUInt(const char* label, uint* v, uint v_min, uint v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
IMGUI_API void EndCombo();
IMGUI_API bool Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
IMGUI_API bool CheckBoxTristate(const char* label, int* v_tristate);

IMGUI_API bool MultistateToggleButton(const char* label, int* current_item, const char* items_separated_by_zeros);

}

struct ImGuiTextureExt
{
    unsigned int env = 0;
    unsigned int id = 0;

    ImGuiTextureExt() {}

    ImGuiTextureExt(ImTextureID imTexId) {
        env = (unsigned int)(intptr_t(imTexId) >> 32u);
        id = (unsigned int)(intptr_t(imTexId) & 0xFFFFffffu);
    }

    ImTextureID getImTexID() { return (ImTextureID)(intptr_t(env) << 32u | intptr_t(id)); }
};
