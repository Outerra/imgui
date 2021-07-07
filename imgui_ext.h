#pragma once

#include "comm/commtypes.h"
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui.h"
#undef IMGUI_DISABLE_OBSOLETE_FUNCTIONS

static constexpr ImU32 IM_FORCED_COLOR = IM_COL32(255, 130, 40, 255);

namespace ImGui
{

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

IMGUI_API void PushDisable();
IMGUI_API void PopDisable();

IMGUI_API bool SliderUInt(const char* label, uint* v, uint v_min, uint v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
IMGUI_API bool Combo(const char* label, uint8* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
IMGUI_API bool CheckBoxTristate(const char* label, int* v_tristate);

IMGUI_API void LabelEx(const char* label);

}