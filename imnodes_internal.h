#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <assert.h>
#include <limits.h>

// the structure of this file:
//
// [SECTION] internal enums
// [SECTION] internal data structures
// [SECTION] global and editor context structs
// [SECTION] object pool implementation

struct ImNodesContext;

extern ImNodesContext* GImNodes;

// [SECTION] internal enums

typedef int ImNodesScope;
typedef int ImNodesAttributeType;
typedef int ImNodesUIState;
typedef int ImNodesClickInteractionType;
typedef int ImNodesLinkCreationType;

enum ImNodesScope_
{
    ImNodesScope_None = 1,
    ImNodesScope_Editor = 1 << 1,
    ImNodesScope_Node = 1 << 2,
    ImNodesScope_Attribute = 1 << 3
};

enum ImNodesAttributeType_
{
    ImNodesAttributeType_None,
    ImNodesAttributeType_Input,
    ImNodesAttributeType_Output
};

enum ImNodesUIState_
{
    ImNodesUIState_None = 0,
    ImNodesUIState_LinkStarted = 1 << 0,
    ImNodesUIState_LinkDropped = 1 << 1,
    ImNodesUIState_LinkCreated = 1 << 2
};

enum ImNodesClickInteractionType_
{
    ImNodesClickInteractionType_Node,
    ImNodesClickInteractionType_Link,
    ImNodesClickInteractionType_LinkCreation,
    ImNodesClickInteractionType_Panning,
    ImNodesClickInteractionType_BoxSelection,
    ImNodesClickInteractionType_MiniMapPanning,
    ImNodesClickInteractionType_MiniMapZooming,
    ImNodesClickInteractionType_MiniMapSnapping,
    ImNodesClickInteractionType_None
};

enum ImNodesLinkCreationType_
{
    ImNodesLinkCreationType_Standard,
    ImNodesLinkCreationType_FromDetach
};

// Callback type used to specify special behavior when hovering a node in the minimap
typedef void (*ImNodesMiniMapNodeHoveringCallback)(int, void*);

// [SECTION] internal data structures

// The object T must have the following interface:
//
// struct T
// {
//     T();
//
//     int id;
// };
template<typename T>
struct ImObjectPool
{
    ImVector<T>    Pool;
    ImVector<bool> InUse;
    ImVector<int>  FreeList;
    ImGuiStorage   IdMap;
};

// Emulates std::optional<int> using the sentinel value `INVALID_INDEX`.
struct ImOptionalIndex
{
    ImOptionalIndex() = default;
    ImOptionalIndex(const int value) : _Index(value) {}

    // Observers

    inline bool HasValue() const { return _Index != INVALID_INDEX; }

    inline int Value() const
    {
        assert(HasValue());
        return _Index;
    }

    // Modifiers

    inline ImOptionalIndex& operator=(const int value)
    {
        _Index = value;
        return *this;
    }

    inline void Reset() { _Index = INVALID_INDEX; }

    inline bool operator==(const ImOptionalIndex& rhs) const { return _Index == rhs._Index; }

    inline bool operator==(const int rhs) const { return _Index == rhs; }

    inline bool operator!=(const ImOptionalIndex& rhs) const { return _Index != rhs._Index; }

    inline bool operator!=(const int rhs) const { return _Index != rhs; }

    static const int INVALID_INDEX = -1;

private:
    int _Index = INVALID_INDEX;
};

struct ImNodeData
{
    int    Id;
    int    ParentNodeIdx = INT_MAX;
    ImVec2 Origin = { 100.0f, 100.0f }; // The node origin is in editor space
    ImRect TitleBarContentRect;
    ImRect Rect = { ImVec2(0.0f, 0.0f), ImVec2(0.0f, 0.0f) };

    struct
    {
        ImU32 Background = 0;
        ImU32 BackgroundHovered = 0;
        ImU32 BackgroundSelected = 0;
        ImU32 Outline = 0;
        ImU32 OutlineHovered = 0;
        ImU32 OutlineSelected = 0;
        ImU32 OutlineActive = 0;
        ImU32 Titlebar = 0;
        ImU32 TitlebarHovered = 0;
        ImU32 TitlebarSelected = 0;
    } ColorStyle;

    struct
    {
        float  CornerRounding = 0.0f;
        ImVec2 Padding;
        float  BorderThickness = 0.0f;
    } LayoutStyle;

    ImVector<int> PinIndices;
    bool          Draggable = true;
    bool          Active = false;

    ImNodeData(const int node_id) : Id(node_id) {}
};

struct ImPinData
{
    int                  Id = -1;
    int                  ParentNodeIdx = -1;
    ImRect               AttributeRect;
    ImNodesAttributeType Type = ImNodesAttributeType_None;
    ImNodesPinShape      Shape = ImNodesPinShape_CircleFilled;
    ImVec2               Pos; // screen-space coordinates
    int                  Flags = ImNodesAttributeFlags_None;

    struct
    {
        ImU32 Background = 0;
        ImU32 Hovered = 0;
    } ColorStyle;

    ImPinData(const int pin_id) : Id(pin_id) {}
};

struct ImLinkData
{
    int Id = -1;
    int StartIdx = -1;
    int EndIdx = -1;
    bool Active = false;

    struct
    {
        ImU32 Base = 0;
        ImU32 Hovered = 0;
        ImU32 Selected = 0;
        ImU32 Active = 0;
    } ColorStyle;

    ImLinkData(const int link_id) : Id(link_id) {}
};

struct ImClickInteractionState
{
    ImNodesClickInteractionType Type = ImNodesClickInteractionType_None;

    struct
    {
        int                     StartIdx = -1;
        ImOptionalIndex         EndIdx;
        ImNodesLinkCreationType Type;
    } LinkCreation;

    struct
    {
        ImRect Rect;
    } BoxSelector;
};

struct ImNodesColElement
{
    ImU32      Color;
    ImNodesCol Item;

    ImNodesColElement(const ImU32 c, const ImNodesCol s) : Color(c), Item(s) {}
};

struct ImNodesStyleVarElement
{
    ImNodesStyleVar Item;
    float           Value;

    ImNodesStyleVarElement(const float value, const ImNodesStyleVar variable)
        : Item(variable), Value(value)
    {
    }
};

// [SECTION] global and editor context structs

struct ImNodesEditorContext
{
    ImObjectPool<ImNodeData> Nodes;
    ImObjectPool<ImPinData>  Pins;
    ImObjectPool<ImLinkData> Links;

    ImVector<int> NodeDepthOrder;

    // ui related fields
    ImVec2 Panning = { 0.f, 0.f };

    ImVector<int> SelectedNodeIndices;
    ImVector<int> SelectedLinkIndices;

    ImClickInteractionState ClickInteraction;
    bool NodesMoving = false;
};

struct ImNodesContext
{
    ImNodesEditorContext* DefaultEditorCtx;
    ImNodesEditorContext* EditorCtx;

    // Canvas draw list and helper state
    ImDrawList*   CanvasDrawList;
    ImGuiStorage  NodeIdxToSubmissionIdx;
    ImVector<int> NodeIdxSubmissionOrder;
    ImVector<int> NodeIndicesOverlappingWithMouse;
    ImVector<int> OccludedPinIndices;

    // Canvas extents
    ImVec2 CanvasOriginScreenSpace;
    ImRect CanvasRectScreenSpace;

    // MiniMap state
    ImRect                             MiniMapRectScreenSpace;
    ImVec2                             MiniMapRectSnappingOffset;
    float                              MiniMapZoom;
    ImNodesMiniMapNodeHoveringCallback MiniMapNodeHoveringCallback;
    void*                              MiniMapNodeHoveringCallbackUserData;

    // Debug helpers
    ImNodesScope CurrentScope;

    // Configuration state
    ImNodesContextFlags              Flags;
    ImNodesIO                        Io;
    ImNodesStyle                     Style;
    ImVector<ImNodesColElement>      ColorModifierStack;
    ImVector<ImNodesStyleVarElement> StyleModifierStack;
    ImGuiTextBuffer                  TextBuffer;

    int           CurrentAttributeFlags;
    ImVector<int> AttributeFlagStack;

    // UI element state
    int CurrentNodeIdx;
    int CurrentPinIdx;
    int CurrentAttributeId;

    ImOptionalIndex HoveredNodeIdx;
    ImOptionalIndex InteractiveNodeIdx;
    ImOptionalIndex HoveredLinkIdx;
    ImOptionalIndex HoveredPinIdx;
    int             HoveredPinFlags;

    ImOptionalIndex DeletedLinkIdx;
    ImOptionalIndex SnapLinkIdx;

    // Event helper state
    // TODO: this should be a part of a state machine, and not a member of the global struct.
    // Unclear what parts of the code this relates to.
    int ImNodesUIState;

    int  ActiveAttributeId;
    bool ActiveAttribute;

    // ImGui::IO cache

    ImVec2 MousePos;

    bool  LeftMouseClicked;
    bool  LeftMouseReleased;
    bool  AltMouseClicked;
    bool  LeftMouseDragging;
    bool  AltMouseDragging;
    float AltMouseScrollDelta;
};

namespace ImNodes
{
static inline ImNodesEditorContext& EditorContextGet()
{
    // No editor context was set! Did you forget to call ImNodes::CreateContext()?
    assert(GImNodes->EditorCtx != NULL);
    return *GImNodes->EditorCtx;
}

// [SECTION] ObjectPool implementation

template<typename T>
static inline int ObjectPoolFind(ImObjectPool<T>& objects, const int id)
{
    const int index = objects.IdMap.GetInt(static_cast<ImGuiID>(id), -1);
    return index;
}

template<typename T>
static inline void ObjectPoolUpdate(ImObjectPool<T>& objects)
{
    for (int i = 0; i < objects.InUse.size(); ++i)
    {
        const int id = objects.Pool[i].Id;

        if (!objects.InUse[i] && objects.IdMap.GetInt(id, -1) == i)
        {
            objects.IdMap.SetInt(id, -1);
            objects.FreeList.push_back(i);
            (objects.Pool.Data + i)->~T();
        }
    }
}

template<>
inline void ObjectPoolUpdate(ImObjectPool<ImNodeData>& nodes)
{
    for (int i = 0; i < nodes.InUse.size(); ++i)
    {
        if (nodes.InUse[i])
        {
            nodes.Pool[i].PinIndices.clear();
        }
        else
        {
            const int id = nodes.Pool[i].Id;

            if (nodes.IdMap.GetInt(id, -1) == i)
            {
                // Remove node idx form depth stack the first time we detect that this idx slot is
                // unused
                ImVector<int>& depth_stack = EditorContextGet().NodeDepthOrder;
                const int* const elem = depth_stack.find(i);
                IM_ASSERT(elem != depth_stack.end());
                depth_stack.erase(elem);

                nodes.IdMap.SetInt(id, -1);
                nodes.FreeList.push_back(i);
                (nodes.Pool.Data + i)->~ImNodeData();
            }
        }
    }
}

template<typename T>
static inline void ObjectPoolReset(ImObjectPool<T>& objects)
{
    if (!objects.InUse.empty())
    {
        memset(objects.InUse.Data, 0, objects.InUse.size_in_bytes());
    }
}

template<typename T>
static inline int ObjectPoolFindOrCreateIndex(ImObjectPool<T>& objects, const int id)
{
    int index = objects.IdMap.GetInt(static_cast<ImGuiID>(id), -1);

    // Construct new object
    if (index == -1)
    {
        if (objects.FreeList.empty())
        {
            index = objects.Pool.size();
            IM_ASSERT(objects.Pool.size() == objects.InUse.size());
            const int new_size = objects.Pool.size() + 1;
            objects.Pool.resize(new_size);
            objects.InUse.resize(new_size);
        }
        else
        {
            index = objects.FreeList.back();
            objects.FreeList.pop_back();
        }
        IM_PLACEMENT_NEW(objects.Pool.Data + index) T(id);
        objects.IdMap.SetInt(static_cast<ImGuiID>(id), index);
    }

    // Flag it as used
    objects.InUse[index] = true;

    return index;
}

template<>
inline int ObjectPoolFindOrCreateIndex(ImObjectPool<ImNodeData>& nodes, const int node_id)
{
    int node_idx = nodes.IdMap.GetInt(static_cast<ImGuiID>(node_id), -1);

    // Construct new node
    if (node_idx == -1)
    {
        if (nodes.FreeList.empty())
        {
            node_idx = nodes.Pool.size();
            IM_ASSERT(nodes.Pool.size() == nodes.InUse.size());
            const int new_size = nodes.Pool.size() + 1;
            nodes.Pool.resize(new_size);
            nodes.InUse.resize(new_size);
        }
        else
        {
            node_idx = nodes.FreeList.back();
            nodes.FreeList.pop_back();
        }
        IM_PLACEMENT_NEW(nodes.Pool.Data + node_idx) ImNodeData(node_id);
        nodes.IdMap.SetInt(static_cast<ImGuiID>(node_id), node_idx);

        ImNodesEditorContext& editor = EditorContextGet();
        editor.NodeDepthOrder.push_back(node_idx);
    }

    // Flag node as used
    nodes.InUse[node_idx] = true;

    return node_idx;
}

template<typename T>
static inline T& ObjectPoolFindOrCreateObject(ImObjectPool<T>& objects, const int id)
{
    const int index = ObjectPoolFindOrCreateIndex(objects, id);
    return objects.Pool[index];
}
} // namespace ImNodes
