#pragma once

#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_type.h"

namespace LunarYue
{
    // EditorTranslationAxis クラスは、編集オブジェクトの移動軸を表します。
    class EditorTranslationAxis : public RenderEntity
    {
    public:
        EditorTranslationAxis();
        RenderMeshData m_mesh_data;
    };

    // EditorRotationAxis クラスは、編集オブジェクトの回転軸を表します。
    class EditorRotationAxis : public RenderEntity
    {
    public:
        EditorRotationAxis();
        RenderMeshData m_mesh_data;
    };

    // EditorScaleAxis クラスは、編集オブジェクトの拡大縮小軸を表します。
    class EditorScaleAxis : public RenderEntity
    {
    public:
        EditorScaleAxis();
        RenderMeshData m_mesh_data;
    };
} // namespace LunarYue
