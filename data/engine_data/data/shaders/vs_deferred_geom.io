vec3 a_position  : POSITION;
vec4 a_normal    : NORMAL;
vec4 a_tangent   : TANGENT;
vec4 a_bitangent : BITANGENT;
vec2 a_texcoord0 : TEXCOORD0;

vec2 v_texcoord0 : TEXCOORD0 = vec2(0.0, 0.0);
vec3 v_pos       : TEXCOORD1 = vec3(0.0, 0.0, 0.0);
vec3 v_wpos      : TEXCOORD2 = vec3(0.0, 0.0, 0.0);
vec3 v_wnormal    : NORMAL    = vec3(0.0, 0.0, 1.0);
vec3 v_wtangent   : TANGENT   = vec3(1.0, 0.0, 0.0);
vec3 v_wbitangent : BITANGENT  = vec3(0.0, 1.0, 0.0);