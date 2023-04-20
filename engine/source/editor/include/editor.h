#pragma once

#include "runtime/core/math/vector2.h"

#include <memory>

namespace LunarYue
{
    class EditorUI;
    class LunarYueEngine;

    // LunarYueEditorクラスは、LunarYueエンジンのエディタ機能を提供します。
    // このクラスは、EditorUIとLunarYueEngineと連携して動作します。
    class LunarYueEditor
    {
        friend class EditorUI;

    public:
        // コンストラクタ
        LunarYueEditor();

        // デストラクタ
        virtual ~LunarYueEditor();

        // エンジンの初期化
        // engine_runtime: LunarYueEngineインスタンスへのポインタ
        void initialize(LunarYueEngine* engine_runtime);

        // エディタの状態をクリア
        void clear();

        // エディタの実行
        void run();

    protected:
        // EditorUIのインスタンスへの共有ポインタ
        std::shared_ptr<EditorUI> m_editor_ui;

        // LunarYueEngineインスタンスへのポインタ
        LunarYueEngine* m_engine_runtime {nullptr};
    };
} // namespace LunarYue
