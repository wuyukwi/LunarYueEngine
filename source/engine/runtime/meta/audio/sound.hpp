#pragma once

#include <core/audio/sound.h>
#include <core/reflection/reflection.h>
#include <core/serialization/serialization.h>

namespace audio
{
REFLECT_EXTERN(sound_info);
SAVE_EXTERN(sound_info);
LOAD_EXTERN(sound_info);
SAVE_EXTERN(sound_data);
LOAD_EXTERN(sound_data);
}
