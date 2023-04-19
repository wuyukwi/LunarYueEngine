#pragma once

#define LunarYue_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define LunarYue_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define LunarYue_PIN(a, min_value, max_value) LunarYue_MIN(max_value, LunarYue_MAX(a, min_value))

#define LunarYue_VALID_INDEX(idx, range) (((idx) >= 0) && ((idx) < (range)))
#define LunarYue_PIN_INDEX(idx, range) LunarYue_PIN(idx, 0, (range)-1)

#define LunarYue_SIGN(x) ((((x) > 0.0f) ? 1.0f : 0.0f) + (((x) < 0.0f) ? -1.0f : 0.0f))
