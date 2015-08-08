#define PERSPECTIVE_SPRITE_SHADER \
	"																\n\
uniform vec4 g_v4Params[2];											\n\
attribute vec4 a_position;											\n\
attribute vec2 a_texCoord;											\n\
attribute vec4 a_color;												\n\
																	\n\
#ifdef GL_ES														\n\
varying lowp vec4 v_fragmentColor;									\n\
varying mediump vec2 v_texCoord;									\n\
#else																\n\
varying vec4 v_fragmentColor;										\n\
varying vec2 v_texCoord;											\n\
#endif																\n\
																	\n\
void main()															\n\
{																	\n\
	vec4 v4Pos;														\n\
	v4Pos.xy = a_position.xy - g_v4Params[0].xy;					\n\
	v4Pos.zw = vec2(0,1);											\n\
	vec3 uv = cross(g_v4Params[1].xyz, v4Pos.xyz);					\n\
	vec3 uuv = cross(g_v4Params[1].xyz, uv) * 2.0;					\n\
	uv *= (2.0 * g_v4Params[1].w);									\n\
	v4Pos.xyz += uv + uuv;											\n\
	float v2Scale;													\n\
	v2Scale = (g_v4Params[0].z / (-v4Pos.z + g_v4Params[0].z));     \n\
	v4Pos.xy *= v2Scale;											\n\
	v4Pos.xy += g_v4Params[0].xy;									\n\
	v4Pos.zw = vec2(0,1);											\n\
	gl_Position = CC_MVPMatrix * v4Pos;								\n\
	v_fragmentColor = a_color;										\n\
	v_texCoord = a_texCoord;										\n\
}																	\n\
"
