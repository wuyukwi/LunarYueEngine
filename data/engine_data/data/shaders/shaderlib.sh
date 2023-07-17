/*
 * Copyright 2011-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#ifndef __SHADERLIB_SH__
#define __SHADERLIB_SH__

vec4 encodeRE8(float _r)
{
	float exponent = ceil(log2(_r) );
	return vec4(_r / exp2(exponent)
			, 0.0
			, 0.0
			, (exponent + 128.0) / 255.0
			);
}

float decodeRE8(vec4 _re8)
{
	float exponent = _re8.w * 255.0 - 128.0;
	return _re8.x * exp2(exponent);
}

vec4 encodeRGBE8(vec3 _rgb)
{
	vec4 rgbe8;
	float maxComponent = max(max(_rgb.x, _rgb.y), _rgb.z);
	float exponent = ceil(log2(maxComponent) );
	rgbe8.xyz = _rgb / exp2(exponent);
	rgbe8.w = (exponent + 128.0) / 255.0;
	return rgbe8;
}

vec3 decodeRGBE8(vec4 _rgbe8)
{
	float exponent = _rgbe8.w * 255.0 - 128.0;
	vec3 rgb = _rgbe8.xyz * exp2(exponent);
	return rgb;
}


vec3 encodeNormalUint(vec3 _normal)
{
	return _normal * 0.5 + 0.5;
}

vec3 decodeNormalUint(vec3 _encodedNormal)
{
	return _encodedNormal * 2.0 - 1.0;
}

vec2 octahedronWrap(vec2 _val)
{
	// Reference:
	// Octahedron normal vector encoding
	// http://kriscg.blogspot.com/2014/04/octahedron-normal-vector-encoding.html
	return (1.0 - abs(_val.yx) )
		 * mix(vec2_splat(-1.0), vec2_splat(1.0), vec2(greaterThanEqual(_val.xy, vec2_splat(0.0) ) ) );
}

vec2 encodeNormalOctahedron(vec3 _normal)
{
	_normal /= abs(_normal.x) + abs(_normal.y) + abs(_normal.z);
	_normal.xy = _normal.z >= 0.0 ? _normal.xy : octahedronWrap(_normal.xy);
	_normal.xy = _normal.xy * 0.5 + 0.5;
	return _normal.xy;
}

vec3 decodeNormalOctahedron(vec2 _encodedNormal)
{
	_encodedNormal = _encodedNormal * 2.0 - 1.0;

	vec3 normal;
	normal.z  = 1.0 - abs(_encodedNormal.x) - abs(_encodedNormal.y);
	normal.xy = normal.z >= 0.0 ? _encodedNormal.xy : octahedronWrap(_encodedNormal.xy);
	return normalize(normal);
}

vec3 convertRGB2XYZ(vec3 _rgb)
{
	// Reference:
	// RGB/XYZ Matrices
	// http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
	vec3 xyz;
	xyz.x = dot(vec3(0.4124564, 0.3575761, 0.1804375), _rgb);
	xyz.y = dot(vec3(0.2126729, 0.7151522, 0.0721750), _rgb);
	xyz.z = dot(vec3(0.0193339, 0.1191920, 0.9503041), _rgb);
	return xyz;
}

vec3 convertXYZ2RGB(vec3 _xyz)
{
	vec3 rgb;
	rgb.x = dot(vec3( 3.2404542, -1.5371385, -0.4985314), _xyz);
	rgb.y = dot(vec3(-0.9692660,  1.8760108,  0.0415560), _xyz);
	rgb.z = dot(vec3( 0.0556434, -0.2040259,  1.0572252), _xyz);
	return rgb;
}

vec3 convertXYZ2Yxy(vec3 _xyz)
{
	// Reference:
	// http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_xyY.html
	float inv = 1.0/dot(_xyz, vec3(1.0, 1.0, 1.0) );
	return vec3(_xyz.y, _xyz.x*inv, _xyz.y*inv);
}

vec3 convertYxy2XYZ(vec3 _Yxy)
{
	// Reference:
	// http://www.brucelindbloom.com/index.html?Eqn_xyY_to_XYZ.html
	vec3 xyz;
	xyz.x = _Yxy.x*_Yxy.y/_Yxy.z;
	xyz.y = _Yxy.x;
	xyz.z = _Yxy.x*(1.0 - _Yxy.y - _Yxy.z)/_Yxy.z;
	return xyz;
}

vec3 convertRGB2Yxy(vec3 _rgb)
{
	return convertXYZ2Yxy(convertRGB2XYZ(_rgb) );
}

vec3 convertYxy2RGB(vec3 _Yxy)
{
	return convertXYZ2RGB(convertYxy2XYZ(_Yxy) );
}

vec3 convertRGB2Yuv(vec3 _rgb)
{
	vec3 yuv;
	yuv.x = dot(_rgb, vec3(0.299, 0.587, 0.114) );
	yuv.y = (_rgb.x - yuv.x)*0.713 + 0.5;
	yuv.z = (_rgb.z - yuv.x)*0.564 + 0.5;
	return yuv;
}

vec3 convertYuv2RGB(vec3 _yuv)
{
	vec3 rgb;
	rgb.x = _yuv.x + 1.403*(_yuv.y-0.5);
	rgb.y = _yuv.x - 0.344*(_yuv.y-0.5) - 0.714*(_yuv.z-0.5);
	rgb.z = _yuv.x + 1.773*(_yuv.z-0.5);
	return rgb;
}

vec3 convertRGB2YIQ(vec3 _rgb)
{
	vec3 yiq;
	yiq.x = dot(vec3(0.299,     0.587,     0.114   ), _rgb);
	yiq.y = dot(vec3(0.595716, -0.274453, -0.321263), _rgb);
	yiq.z = dot(vec3(0.211456, -0.522591,  0.311135), _rgb);
	return yiq;
}

vec3 convertYIQ2RGB(vec3 _yiq)
{
	vec3 rgb;
	rgb.x = dot(vec3(1.0,  0.9563,  0.6210), _yiq);
	rgb.y = dot(vec3(1.0, -0.2721, -0.6474), _yiq);
	rgb.z = dot(vec3(1.0, -1.1070,  1.7046), _yiq);
	return rgb;
}

vec3 toLinear(vec3 _rgb)
{
	return pow(abs(_rgb), vec3_splat(2.2) );
}

vec4 toLinear(vec4 _rgba)
{
	return vec4(toLinear(_rgba.xyz), _rgba.w);
}

vec3 toLinearAccurate(vec3 _rgb)
{
	vec3 lo = _rgb / 12.92;
	vec3 hi = pow( (_rgb + 0.055) / 1.055, vec3_splat(2.4) );
	vec3 rgb = mix(hi, lo, vec3(lessThanEqual(_rgb, vec3_splat(0.04045) ) ) );
	return rgb;
}

vec4 toLinearAccurate(vec4 _rgba)
{
	return vec4(toLinearAccurate(_rgba.xyz), _rgba.w);
}

float toGamma(float _r)
{
	return pow(abs(_r), 1.0/2.2);
}

vec3 toGamma(vec3 _rgb)
{
	return pow(abs(_rgb), vec3_splat(1.0/2.2) );
}

vec4 toGamma(vec4 _rgba)
{
	return vec4(toGamma(_rgba.xyz), _rgba.w);
}

vec3 toGammaAccurate(vec3 _rgb)
{
	vec3 lo  = _rgb * 12.92;
	vec3 hi  = pow(abs(_rgb), vec3_splat(1.0/2.4) ) * 1.055 - 0.055;
	vec3 rgb = mix(hi, lo, vec3(lessThanEqual(_rgb, vec3_splat(0.0031308) ) ) );
	return rgb;
}

vec4 toGammaAccurate(vec4 _rgba)
{
	return vec4(toGammaAccurate(_rgba.xyz), _rgba.w);
}

vec3 toReinhard(vec3 _rgb)
{
	return toGamma(_rgb/(_rgb+vec3_splat(1.0) ) );
}

vec4 toReinhard(vec4 _rgba)
{
	return vec4(toReinhard(_rgba.xyz), _rgba.w);
}

vec3 toFilmic(vec3 _rgb)
{
	_rgb = max(vec3_splat(0.0), _rgb - 0.004);
	_rgb = (_rgb*(6.2*_rgb + 0.5) ) / (_rgb*(6.2*_rgb + 1.7) + 0.06);
	return _rgb;
}

vec4 toFilmic(vec4 _rgba)
{
	return vec4(toFilmic(_rgba.xyz), _rgba.w);
}

vec3 toAcesFilmic(vec3 _rgb)
{
	// Reference:
	// ACES Filmic Tone Mapping Curve
	// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
	float aa = 2.51f;
	float bb = 0.03f;
	float cc = 2.43f;
	float dd = 0.59f;
	float ee = 0.14f;
	return saturate( (_rgb*(aa*_rgb + bb) )/(_rgb*(cc*_rgb + dd) + ee) );
}

vec4 toAcesFilmic(vec4 _rgba)
{
	return vec4(toAcesFilmic(_rgba.xyz), _rgba.w);
}

vec3 luma(vec3 _rgb)
{
	float yy = dot(vec3(0.2126729, 0.7151522, 0.0721750), _rgb);
	return vec3_splat(yy);
}

vec4 luma(vec4 _rgba)
{
	return vec4(luma(_rgba.xyz), _rgba.w);
}

vec3 conSatBri(vec3 _rgb, vec3 _csb)
{
	vec3 rgb = _rgb * _csb.z;
	rgb = mix(luma(rgb), rgb, _csb.y);
	rgb = mix(vec3_splat(0.5), rgb, _csb.x);
	return rgb;
}

vec4 conSatBri(vec4 _rgba, vec3 _csb)
{
	return vec4(conSatBri(_rgba.xyz, _csb), _rgba.w);
}

vec3 posterize(vec3 _rgb, float _numColors)
{
	return floor(_rgb*_numColors) / _numColors;
}

vec4 posterize(vec4 _rgba, float _numColors)
{
	return vec4(posterize(_rgba.xyz, _numColors), _rgba.w);
}

vec3 sepia(vec3 _rgb)
{
	vec3 color;
	color.x = dot(_rgb, vec3(0.393, 0.769, 0.189) );
	color.y = dot(_rgb, vec3(0.349, 0.686, 0.168) );
	color.z = dot(_rgb, vec3(0.272, 0.534, 0.131) );
	return color;
}

vec4 sepia(vec4 _rgba)
{
	return vec4(sepia(_rgba.xyz), _rgba.w);
}

vec3 blendOverlay(vec3 _base, vec3 _blend)
{
	vec3 lt = 2.0 * _base * _blend;
	vec3 gte = 1.0 - 2.0 * (1.0 - _base) * (1.0 - _blend);
	return mix(lt, gte, step(vec3_splat(0.5), _base) );
}

vec4 blendOverlay(vec4 _base, vec4 _blend)
{
	return vec4(blendOverlay(_base.xyz, _blend.xyz), _base.w);
}

vec3 adjustHue(vec3 _rgb, float _hue)
{
	vec3 yiq = convertRGB2YIQ(_rgb);
	float angle = _hue + atan2(yiq.z, yiq.y);
	float len = length(yiq.yz);
	return convertYIQ2RGB(vec3(yiq.x, len*cos(angle), len*sin(angle) ) );
}

vec4 packFloatToRgba(float _value)
{
	const vec4 shift = vec4(256 * 256 * 256, 256 * 256, 256, 1.0);
	const vec4 mask = vec4(0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
	vec4 comp = fract(_value * shift);
	comp -= comp.xxyz * mask;
	return comp;
}

float unpackRgbaToFloat(vec4 _rgba)
{
	const vec4 shift = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
	return dot(_rgba, shift);
}

vec2 packHalfFloat(float _value)
{
	const vec2 shift = vec2(256, 1.0);
	const vec2 mask = vec2(0, 1.0 / 256.0);
	vec2 comp = fract(_value * shift);
	comp -= comp.xx * mask;
	return comp;
}

float unpackHalfFloat(vec2 _rg)
{
	const vec2 shift = vec2(1.0 / 256.0, 1.0);
	return dot(_rg, shift);
}

float random(vec2 _uv)
{
	return fract(sin(dot(_uv.xy, vec2(12.9898, 78.233) ) ) * 43758.5453);
}

vec3 fixCubeLookup(vec3 _v, float _lod, float _topLevelCubeSize)
{
	// Reference:
	// Seamless cube-map filtering
	// http://the-witness.net/news/2012/02/seamless-cube-map-filtering/
	float ax = abs(_v.x);
	float ay = abs(_v.y);
	float az = abs(_v.z);
	float vmax = max(max(ax, ay), az);
	float scale = 1.0 - exp2(_lod) / _topLevelCubeSize;
	if (ax != vmax) { _v.x *= scale; }
	if (ay != vmax) { _v.y *= scale; }
	if (az != vmax) { _v.z *= scale; }
	return _v;
}

vec3 getTangentSpaceNormal( sampler2D bumpTexture, vec2 texCoords, float bumpiness )
{
    vec3 normal = texture2D(bumpTexture, texCoords).xyz;
  	normal = normal * 2.0f - 1.0f;

#ifdef NORMAL_MAP_2CHANNEL
  	normal.z  = sqrt(1.0 - dot(normal.xy, normal.xy) );
#elif NORMAL_MAP_Y_UP
  	normal.y = -normal.y;
#endif
	  normal.xy *= bumpiness;
    return normalize(normal);
}

float dither5x5(vec2 fragCoord)
{
float aa = 0.0f;
float Dither5 = fract( ( fragCoord.x + fragCoord.y * 2.0f - 1.5f + aa-2.5f ) / 5.0f );
float Noise = fract( dot( vec2( 171.0f, 231.0f ) / 71.0f, fragCoord.xy ) );
float Dither = ( Dither5 * 5.0f + Noise ) * (1.0f / 6.0f);
return Dither;
}

float dither16x16(vec2 fragCoord)
{
  int x = int(mod(fragCoord.x, 16));
  int y = int(mod(fragCoord.y, 16));

CONST(int) bayer_matrix[16*16] =
#if defined BGFX_SHADER_LANGUAGE_HLSL || defined BGFX_SHADER_LANGUAGE_SPIRV
{
#else
int[](
#endif
     0,192, 48,240, 12,204, 60,252,  3,195, 51,243, 15,207, 63,255, 128, 64,176,112,140, 76,188,124,131, 67,179,115,143, 79,191,127, 32,224, 16,208, 44,236, 28,220, 35,227, 19,211, 47,239, 31,223, 160, 96,144, 80,172,108,156, 92,163, 99,147, 83,175,111,159, 95, 8,200, 56,248,  4,196, 52,244, 11,203, 59,251,  7,199, 55,247, 136, 72,184,120,132, 68,180,116,139, 75,187,123,135, 71,183,119, 40,232, 24,216, 36,228, 20,212, 43,235, 27,219, 39,231, 23,215, 168,104,152, 88,164,100,148, 84,171,107,155, 91,167,103,151, 87, 2,194, 50,242, 14,206, 62,254,  1,193, 49,241, 13,205, 61,253, 130, 66,178,114,142, 78,190,126,129, 65,177,113,141, 77,189,125, 34,226, 18,210, 46,238, 30,222, 33,225, 17,209, 45,237, 29,221, 162, 98,146, 82,174,110,158, 94,161, 97,145, 81,173,109,157, 93, 10,202, 58,250,  6,198, 54,246,  9,201, 57,249,  5,197, 53,245, 138, 74,186,122,134, 70,182,118,137, 73,185,121,133, 69,181,117, 42,234, 26,218, 38,230, 22,214, 41,233, 25,217, 37,229, 21,213, 170,106,154, 90,166,102,150, 86,169,105,153, 89,165,101,149, 85
#if defined BGFX_SHADER_LANGUAGE_HLSL  || defined BGFX_SHADER_LANGUAGE_SPIRV
};
#else
);
#endif

float limit = (bayer_matrix[x + y * 16])/256.0f;
return limit;
}

float dither8x8(vec2 fragCoord)
{

int x = int(mod(fragCoord.x, 8));
int y = int(mod(fragCoord.y, 8));

/* 8x8 Bayer ordered dithering */
/* pattern. Each input pixel */
/* is scaled to the 0..63 range */
/* before looking in this table */
/* to determine the action. */
const int bayer_matrix8x8[8*8] =
#if defined BGFX_SHADER_LANGUAGE_HLSL || defined BGFX_SHADER_LANGUAGE_SPIRV
{
#else
int[](
#endif
     0, 32, 8,  40, 2,  34, 10, 42, 48, 16, 56, 24, 50, 18, 58, 26, 12, 44, 4,  36, 14, 46, 6,  38, 60, 28, 52, 20, 62, 30, 54, 22, 3, 35, 11, 43, 1,  33, 9,  41, 51, 19, 59, 27, 49, 17, 57, 25, 15, 47, 7,  39, 13, 45, 5,  37, 63, 31, 55, 23, 61, 29, 53, 21
#if defined BGFX_SHADER_LANGUAGE_HLSL || defined BGFX_SHADER_LANGUAGE_SPIRV
};
#else
);
#endif

float limit = (bayer_matrix8x8[x + y * 8])/64.0f;
return limit;
}


float toClipSpaceDepth(float _depthTextureZ)
{
#if BGFX_SHADER_LANGUAGE_HLSL
	return _depthTextureZ;
#else
	return _depthTextureZ * 2.0 - 1.0;
#endif // BGFX_SHADER_LANGUAGE_HLSL
}

vec3 clipTransform(vec3 clip)
{
#if BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_METAL
	clip.y = -clip.y;
#endif // BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_METAL
	return clip;
}

vec3 clipToWorld(mat4 _invViewProj, vec3 _clipPos)
{
	vec4 wpos = mul(_invViewProj, vec4(_clipPos, 1.0) );
	return wpos.xyz / wpos.w;
}

mat3 invert_3x3( mat3 M )
{
	float det = dot( cross(M[0], M[1]), M[2] );
	mat3 T = transpose(M);
	return mat3( cross(T[1], T[2]), cross(T[2], T[0]), cross(T[0], T[1]) ) / det;
}

mat3 invert_3x3( mat4 M4 )
{
#if BGFX_SHADER_LANGUAGE_HLSL
	mat3 M = (mat3)M4;
#else
	mat3 M = mat3(M4);
#endif
	return invert_3x3(M);
}

mat3 constructTangentToWorldSpaceMatrix( vec3 T, vec3 B, vec3 N )
{
	mat3 TBN = mat3(
			normalize(T),
			normalize(B),
			normalize(N)
			);
#if BGFX_SHADER_LANGUAGE_HLSL
	TBN = transpose( TBN );
#endif
	return TBN;
}

mat3 calculateInverseTranspose( mat4 m )
{
	return transpose(invert_3x3(m));
}

#if BGFX_SHADER_TYPE_FRAGMENT
mat3 computeTangentToWorldSpaceMatrix( vec3 N, vec3 p, vec2 uv )
{
	// Compute tangentToWorldSpace matrix
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p );
	vec2 duv1 = dFdx( uv );
	vec2 duv2 = dFdy( uv );

	/* solve the linear system */
	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 );
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	/* construct a scale-invariant frame */
	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
	mat3 TBN =  mat3( T * invmax, B * invmax, N );
#if BGFX_SHADER_LANGUAGE_HLSL
	TBN = transpose( TBN );
#endif
	return TBN;
}
#endif


vec3 evalSh(vec3 _dir)
{
#	define k01 0.2820947918 // sqrt( 1/PI)/2
#	define k02 0.4886025119 // sqrt( 3/PI)/2
#	define k03 1.0925484306 // sqrt(15/PI)/2
#	define k04 0.3153915652 // sqrt( 5/PI)/4
#	define k05 0.5462742153 // sqrt(15/PI)/4

	vec3 shEnv[9];
	shEnv[0] = vec3( 0.967757057878229854,  0.976516067990363390,  0.891218272348969998); /* Band 0 */
	shEnv[1] = vec3(-0.384163503608655643, -0.423492289131209787, -0.425532726148547868); /* Band 1 */
	shEnv[2] = vec3( 0.055906294587354334,  0.056627436881069373,  0.069969936396987467);
	shEnv[3] = vec3( 0.120985157386215209,  0.119297994074027414,  0.117111965829213599);
	shEnv[4] = vec3(-0.176711633774331106, -0.170331404095516392, -0.151345020570876621); /* Band 2 */
	shEnv[5] = vec3(-0.124682114349692147, -0.119340785411183953, -0.096300354204368860);
	shEnv[6] = vec3( 0.001852378550138503, -0.032592784164597745, -0.088204495001329680);
	shEnv[7] = vec3( 0.296365482782109446,  0.281268696656263029,  0.243328223888495510);
	shEnv[8] = vec3(-0.079826665303240341, -0.109340956251195970, -0.157208859664677764);

	vec3 nn = _dir.zxy;


	float sh[9];
	sh[0] =  k01;
	sh[1] = -k02*nn.y;
	sh[2] =  k02*nn.z;
	sh[3] = -k02*nn.x;
	sh[4] =  k03*nn.y*nn.x;
	sh[5] = -k03*nn.y*nn.z;
	sh[6] =  k04*(3.0*nn.z*nn.z-1.0);
	sh[7] = -k03*nn.x*nn.z;
	sh[8] =  k05*(nn.x*nn.x-nn.y*nn.y);

	vec3 rgb = vec3_splat(0.0);
	rgb += shEnv[0] * sh[0] * 1.0;
	rgb += shEnv[1] * sh[1] * 2.0/2.5;
	rgb += shEnv[2] * sh[2] * 2.0/2.5;
	rgb += shEnv[3] * sh[3] * 2.0/2.5;
	rgb += shEnv[4] * sh[4] * 1.0/2.5;
	rgb += shEnv[5] * sh[5] * 0.5;
	rgb += shEnv[6] * sh[6] * 0.5;
	rgb += shEnv[7] * sh[7] * 0.5;
	rgb += shEnv[8] * sh[8] * 0.5;

	return rgb;
}

#endif // __SHADERLIB_SH__
