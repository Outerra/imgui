#include "imgui_ext.h"

#include <utility>
#include "imgui_internal.h"


namespace ImGui
{


void PushDragDropStyle()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_TextDisabled));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetColorU32(ImGuiCol_TextDisabled));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_TextDisabled));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetColorU32(ImGuiCol_TextDisabled));
}

void PopDragDropStyle()
{
    ImGui::PopStyleColor(4);
}



void TextClipped(const char* text, float max_width, ImGuiTextClippedFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImRect textRect;
    textRect.Min = ImGui::GetCursorScreenPos();
    textRect.Max = textRect.Min;
    textRect.Max.x += max_width - style.ItemSpacing.x;
    textRect.Max.y += textSize.y;

    if (flags & ImGuiTextClippedFlags_Center && max_width > textSize.x) {
        float half = (max_width - textSize.x) * 0.5f;
        textRect.Min.x += half;
    }

    ImGui::SetCursorScreenPos(textRect.Min);

    ImGui::AlignTextToFramePadding();
    textRect.Min.y += window->DC.CurrLineTextBaseOffset;
    textRect.Max.y += window->DC.CurrLineTextBaseOffset;

    ImGui::ItemSize(textRect);
    if (ImGui::ItemAdd(textRect, window->GetID(text)))
    {
        ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), textRect.Min, textRect.Max, textRect.Max.x,
            textRect.Max.x, text, nullptr, &textSize);

        if (flags & ImGuiTextClippedFlags_UseTooltip && textRect.GetWidth() < textSize.x && ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", text);
    }
    ImGui::SetCursorScreenPos(textRect.Max - ImVec2{ 0, textSize.y + window->DC.CurrLineTextBaseOffset });
}

bool TextFilter(const char* hint, char* buf, size_t buf_size, float width)
{
    ImGui::PushID(hint);
    ImGui::PushItemWidth(width);
    bool result = ImGui::InputTextWithHint("##filter", hint, buf, buf_size);
    ImGui::PopItemWidth();
    if (*buf != '\0') {
        ImGui::SetItemAllowOverlap();
        ImGui::SameLine(0, -1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 26);
        if (ImGui::SmallButton("x")) {
            buf[0] = '\0';
            result = true;
        }
    }
    ImGui::PopID();
    return result;
}



template <auto fn, class ...Args>
bool core_gui(bool& forced, const char* label, Args ...args)
{
    bool result = false;
    const bool f = forced;
    if (f) {
        ImGui::PushStyleColor(ImGuiCol_Border, IM_FORCED_COLOR);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    }
    if (fn(label, std::forward<Args>(args)...)) {
        result = true;
        forced = true;
    }
    if (f) {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        if (ImGui::BeginPopupContextItem(label)) {
            if (ImGui::MenuItem("Reset")) {
                forced = false;
            }
            ImGui::EndPopup();
        }
    }
    else {
        if (ImGui::BeginPopupContextItem(label)) {
            if (ImGui::MenuItem("Force")) {
                forced = true;
            }
            ImGui::EndPopup();
        }
    }
    return result;
}


bool SliderFloatForced(const char* label, bool& forced, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) {
    return core_gui<&ImGui::SliderFloat>(forced, label, v, v_min, v_max, format, flags);
}

bool InputFloatForced(const char* label, bool& forced, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags) {
    return core_gui<&ImGui::InputFloat>(forced, label, v, step, step_fast, format, flags | ImGuiInputTextFlags_EnterReturnsTrue);
}


bool InputIntForced(const char* label, bool& forced, int* v, int step, int step_fast, ImGuiInputTextFlags flags) {
    return core_gui<&ImGui::InputInt>(forced, label, v, step, step_fast, flags | ImGuiInputTextFlags_EnterReturnsTrue);
}

bool InputInt64Forced(const char* label, bool& forced, int64* v, int64 step, int64 step_fast, ImGuiInputTextFlags flags) {
    return core_gui<&ImGui::InputScalar>(forced, label, ImGuiDataType_S64, v, &step, &step_fast, "%d", flags | ImGuiInputTextFlags_EnterReturnsTrue);
}


bool DragFloatForced(const char* label, bool& forced, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) {
    return core_gui<&ImGui::DragFloat>(forced, label, v, v_speed, v_min, v_max, format, flags);
}

bool DragFloat2Forced(const char* label, bool& forced, float v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) {
    return core_gui<&ImGui::DragFloat2>(forced, label, v, v_speed, v_min, v_max, format, flags);
}

bool DragFloat3Forced(const char* label, bool& forced, float v[3], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) {
    return core_gui<&ImGui::DragFloat3>(forced, label, v, v_speed, v_min, v_max, format, flags);
}

bool DragFloat4Forced(const char* label, bool& forced, float v[4], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) {
    return core_gui<&ImGui::DragFloat4>(forced, label, v, v_speed, v_min, v_max, format, flags);
}


bool CheckboxForced(const char* label, bool& forced, bool* v) {
    return core_gui<&ImGui::Checkbox>(forced, label, v);
}


void PushDisable()
{
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
}
void PopDisable()
{
    ImGui::PopStyleVar();
    ImGui::PopItemFlag();
}

bool SliderUInt(const char* label, uint* v, uint v_min, uint v_max, const char* format, ImGuiSliderFlags flags)
{
    return ImGui::SliderScalar(label, ImGuiDataType_U32, v, &v_min, &v_max, format, flags);
}

bool Combo(const char* label, uint8* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items)
{
    int current = *current_item;
    if (ImGui::Combo(label, &current, items_separated_by_zeros, popup_max_height_in_items)) {
        *current_item = (uint8)current;
        return true;
    }
    return false;
}

bool CheckBoxTristate(const char* label, int* v_tristate)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    float max_width = ImGui::CalcItemWidth();
    float width = ImGui::GetFrameHeight() + style.FramePadding.y * 2.0f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (max_width - width) * 0.5f);

    bool ret;
    if (*v_tristate == -1)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
        bool b = false;
        ret = ImGui::Checkbox(label, &b);
        if (ret)
            *v_tristate = 1;
        ImGui::PopItemFlag();
    }
    else
    {
        bool b = (*v_tristate != 0);
        ret = ImGui::Checkbox(label, &b);
        if (ret)
            *v_tristate = (int)b;
    }
    return ret;
}


void LabelEx(const char* label)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    //const ImVec2 lineStart = ImGui::GetCursorScreenPos();
    const ImGuiStyle& style = ImGui::GetStyle();
    float fullWidth = ImGui::GetContentRegionAvail().x;
    float itemWidth = fullWidth * 0.65f;
    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImRect textRect;
    textRect.Min = ImGui::GetCursorScreenPos();
    textRect.Max = textRect.Min;
    textRect.Max.x += fullWidth - itemWidth - style.ItemSpacing.x;;
    textRect.Max.y += textSize.y;

    ImGui::SetCursorScreenPos(textRect.Min);

    ImGui::AlignTextToFramePadding();
    textRect.Min.y += window->DC.CurrLineTextBaseOffset;
    textRect.Max.y += window->DC.CurrLineTextBaseOffset;

    ImGui::ItemSize(textRect);
    if (ImGui::ItemAdd(textRect, window->GetID(label)))
    {
        ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), textRect.Min, textRect.Max, textRect.Max.x,
            textRect.Max.x, label, nullptr, &textSize);

        if (textRect.GetWidth() < textSize.x && ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", label);
    }
    ImGui::SetCursorScreenPos(textRect.Max - ImVec2{ 0, textSize.y + window->DC.CurrLineTextBaseOffset });
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth);
}

///////////////////////////////////////////////////////////////////////////////

void DockWindow(const char* window_name, ImGuiID node_id)
{
    ImGui::DockBuilderDockWindow(window_name, node_id);
}

bool IsWindowDocked(const char* window_name)
{
    ImGuiID window_id = ImHashStr(window_name);
    ImGuiWindow* window = ImGui::FindWindowByID(window_id);
    if (window == nullptr)
        return false;

    return window->DockIsActive;
}

ImGuiID GetWindowDockID(const char* window_name)
{
    ImGuiID window_id = ImHashStr(window_name);
    ImGuiWindow* window = ImGui::FindWindowByID(window_id);
    if (window == nullptr)
        return 0;

    ImGuiDockNode* node = window->DockNode;
    if (node == nullptr)
        return 0;

    return node->ID;
}

ImGuiID AddDockNode(ImGuiID node_id, ImGuiDockNodeFlags flags)
{
    return ImGui::DockBuilderAddNode(node_id, flags);
}

void SetDockNodePos(ImGuiID node_id, ImVec2 pos)
{
    ImGui::DockBuilderSetNodePos(node_id, pos);
}

void SetDockNodeSize(ImGuiID node_id, ImVec2 size)
{
    ImGui::DockBuilderSetNodeSize(node_id, size);
}

ImGuiID SplitDockNode(ImGuiID node_id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_at_dir, ImGuiID* out_id_at_opposite_dir)
{
    return ImGui::DockBuilderSplitNode(node_id, split_dir, size_ratio_for_node_at_dir, out_id_at_dir, out_id_at_opposite_dir);
}

void DockChangesFinish(ImGuiID node_id)
{
    ImGui::DockBuilderFinish(node_id);
}

}
