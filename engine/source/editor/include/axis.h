#pragma once

#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_type.h"

namespace LunarYue
{
    // EditorTranslationAxis �N���X�́A�ҏW�I�u�W�F�N�g�̈ړ�����\���܂��B
    class EditorTranslationAxis : public RenderEntity
    {
    public:
        EditorTranslationAxis();
        RenderMeshData m_mesh_data;
    };

    // EditorRotationAxis �N���X�́A�ҏW�I�u�W�F�N�g�̉�]����\���܂��B
    class EditorRotationAxis : public RenderEntity
    {
    public:
        EditorRotationAxis();
        RenderMeshData m_mesh_data;
    };

    // EditorScaleAxis �N���X�́A�ҏW�I�u�W�F�N�g�̊g��k������\���܂��B
    class EditorScaleAxis : public RenderEntity
    {
    public:
        EditorScaleAxis();
        RenderMeshData m_mesh_data;
    };
} // namespace LunarYue
