#pragma once

#include <chrono>
#include <string>
#include <unordered_set>

namespace LunarYue
{
    extern bool                            g_is_editor_mode;
    extern std::unordered_set<std::string> g_editor_tick_component_types;

    class LunarYueEngine
    {
        friend class LunarYueEditor;

        // FPS �v�Z�̂��߂̒萔
        static const float s_fps_alpha;

    public:
        // �G���W���̊J�n
        static void startEngine(const std::string& config_file_path);
        // �G���W���̃V���b�g�_�E��
        void shutdownEngine();

        // ������
        void initialize();
        // �N���A
        void clear();

        // �I���t���O�̎擾
        bool isQuit() const { return m_is_quit; }
        // ���s
        void run();
        // 1�t���[�����Ƃ̍X�V
        bool tickOneFrame(float delta_time);

        // ���݂� FPS �̎擾
        int getFPS() const { return m_fps; }

    protected:
        // �_���X�V
        void logicalTick(float delta_time);
        // �����_�����O�X�V
        bool rendererTick(float delta_time);

        // FPS �v�Z
        void calculateFPS(float delta_time);

        /**
         *  �e�t���[����1�񂾂��Ăяo�����
         */
        // �f���^���Ԃ̌v�Z
        float calculateDeltaTime();

    protected:
        // �I���t���O
        bool m_is_quit {false};

        // �O��̍X�V���_
        std::chrono::steady_clock::time_point m_last_tick_time_point {std::chrono::steady_clock::now()};

        // ���σt���[������
        float m_average_duration {0.f};
        // �t���[���J�E���g
        int m_frame_count {0};
        // FPS
        int m_fps {0};
    };

} // namespace LunarYue
