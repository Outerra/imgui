#include "imgui_ext.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "IconsFontAwesome6Pro.h"


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


bool SliderUInt(const char* label, uint* v, uint v_min, uint v_max, const char* format, ImGuiSliderFlags flags)
{
    return ImGui::SliderScalar(label, ImGuiDataType_U32, v, &v_min, &v_max, format, flags);
}

bool SliderInt16(const char* label, uint* v, uint v_min, uint v_max, const char* format = "%d", ImGuiSliderFlags flags = 0)
{
    return ImGui::SliderScalar(label, ImGuiDataType_S16, v, &v_min, &v_max, format, flags);
}

bool SliderUInt16(const char* label, uint* v, uint v_min, uint v_max, const char* format = "%d", ImGuiSliderFlags flags = 0)
{
    return ImGui::SliderScalar(label, ImGuiDataType_U16, v, &v_min, &v_max, format, flags);
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

ImVec2 DockSpaceGetCentralNodeMin(ImGuiID dockspace_id)
{
    ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(dockspace_id);
    IM_ASSERT(node);
    return node->Pos;
}

ImVec2 DockSpaceGetCentralNodeMax(ImGuiID dockspace_id)
{
    ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(dockspace_id);
    IM_ASSERT(node);
    return node->Pos + node->Size;
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

IMGUI_API void RegisterSettingsHandler(ImGuiContext* context, const char* name, void* ClearAllFn, void* ReadInitFn, void* ReadOpenFn, void* ReadLineFn, void* ApplyAllFn, void* WriteAllFn)
{
    using ClearAllFnPtr = void(*)(ImGuiContext * ctx, ImGuiSettingsHandler * handler);
    using ReadInitFnPtr = void(*)(ImGuiContext * ctx, ImGuiSettingsHandler * handler);
    using ReadOpenFnPtr = void*(*)(ImGuiContext * ctx, ImGuiSettingsHandler * handler, const char* name);
    using ReadLineFnPtr = void(*)(ImGuiContext * ctx, ImGuiSettingsHandler * handler, void* entry, const char* line);
    using ApplyAllFnPtr = void(*)(ImGuiContext * ctx, ImGuiSettingsHandler * handler);
    using WriteAllFnPtr = void(*)(ImGuiContext * ctx, ImGuiSettingsHandler * handler, ImGuiTextBuffer * out_buf);

    ImGuiSettingsHandler new_handler;
    new_handler.TypeName = name;
    new_handler.TypeHash = ImHashStr(name);
    new_handler.ClearAllFn = reinterpret_cast<ClearAllFnPtr>(ClearAllFn);
    new_handler.ReadInitFn = reinterpret_cast<ReadInitFnPtr>(ReadInitFn);
    new_handler.ReadOpenFn = reinterpret_cast<ReadOpenFnPtr>(ReadOpenFn);
    new_handler.ReadLineFn = reinterpret_cast<ReadLineFnPtr>(ReadLineFn);
    new_handler.ApplyAllFn = reinterpret_cast<ApplyAllFnPtr>(ApplyAllFn);
    new_handler.WriteAllFn = reinterpret_cast<WriteAllFnPtr>(WriteAllFn);

    context->SettingsHandlers.push_back(new_handler);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void ItemSeparator()
{
    ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetColorU32(ImGuiCol_Border));
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::PopStyleColor();
}

}


namespace ImGuiEx
{

void Label(const char* label)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImVec2 textSize = ImGui::CalcTextSize(label, 0, true);
    if (textSize.x == 0.0f) return;

    const ImGuiStyle& style = ImGui::GetStyle();
    float fullWidth = ImGui::GetContentRegionAvail().x;
    float itemWidth = fullWidth * 0.65f;
    ImRect textRect;
    textRect.Min = ImGui::GetCursorScreenPos();
    textRect.Max = textRect.Min;
    textRect.Max.x += fullWidth - itemWidth - style.ItemSpacing.x;
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

bool Checkbox(const char* label, bool* v)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);

    const ImGuiStyle& style = ImGui::GetStyle();
    float max_width = ImGui::CalcItemWidth();
    float width = ImGui::GetFrameHeight() + style.FramePadding.y * 2.0f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (max_width - width) * 0.5f);
    bool result = ImGui::Checkbox("##Checkbox", v);
    ImGui::PopID();
    return result;
}


bool DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloat("##DragFloat", v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloat2("##DragFloat2", v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloat3("##DragFloat3", v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloat4("##DragFloat4", v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* format, const char* format_max, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloatRange2("##DragFloatRange2", v_current_min, v_current_max, v_speed, v_min, v_max, format, format_max, flags);
    ImGui::PopID();
    return result;
}

bool DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragInt("##DragInt", v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragInt2("##DragInt2", v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragInt3("##DragInt3", v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragInt4("##DragInt4", v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* format, const char* format_max, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragIntRange2("##DragIntRange2", v_current_min, v_current_max, v_speed, v_min, v_max, format, format_max, flags);
    ImGui::PopID();
    return result;
}

bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragScalar("##DragScalar", data_type, p_data, v_speed, p_min, p_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragScalarN("##DragScalarN", data_type, p_data, components, v_speed, p_min, p_max, format, flags);
    ImGui::PopID();
    return result;
}


bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderFloat("##SliderFloat", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderFloat2("##SliderFloat2", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderFloat3("##SliderFloat3", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderFloat4("##SliderFloat4", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderAngle("##SliderAngle", v_rad, v_degrees_min, v_degrees_max, format, flags);
    ImGui::PopID();
    return result;
}

bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderInt("##SliderInt", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderInt2("##SliderInt2", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}
bool SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderInt3("##SliderInt3", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}
bool SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderInt4("##SliderInt4", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}
bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderScalar("##SliderScalar", data_type, p_data, p_min, p_max, format, flags);
    ImGui::PopID();
    return result;
}

bool SliderScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderScalarN("##SliderScalarN", data_type, p_data, components, p_min, p_max, format, flags);
    ImGui::PopID();
    return result;
}

bool VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::VSliderFloat("##VSliderFloat", size, v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::VSliderInt("##VSliderInt", size, v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool VSliderScalar(const char* label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::VSliderScalar("##VSliderScalar", size, data_type, p_data, p_min, p_max, format, flags);
    ImGui::PopID();
    return result;
}


bool InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputText("##InputText", buf, buf_size, flags, callback, user_data);
    ImGui::PopID();
    return result;
}

bool InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputTextMultiline("##InputTextMultiline", buf, buf_size, size, flags, callback, user_data);
    ImGui::PopID();
    return result;
}

bool InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputTextWithHint("##InputTextWithHint", hint, buf, buf_size, flags, callback, user_data);
    ImGui::PopID();
    return result;
}

bool InputFloat(const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputFloat("##InputFloat", v, step, step_fast, format, flags);
    ImGui::PopID();
    return result;
}

bool InputFloat2(const char* label, float v[2], const char* format, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputFloat2("##InputFloat2", v, format, flags);
    ImGui::PopID();
    return result;
}

bool InputFloat3(const char* label, float v[3], const char* format, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputFloat3("##InputFloat3", v, format, flags);
    ImGui::PopID();
    return result;
}

bool InputFloatCheckbox(const char* label, float* v, bool* s, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
{
    //shows checkbox before the value, for combined and compact enable+value display
    ImGuiEx::Label(label);
    float width = ImGui::CalcItemWidth();

    ImGui::PushID(label);
    bool state = ImGui::Checkbox("##InputFloatCheckbox1", s);
    if (*s == false)
        format = "";

    const ImGuiStyle& style = ImGui::GetStyle();
    const float button_size = ImGui::GetFrameHeight();
    ImGui::SetNextItemWidth(width - button_size - style.ItemInnerSpacing.x);

    ImGui::SameLine(0, style.ItemInnerSpacing.x);

    bool result = ImGui::InputFloat("##InputFloatCheckbox2", v, step, step_fast, format, flags);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        *s = true;
    }
    ImGui::PopID();
    return state || result;
}

bool InputFloat4(const char* label, float v[4], const char* format, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputFloat4("##InputFloat4", v, format, flags);
    ImGui::PopID();
    return result;
}

bool InputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputInt("##InputInt", v, step, step_fast, flags);
    ImGui::PopID();
    return result;
}

bool InputInt2(const char* label, int v[2], ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputInt2("##InputInt2", v, flags);
    ImGui::PopID();
    return result;
}

bool InputInt3(const char* label, int v[3], ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputInt3("##InputInt3", v, flags);
    ImGui::PopID();
    return result;
}

bool InputInt4(const char* label, int v[4], ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputInt4("##InputInt4", v, flags);
    ImGui::PopID();
    return result;
}

bool InputDouble(const char* label, double* v, double step, double step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputDouble("##InputDouble", v, step, step_fast, format, flags);
    ImGui::PopID();
    return result;
}

bool InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputScalar("##InputScalar", data_type, p_data, p_step, p_step_fast, format, flags);
    ImGui::PopID();
    return result;
}

bool InputScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputScalarN("##InputScalarN", data_type, p_data, components, p_step, p_step_fast, format, flags);
    ImGui::PopID();
    return result;
}


bool ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::ColorEdit3("##ColorEdit3", col, flags);
    ImGui::PopID();
    return result;
}

bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::ColorEdit4("##ColorEdit4", col, flags);
    ImGui::PopID();
    return result;
}

bool ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::ColorPicker3("##ColorPicker3", col, flags);
    ImGui::PopID();
    return result;
}

bool ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::ColorPicker4("##ColorPicker4", col, flags);
    ImGui::PopID();
    return result;
}


bool BeginStatusBar()
{
    ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
    float height = ImGui::GetFrameHeight();

    bool is_open = ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags);

    if (is_open)
        ImGui::BeginMenuBar();
    else
        ImGui::End();
    return is_open;
}

void EndStatusBar()
{
    ImGui::EndMenuBar();

    ImGui::End();
}


bool SliderFloatForced(const char* label, bool& forced, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderFloatForced("##SliderFloatForced", forced, v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool InputFloatForced(const char* label, bool& forced, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputFloatForced("##InputFloatForced", forced, v, step, step_fast, format, flags);
    ImGui::PopID();
    return result;
}

bool InputIntForced(const char* label, bool& forced, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputIntForced("##InputIntForced", forced, v, step, step_fast, flags);
    ImGui::PopID();
    return result;
}

bool InputInt64Forced(const char* label, bool& forced, int64* v, int64 step, int64 step_fast, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::InputInt64Forced("##InputInt64Forced", forced, v, step, step_fast, flags);
    ImGui::PopID();
    return result;
}

bool DragFloatForced(const char* label, bool& forced, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloatForced("##DragFloatForced", forced, v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragFloat2Forced(const char* label, bool& forced, float v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloat2Forced("##DragFloat2Forced", forced, v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragFloat3Forced(const char* label, bool& forced, float v[3], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloat3Forced("##DragFloat3Forced", forced, v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool DragFloat4Forced(const char* label, bool& forced, float v[4], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::DragFloat4Forced("##DragFloat4Forced", forced, v, v_speed, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool CheckboxForced(const char* label, bool& forced, bool* v)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);

    const ImGuiStyle& style = ImGui::GetStyle();
    float max_width = ImGui::CalcItemWidth();
    float width = ImGui::GetFrameHeight() + style.FramePadding.y * 2.0f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (max_width - width) * 0.5f);

    bool result = ImGui::CheckboxForced("##CheckboxForced", forced, v);
    ImGui::PopID();
    return result;
}

bool SliderUInt(const char* label, uint* v, uint v_min, uint v_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::SliderUInt("##SliderUInt", v, v_min, v_max, format, flags);
    ImGui::PopID();
    return result;
}

bool BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
    ImGuiEx::Label(label);
    static char id_buff[128] = { 0 };
    sprintf_s(id_buff, sizeof(id_buff), "## %s", label);
    return ImGui::BeginCombo(id_buff, preview_value, flags);
}

void EndCombo()
{
    ImGui::EndCombo();
}

bool Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::Combo("##Combo", current_item, items_separated_by_zeros, popup_max_height_in_items);
    ImGui::PopID();
    return result;
}

bool CheckBoxTristate(const char* label, int* v_tristate)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);
    bool result = ImGui::CheckBoxTristate("##CheckBoxTristate", v_tristate);
    ImGui::PopID();
    return result;
}

bool BeginListBox(const char* label, const ImVec2& size_arg)
{
    ImGuiEx::Label(label);
    static char id_buff[128] = { 0 };
    sprintf_s(id_buff, sizeof(id_buff), "##%s", label);
    return ImGui::BeginListBox(id_buff, size_arg);
}

void EndListBox()
{
    ImGui::EndListBox();
}

bool MultistateToggleButton(const char* label, int* current_item, const char* items_separated_by_zeros)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while (*p) {
        p += strlen(p) + 1;
        items_count++;
    }
    p = items_separated_by_zeros;

    ImGuiEx::Label(label);

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 size(ImGui::GetContentRegionAvail().x, g.FontSize + 2.0f * style.FramePadding.y);
    const ImVec2 label_size = ImVec2(size.x / items_count, size.y);

    ImVec2 pos = window->DC.CursorPos;

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    // Render
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_Button);
    ImGui::RenderNavHighlight(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    for (int i = 0; i < items_count; ++i) {
        const bool item_selected = (i == *current_item);
        const ImRect label_bb(bb.Min + ImVec2(i * label_size.x, 0), bb.Min + ImVec2((i + 1) * label_size.x, label_size.y));

        if (label_bb.Contains(g.IO.MousePos) && pressed) {
            *current_item = i;
        }

        bool lhovered = hovered && label_bb.Contains(g.IO.MousePos);
        bool lheld = held && lhovered;
        if (lhovered || item_selected) {
            bool lactive = (lheld && lhovered) || (item_selected && !lhovered);
            const ImU32 label_col = ImGui::GetColorU32(lactive ? ImGuiCol_ButtonActive : lhovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            ImGui::RenderFrame(label_bb.Min, label_bb.Max, label_col, false, style.FrameRounding);
        }

        if (g.LogEnabled)
            ImGui::LogSetNextTextDecoration("[", "]");

        const ImVec2 label_text_size = ImGui::CalcTextSize(p, NULL, true);
        ImGui::RenderTextClipped(label_bb.Min + style.FramePadding, label_bb.Max - style.FramePadding, p, NULL, &label_text_size, style.ButtonTextAlign, &label_bb);
        p += strlen(p) + 1;
    }

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool InputBitfield(const char* label, uint* bits, const char* items_separated_by_zeros, ImGuiInputBitfieldFlags flags)
{
    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while (*p) {
        p += strlen(p) + 1;
        items_count++;
    }
    p = items_separated_by_zeros;

    ImGuiEx::Label(label);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const bool is_readonly = (flags & ImGuiInputBitfieldFlags_ReadOnly) != 0;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 size(ImGui::GetContentRegionAvail().x, g.FontSize + 2.0f * style.FramePadding.y);
    const ImVec2 label_size = ImVec2(size.x / items_count, size.y);

    ImVec2 pos = window->DC.CursorPos;

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered = false, held = false;
    bool pressed = !is_readonly && ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    // Render
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_Button);
    ImGui::RenderNavHighlight(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    for (int i = 0; i < items_count; ++i) {
        const bool item_selected = (*bits & (1u << i));
        const ImRect label_bb(bb.Min + ImVec2(i * label_size.x, 0), bb.Min + ImVec2((i + 1) * label_size.x, label_size.y));

        if (label_bb.Contains(g.IO.MousePos) && pressed) {
            *bits ^= (1u << i);
        }

        bool lhovered = hovered && label_bb.Contains(g.IO.MousePos);
        bool lheld = held && lhovered;
        if (lhovered || item_selected) {
            bool lactive = (lheld && lhovered) || (item_selected && !lhovered);
            const ImU32 label_col = ImGui::GetColorU32(lactive ? ImGuiCol_ButtonActive : lhovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            ImGui::RenderFrame(label_bb.Min, label_bb.Max, label_col, false, style.FrameRounding);
        }

        if (g.LogEnabled)
            ImGui::LogSetNextTextDecoration("[", "]");

        const ImVec2 label_text_size = ImGui::CalcTextSize(p, NULL, true);
        ImGui::RenderTextClipped(label_bb.Min + style.FramePadding, label_bb.Max - style.FramePadding, p, NULL, &label_text_size, style.ButtonTextAlign, &label_bb);
        p += strlen(p) + 1;
    }

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool ActiveButton(const char* label, bool active, const ImVec2& size_arg)
{
    if (active) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    }

    bool result = ImGui::Button(label, size_arg);

    if (active) {
        ImGui::PopStyleColor(2);
    }

    return result;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void TextFramed(const char* label, const char* fmt, ...)
{
    const ImGuiStyle& style = ImGui::GetStyle();

    ImGuiEx::Label(label);
    ImGui::PushID(label);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 bb_size = ImGui::CalcItemSize(ImVec2(0, 0), ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + bb_size);

    const ImGuiID id = ImGui::GetCurrentWindow()->GetID("child");
    ImGui::ItemSize(bb_size, style.FramePadding.y);
    if (ImGui::ItemAdd(frame_bb, id, &frame_bb, ImGuiItemFlags_Inputable))
    {
        ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
        //ImGui::GetCurrentWindow()->DC.CursorPos += style.FramePadding;

        va_list args;
        va_start(args, fmt);
        const char* text, * text_end;
        ImFormatStringToTempBufferV(&text, &text_end, fmt, args);
        if (text != text_end)
            ImGui::RenderText(frame_bb.Min + style.FramePadding, text);

        va_end(args);
    }

    ImGui::PopID();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static int charstr_input_text_callback(ImGuiInputTextCallbackData* data)
{

    coid::charstr* buf = static_cast<coid::charstr*>(data->UserData);

    if ((data->EventFlag & ImGuiInputTextFlags_CallbackResize) != 0)
    {
        buf->get_buf(data->BufSize - 1);
    }

    return 0;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool InputTextCharstr(const char* label, coid::charstr& buf, size_t max_size, ImGuiInputTextFlags flags)
{
    ImGuiEx::Label(label);
    ImGui::PushID(label);

    if (buf.is_empty())
    {
        buf.reserve(16);
        *buf.ptr_ref() = 0;
    }

    size_t buf_size = max_size > 0 ? ImMin<size_t>(max_size, buf.len() + 1) : buf.len() + 1;
    bool result = ImGui::InputText("##InputText", buf.ptr_ref(), buf_size, flags | ImGuiInputTextFlags_CallbackResize, &charstr_input_text_callback, &buf);
    if (result) buf.correct_size();
    ImGui::PopID();
    return result;
}

bool InputTextWithHintCharstr(const char* label, const char* hint, coid::charstr& buf, size_t max_size, ImGuiInputTextFlags flags)
{
    if (buf.is_empty())
    {
        buf.reserve(16);
        *buf.ptr_ref() = 0;
    }

    size_t buf_size = max_size > 0 ? ImMin<size_t>(max_size, buf.len() + 1) : buf.len() + 1;
    return ImGui::InputTextWithHint(label, hint, buf.ptr_ref(), buf_size, flags | ImGuiInputTextFlags_CallbackResize, &charstr_input_text_callback, &buf);
}


bool SliderStepScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const void* p_step, const char* format, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = ImGui::CalcItemWidth();

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;

    // Tabbing or CTRL-clicking on Slider turns it into an input box
    const bool hovered = ImGui::ItemHoverable(frame_bb, id);
    bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
    if (!temp_input_is_active)
    {
        const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        if (input_requested_by_tabbing || clicked || g.NavActivateId == id || g.NavActivateInputId == id)
        {
            ImGui::SetActiveID(id, window);
            ImGui::SetFocusID(id, window);
            ImGui::FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (temp_input_allowed && (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || g.NavActivateInputId == id))
                temp_input_is_active = true;
        }
    }

    if (temp_input_is_active)
    {
        // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
        const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
        return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
    }

    // Draw frame
    const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImGui::RenderNavHighlight(frame_bb, id);
    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, p_step, format, flags, &grab_bb);
    if (value_changed) {
        ImGui::MarkItemEdited(id);
    }

    // Render grab
    if (grab_bb.Max.x > grab_bb.Min.x)
        window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("{", "}");
    ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return value_changed;
}

bool SliderWithArrows(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const void* p_step, const char* format, ImGuiSliderFlags flags)
{
    ImGuiEx::Label(label);

    ImGui::PushID(label);
    bool changed = false;
    const ImGuiStyle& style = ImGui::GetStyle();
    float width = ImGui::GetContentRegionAvail().x;
    float slider_width = width - (style.ItemInnerSpacing.x + style.FramePadding.x * 2.0f + ImGui::CalcTextSize(ICON_FA_ANGLE_LEFT).x) * 2.0f;
    if (ImGui::Button(ICON_FA_ANGLE_LEFT)) {
        if (ImGui::DataTypeCompare(data_type, p_data, p_min) > 0) {
            ImGui::DataTypeApplyOp(data_type, '-', p_data, p_data, p_step);
            ImGui::DataTypeClamp(data_type, p_data, p_min, p_max);
            changed = true;
        }
    }
    ImGui::SameLine(0, style.ItemInnerSpacing.x);
    ImGui::SetNextItemWidth(slider_width);
    if (SliderStepScalar("##slider", data_type, p_data, p_min, p_max, p_step, format, flags)) {
        changed = true;
    }
    ImGui::SameLine(0, style.ItemInnerSpacing.x);
    if (ImGui::Button(ICON_FA_ANGLE_RIGHT)) {
        if (ImGui::DataTypeCompare(data_type, p_data, p_max) < 0) {
            ImGui::DataTypeApplyOp(data_type, '+', p_data, p_data, p_step);
            ImGui::DataTypeClamp(data_type, p_data, p_min, p_max);
            changed = true;
        }
    }
    ImGui::PopID();
    return changed;
}

bool SliderWithArrowsFloat(const char* label, float* v, float v_min, float v_max, float v_step, const char* format, ImGuiSliderFlags flags)
{
    return SliderWithArrows(label, ImGuiDataType_Float, v, &v_min, &v_max, &v_step, format, flags);
}

bool SliderWithArrowsInt(const char* label, int* v, int v_min, int v_max, int v_step, const char* format, ImGuiSliderFlags flags)
{
    return SliderWithArrows(label, ImGuiDataType_S32, v, &v_min, &v_max, &v_step, format, flags);
}

bool SliderWithArrowsUInt(const char* label, uint* v, uint v_min, uint v_max, uint v_step, const char* format, ImGuiSliderFlags flags)
{
    return SliderWithArrows(label, ImGuiDataType_U32, v, &v_min, &v_max, &v_step, format, flags);
}

}
