static const uint8_t fs_debugdraw_fill_texture_glsl[204] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x73, // FSH............s
	0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, // _texColor.......
	0x00, 0x00, 0x00, 0xa4, 0x00, 0x00, 0x00, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x76, // .......varying v
	0x65, 0x63, 0x34, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x3b, 0x0a, 0x76, 0x61, // ec4 v_color0;.va
	0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, // rying vec2 v_tex
	0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, // coord0;.uniform 
	0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x32, 0x44, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x43, // sampler2D s_texC
	0x6f, 0x6c, 0x6f, 0x72, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, // olor;.void main 
	0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, // ().{.  gl_FragCo
	0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x44, // lor = (texture2D
	0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x2c, 0x20, 0x76, 0x5f, //  (s_texColor, v_
	0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x20, 0x2a, 0x20, 0x76, 0x5f, 0x63, // texcoord0) * v_c
	0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x29, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,                         // olor0);.}...
};
static const uint8_t fs_debugdraw_fill_texture_essl[261] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x73, // FSH............s
	0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, // _texColor.......
	0x00, 0x00, 0x00, 0xdd, 0x00, 0x00, 0x00, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x68, // .......varying h
	0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, // ighp vec4 v_colo
	0x72, 0x30, 0x3b, 0x0a, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x68, 0x69, 0x67, 0x68, // r0;.varying high
	0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, // p vec2 v_texcoor
	0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x73, 0x61, 0x6d, 0x70, // d0;.uniform samp
	0x6c, 0x65, 0x72, 0x32, 0x44, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, // ler2D s_texColor
	0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, // ;.void main ().{
	0x0a, 0x20, 0x20, 0x6c, 0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, // .  lowp vec4 tmp
	0x76, 0x61, 0x72, 0x5f, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // var_1;.  tmpvar_
	0x31, 0x20, 0x3d, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x44, 0x20, 0x28, 0x73, // 1 = texture2D (s
	0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x2c, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, // _texColor, v_tex
	0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, // coord0);.  gl_Fr
	0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, // agColor = (tmpva
	0x72, 0x5f, 0x31, 0x20, 0x2a, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x29, 0x3b, // r_1 * v_color0);
	0x0a, 0x7d, 0x0a, 0x0a, 0x00,                                                                   // .}...
};
static const uint8_t fs_debugdraw_fill_texture_spv[863] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x73, // FSH............s
	0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, // _texColor0......
	0x02, 0x22, 0x00, 0x34, 0x03, 0x00, 0x00, 0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0b, // .".4.....#......
	0x00, 0x08, 0x00, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, // ................
	0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, // ...........GLSL.
	0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, // std.450.........
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, // ................
	0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x53, // ...main....O...S
	0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, // ...`............
	0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x05, // ................
	0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, // .......main.....
	0x00, 0x07, 0x00, 0x23, 0x00, 0x00, 0x00, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, // ...#...s_texColo
	0x72, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00, 0x26, // rSampler.......&
	0x00, 0x00, 0x00, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x54, 0x65, 0x78, // ...s_texColorTex
	0x74, 0x75, 0x72, 0x65, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x76, // ture.......O...v
	0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x53, // _color0........S
	0x00, 0x00, 0x00, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x00, 0x05, // ...v_texcoord0..
	0x00, 0x06, 0x00, 0x60, 0x00, 0x00, 0x00, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, // ...`...bgfx_Frag
	0x44, 0x61, 0x74, 0x61, 0x30, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x22, // Data0..G...#..."
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x21, // .......G...#...!
	0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x26, 0x00, 0x00, 0x00, 0x22, // .......G...&..."
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x26, 0x00, 0x00, 0x00, 0x21, // .......G...&...!
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x1e, // .......G...O....
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x53, 0x00, 0x00, 0x00, 0x1e, // .......G...S....
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x60, 0x00, 0x00, 0x00, 0x1e, // .......G...`....
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, // ...............!
	0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x06, // ................
	0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x19, // ........... ....
	0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x02, // ................
	0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x04, // ................
	0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, // ... ..."........
	0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, // ...;..."...#....
	0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, // ... ...%........
	0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, // ...;...%...&....
	0x00, 0x00, 0x00, 0x1b, 0x00, 0x03, 0x00, 0x32, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, // .......2....... 
	0x00, 0x04, 0x00, 0x4b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3b, // ...K...........;
	0x00, 0x04, 0x00, 0x4b, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, // ...K...O....... 
	0x00, 0x04, 0x00, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x3b, // ...R...........;
	0x00, 0x04, 0x00, 0x52, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, // ...R...S....... 
	0x00, 0x04, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3b, // ..._...........;
	0x00, 0x04, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x36, // ..._...`.......6
	0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, // ................
	0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x06, // ...........=....
	0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, // ...$...#...=....
	0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x0d, // ...'...&...=....
	0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x0b, // ...P...O...=....
	0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x56, 0x00, 0x05, 0x00, 0x32, // ...T...S...V...2
	0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x57, // .......'...$...W
	0x00, 0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x54, // ...............T
	0x00, 0x00, 0x00, 0x85, 0x00, 0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0x92, // ................
	0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x60, 0x00, 0x00, 0x00, 0x82, // ...P...>...`....
	0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,       // .......8.......
};
static const uint8_t fs_debugdraw_fill_texture_dx11[423] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x73, // FSH............s
	0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, // _texColor0......
	0x00, 0x00, 0x00, 0x7c, 0x01, 0x00, 0x00, 0x44, 0x58, 0x42, 0x43, 0x9f, 0xc3, 0xdd, 0xee, 0xdf, // ...|...DXBC.....
	0xe0, 0xb0, 0x54, 0xaa, 0x51, 0xec, 0xb4, 0x2a, 0xf6, 0x8e, 0x43, 0x01, 0x00, 0x00, 0x00, 0x7c, // ..T.Q..*..C....|
	0x01, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0xd4, // .......,........
	0x00, 0x00, 0x00, 0x49, 0x53, 0x47, 0x4e, 0x6c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, // ...ISGNl........
	0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, // ...P............
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, // ................
	0x0f, 0x00, 0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, // ...b............
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, // ...........SV_PO
	0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x00, 0x43, 0x4f, 0x4c, 0x4f, 0x52, 0x00, 0x54, 0x45, 0x58, // SITION.COLOR.TEX
	0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x2c, 0x00, 0x00, 0x00, 0x01, // COORD..OSGN,....
	0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ....... ........
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, // ...............S
	0x56, 0x5f, 0x54, 0x41, 0x52, 0x47, 0x45, 0x54, 0x00, 0xab, 0xab, 0x53, 0x48, 0x45, 0x58, 0xa0, // V_TARGET...SHEX.
	0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x6a, 0x08, 0x00, 0x01, 0x5a, // ...P...(...j...Z
	0x00, 0x00, 0x03, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x18, 0x00, 0x04, 0x00, // ....`......X....
	0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03, 0xf2, // p......UU..b....
	0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x02, // .......b...2....
	0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, // ...e.... ......h
	0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x8b, 0xc2, 0x00, 0x00, 0x80, 0x43, // .......E.......C
	0x55, 0x15, 0x00, 0xf2, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x02, // U..........F....
	0x00, 0x00, 0x00, 0x46, 0x7e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00, 0x00, // ...F~.......`...
	0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x07, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, // ...8.... ......F
	0x0e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3e, // .......F.......>
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,                                                       // .......
};
static const uint8_t fs_debugdraw_fill_texture_mtl[644] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x11, 0x73, // FSH............s
	0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, // _texColorSampler
	0x11, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x73, 0x5f, 0x74, 0x65, 0x78, // ...........s_tex
	0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x11, 0x01, 0xff, 0xff, // ColorTexture....
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, // .......s_texColo
	0x72, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x02, 0x00, 0x00, 0x23, // r..........!...#
	0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, 0x6d, 0x65, 0x74, 0x61, 0x6c, 0x5f, 0x73, // include <metal_s
	0x74, 0x64, 0x6c, 0x69, 0x62, 0x3e, 0x0a, 0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, // tdlib>.#include 
	0x3c, 0x73, 0x69, 0x6d, 0x64, 0x2f, 0x73, 0x69, 0x6d, 0x64, 0x2e, 0x68, 0x3e, 0x0a, 0x0a, 0x75, // <simd/simd.h>..u
	0x73, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x73, 0x70, 0x61, 0x63, 0x65, 0x20, 0x6d, // sing namespace m
	0x65, 0x74, 0x61, 0x6c, 0x3b, 0x0a, 0x0a, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x78, 0x6c, // etal;..struct xl
	0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x0a, 0x7b, 0x0a, // atMtlMain_out.{.
	0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, //     float4 bgfx_
	0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, 0x61, 0x30, 0x20, 0x5b, 0x5b, 0x63, 0x6f, 0x6c, 0x6f, // FragData0 [[colo
	0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x73, 0x74, 0x72, 0x75, // r(0)]];.};..stru
	0x63, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x69, // ct xlatMtlMain_i
	0x6e, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x76, // n.{.    float4 v
	0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x20, 0x5b, 0x5b, 0x75, 0x73, 0x65, 0x72, 0x28, 0x6c, // _color0 [[user(l
	0x6f, 0x63, 0x6e, 0x30, 0x29, 0x5d, 0x5d, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, // ocn0)]];.    flo
	0x61, 0x74, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, // at2 v_texcoord0 
	0x5b, 0x5b, 0x75, 0x73, 0x65, 0x72, 0x28, 0x6c, 0x6f, 0x63, 0x6e, 0x31, 0x29, 0x5d, 0x5d, 0x3b, // [[user(locn1)]];
	0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x66, 0x72, 0x61, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x78, 0x6c, // .};..fragment xl
	0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x20, 0x78, 0x6c, // atMtlMain_out xl
	0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x28, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, // atMtlMain(xlatMt
	0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x69, 0x6e, 0x20, 0x69, 0x6e, 0x20, 0x5b, 0x5b, 0x73, 0x74, // lMain_in in [[st
	0x61, 0x67, 0x65, 0x5f, 0x69, 0x6e, 0x5d, 0x5d, 0x2c, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, // age_in]], textur
	0x65, 0x32, 0x64, 0x3c, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3e, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, // e2d<float> s_tex
	0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x5b, 0x5b, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x28, // Color [[texture(
	0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x73, 0x5f, // 0)]], sampler s_
	0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, // texColorSampler 
	0x5b, 0x5b, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x29, 0x0a, // [[sampler(0)]]).
	0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, // {.    xlatMtlMai
	0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x20, 0x6f, 0x75, 0x74, 0x20, 0x3d, 0x20, 0x7b, 0x7d, 0x3b, 0x0a, // n_out out = {};.
	0x20, 0x20, 0x20, 0x20, 0x6f, 0x75, 0x74, 0x2e, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, //     out.bgfx_Fra
	0x67, 0x44, 0x61, 0x74, 0x61, 0x30, 0x20, 0x3d, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, // gData0 = s_texCo
	0x6c, 0x6f, 0x72, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, // lor.sample(s_tex
	0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x69, 0x6e, // ColorSampler, in
	0x2e, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x20, 0x2a, 0x20, // .v_texcoord0) * 
	0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, // in.v_color0;.   
	0x20, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x20, 0x6f, 0x75, 0x74, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, //  return out;.}..
	0x00, 0x00, 0x20, 0x00,                                                                         // .. .
};
extern const uint8_t* fs_debugdraw_fill_texture_pssl;
extern const uint32_t fs_debugdraw_fill_texture_pssl_size;
