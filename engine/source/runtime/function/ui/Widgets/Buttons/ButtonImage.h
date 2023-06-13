#pragma once

#include "runtime/core/math/vector2.h"

#include "function/ui/Widgets/Buttons/AButton.h"
#include "runtime/function/ui/Internal/TextureID.h"
#include "runtime/function/ui/Types/Color.h"

namespace LunarYue::UI::Widgets::Buttons
{
    /**
     * Button widget with an image
     */
    class ButtonImage : public AButton
    {
    public:
        /**
         * Constructor
         * @param p_textureID
         * @param p_size
         */
        ButtonImage(uint32_t p_textureID, const Vector2& p_size);

    protected:
        void _Draw_Impl() override;

    public:
        bool disabled = false;

        LunarYue::UI::Types::Color background = {0, 0, 0, 0};
        LunarYue::UI::Types::Color tint       = {1, 1, 1, 1};

        Internal::TextureID textureID;
        Vector2             size;
    };
} // namespace LunarYue::UI::Widgets::Buttons