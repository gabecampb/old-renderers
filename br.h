// Bear API | br.h
// March 15 2018
// Last updated May 5, 2018.
// Gabriel Campbell
#ifndef BEAR_API_H
#define BEAR_API_H

// a low-overhead CPU graphics API for the GBA and other embedded devices
// primarily fixed-point
// scanline-based w/ 8 bits of sub-pixel precision
// rasterizer based on http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
// rasterization requires that v0.y <= v1.y <= v2.y
//
// remember to compile with available compiler optimizations (for example, gcc -Ofast)
//
// non-compressed textures are uint8.

// macros use all caps & prefix BR_
// function macros use all caps & prefix _BR_
// types use no capitals and prefix br
// functions use capitals at beginning of words and are prefixed with br
// ANYTHING ELSE IS NOT FORMALIZED.
//--------------------------------------------------------------------//

#include <memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define BR_VERSION_STRING "1.0"

#define BR_NUM_TEXTURE_UNITS 256

#define BR_DOUBLE_BUFFER				0
#define BR_DEPTH_WRITE					1
#define BR_DEPTH_TEST					2
#define BR_PERSPECTIVE_CORRECTION		3
#define BR_TEXTURE						4
#define BR_BLEND						5
#define BR_CULL							6
#define BR_CLIP							7
#define BR_PERSPECTIVE_DIVISION			8
#define BR_SCALE_Z						9
#define BR_VERTEX_SHADER				10
#define BR_FRAGMENT_SHADER				11
#define BR_FRONT_BUFFERS				12
#define BR_BACK_BUFFERS					13
#define BR_FILL							14	// polygon modes ...
#define BR_LINE							15
#define BR_POINT						16
#define BR_TRIANGLE						17	// vertex type
#define BR_TRIANGLES					18	// primitive description type
#define BR_LINES						19
#define BR_POINTS						20
#define BR_VERTEX_ARRAY					21	// whether or not to read vertex position from drawn arrays
#define BR_COLOR_ARRAY					22
#define BR_NORMAL_ARRAY					23
#define BR_TEXCOORD_ARRAY				24

// windings
#define BR_CW							25
#define BR_CCW							26

// 32-bit pixel formats
#define BR_R8G8B8A8						27
#define BR_R8G8B8						28
#define BR_A8B8G8R8						29
#define BR_B8G8R8						30
// 16-bit pixel formats
#define BR_R5G5B5A1						31
#define BR_R5G5B5						32
#define BR_A1B5G5R5						33
#define BR_B5G5R5						34
// 8-bit pixel formats
#define BR_R3G2B2A1						35
#define BR_R3G3B2						36
#define BR_A1B2G2R3						37
#define BR_B2G3R3						38
// depth formats
#define BR_D16							39
#define BR_D32							40

#define BR_VERTEX_TYPE					41
#define BR_VERTEX_POSITION				42
#define BR_VERTEX_COLOR					43
#define BR_VERTEX_NORMALS				44
#define BR_VERTEX_TEXTURE_COORDINATES	45
#define BR_PRIMITIVE_COLOR				46
#define BR_TEXTURE_COLOR				47
#define BR_FRAGMENT_COLOR				48
#define BR_BARY_LINEAR					49
#define BR_BARY_PERSPECTIVE				50
#define BR_FRAGMENT_POSITION			51
#define BR_FRAGMENT_DEPTH				52

#define BR_GLOBAL_STATE					53
#define BR_CONTEXT_ADDRESS				54
#define BR_RENDERBUFFER_STATE			55
#define BR_FRONT_COLOR_TYPE				56
#define BR_FRONT_COLOR_ADDRESS			57
#define BR_FRONT_DEPTH_TYPE				58
#define BR_FRONT_DEPTH_ADDRESS			59
#define BR_FRONT_DIMENSIONS				60
#define BR_BACK_COLOR_TYPE				61
#define BR_BACK_COLOR_ADDRESS			62
#define BR_BACK_DEPTH_TYPE				63
#define BR_BACK_DEPTH_ADDRESS			64
#define BR_BACK_DIMENSIONS				65
#define BR_CLEAR_COLOR					66
#define BR_CLEAR_DEPTH					67
#define BR_RENDER_STATE					68
#define BR_POINT_SIZE					69
#define BR_CULL_WINDING					70
#define BR_POLYGON_MODE					71
#define BR_VERTEX_SHADER_ADDRESS		72
#define BR_FRAGMENT_SHADER_ADDRESS		73
#define BR_ARRAY_STATE					74
#define BR_VERTEX_STRIDE				75
#define BR_COLOR_STRIDE					76
#define BR_NORMAL_STRIDE				77
#define BR_TEXCOORD_STRIDE				78
#define BR_VERTEX_OFFSET				79
#define BR_COLOR_OFFSET					80
#define BR_NORMAL_OFFSET				81
#define BR_TEXCOORD_OFFSET				82
#define BR_VERTEX_COUNT					83
#define BR_COLOR_COUNT					84

#define BR_COLOR_BUFFER_BIT				0x80000000
#define BR_DEPTH_BUFFER_BIT				0x40000000

// pixel read/write macros
#define _BR_R8G8B8A8(r,g,b,a)	(uint32_t)(a | (b<<8) | (g<<16) | (r<<24))
#define _BR_R8G8B8A8_R(x)		(uint8_t)((x & 0xFF000000)>>24)
#define _BR_R8G8B8A8_G(x)		(uint8_t)((x & 0x00FF0000)>>16)
#define _BR_R8G8B8A8_B(x)		(uint8_t)((x & 0x0000FF00)>>8)
#define _BR_R8G8B8A8_A(x)		(uint8_t)(x & 0x000000FF)
#define _BR_R8G8B8(r,g,b)		(uint32_t)(b | (g<<8) | (r<<16))
#define _BR_R8G8B8_R(x)			(uint8_t)((x & 0xFF0000)>>16)
#define _BR_R8G8B8_G(x)			(uint8_t)((x & 0x00FF00)>>8)
#define _BR_R8G8B8_B(x)			(uint8_t)(x & 0x0000FF)
#define _BR_A8B8G8R8(r,g,b,a)	(uint32_t)(r | (g<<8) | (b<<16) | (a<<24))
#define _BR_A8B8G8R8_R(x)		(uint8_t)(x & 0x000000FF)
#define _BR_A8B8G8R8_G(x)		(uint8_t)((x & 0x0000FF00)>>8)
#define _BR_A8B8G8R8_B(x)		(uint8_t)((x & 0x00FF0000)>>16)
#define _BR_A8B8G8R8_A(x)		(uint8_t)((x & 0xFF000000)>>24)
#define _BR_B8G8R8(r,g,b)		(uint32_t)(r | (g << 8) | (b << 16))
#define _BR_B8G8R8_R(x)			(uint8_t)(x & 0x0000FF)
#define _BR_B8G8R8_G(x)			(uint8_t)((x & 0x00FF00)>>8)
#define _BR_B8G8R8_B(x)			(uint8_t)((x & 0xFF0000)>>16)
#define _BR_R5G5B5A1(r,g,b,a)	(uint16_t)(a | (b<<1) | (g<<6) | (r<<11))
#define _BR_R5G5B5A1_R(x)		(uint8_t)((x & 0xF800)>>11)
#define _BR_R5G5B5A1_G(x)		(uint8_t)((x & 0x7C0)>>6)
#define _BR_R5G5B5A1_B(x)		(uint8_t)((x & 0x3E)>>1)
#define _BR_R5G5B5A1_A(x)		(uint8_t)(x & 0x1)
#define _BR_R5G5B5(r,g,b)		(uint16_t)(b | (g<<5) | (r<<10))
#define _BR_R5G5B5_R(x)			(uint8_t)((x & 0x7C00)>>10)
#define _BR_R5G5B5_G(x)			(uint8_t)((x & 0x3E0)>>5)
#define _BR_R5G5B5_B(x)			(uint8_t)(x & 0x1F)
#define _BR_A1B5G5R5(r,g,b,a)	(uint16_t)(r | (g<<5) | (b<<10) | (a<<15))
#define _BR_A1B5G5R5_R(x)		(uint8_t)(x & 0x1F)
#define _BR_A1B5G5R5_G(x)		(uint8_t)((x & 0x3E0)>>5)
#define _BR_A1B5G5R5_B(x)		(uint8_t)((x & 0x7C00)>>10)
#define _BR_A1B5G5R5_A(x)		(uint8_t)((x & 0x8000)>>15)
#define _BR_B5G5R5(r,g,b)		(uint16_t)(r | (g<<5) | (b<<10))
#define _BR_B5G5R5_R(x)			(uint8_t)(x & 0x1F)
#define _BR_B5G5R5_G(x)			(uint8_t)((x & 0x3E0)>>5)
#define _BR_B5G5R5_B(x)			(uint8_t)((x & 0x7C00)>>10)
#define _BR_R3G2B2A1(r,g,b,a)	(uint8_t)(a | (b<<1) | (g<<3) | (r<<5))
#define _BR_R3G2B2A1_R(x)		(uint8_t)((x & 0xE0)>>5)
#define _BR_R3G2B2A1_G(x)		(uint8_t)((x & 0x18)>>3)
#define _BR_R3G2B2A1_B(x)		(uint8_t)((x & 0x6)>>1)
#define _BR_R3G2B2A1_A(x)		(uint8_t)(x & 0x1)
#define _BR_R3G3B2(r,g,b)		(uint8_t)(b | (g<<2) | (r<<5))
#define _BR_R3G3B2_R(x)			(uint8_t)((x & 0xE0)>>5)
#define _BR_R3G3B2_G(x)			(uint8_t)((x & 0x1C)>>2)
#define _BR_R3G3B2_B(x)			(uint8_t)(x & 0x3)
#define _BR_A1B2G2R3(r,g,b,a)	(uint8_t)(r | (g<<3) | (b<<5) | (a<<7))
#define _BR_A1B2G2R3_R(x)		(uint8_t)(x & 0x7)
#define _BR_A1B2G2R3_G(x)		(uint8_t)((x & 0x18)>>3)
#define _BR_A1B2G2R3_B(x)		(uint8_t)((x & 0x60)>>5)
#define _BR_A1B2G2R3_A(x)		(uint8_t)((x & 0x80)>>7)
#define _BR_B2G3R3(r,g,b)		(uint8_t)(r | (g<<3) | (b<<6))
#define _BR_B2G3R3_R(x)			(uint8_t)(x & 0x7)
#define _BR_B2G3R3_G(x)			(uint8_t)((x & 0x38)>>3)
#define _BR_B2G3R3_B(x)			(uint8_t)((x & 0xC0)>>6)

// reciprocals, for removal of divisions
// undefined at end of header
#define _INV_65536	.00001525878f
#define _INV_255	.00392156862f
#define _INV_31		.03225806451f
#define _INV_7		.14285714285f
#define _INV_3		.33333333333f

typedef struct brvec2 brvec2;
typedef struct brvec3 brvec3;
typedef struct brvec4 brvec4;
typedef struct brvec2i brvec2i;
typedef struct brvec3i brvec3i;
typedef struct brvec4i brvec4i;
typedef struct brmat4 brmat4;

struct brvec2 { float x,y; };
struct brvec3 { float x,y,z; };
struct brvec4 { float x,y,z,w; };
struct brvec2i { int x,y; };
struct brvec3i { int x,y,z; };
struct brvec4i { int x,y,z,w; };
struct brvec2ui { uint32_t x,y; };
struct brvec3ui { uint32_t x,y,z; };
struct brvec4ui { uint32_t x,y,z,w; };
struct brmat4 {				// data[row][col]
	float m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
};

// Bear context definition
typedef struct brcontext brcontext;
struct brcontext
{
	void* cb, *cb2;
	void* db, *db2;
	uint32_t cb_type, cb2_type;
	uint32_t db_type, db2_type;
	uint32_t rb_width, rb_height;
	uint32_t rb2_width, rb2_height;
	brvec4 clear_color;
	float clear_depth;
	float point_radius;
	bool double_buffer;
	bool depth_write;
	bool depth_test;
	bool persp_corr;
	bool texture;
	bool blend;
	bool cull;
	uint32_t cull_winding;
	bool clip;
	bool persp_div;
	bool scale_z;

	uint32_t poly_mode;
	bool vertex_array;
	bool color_array;
	bool normal_array;
	bool tcoord_array;
	
	// vertex array attribute (byte) offsets
	size_t vertex_stride;
	size_t color_stride;
	size_t normal_stride;
	size_t tcoord_stride;
	void* vertex_offset;
	void* color_offset;
	void* normal_offset;
	void* tcoord_offset;
	uint32_t vertex_count;
	uint32_t color_count;

	uint32_t texture_unit;
	void* textures[BR_NUM_TEXTURE_UNITS];
	uint32_t texture_widths[BR_NUM_TEXTURE_UNITS];
	uint32_t texture_heights[BR_NUM_TEXTURE_UNITS];
	uint32_t texture_formats[BR_NUM_TEXTURE_UNITS];
	bool texture_compressed_booleans[BR_NUM_TEXTURE_UNITS];

	brvec4 (*vshader) (void* data, uint32_t* format, uint32_t attrib_count);	// current vertex shader
	brvec4 (*fshader) (void* data, uint32_t* format, uint32_t attrib_count, bool* discard);	// current fragment shader
	
	/// vertex shader attributes
	bool sh_vposition;	// whether or not to pass vertex position to vertex shader
	bool sh_vcolor;		// whether or not to pass vertex color to vertex shader
	bool sh_vtcoords;	// whether or not to pass vertex texture coordinates to vertex shader
	bool sh_vnormals;	// whether or not to pass vertex normals to vertex shader
	bool sh_vtype;		// whether or not to pass vertex type to vertex shader
	/// fragment shader attributes
	bool sh_prim_color;	// whether or not to pass primitive color to fragment shader
	bool sh_tex_color;	// whether or not to pass texture color to fragment shader
	bool sh_frag_color;	// whether or not to pass fragment color to fragment shader
	bool sh_bary_linear;	// whether or not to pass linear bary coords to fragment shader
	bool sh_bary_persp;		// whether or not to pass perspective-correct bary coords to fragment shader
	bool sh_fposition;		// whether or not to pass pixel coordinates to fragment shader
	bool sh_fdepth;			// whether or not to pass depth to fragment shader
};
static brcontext* _brcontext = NULL;	// current context

float _fdiv(float a, float b)
{
	if(b == 0.0f)
		return 0.0f;
	return a / b;
}

int _idiv(int a, int b)
{
	if(!b)
		return 0;
	return a / b;
}

// plot a pixel to the (assumed to exist) color buffer.
// rgba components are 16.16 fixed point (representing 0-1)
// may blend with destination
void _plot_pixel(uint32_t index, brvec4ui rgba, bool blend)
{
	void* cb = _brcontext->cb;
	uint32_t cb_type = _brcontext->cb_type;
	switch(cb_type)
	{
	case BR_R8G8B8: {
		uint8_t r = (rgba.x * 255) >> 16;
		uint8_t g = (rgba.y * 255) >> 16;
		uint8_t b = (rgba.z * 255) >> 16;
		uint8_t a = (rgba.w * 255) >> 16;
		if(!blend)
			((uint32_t*)cb)[index] = _BR_B8G8R8(r,g,b);
		else
		{
			if(a == 255) {
				((uint32_t*)cb)[index] = _BR_R8G8B8(r,g,b);
				return;
			}
			if(a == 0)   return;
			uint32_t* dst = &(((uint32_t*)cb)[index]);
			uint32_t dst_val = *dst;
			uint8_t dst_r = _BR_R8G8B8_R(r);
			uint8_t dst_g = _BR_R8G8B8_G(g);
			uint8_t dst_b = _BR_R8G8B8_B(b);
			float alpha = (float)a*_INV_255;
			float _1_minus_alpha = 1.0f - alpha;
			uint8_t pr = (r*alpha) + (dst_r*_1_minus_alpha);
			uint8_t pg = (g*alpha) + (dst_g*_1_minus_alpha);
			uint8_t pb = (b*alpha) + (dst_b*_1_minus_alpha);
			*dst = _BR_R8G8B8(pr,pg,pb);
		} }
		break;
	case BR_R8G8B8A8: {
		uint8_t r = (rgba.x * 255) >> 16;
		uint8_t g = (rgba.y * 255) >> 16;
		uint8_t b = (rgba.z * 255) >> 16;
		uint8_t a = (rgba.w * 255) >> 16;
		if(!blend)
			((uint32_t*)cb)[index] = _BR_R8G8B8A8(r, g, b, a);
		else
		{
			if(a == 255) {
				((uint32_t*)cb)[index] = _BR_R8G8B8A8(r, g, b, a);
				return;
			}
			if(a == 0)   return;
			uint32_t* dst = &(((uint32_t*)cb)[index]);
			uint32_t dst_val = *dst;
			uint8_t dst_r = _BR_R8G8B8A8_R(dst_val);
			uint8_t dst_g = _BR_R8G8B8A8_G(dst_val);
			uint8_t dst_b = _BR_R8G8B8A8_B(dst_val);
			uint8_t dst_a = _BR_R8G8B8A8_A(dst_val);
			float alpha = (float)a*_INV_255;
			float _1_minus_alpha = 1.0f - alpha;
			uint8_t pr = (r*alpha) + (dst_r*_1_minus_alpha);
			uint8_t pg = (g*alpha) + (dst_g*_1_minus_alpha);
			uint8_t pb = (b*alpha) + (dst_b*_1_minus_alpha);
			uint8_t pa = (a*alpha) + (dst_a*_1_minus_alpha);
			*dst = _BR_R8G8B8A8(pr, pg, pb, pa);
		} }
		break;
	case BR_B8G8R8: {
		uint8_t r = (rgba.x * 255) >> 16;
		uint8_t g = (rgba.y * 255) >> 16;
		uint8_t b = (rgba.z * 255) >> 16;
		uint8_t a = (rgba.w * 255) >> 16;
		if(!blend)
			((uint32_t*)cb)[index] = _BR_B8G8R8(r,g,b);
		else
		{
			if(a == 255) {
				((uint32_t*)cb)[index] = _BR_B8G8R8(r,g,b);
				return;
			}
			if(a == 0)   return;
			uint32_t* dst = &(((uint32_t*)cb)[index]);
			uint32_t dst_val = *dst;
			uint8_t dst_r = _BR_B8G8R8_R(r);
			uint8_t dst_g = _BR_B8G8R8_G(g);
			uint8_t dst_b = _BR_B8G8R8_B(b);
			float alpha = (float)a*_INV_255;
			float _1_minus_alpha = 1.0f - alpha;
			uint8_t pr = (r*alpha) + (dst_r*_1_minus_alpha);
			uint8_t pg = (g*alpha) + (dst_g*_1_minus_alpha);
			uint8_t pb = (b*alpha) + (dst_b*_1_minus_alpha);
			*dst = _BR_B8G8R8(pr,pg,pb);
		}
		break; }
	case BR_A8B8G8R8: {
		uint8_t r = (rgba.x * 255) >> 16;
		uint8_t g = (rgba.y * 255) >> 16;
		uint8_t b = (rgba.z * 255) >> 16;
		uint8_t a = (rgba.w * 255) >> 16;
		if(!blend)
			((uint32_t*)cb)[index] = _BR_A8B8G8R8(r, g, b, a);
		else
		{
			if(a == 255) {
				((uint32_t*)cb)[index] = _BR_A8B8G8R8(r, g, b, a);
				return;
			}
			if(a == 0)   return;
			uint32_t* dst = &(((uint32_t*)cb)[index]);
			uint32_t dst_val = *dst;
			uint8_t dst_r = _BR_A8B8G8R8_R(dst_val);
			uint8_t dst_g = _BR_A8B8G8R8_G(dst_val);
			uint8_t dst_b = _BR_A8B8G8R8_B(dst_val);
			uint8_t dst_a = _BR_A8B8G8R8_A(dst_val);
			float alpha = (float)a*_INV_255;
			float _1_minus_alpha = 1.0f - alpha;
			uint8_t pr = (r*alpha) + (dst_r*_1_minus_alpha);
			uint8_t pg = (g*alpha) + (dst_g*_1_minus_alpha);
			uint8_t pb = (b*alpha) + (dst_b*_1_minus_alpha);
			uint8_t pa = (a*alpha) + (dst_a*_1_minus_alpha);
			*dst = _BR_A8B8G8R8(pr, pg, pb, pa);
		} }
		break;
	case BR_R5G5B5: {
		uint8_t r = (rgba.x * 31) >> 16;
		uint8_t g = (rgba.y * 31) >> 16;
		uint8_t b = (rgba.z * 31) >> 16;
		if(!blend)
			((uint16_t*)cb)[index] = _BR_R5G5B5(r,g,b);
		else
		{
			if(rgba.w<32768) return;
			((uint16_t*)cb)[index] = _BR_R5G5B5(r,g,b);
			return;
		} }
		break;
	case BR_R5G5B5A1: {
		uint8_t r = (rgba.x * 31) >> 16;
		uint8_t g = (rgba.y * 31) >> 16;
		uint8_t b = (rgba.z * 31) >> 16;
		if(!blend)
			((uint16_t*)cb)[index] = _BR_R5G5B5A1(r,g,b,1);
		else
		{
			if(rgba.w<32768) return;
			((uint16_t*)cb)[index] = _BR_R5G5B5A1(r,g,b,1);
			return;
		} }
		break;
	case BR_B5G5R5: {
		uint8_t r = (rgba.x * 31) >> 16;
		uint8_t g = (rgba.y * 31) >> 16;
		uint8_t b = (rgba.z * 31) >> 16;
		if(!blend)
			((uint16_t*)cb)[index] = _BR_B5G5R5(r,g,b);
		else
		{
			if(rgba.w<32768) return;
			((uint16_t*)cb)[index] = _BR_B5G5R5(r,g,b);
			return;
		} }
		break;
	case BR_A1B5G5R5: {
		uint8_t r = (rgba.x * 31) >> 16;
		uint8_t g = (rgba.y * 31) >> 16;
		uint8_t b = (rgba.z * 31) >> 16;
		if(!blend)
			((uint16_t*)cb)[index] = _BR_A1B5G5R5(r,g,b,1);
		else
		{
			if(rgba.w<32768) return;
			((uint16_t*)cb)[index] = _BR_A1B5G5R5(r,g,b,1);
			return;
		} }
		break;
	case BR_R3G3B2: {
		uint8_t r = (rgba.x * 7) >> 16;
		uint8_t g = (rgba.y * 7) >> 16;
		uint8_t b = (rgba.z * 3) >> 16;
		if(!blend)
			((uint8_t*)cb)[index] = _BR_R3G3B2(r,g,b);
		else
		{
			if(rgba.w<32768) return;
			((uint8_t*)cb)[index] = _BR_R3G3B2(r,g,b);
			return;
		} }
		break;
	case BR_R3G2B2A1: {
		uint8_t r = (rgba.x * 7) >> 16;
		uint8_t g = (rgba.y * 3) >> 16;
		uint8_t b = (rgba.z * 3) >> 16;
		if(!blend)
			((uint8_t*)cb)[index] = _BR_R3G2B2A1(r,g,b,1);
		else
		{
			if(rgba.w<32768) return;
			((uint8_t*)cb)[index] = _BR_R3G2B2A1(r,g,b,1);
			return;
		} }
		break;
	case BR_B2G3R3: {
		uint8_t r = (rgba.x * 7) >> 16;
		uint8_t g = (rgba.y * 7) >> 16;
		uint8_t b = (rgba.z * 3) >> 16;
		if(!blend)
			((uint8_t*)cb)[index] = _BR_B2G3R3(r,g,b);
		else
		{
			if(rgba.w<32768) return;
			((uint8_t*)cb)[index] = _BR_B2G3R3(r,g,b);
			return;
		} }
		break;
	case BR_A1B2G2R3: {
		uint8_t r = (rgba.x * 7) >> 16;
		uint8_t g = (rgba.y * 3) >> 16;
		uint8_t b = (rgba.z * 3) >> 16;
		if(!blend)
			((uint8_t*)cb)[index] = _BR_A1B2G2R3(r,g,b,1);
		else
		{
			if(rgba.w<32768) return;
			((uint8_t*)cb)[index] = _BR_A1B2G2R3(r,g,b,1);
			return;
		} }
		break;
	}
}

// plot a depth to the (assumed to exist) depth buffer.
void _plot_depth(uint32_t index, int64_t depth)
{
	if(_brcontext->db_type == BR_D16)
		((uint16_t*)_brcontext->db) [index] = depth;
	if(_brcontext->db_type == BR_D32)
		((uint32_t*)_brcontext->db) [index] = depth;
}

// get a depth from the (assumed to exist) depth buffer.
int64_t _get_depth(uint32_t index)
{
	if(_brcontext->db_type == BR_D16)
		return ((uint16_t*)_brcontext->db) [index];
	if(_brcontext->db_type == BR_D32)
		return ((uint32_t*)_brcontext->db) [index];
}

// convert depth from clip-space to raster-space
// note depth of 1.0 has accuracy error (will result in (depth*range)+1)
int64_t _convert_depth(float depth)
{
	if(_brcontext->db_type == BR_D16)
		return depth * 0xFFFF;
	if(_brcontext->db_type == BR_D32)
		return depth * 0xFFFFFFFF;
}

// return whether or not a depth is a depth valid for depth buffer
bool _is_valid_depth(int64_t depth)
{
	if(_brcontext->db_type == BR_D16)
		return (depth >= 0 && depth <= 0xFFFF);
	if(_brcontext->db_type == BR_D32)
		return (depth >= 0 && depth <= 0xFFFFFFFF);
		
	return false;
}

// return whether or not a value is a pixel format
bool _is_pixel_format(uint32_t value)
{
	switch(value)
	{
		case BR_R8G8B8A8:
		case BR_R8G8B8:
		case BR_A8B8G8R8:
		case BR_B8G8R8:
		case BR_R5G5B5A1:
		case BR_R5G5B5:
		case BR_A1B5G5R5:
		case BR_B5G5R5:
		case BR_R3G2B2A1:
		case BR_R3G3B2:
		case BR_A1B2G2R3:
		case BR_B2G3R3:
			return true;
		default:
			return false;
	}
}

// get texel from texture and return 0-1 RGBA components
// assume alpha of 1 in absence alpha channel
void _get_texel(int x, int y, brvec4* col, void* texture, uint32_t format, uint32_t width, uint32_t height, bool compressed)
{
	if(!_brcontext || !_is_pixel_format(format))
		return;

	if(x >= width)
		x = width - 1;
	if(x < 0)
		x = 0;
	if(y >= height)
		y = height - 1;
	if(y < 0)
		y = 0;

	if(!compressed)
	{
		uint8_t texel_width = 1;
		switch(format)
		{
			case BR_R8G8B8A8:
			case BR_A8B8G8R8:
			case BR_R5G5B5A1:
			case BR_A1B5G5R5:
			case BR_R3G2B2A1:
			case BR_A1B2G2R3:
				texel_width = 4;
				break;
			default:
				texel_width = 3;
		}
		uint8_t* tex = (uint8_t*) texture;
		uint8_t* texel = &tex[(y*width+x)*texel_width];
		switch(format)
		{
			case BR_R8G8B8A8:
				col->x = texel[0]*_INV_255;
				col->y = texel[1]*_INV_255;
				col->z = texel[2]*_INV_255;
				col->w = texel[3]*_INV_255;
				return;
			case BR_R8G8B8:
				col->x = texel[0]*_INV_255;
				col->y = texel[1]*_INV_255;
				col->z = texel[2]*_INV_255;
				col->w = 1;
				return;
			case BR_A8B8G8R8:
				col->x = texel[3]*_INV_255;
				col->y = texel[2]*_INV_255;
				col->z = texel[1]*_INV_255;
				col->w = texel[0]*_INV_255;
				return;
			case BR_B8G8R8:
				col->x = texel[2]*_INV_255;
				col->y = texel[1]*_INV_255;
				col->z = texel[0]*_INV_255;
				col->w = 1;
				return;
			case BR_R5G5B5A1:
				col->x = texel[0]*_INV_31;
				col->y = texel[1]*_INV_31;
				col->z = texel[2]*_INV_31;
				col->w = (texel[3] != 0);
				return;
			case BR_R5G5B5:
				col->x = texel[0]*_INV_31;
				col->y = texel[1]*_INV_31;
				col->z = texel[2]*_INV_31;
				col->w = 1;
				return;
			case BR_A1B5G5R5:
				col->x = texel[3]*_INV_31;
				col->y = texel[2]*_INV_31;
				col->z = texel[1]*_INV_31;
				col->w = (texel[0] != 0);
				return;
			case BR_B5G5R5:
				col->x = texel[2]*_INV_31;
				col->y = texel[1]*_INV_31;
				col->z = texel[0]*_INV_31;
				col->w = 1;
				return;
			case BR_R3G2B2A1:
				col->x = texel[0]*_INV_7;
				col->y = texel[1]*_INV_3;
				col->z = texel[2]*_INV_3;
				col->w = (texel[3] != 0);
				return;
			case BR_R3G3B2:
				col->x = texel[0]*_INV_7;
				col->y = texel[1]*_INV_7;
				col->z = texel[2]*_INV_3;
				col->w = 1;
				return;
			case BR_A1B2G2R3:
				col->x = texel[3]*_INV_7;
				col->y = texel[2]*_INV_3;
				col->z = texel[1]*_INV_3;
				col->w = (texel[0] != 0);
				return;
			case BR_B2G3R3:
				col->x = texel[2]*_INV_7;
				col->y = texel[1]*_INV_7;
				col->z = texel[0]*_INV_3;
				col->w = 1;
				return;
		}
	}
	else
	{
		uint8_t texel8;
		uint16_t texel16;
		uint32_t texel32;
		uint8_t* tex8;
		uint16_t* tex16;
		uint32_t* tex32;

		switch(format)
		{
			case BR_R8G8B8A8:
			tex32 = (uint32_t*) texture;
			texel32 = tex32[y*width+x];
			col->x = _BR_R8G8B8A8_R(texel32)*_INV_255;
			col->y = _BR_R8G8B8A8_G(texel32)*_INV_255;
			col->z = _BR_R8G8B8A8_B(texel32)*_INV_255;
			col->w = _BR_R8G8B8A8_A(texel32)*_INV_255;
			return;
			case BR_R8G8B8:
			tex32 = (uint32_t*) texture;
			texel32 = tex32[y*width+x];
			col->x = _BR_R8G8B8_R(texel32)*_INV_255;
			col->y = _BR_R8G8B8_G(texel32)*_INV_255;
			col->z = _BR_R8G8B8_B(texel32)*_INV_255;
			col->w = 1;
			return;
			case BR_A8B8G8R8:
			tex32 = (uint32_t*) texture;
			texel32 = tex32[y*width+x];
			col->x = _BR_A8B8G8R8_R(texel32)*_INV_255;
			col->y = _BR_A8B8G8R8_G(texel32)*_INV_255;
			col->z = _BR_A8B8G8R8_B(texel32)*_INV_255;
			col->w = _BR_A8B8G8R8_A(texel32)*_INV_255;
			return;
			case BR_B8G8R8:
			tex32 = (uint32_t*) texture;
			texel32 = tex32[y*width+x];
			col->x = _BR_B8G8R8_R(texel32)*_INV_255;
			col->y = _BR_B8G8R8_G(texel32)*_INV_255;
			col->z = _BR_B8G8R8_B(texel32)*_INV_255;
			col->w = 1;
			return;
			case BR_R5G5B5A1:
			tex16 = (uint16_t*) texture;
			texel16 = tex16[y*width+x];
			col->x = _BR_R5G5B5A1_R(texel16)*_INV_31;
			col->y = _BR_R5G5B5A1_G(texel16)*_INV_31;
			col->z = _BR_R5G5B5A1_B(texel16)*_INV_31;
			col->w = _BR_R5G5B5A1_A(texel16);
			return;
			case BR_R5G5B5:
			tex16 = (uint16_t*) texture;
			texel16 = tex16[y*width+x];
			col->x = _BR_R5G5B5_R(texel16)*_INV_31;
			col->y = _BR_R5G5B5_G(texel16)*_INV_31;
			col->z = _BR_R5G5B5_B(texel16)*_INV_31;
			col->w = 1;
			return;
			case BR_A1B5G5R5:
			tex16 = (uint16_t*) texture;
			texel16 = tex16[y*width+x];
			col->x = _BR_A1B5G5R5_R(texel16)*_INV_31;
			col->y = _BR_A1B5G5R5_G(texel16)*_INV_31;
			col->z = _BR_A1B5G5R5_B(texel16)*_INV_31;
			col->w = _BR_A1B5G5R5_A(texel16);
			return;
			case BR_B5G5R5:
			tex16 = (uint16_t*) texture;
			texel16 = tex16[y*width+x];
			col->x = _BR_A1B5G5R5_R(texel16)*_INV_31;
			col->y = _BR_A1B5G5R5_G(texel16)*_INV_31;
			col->z = _BR_A1B5G5R5_B(texel16)*_INV_31;
			col->w = 1;
			return;
			case BR_R3G2B2A1:
			tex8 = (uint8_t*) texture;
			texel8 = tex8[y*width+x];
			col->x = _BR_R3G2B2A1_R(texel8)*_INV_7;
			col->y = _BR_R3G2B2A1_G(texel8)*_INV_3;
			col->z = _BR_R3G2B2A1_B(texel8)*_INV_3;
			col->w = _BR_R3G2B2A1_A(texel8);
			return;
			case BR_R3G3B2:
			tex8 = (uint8_t*) texture;
			texel8 = tex8[y*width+x];
			col->x = _BR_R3G3B2_R(texel8)*_INV_7;
			col->y = _BR_R3G3B2_G(texel8)*_INV_7;
			col->z = _BR_R3G3B2_B(texel8)*_INV_3;
			col->w = 1;
			return;
			case BR_A1B2G2R3:
			tex8 = (uint8_t*) texture;
			texel8 = tex8[y*width+x];
			col->x = _BR_R3G2B2A1_R(texel8)*_INV_7;
			col->y = _BR_R3G2B2A1_G(texel8)*_INV_3;
			col->z = _BR_R3G2B2A1_B(texel8)*_INV_3;
			col->w = _BR_R3G2B2A1_A(texel8);
			return;
			case BR_B2G3R3:
			tex8 = (uint8_t*) texture;
			texel8 = tex8[y*width+x];
			col->x = _BR_R3G3B2_R(texel8)*_INV_7;
			col->y = _BR_R3G3B2_G(texel8)*_INV_7;
			col->z = _BR_R3G3B2_B(texel8)*_INV_3;
			col->w = 1;
			return;
		}
	}
}

brvec3 _normalize_vec3(brvec3 v)
{
	float length = sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
	brvec3 norm;
	norm.x = _fdiv(v.x, length);
	norm.y = _fdiv(v.y, length);
	norm.z = _fdiv(v.z, length);
	return norm;
}

brvec3 _sub_vec3(brvec3 a, brvec3 b)
{
	brvec3 diff;
	diff.x = a.x - b.x;
	diff.y = a.y - b.y;
	diff.z = a.z - b.z;
	return diff;
}

float _dot_vec3(brvec3 a, brvec3 b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}

brvec3 _cross_vec3(brvec3 a, brvec3 b)
{
	brvec3 cross;
	cross.x = a.y*b.z - a.z*b.y;
	cross.y = a.z*b.x - a.x*b.z;
	cross.z = a.x*b.y - a.y*b.x;
	return cross;
}

// defines a vertex for passes through vertex shader
typedef struct _vertex_t _vertex_t;
struct _vertex_t
{
	uint32_t type;		// type of primitive vertex belongs to
	brvec4 position;	// vertex position, default (0,0,0,1)
	brvec2* tcoords;	// vertex texture coords, default (0,0)
	brvec3* normals;	// vertex normals, default (0,0,0)
	brvec4* color;		// vertex color, default (0,0,0,1)
};

// pass a vertex through the vertex shader, if bound.
brvec4 _vertex_pass(_vertex_t* vertex)
{
	brvec4 out;
	
	if(!_brcontext->vshader)
		return vertex->position;
	else
	{
		void* data = NULL;
		uint32_t attrib_count = 0;
		uint32_t size = 0;
		
		if(_brcontext->sh_vtype)		{ attrib_count += 1; size += sizeof(uint32_t);}
		if(_brcontext->sh_vposition)	{ attrib_count += 1; size += sizeof(brvec4);  }
		if(_brcontext->sh_vcolor)		{ attrib_count += 1; size += sizeof(brvec4*); }
		if(_brcontext->sh_vnormals)		{ attrib_count += 1; size += sizeof(brvec3*); }
		if(_brcontext->sh_vtcoords)		{ attrib_count += 1; size += sizeof(brvec2*); }
		
		data = malloc(size);
		
		uint32_t format[attrib_count];
		uint32_t i = 0;
		uint32_t offset = 0;
		if(_brcontext->sh_vtype)		{ format[i] = BR_VERTEX_TYPE; *((uint32_t*)(data+offset)) = vertex->type; offset += sizeof(uint32_t); i += 1; }
		if(_brcontext->sh_vposition)	{ format[i] = BR_VERTEX_POSITION; *((brvec4*)(data+offset)) = vertex->position; offset += sizeof(brvec4); i += 1; }
		if(_brcontext->sh_vcolor)		{ format[i] = BR_VERTEX_COLOR; *((brvec4**)(data+offset)) = vertex->color; offset += sizeof(brvec4*); i += 1; }
		if(_brcontext->sh_vnormals)		{ format[i] = BR_VERTEX_NORMALS; *((brvec3**)(data+offset)) = vertex->normals; offset += sizeof(brvec3*); i += 1; }
		if(_brcontext->sh_vtcoords)		{ format[i] = BR_VERTEX_TEXTURE_COORDINATES; *((brvec2**)(data+offset)) = vertex->tcoords; offset += sizeof(brvec2*); i += 1; }
		
		if(attrib_count)
			out = _brcontext->vshader(data, format, attrib_count);
		else
			out = _brcontext->vshader(NULL, NULL, 0);
	}
	
	return out;
}

// defines an object used for fragment shader passes; returns and provides required data
typedef struct _fragment_t _fragment_t;
struct _fragment_t
{
	void* pass_data;				// data block used for pass
	uint32_t pass_attrib_count;		// count of passed attributes
	uint32_t* pass_attribs;			// layout of passed attributes
	brvec4 primitive_color;			// primitive color
	brvec4 texture_color;			// texture color
	brvec4 color;					// color before fragment pass
	brvec3 linear_bary;				// linear barycentric coordinates
	brvec3 bary;					// perspective-corrected bary
	brvec2i position;				// pixel coordinates
	float depth;					// depth
	bool discard;					// whether or not the fragment should be discarded
};

// allocate the block of attributes for a re-usable fragment pass object.
// do this per-primitive, instead of per-fragment.
void _init_fragment(_fragment_t* fragment)
{
	fragment->pass_attrib_count = 0;
	uint32_t data_size = 0;
	
	uint32_t i = 0;
	if(_brcontext->sh_prim_color)	{ fragment->pass_attrib_count += 1; data_size += sizeof(brvec4); }
	if(_brcontext->sh_tex_color)	{ fragment->pass_attrib_count += 1; data_size += sizeof(brvec4); }
	if(_brcontext->sh_frag_color)	{ fragment->pass_attrib_count += 1; data_size += sizeof(brvec4); }
	if(_brcontext->sh_bary_linear)	{ fragment->pass_attrib_count += 1; data_size += sizeof(brvec3); }
	if(_brcontext->sh_bary_persp)	{ fragment->pass_attrib_count += 1; data_size += sizeof(brvec3); }
	if(_brcontext->sh_fposition)	{ fragment->pass_attrib_count += 1; data_size += sizeof(brvec2i); }
	if(_brcontext->sh_fdepth)		{ fragment->pass_attrib_count += 1; data_size += sizeof(float); }
	
	fragment->pass_attribs = (uint32_t*) calloc(fragment->pass_attrib_count, sizeof(uint32_t));
	uint32_t* attribs = fragment->pass_attribs;
	if(_brcontext->sh_prim_color)	{ attribs[i] = BR_PRIMITIVE_COLOR; i += 1; }
	if(_brcontext->sh_tex_color)	{ attribs[i] = BR_TEXTURE_COLOR;   i += 1; }
	if(_brcontext->sh_frag_color)	{ attribs[i] = BR_FRAGMENT_COLOR;  i += 1; }
	if(_brcontext->sh_bary_linear)	{ attribs[i] = BR_BARY_LINEAR;     i += 1; }
	if(_brcontext->sh_bary_persp)	{ attribs[i] = BR_BARY_PERSPECTIVE;i += 1; }
	if(_brcontext->sh_fposition)	{ attribs[i] = BR_FRAGMENT_POSITION; i += 1; }
	if(_brcontext->sh_fdepth)		{ attribs[i] = BR_FRAGMENT_DEPTH;    i += 1; }
	
	if(data_size)
		fragment->pass_data = malloc(data_size);
}

// pass a fragment (see _init_fragment and _fragment_t) through the fragment shader.
// returns final color.
brvec4 _fragment_pass(_fragment_t* frag)
{
	uint32_t i = 0;
	uint32_t offset = 0;
	if(_brcontext->sh_prim_color)	{ *((brvec4*)(frag->pass_data+offset)) = frag->primitive_color; offset += sizeof(brvec4); }
	if(_brcontext->sh_tex_color)	{ *((brvec4*)(frag->pass_data+offset)) = frag->texture_color; offset += sizeof(brvec4); }
	if(_brcontext->sh_frag_color)	{ *((brvec4*)(frag->pass_data+offset)) = frag->color; offset += sizeof(brvec4); }
	if(_brcontext->sh_bary_linear)	{ *((brvec3*)(frag->pass_data+offset)) = frag->linear_bary; offset += sizeof(brvec3); }
	if(_brcontext->sh_bary_persp)	{ *((brvec3*)(frag->pass_data+offset)) = frag->bary; offset += sizeof(brvec3); }
	if(_brcontext->sh_fposition)	{ *((brvec2i*)(frag->pass_data+offset)) = frag->position; offset += sizeof(brvec2i); }
	if(_brcontext->sh_fdepth)		{ *((float*)(frag->pass_data+offset)) = frag->depth; offset += sizeof(float); }
	
	if(frag->pass_attrib_count)
		return _brcontext->fshader(frag->pass_data, frag->pass_attribs, frag->pass_attrib_count, &frag->discard);
	else
		return _brcontext->fshader(NULL, NULL, 0, &frag->discard);
}

// a triangle ready for (or which currently is being) post-processed
typedef struct _triangle_t _triangle_t;
struct _triangle_t
{
	// clip-space vertex positions
	brvec4 v0, v1, v2;
	
	// vertex colors
	brvec4 rgba0, rgba1, rgba2;
	
	// texture coordinates (0-1), origin in bottom left
	brvec2 tcoords0, tcoords1, tcoords2;
	
	// pointer to a triangle that this triangle may have been built off of, otherwise NULL.
	// for original (parent) triangles, this should be NULL.
	// for processed (clipped) triangles (handled internally) this will point to the original triangle.
	_triangle_t* parent;
	brvec2i parent_orig_v0, parent_orig_v1, parent_orig_v2;	
	// calculated once for & within the parent triangle (for use by child triangle's raster trinagles)
};

// a triangle ready to be rastered (after being processed)
typedef struct _raster_triangle_t _raster_triangle_t;
struct _raster_triangle_t
{
	// raster-space coordinates of vertices
	// will change per sub-triangle
	float x0, x1, x2;
	float y0, y1, y2;
	// original 24.8 fixed-point raster-space coordinates of vertices (sorted @ triangle processing)
	// for accuracy. This is the region through which bary are interpolated 
	// and the triangle's edge slopes are calculated. Will not vary among clipped triangles.
	brvec2i orig_v0, orig_v1, orig_v2;
	// overrides barycentric coordinates of vertices
	// used for sorting vertices during spliting of triangles
	brvec3 bary0;
	brvec3 bary1;
	brvec3 bary2;
	// vertex z and w; w is in clip-space and z in raster-space
	int64_t z0, z1, z2;
	float w0, w1, w2;
	// 16.16 fixed-point vertex colors (*65536)
	brvec4ui rgba0;
	brvec4ui rgba1;
	brvec4ui rgba2;
	// 16.16 fixed-point texel coordinates (*65536)
	brvec2ui tx0, tx1, tx2;
	// whether or not to draw top row
	bool draw_top;
	// texture unit information @ time of raster
	void* texture;
	uint32_t texture_width;
	uint32_t texture_height;
	uint32_t texture_format;
	bool texture_compressed;
	bool complete_texture_unit;
};

// raster a flat bottomed or flat topped triangle
void _raster_triangle(_raster_triangle_t* params)
{
	if(!params)
		return;
	if(!_brcontext)
		return;
		
	bool depth_test = (_brcontext->depth_test && _brcontext->db);
	bool plot_color = _brcontext->cb;
	bool plot_depth = (_brcontext->depth_write && _brcontext->db);
	bool textured = (_brcontext->texture && params->complete_texture_unit);
	
	// for fragment passes
	_fragment_t frag_pass;
	if(_brcontext->fshader)
		_init_fragment(&frag_pass);
		
	// 24.8 fixed point
	int x0 = params->x0 * 256.0f;
	int x1 = params->x1 * 256.0f;
	int x2 = params->x2 * 256.0f;
	int y0 = params->y0 * 256.0f;
	int y1 = params->y1 * 256.0f;
	int y2 = params->y2 * 256.0f;
	// 16.16 fixed point attributes
	uint32_t r0 = params->rgba0.x;
	uint32_t g0 = params->rgba0.y;
	uint32_t b0 = params->rgba0.z;
	uint32_t a0 = params->rgba0.w;
	uint32_t r1 = params->rgba1.x;
	uint32_t g1 = params->rgba1.y;
	uint32_t b1 = params->rgba1.z;
	uint32_t a1 = params->rgba1.w;
	uint32_t r2 = params->rgba2.x;
	uint32_t g2 = params->rgba2.y;
	uint32_t b2 = params->rgba2.z;
	uint32_t a2 = params->rgba2.w;
	uint32_t tx0 = params->tx0.x;
	uint32_t tx1 = params->tx1.x;
	uint32_t tx2 = params->tx2.x;
	uint32_t ty0 = params->tx0.y;
	uint32_t ty1 = params->tx1.y;
	uint32_t ty2 = params->tx2.y;

	// for bary interpolation
	brvec2i a,b;
	a.x = (params->orig_v1.x>>8) - (params->orig_v0.x>>8);
	a.y = (params->orig_v1.y>>8) - (params->orig_v0.y>>8);
	b.x = (params->orig_v2.x>>8) - (params->orig_v0.x>>8);
	b.y = (params->orig_v2.y>>8) - (params->orig_v0.y>>8);
	float den = _fdiv(256.0f, (a.x*b.y-b.x*a.y));
	
	int64_t depth = 0;
	
	// X Y coordinates are 24.8
	// interpolate attribs as 16.16 fixed point
	
	float inv_v0_w = 0;
	float inv_v1_w = 0;
	float inv_v2_w = 0;
	if(_brcontext->persp_corr)
	{
		inv_v0_w = _fdiv(1.0f, fabs(params->w0));
		inv_v1_w = _fdiv(1.0f, fabs(params->w1));
		inv_v2_w = _fdiv(1.0f, fabs(params->w2));
	}

	// flat bottom
	if(y1 == y2 && x1 != x2)
	{
//		int invslope1 = ((float)(params->orig_v1.x - params->orig_v0.x) / (float)(params->orig_v1.y - params->orig_v0.y)) * 256.0f;
//		int invslope2 = ((float)(params->orig_v2.x - params->orig_v0.x) / (float)(params->orig_v2.y - params->orig_v0.y)) * 256.0f;
		int invslope1 = _idiv(params->orig_v1.x - params->orig_v0.x, (params->orig_v1.y - params->orig_v0.y)>>8);
		int invslope2 = _idiv(params->orig_v2.x - params->orig_v0.x, (params->orig_v2.y - params->orig_v0.y)>>8);

		// 24.8 scanline left and right x
		int curfx1 = x0;
		int curfx2 = x0;

		int y1_int = y1 >> 8;

		for(int y = (y0>>8)+1; y <= y1_int; y += 1)
		{
			if(y < 0)
			{
				curfx1 += invslope1;
				curfx2 += invslope2;
				continue;
			}
			if(y >= _brcontext->rb_height)
				break;

			int cx1, cx2;
			int sx1, sx2;
			cx1 = curfx1;
			cx2 = curfx2;
			if(cx1 > cx2)
			{
				int tmp = cx1;
				cx1 = cx2;
				cx2 = tmp;
			}
			sx1 = (cx1-128)>>8;
			sx2 = (cx2+128)>>8;

			if(sx1 > sx2)
			{
				curfx1 += invslope1;
				curfx2 += invslope2;
				continue;
			}
			int sy = y;

			if(sx1 < 0)
				sx1 = 0;
			if(sx1 >= _brcontext->rb_width)
			{
				curfx1 += invslope1;
				curfx2 += invslope2;
				continue;
			}

			int slength = (sx2-sx1)+1;

			// do scanline sx1->sx2 @ y sy
			// calculate 24.8 bary coords for start and end of scanline
			// if _brcontext->persp_corr, perspective correct bary
			// convert bary to 16.16 to interpolate 16.16 fixed point attributes
			// for texel coordinates, discard last 16 bits.
			// to convert from 16.16 to float, divide by 65536 (use _INV_65536).

			// if appropriate, scanline beginning/end will be offset by 256 (1 pixel; working with 1/256 coordinates)
			// in order to avoid sub-pixels that fall outside of triangle given Y increases by a full pixel
			// If this fails to generate valid barycentric coordinates, a loop will be run to iterate by half a pixel
			// until valid bary coordinates are generated.

			brvec3 bary_s1;
			brvec2i c;
			if(sx1 != sx2)
				c.x = ((sx1<<8)+256) - params->orig_v0.x;
			else
				c.x = cx1 - params->orig_v0.x;
			c.y = (y<<8) - params->orig_v0.y;
			bary_s1.y = (c.x*b.y-b.x*c.y)*den;
			bary_s1.z = (a.x*c.y-c.x*a.y)*den;
			bary_s1.x = 65536.0f - bary_s1.y - bary_s1.z;
			if(bary_s1.x < 0.0f || bary_s1.y < 0.0f || bary_s1.z < 0.0f) {
				int x = (sx1<<8)+384;
				bool cont = false;
				while(bary_s1.x < 0.0f || bary_s1.y < 0.0f || bary_s1.z < 0.0f) {
					c.x = x - params->orig_v0.x;
					bary_s1.y = (c.x*b.y-b.x*c.y)*den;
					bary_s1.z = (a.x*c.y-c.x*a.y)*den;
					bary_s1.x = 65536.0f - bary_s1.y - bary_s1.z;
					x += 128;
					if(x > cx2) {
						curfx1 += invslope1;
						curfx2 += invslope2;
						cont = true;
						break;
					}
				}
				if(cont) continue;
			}
			{
				float bx = bary_s1.x;
				float by = bary_s1.y;
				float bz = bary_s1.z;
				bary_s1.x = bx * (params->bary0.x) + by * (params->bary1.x) + bz * (params->bary2.x);
				bary_s1.y = bx * (params->bary0.y) + by * (params->bary1.y) + bz * (params->bary2.y);
				bary_s1.z = bx * (params->bary0.z) + by * (params->bary1.z) + bz * (params->bary2.z);
			}

			brvec3 bary_s2;
			if(sx1 != sx2)
				c.x = ((sx2<<8)-256) - params->orig_v0.x;
			else
				c.x = cx2 - params->orig_v0.x;
			bary_s2.y = (c.x*b.y-b.x*c.y)*den;
			bary_s2.z = (a.x*c.y-c.x*a.y)*den;
			bary_s2.x = 65536.0f - bary_s2.y - bary_s2.z;
			if(bary_s2.x < 0.0f || bary_s2.y < 0.0f || bary_s2.z < 0.0f) {
				int x = (sx2<<8)-384;
				bool cont = false;
				while(bary_s2.x < 0.0f || bary_s2.y < 0.0f || bary_s2.z < 0.0f) {
					c.x = x - params->orig_v0.x;
					bary_s2.y = (c.x*b.y-b.x*c.y)*den;
					bary_s2.z = (a.x*c.y-c.x*a.y)*den;
					bary_s2.x = 65536.0f - bary_s2.y - bary_s2.z;
					x -= 128;
					if(x < cx1) {
						curfx1 += invslope1;
						curfx2 += invslope2;
						cont = true;
						break;
					}
				}
				if(cont) continue;
			}
			{
				float bx = bary_s2.x;
				float by = bary_s2.y;
				float bz = bary_s2.z;
				bary_s2.x = bx * (params->bary0.x) + by * (params->bary1.x) + bz * (params->bary2.x);
				bary_s2.y = bx * (params->bary0.y) + by * (params->bary1.y) + bz * (params->bary2.y);
				bary_s2.z = bx * (params->bary0.z) + by * (params->bary1.z) + bz * (params->bary2.z);
			}
			
			int inc_bx = (bary_s2.x - bary_s1.x)/slength;
			int inc_by = (bary_s2.y - bary_s1.y)/slength;
			int inc_bz = (bary_s2.z - bary_s1.z)/slength;
			
			uint32_t pixel_index = y * _brcontext->rb_width + sx1;

			brvec3ui linear_bary;
			linear_bary.x = bary_s1.x;
			linear_bary.y = bary_s1.y;
			linear_bary.z = bary_s1.z;
			for(int x = sx1; x <= sx2; x += 1)
			{
				if(x >= _brcontext->rb_width)
					break;

				if((x<<8) >= cx2/* || (y<<8) >= params->orig_v2.y*/)
					break;

				brvec3ui bary = linear_bary;
				if(_brcontext->persp_corr)
				{
					float w = 65536.0f / ((int)(bary.x*inv_v0_w + bary.y*inv_v1_w + bary.z*inv_v2_w));
					bary.x *= inv_v0_w * w;
					bary.y *= inv_v1_w * w;
					bary.z *= inv_v2_w * w;
				}

				brvec3 flt_bary = { (float)bary.x * _INV_65536, 
					(float)bary.y * _INV_65536, (float)bary.z * _INV_65536 };

				// safest to floating-point interpolate depths; they are in a large range and do not fit nicely to 16.16 fixed-point
				int64_t depth = 0;
				depth = params->z0 * flt_bary.x + params->z1 * flt_bary.y + params->z2 * flt_bary.z;

				if(depth_test)
				{
					int64_t dst = _get_depth(pixel_index);
					if(!_is_valid_depth(depth) || depth > dst)
					{
						linear_bary.x += inc_bx;
						linear_bary.y += inc_by;
						linear_bary.z += inc_bz;
						pixel_index += 1;
						continue;
					}
				}

				// 16.16 attributes multiplied by 16.16 barycentric coordinates
				uint32_t r = (((uint32_t)r0 * bary.x)>>16) + (((uint32_t)r1 * bary.y)>>16) + (((uint32_t)r2 * bary.z)>>16);
				uint32_t g = (((uint32_t)g0 * bary.x)>>16) + (((uint32_t)g1 * bary.y)>>16) + (((uint32_t)g2 * bary.z)>>16);
				uint32_t b = (((uint32_t)b0 * bary.x)>>16) + (((uint32_t)b1 * bary.y)>>16) + (((uint32_t)b2 * bary.z)>>16);
				uint32_t a = (((uint32_t)a0 * bary.x)>>16) + (((uint32_t)a1 * bary.y)>>16) + (((uint32_t)a2 * bary.z)>>16);
				uint32_t tx = 0;
				uint32_t ty = 0;
				if(params->complete_texture_unit)
				{
					tx = (((uint64_t)tx0 * bary.x)>>16) + (((uint64_t)tx1 * bary.y)>>16) + (((uint64_t)tx2 * bary.z)>>16);
					ty = (((uint64_t)ty0 * bary.x)>>16) + (((uint64_t)ty1 * bary.y)>>16) + (((uint64_t)ty2 * bary.z)>>16);
				}

				// actual texel coordinates
				tx = tx>>16;
				ty = ty>>16;

				// fragment shading operations
				brvec4ui rgba = { r, g, b, a };
				if(_brcontext->fshader || textured)
				{
					brvec4 primary = { r*_INV_65536, g*_INV_65536, b*_INV_65536, a*_INV_65536 };
					brvec4 secondary = { 0,0,0,0 };
					if(textured)
						_get_texel(tx, ty, &secondary, params->texture, params->texture_format, 
							params->texture_width, params->texture_height, params->texture_compressed);
					if(_brcontext->fshader)
					{
						if(textured)	frag_pass.color = secondary;
						else			frag_pass.color = primary;
						frag_pass.primitive_color = primary;
						frag_pass.texture_color = secondary;
						frag_pass.linear_bary.x = linear_bary.x * _INV_65536;
						frag_pass.linear_bary.y = linear_bary.y * _INV_65536;
						frag_pass.linear_bary.z = linear_bary.z * _INV_65536;
						frag_pass.bary = flt_bary;
						frag_pass.position.x = x;
						frag_pass.position.y = y;
						frag_pass.discard = false;

						// convert result fragment to 16.16, setting 'rgba'
						brvec4 color = _fragment_pass(&frag_pass);
						if(frag_pass.discard)
						{
							linear_bary.x += inc_bx;
							linear_bary.y += inc_by;
							linear_bary.z += inc_bz;
							pixel_index += 1;
							continue;
						}
						rgba.x = color.x * 65536.0f;
						rgba.y = color.y * 65536.0f;
						rgba.z = color.z * 65536.0f;
						rgba.w = color.w * 65536.0f;
					}
					else
					{
						// convert secondary color to 16.16, setting 'rgba'
						rgba.x = secondary.x * 65536.0f;
						rgba.y = secondary.y * 65536.0f;	
						rgba.z = secondary.z * 65536.0f;
						rgba.w = secondary.w * 65536.0f;
					}
				}

				if(plot_color)
					_plot_pixel(pixel_index, rgba, _brcontext->blend);

				if(plot_depth && _is_valid_depth(depth))
					_plot_depth(pixel_index, depth);

				linear_bary.x += inc_bx;
				linear_bary.y += inc_by;
				linear_bary.z += inc_bz;
				pixel_index += 1;
			}

			curfx1 += invslope1;
			curfx2 += invslope2;
		}
	}
	// flat topped
	if(y0 == y1 && x0 != x1)
	{
//		int invslope1 = ((float)(params->orig_v2.x - params->orig_v0.x) / (float)(params->orig_v2.y - params->orig_v0.y)) * 256.0f;
//		int invslope2 = ((float)(params->orig_v2.x - params->orig_v1.x) / (float)(params->orig_v2.y - params->orig_v1.y)) * 256.0f;
		int invslope1 = _idiv(params->orig_v2.x - params->orig_v0.x, (params->orig_v2.y - params->orig_v0.y)>>8);
		int invslope2 = _idiv(params->orig_v2.x - params->orig_v1.x, (params->orig_v2.y - params->orig_v1.y)>>8);

		// 24.8 scanline left and right x
		int curfx1 = x2;
		int curfx2 = x2;

		
		int y0_int = y0 >> 8;
		if(params->draw_top)
			y0_int -= 1;
		for(int y = (y2>>8); y > y0_int; y -= 1)
		{
			if(y < 0)
				break;
			if(y >= _brcontext->rb_height)
			{
				curfx1 -= invslope1;
				curfx2 -= invslope2;
				continue;
			}
			
			int cx1, cx2;
			int sx1, sx2;
			cx1 = curfx1;
			cx2 = curfx2;
			if(cx1 > cx2)
			{
				int tmp = cx1;
				cx1 = cx2;
				cx2 = tmp;
			}
			sx1 = (cx1-128)>>8;
			sx2 = (cx2+128)>>8;
			
			if(sx1 > sx2)
			{
				curfx1 -= invslope1;
				curfx2 -= invslope2;
				continue;
			}
			int sy = y;

			if(sx1 < 0)
				sx1 = 0;
			if(sx1 >= _brcontext->rb_width)
			{
				curfx1 -= invslope1;
				curfx2 -= invslope2;
				continue;
			}

			int slength = (sx2-sx1)+1;

			// do scanline sx1->sx2 @ y sy
			// calculate 24.8 bary coords for start and end of scanline
			// if _brcontext->persp_corr, perspective correct bary
			// convert bary to 16.16 to interpolate 16.16 fixed point attributes
			// for texel coordinates, discard last 16 bits.
			// to convert from 16.16 to float, divide by 65536 (use _INV_65536).
			
			// if appropriate, scanline beginning/end will be offset by 256 (1 pixel; working with 1/256 coordinates)
			// in order to avoid sub-pixels that fall outside of triangle given Y increases by a full pixel
			// If this fails to generate valid barycentric coordinates, a loop will be run to iterate by half a pixel
			// until valid bary coordinates are generated.

			brvec3 bary_s1;
			brvec2i c;
			if(sx1 != sx2)
				c.x = ((sx1<<8)+256) - params->orig_v0.x;
			else
				c.x = cx1 - params->orig_v0.x;
			c.y = (y<<8) - params->orig_v0.y;
			bary_s1.y = (c.x*b.y-b.x*c.y)*den;
			bary_s1.z = (a.x*c.y-c.x*a.y)*den;
			bary_s1.x = 65536.0f - bary_s1.y - bary_s1.z;
			if(bary_s1.x < 0.0f || bary_s1.y < 0.0f || bary_s1.z < 0.0f) {
				int x = (sx1<<8)+384;
				bool cont = false;
				while(bary_s1.x < 0.0f || bary_s1.y < 0.0f || bary_s1.z < 0.0f) {
					c.x = x - params->orig_v0.x;
					bary_s1.y = (c.x*b.y-b.x*c.y)*den;
					bary_s1.z = (a.x*c.y-c.x*a.y)*den;
					bary_s1.x = 65536.0f - bary_s1.y - bary_s1.z;
					x += 128;
					if(x > cx2) {
						curfx1 -= invslope1;
						curfx2 -= invslope2;
						cont = true;
						break;
					}
				}
				if(cont) continue;
			}
			{
				float bx = bary_s1.x;
				float by = bary_s1.y;
				float bz = bary_s1.z;
				bary_s1.x = bx * (params->bary0.x) + by * (params->bary1.x) + bz * (params->bary2.x);
				bary_s1.y = bx * (params->bary0.y) + by * (params->bary1.y) + bz * (params->bary2.y);
				bary_s1.z = bx * (params->bary0.z) + by * (params->bary1.z) + bz * (params->bary2.z);
			}

			brvec3 bary_s2;
			if(sx1 != sx2)
				c.x = ((sx2<<8)-256) - params->orig_v0.x;
			else
				c.x = cx2 - params->orig_v0.x;
			bary_s2.y = (c.x*b.y-b.x*c.y)*den;
			bary_s2.z = (a.x*c.y-c.x*a.y)*den;
			bary_s2.x = 65536.0f - bary_s2.y - bary_s2.z;
			if(bary_s2.x < 0.0f || bary_s2.y < 0.0f || bary_s2.z < 0.0f) {
				int x = (sx2<<8)-384;
				bool cont = false;
				while(bary_s2.x < 0.0f || bary_s2.y < 0.0f || bary_s2.z < 0.0f) {
					c.x = x - params->orig_v0.x;
					bary_s2.y = (c.x*b.y-b.x*c.y)*den;
					bary_s2.z = (a.x*c.y-c.x*a.y)*den;
					bary_s2.x = 65536.0f - bary_s2.y - bary_s2.z;
					x -= 128;
					if(x < cx1) {
						curfx1 -= invslope1;
						curfx2 -= invslope2;
						cont = true;
						break;
					}
				}
				if(cont) continue;
			}
			{
				float bx = bary_s2.x;
				float by = bary_s2.y;
				float bz = bary_s2.z;
				bary_s2.x = bx * (params->bary0.x) + by * (params->bary1.x) + bz * (params->bary2.x);
				bary_s2.y = bx * (params->bary0.y) + by * (params->bary1.y) + bz * (params->bary2.y);
				bary_s2.z = bx * (params->bary0.z) + by * (params->bary1.z) + bz * (params->bary2.z);
			}
			
			int inc_bx = (bary_s2.x - bary_s1.x)/slength;
			int inc_by = (bary_s2.y - bary_s1.y)/slength;
			int inc_bz = (bary_s2.z - bary_s1.z)/slength;
						
			uint32_t pixel_index = y * _brcontext->rb_width + sx1;

			brvec3ui linear_bary;
			linear_bary.x = bary_s1.x;
			linear_bary.y = bary_s1.y;
			linear_bary.z = bary_s1.z;
			for(int x = sx1; x <= sx2; x += 1)
			{
				if(x >= _brcontext->rb_width)
					break;

				if((x<<8) >= cx2/* || (y<<8) >= params->orig_v2.y*/)
					break;

				brvec3ui bary = linear_bary;
				if(_brcontext->persp_corr)
				{
					float w = 65536.0f / ((int)(bary.x*inv_v0_w + bary.y*inv_v1_w + bary.z*inv_v2_w));
					bary.x *= inv_v0_w * w;
					bary.y *= inv_v1_w * w;
					bary.z *= inv_v2_w * w;
				}

				brvec3 flt_bary = { (float)bary.x * _INV_65536, 
					(float)bary.y * _INV_65536, (float)bary.z * _INV_65536 };

				// safest to floating-point interpolate depths; they are in a large range and do not fit nicely to 16.16 fixed-point
				int64_t depth = 0;
				depth = params->z0 * flt_bary.x + params->z1 * flt_bary.y + params->z2 * flt_bary.z;

				if(depth_test)
				{
					int64_t dst = _get_depth(pixel_index);
					if(!_is_valid_depth(depth) || depth > dst)
					{
						linear_bary.x += inc_bx;
						linear_bary.y += inc_by;
						linear_bary.z += inc_bz;
						pixel_index += 1;
						continue;
					}
				}

				// 16.16 attributes multiplied by 16.16 barycentric coordinates
				uint32_t r = (((uint32_t)r0 * bary.x)>>16) + (((uint32_t)r1 * bary.y)>>16) + (((uint32_t)r2 * bary.z)>>16);
				uint32_t g = (((uint32_t)g0 * bary.x)>>16) + (((uint32_t)g1 * bary.y)>>16) + (((uint32_t)g2 * bary.z)>>16);
				uint32_t b = (((uint32_t)b0 * bary.x)>>16) + (((uint32_t)b1 * bary.y)>>16) + (((uint32_t)b2 * bary.z)>>16);
				uint32_t a = (((uint32_t)a0 * bary.x)>>16) + (((uint32_t)a1 * bary.y)>>16) + (((uint32_t)a2 * bary.z)>>16);
				uint32_t tx = 0;
				uint32_t ty = 0;
				if(params->complete_texture_unit)
				{
					tx = (((uint64_t)tx0 * bary.x)>>16) + (((uint64_t)tx1 * bary.y)>>16) + (((uint64_t)tx2 * bary.z)>>16);
					ty = (((uint64_t)ty0 * bary.x)>>16) + (((uint64_t)ty1 * bary.y)>>16) + (((uint64_t)ty2 * bary.z)>>16);
				}

				// actual texel coordinates
				tx = tx>>16;
				ty = ty>>16;

				// fragment shading operations
				brvec4ui rgba = { r, g, b, a };
				if(_brcontext->fshader || textured)
				{
					brvec4 primary = { r*_INV_65536, g*_INV_65536, b*_INV_65536, a*_INV_65536 };
					brvec4 secondary = { 0,0,0,0 };
					if(textured)
						_get_texel(tx, ty, &secondary, params->texture, params->texture_format, 
							params->texture_width, params->texture_height, params->texture_compressed);
					if(_brcontext->fshader)
					{
						if(textured)	frag_pass.color = secondary;
						else			frag_pass.color = primary;
						frag_pass.primitive_color = primary;
						frag_pass.texture_color = secondary;
						frag_pass.linear_bary.x = linear_bary.x * _INV_65536;
						frag_pass.linear_bary.y = linear_bary.y * _INV_65536;
						frag_pass.linear_bary.z = linear_bary.z * _INV_65536;
						frag_pass.bary = flt_bary;
						frag_pass.position.x = x;
						frag_pass.position.y = y;
						frag_pass.discard = false;

						// convert result fragment to 16.16, setting 'rgba'
						brvec4 color = _fragment_pass(&frag_pass);
						if(frag_pass.discard)
						{
							linear_bary.x += inc_bx;
							linear_bary.y += inc_by;
							linear_bary.z += inc_bz;
							pixel_index += 1;
							continue;
						}
						rgba.x = color.x * 65536.0f;
						rgba.y = color.y * 65536.0f;
						rgba.z = color.z * 65536.0f;
						rgba.w = color.w * 65536.0f;
					}
					else
					{
						// convert secondary color to 16.16, setting 'rgba'
						rgba.x = secondary.x * 65536.0f;
						rgba.y = secondary.y * 65536.0f;	
						rgba.z = secondary.z * 65536.0f;
						rgba.w = secondary.w * 65536.0f;
					}
				}

				if(plot_color)
					_plot_pixel(pixel_index, rgba, _brcontext->blend);

				if(plot_depth && _is_valid_depth(depth))
					_plot_depth(pixel_index, depth);

				linear_bary.x += inc_bx;
				linear_bary.y += inc_by;
				linear_bary.z += inc_bz;
				pixel_index += 1;
			}

			curfx1 -= invslope1;
			curfx2 -= invslope2;
		}
	}
	
	if(_brcontext->fshader) {
	if(frag_pass.pass_data)
		free(frag_pass.pass_data);
	if(frag_pass.pass_attribs)
		free(frag_pass.pass_attribs);
	}
}

// split a triangle and raster both halves
void _split_raster_triangle(_raster_triangle_t* triangle)
{
	// sort vertices v0.y <= v1.y <= v2.y
	// swap raster position & barycentric coordinates
	
	if(triangle->y2 < triangle->y1)
	{
		// swap v2 with v1
		float tmp = triangle->x1;
		triangle->x1 = triangle->x2;
		triangle->x2 = tmp;
		tmp = triangle->y1;
		triangle->y1 = triangle->y2;
		triangle->y2 = tmp;
		
		brvec3 tmp_bary = triangle->bary1;
		triangle->bary1 = triangle->bary2;
		triangle->bary2 = tmp_bary;
		
		brvec2i tmp_orig = triangle->orig_v1;
		triangle->orig_v1 = triangle->orig_v2;
		triangle->orig_v2 = tmp_orig;
	}
	
	if(triangle->y1 < triangle->y0)
	{
		// swap v1 with v0
		float tmp = triangle->x0;
		triangle->x0 = triangle->x1;
		triangle->x1 = tmp;
		tmp = triangle->y0;
		triangle->y0 = triangle->y1;
		triangle->y1 = tmp;
		
		brvec3 tmp_bary = triangle->bary0;
		triangle->bary0 = triangle->bary1;
		triangle->bary1 = tmp_bary;
		
		brvec2i tmp_orig = triangle->orig_v0;
		triangle->orig_v0 = triangle->orig_v1;
		triangle->orig_v1 = tmp_orig;
	}
	
	if(triangle->y2 < triangle->y1)
	{
		// swap v2 with v1
		float tmp = triangle->x1;
		triangle->x1 = triangle->x2;
		triangle->x2 = tmp;
		tmp = triangle->y1;
		triangle->y1 = triangle->y2;
		triangle->y2 = tmp;
		
		brvec3 tmp_bary = triangle->bary1;
		triangle->bary1 = triangle->bary2;
		triangle->bary2 = tmp_bary;
		
		brvec2i tmp_orig = triangle->orig_v1;
		triangle->orig_v1 = triangle->orig_v2;
		triangle->orig_v2 = tmp_orig;
	}
	
	triangle->draw_top = false;
	
	if(triangle->y1 == triangle->y2)			// flat-bottomed
		_raster_triangle(triangle);
	else if(triangle->y0 == triangle->y1)		// flat-topped
	{
		triangle->draw_top = true;
		_raster_triangle(triangle);
	}
	else
	{
		// split 'triangle' in half and raster both halves
		brvec2 v;
		v.x = triangle->x0 + ((triangle->y1 - triangle->y0) / (triangle->y2 - triangle->y0)) * (triangle->x2 - triangle->x0);
		v.y = triangle->y1;

		_raster_triangle_t second_half = *triangle;
		
		// top half: construct triangle as v0, v1, v
		
		triangle->x2 = v.x;
		triangle->y2 = v.y;
		second_half.x0 = second_half.x1;
		second_half.y0 = second_half.y1;
		second_half.x1 = v.x;
		second_half.y1 = v.y;
		_raster_triangle(triangle);
		_raster_triangle(&second_half);
	}
}

bool in_frustum(brvec4 v)
{
	return (-v.w <= v.x) && (v.x <= v.w)
	    && (-v.w <= v.y) && (v.y <= v.w)
	    && (-v.w <= v.z) && (v.z <= v.w);
}

#define LEFT_BIT (1<<0)
#define RIGHT_BIT (1<<1)
#define BOTTOM_BIT (1<<2)
#define TOP_BIT (1<<3)
#define NEAR_BIT (1<<4)
#define FAR_BIT (1<<5)
float dot(int32_t plane, brvec4 v)
{
	switch(plane)
	{
		case 0: return  v.x + v.w;
		case 1: return -v.x + v.w;
		case 2: return  v.y + v.w;
		case 3: return -v.y + v.w;
		case 4: return  v.z + v.w;
		case 5: return -v.w + v.w;
	}
	return 0;	// bad plane code
}

// cohen-sutherland outcode
uint8_t get_outcode(brvec4 v)
{
	uint8_t outcode = 0;
	if(v.x < -v.w) outcode |= LEFT_BIT;
	if(v.x >  v.w) outcode |= RIGHT_BIT;
	if(v.y < -v.w) outcode |= BOTTOM_BIT;
	if(v.y >  v.w) outcode |= TOP_BIT;
	if(v.z < -v.w) outcode |= NEAR_BIT;
	if(v.z >  v.w) outcode |= FAR_BIT;
	return outcode;
}

brvec4 lerp(brvec4 a, brvec4 b, float t)
{
	float t1 = 1.0f - t;
	brvec4 out;
	out.x = t1 * a.x + t * b.x;
	out.y = t1 * a.y + t * b.y;
	out.z = t1 * a.z + t * b.z;
	out.w = t1 * a.w + t * b.w;
	return out;
}

brvec4 old_pos;
uint8_t old_outcode;

brvec4* result_old, *result_new;
void clipper(bool draw, brvec4 v)
{
	brvec4 lerped;
	float aold, anew, alpha;
	uint8_t p, i, new_outcode, mask;
	new_outcode = get_outcode(v);
	
	if(!draw) {
		if(new_outcode == 0)
		{
			old_pos = v;
			old_outcode = new_outcode;
		}
		return;
	}
	else
	{
		if((new_outcode & old_outcode) == 0)
		{
			mask = new_outcode | old_outcode;
			if(mask == 0)
			{
				*result_new = v;
				*result_old = old_pos;
			}
			else
			{
				aold = anew = 0;
				p = 1;
				for(i = 0; i < 6; ++i)
				{
					if(mask & p)
					{
						aold = dot(i,old_pos);
						anew = dot(i,v);
						alpha = _fdiv(aold, aold-anew);
						printf("clipper alpha: %.4f\n", alpha);
						if(old_outcode & p)
						{
							if(aold < alpha) aold = alpha;
						}
						else if(anew > alpha) anew = alpha;
					}
					p <<= 1;						
				}
				if(i >= 6)
				{
					if(old_outcode)
					{
						lerped = lerp(old_pos, v, aold);
						old_pos = lerped;
						old_outcode = get_outcode(lerped);
					}
					if(new_outcode)
					{
						lerped = lerp(old_pos, v, anew);
						*result_new = lerped;
						*result_old = old_pos;
						old_pos = lerped;
						old_outcode = get_outcode(lerped);
					}
					else
					{
						*result_new = v;
						*result_old = old_pos;
						old_pos = v;
						old_outcode = get_outcode(v);
					}
				}
			}
		}
	}
	
	old_outcode = new_outcode;
	old_pos = v;
}

void clip_line(brvec4* a, brvec4* b)
{
	if(!in_frustum(*a) && !in_frustum(*b))
		return;	// line completely outside view
	if(in_frustum(*a) && in_frustum(*b))
		return;	// fast accept
	result_old = a;
	result_new = b;
	clipper(false, *a);
	clipper(true, *b);
}

float get_comp(brvec4 v, int32_t comp)
{
	switch(comp)
	{
		case 0: return v.x;
		case 1: return v.y;
		case 2: return v.z;
		case 3: return v.w;
		default: return 0;
	}
}

void add_vert_to_list(brvec4** array, uint32_t* ecount, brvec4 vertex)
{
	if(!(*array))
	{
		*array = (brvec4*) calloc(*ecount+1, sizeof(brvec4));
		(*array) [*ecount] = vertex;
		(*ecount)++;
	}
	else
	{
		*array = (brvec4*) realloc(*array, (*ecount+1)*sizeof(brvec4));
		(*array) [*ecount] = vertex;
		(*ecount)++;
	}
}

void clear_vert_list(brvec4** array, uint32_t* ecount)
{
	free(*array);
	*array = 0;
	*ecount = 0;
}

// used to clip list of vertices. 
// Initially, verts is an array of clip-space vertices to clip and vert_count is the count of them.
// Only suited for clipping in 2D (Z and W of clipped vertices should not change... barycentric interpolation 
// set to interpolate across the original triangle so they should remain their original Z and W)
bool clip_poly_component(brvec4** verts, uint32_t* vert_count, int comp, float comp_factor)
{
	uint32_t in_count = *vert_count;
	brvec4 in[in_count];
	memcpy(in, *verts, in_count*sizeof(brvec4));
	clear_vert_list(verts, vert_count);
	
	brvec4 previous_vertex = in[in_count - 1];
	float previous_component = get_comp(previous_vertex, comp) * comp_factor;
	bool previous_inside = previous_component <= previous_vertex.w;	
	for(int i = 0; i < in_count; i++)
	{
		brvec4 current_vertex = in[i];
		float current_component = get_comp(current_vertex, comp) * comp_factor;
		bool current_inside = current_component <= current_vertex.w;
		
		if(current_inside ^ previous_inside)
		{	
			float x = previous_vertex.w - previous_component;
			float amt = x / (x - (current_vertex.w - current_component));
//			printf("lerp amt: %.4f\n", amt);
			if(amt > 1.0f || amt < 0.0f)
				printf("WARNING: lerp amount %.4f not within range [0,1]\n", amt);
			brvec4 clipped = { 
					((1.0f - amt) * previous_vertex.x + current_vertex.x * amt),
					((1.0f - amt) * previous_vertex.y + current_vertex.y * amt),
					((1.0f - amt) * previous_vertex.z + current_vertex.z * amt), 
					((1.0f - amt) * previous_vertex.w + current_vertex.w * amt)};
			add_vert_to_list(verts, vert_count, clipped);
		}
		else if(current_inside)
		{
			add_vert_to_list(verts, vert_count, current_vertex);
		}
		
		previous_component = current_component;
		previous_inside = current_inside;
		previous_vertex = current_vertex;
	}
}

// clip a triangle
bool clip_triangle(brvec4** verts, uint32_t* vert_count)
{	
	uint32_t in_count = *vert_count;
	brvec4 in[in_count];
	memcpy(in, *verts, in_count*sizeof(brvec4));
	clear_vert_list(verts, vert_count);

	/*int32_t count = in_frustum(in[0]) + in_frustum(in[1]) + in_frustum(in[2]);
	if(count == 1)
	{
		printf("begin special-case clip\n");

		brvec4 inner_vertex;
		
		for(int i = 0; i < 3; i++)
			if(in_frustum(in[i]))
			{
				inner_vertex = in[i];
				break;
			}
			
		add_vert_to_list(verts, vert_count, inner_vertex);
			
		for(int i = 0; i < 3; i++)
			if(!in_frustum(in[i]))
			{
				brvec4 a = inner_vertex;
				brvec4 b = in[i];
			
				clip_line(&a, &b);
				add_vert_to_list(verts, vert_count, b);
			}
			
		printf("end special-case clip with vert count %d\n", *vert_count);
		return true;
	}*/
	
	brvec4 previous_vertex = in[in_count - 1];
	bool previous_inside = in_frustum(previous_vertex);	
	for(int i = 0; i < in_count; i++)
	{
		brvec4 current_vertex = in[i];
		bool current_inside = in_frustum(current_vertex);
		
		if(current_inside ^ previous_inside)
		{
			brvec4 a = previous_vertex;
			brvec4 b = current_vertex;
			
			clip_line(&a, &b);
			if(previous_inside)
				add_vert_to_list(verts, vert_count, b);
			if(current_inside)
				add_vert_to_list(verts, vert_count, a);
		}
		if(current_inside)
			add_vert_to_list(verts, vert_count, current_vertex);
			
		previous_inside = current_inside;
		previous_vertex = current_vertex;
	}
	
	return (*vert_count) != 0;
}

// used to clip a polygon on one of the axes (specified by comp).
bool clip_poly_axis(brvec4** verts, uint32_t* vert_count, int comp)
{	
	// clip against W
	clip_poly_component(verts, vert_count, comp, 1.0f);

	if(*vert_count == 0)
		return false;

	// clip against -W
	clip_poly_component(verts, vert_count, comp, -1.0f);
	
	return (*vert_count) != 0;
}

typedef struct _raster_point_t _raster_point_t;
struct _raster_point_t
{
	// raster-space position of point
	float x, y;
	
	// 16.16 fixed-point point color
	brvec4ui rgba;
	
	// point radius
	uint32_t r;
	
	// point z and w; w in clip-space and z in raster-space
	int64_t z;
	float w;
};
void _raster_point(_raster_point_t* params);

// post-process and raster a triangle (vertex shader pass, _vertex_pass, not performed here)
// will cause harm to contents of 'triangle'
void _process_triangle(_triangle_t* triangle)
{
	if(!_brcontext || !triangle)
		return;
	
	// perform primitive processing of 'triangle'
	
	float half_width  = _brcontext->rb_width  * 0.5f;
	float half_height = _brcontext->rb_height * 0.5f;
	
	// cull parent triangles when appropriate
	if(_brcontext->cull && !triangle->parent)
	{
		brvec3 n;
		bool cw = false;
		brvec3 w_v0 = { triangle->v0.x, triangle->v0.y, 0 };
		brvec3 w_v1 = { triangle->v1.x, triangle->v1.y, 0 };
		brvec3 w_v2 = { triangle->v2.x, triangle->v2.y, 0 };
		n = _cross_vec3(_sub_vec3(w_v1, w_v0), _sub_vec3(w_v2, w_v0));
		if(n.z > 0)
			cw = true;
			
		if(cw && _brcontext->cull_winding == BR_CW)
			return;
		if(!cw && _brcontext->cull_winding == BR_CCW)
			return;
	}
	
	// this is a parent triangle that is completely on the wrong side of one of the clipping planes
	if(_brcontext->clip && !triangle->parent && 
	!in_frustum(triangle->v0) && !in_frustum(triangle->v1) && !in_frustum(triangle->v2))
		return;
	
	// this is a parent triangle that needs to be clipped (atleast one vertex not in clip bounds)
	// this will only be run once (all clipping is done at once)
	// in the case that all vertices are within clipping bounds; this won't even have to run!
	if(_brcontext->clip && !triangle->parent && 
	( !in_frustum(triangle->v0) || !in_frustum(triangle->v1) || !in_frustum(triangle->v2) ) )
	{
		//printf("clipping triangle\n");

		_triangle_t child = *triangle;
		child.parent = triangle;
		
		// this will be the vertex list prior to clipping
		brvec4* verts = (brvec4*) calloc(3, sizeof(brvec4));
		verts[0] = triangle->v0;
		verts[1] = triangle->v1;
		verts[2] = triangle->v2;
		uint32_t vert_count = 3;
		
		// process vertices in child.parent (this triangle) so that we can calculate original vertex raster positions of parent
		if(_brcontext->persp_div && triangle->v0.w != 0.0f && triangle->v0.w != 1.0f)
		{
			float inv_v0_w = _fdiv(1.0f, triangle->v0.w);
			triangle->v0.x *= inv_v0_w;
			triangle->v0.y *= inv_v0_w;
			triangle->v0.z *= inv_v0_w;
		}
		if(_brcontext->persp_div && triangle->v1.w != 0.0f && triangle->v1.w != 1.0f)
		{
			float inv_v1_w = _fdiv(1.0f, triangle->v1.w);
			triangle->v1.x *= inv_v1_w;
			triangle->v1.y *= inv_v1_w;
			triangle->v1.z *= inv_v1_w;
		}
		if(_brcontext->persp_div && triangle->v2.w != 0.0f && triangle->v2.w != 1.0f)
		{
			float inv_v2_w = _fdiv(1.0f, triangle->v2.w);
			triangle->v2.x *= inv_v2_w;
			triangle->v2.y *= inv_v2_w;
			triangle->v2.z *= inv_v2_w;
		}
		if(_brcontext->scale_z)
		{
			triangle->v0.z *= 0.5f + 0.5f;
			triangle->v1.z *= 0.5f + 0.5f;
			triangle->v2.z *= 0.5f + 0.5f;
		}
		
		triangle->parent_orig_v0 =
			{ (half_width  + ( triangle->v0.x * half_width))  * 256.0f,
			  (half_height + (-triangle->v0.y * half_height)) * 256.0f};
		triangle->parent_orig_v1 =
			{ (half_width  + ( triangle->v1.x * half_width))  * 256.0f,
			  (half_height + (-triangle->v1.y * half_height)) * 256.0f};
		triangle->parent_orig_v2 =
			{ (half_width  + ( triangle->v2.x * half_width))  * 256.0f,
			  (half_height + (-triangle->v2.y * half_height)) * 256.0f};
		
		// generate, read, and process clipped triangles using the clip-space verts list
		// (create copies of 'child', fill with vertices, send through _process_triangle)
		if(clip_triangle(&verts, &vert_count))
		{
			//printf("clipped vert count: %d\n", vert_count);
			
			brvec4 initial = verts[0];
			if(vert_count > 1)
			for(int i = 1; i < vert_count - 1; i++)
			{
				// create child triangle from (initial, verts[i], verts[i+1])
				_triangle_t clipped = child;
				clipped.v0 = initial;
				clipped.v1 = verts[i];
				clipped.v2 = verts[i+1];
				_process_triangle(&clipped);
			}
		}
		
		clear_vert_list(&verts, &vert_count);
		return;
	}
	
	_raster_triangle_t raster_triangle;
	
	raster_triangle.bary0 = { 1, 0, 0 };
	raster_triangle.bary1 = { 0, 1, 0 };
	raster_triangle.bary2 = { 0, 0, 1 };
	
	if(_brcontext->persp_div && triangle->v0.w != 0.0f && triangle->v0.w != 1.0f)
	{
		float inv_v0_w = _fdiv(1.0f, triangle->v0.w);
		triangle->v0.x *= inv_v0_w;
		triangle->v0.y *= inv_v0_w;
		triangle->v0.z *= inv_v0_w;
	}
	if(_brcontext->persp_div && triangle->v1.w != 0.0f && triangle->v1.w != 1.0f)
	{
		float inv_v1_w = _fdiv(1.0f, triangle->v1.w);
		triangle->v1.x *= inv_v1_w;
		triangle->v1.y *= inv_v1_w;
		triangle->v1.z *= inv_v1_w;
	}
	if(_brcontext->persp_div && triangle->v2.w != 0.0f && triangle->v2.w != 1.0f)
	{
		float inv_v2_w = _fdiv(1.0f, triangle->v2.w);
		triangle->v2.x *= inv_v2_w;
		triangle->v2.y *= inv_v2_w;
		triangle->v2.z *= inv_v2_w;
	}
	if(_brcontext->scale_z)
	{
		triangle->v0.z *= 0.5f + 0.5f;
		triangle->v1.z *= 0.5f + 0.5f;
		triangle->v2.z *= 0.5f + 0.5f;
	}
	
	uint32_t tunit = _brcontext->texture_unit;
	raster_triangle.complete_texture_unit = ( _brcontext->textures[tunit] && _brcontext->texture_widths[tunit] > 0
		&& _brcontext->texture_heights[tunit] > 0 && _is_pixel_format(_brcontext->texture_formats[tunit]) );
	if(raster_triangle.complete_texture_unit)
	{
		raster_triangle.texture        = _brcontext->textures[_brcontext->texture_unit];
		raster_triangle.texture_width  = _brcontext->texture_widths[_brcontext->texture_unit];
		raster_triangle.texture_height     = _brcontext->texture_heights[_brcontext->texture_unit];
		raster_triangle.texture_format     = _brcontext->texture_formats[_brcontext->texture_unit];
		raster_triangle.texture_compressed = _brcontext->texture_compressed_booleans[_brcontext->texture_unit];
		raster_triangle.tx0.x = triangle->tcoords0.x * (raster_triangle.texture_width - 1) * 65536;
		raster_triangle.tx0.y = (1.0f - triangle->tcoords0.y) * (raster_triangle.texture_height - 1) * 65536;
		raster_triangle.tx1.x = triangle->tcoords1.x * (raster_triangle.texture_width - 1) * 65536;
		raster_triangle.tx1.y = (1.0f - triangle->tcoords1.y) * (raster_triangle.texture_height - 1) * 65536;
		raster_triangle.tx2.x = triangle->tcoords2.x * (raster_triangle.texture_width - 1) * 65536;
		raster_triangle.tx2.y = (1.0f - triangle->tcoords2.y) * (raster_triangle.texture_height - 1) * 65536;
	}
	
	raster_triangle.x0 = half_width  + ( triangle->v0.x * half_width);
	raster_triangle.y0 = half_height + (-triangle->v0.y * half_height);
	raster_triangle.x1 = half_width  + ( triangle->v1.x * half_width);
	raster_triangle.y1 = half_height + (-triangle->v1.y * half_height);
	raster_triangle.x2 = half_width  + ( triangle->v2.x * half_width);
	raster_triangle.y2 = half_height + (-triangle->v2.y * half_height);
	
	_raster_point_t pt;
	pt.x = raster_triangle.x0;
	pt.y = raster_triangle.y0;
	pt.rgba = { 65536, 65536, 65536, 65536 };
	pt.r = 2;
	pt.z = 0;
	pt.w = 1;
	_raster_point(&pt);
	pt.x = raster_triangle.x1;
	pt.y = raster_triangle.y1;
	pt.rgba = { 65536, 65536, 65536, 65536 };
	pt.r = 2;
	pt.z = 0;
	pt.w = 1;
	_raster_point(&pt);
	pt.x = raster_triangle.x2;
	pt.y = raster_triangle.y2;
	pt.rgba = { 65536, 65536, 65536, 65536 };
	pt.r = 2;
	pt.z = 0;
	pt.w = 1;
	_raster_point(&pt);
	
	if(triangle->parent)	// is a child (clipped) triangle
	{
		raster_triangle.orig_v0 = triangle->parent->parent_orig_v0;
		raster_triangle.orig_v1 = triangle->parent->parent_orig_v1;
		raster_triangle.orig_v2 = triangle->parent->parent_orig_v2;
		triangle->v0.z = triangle->parent->v0.z;
		triangle->v1.z = triangle->parent->v1.z;
		triangle->v2.z = triangle->parent->v2.z;
		triangle->v0.w = triangle->parent->v0.w;
		triangle->v1.w = triangle->parent->v1.w;
		triangle->v2.w = triangle->parent->v2.w;
	}
	else	// clipping was not performed
	{
		raster_triangle.orig_v0 = { raster_triangle.x0 * 256.0f, raster_triangle.y0 * 256.0f };
		raster_triangle.orig_v1 = { raster_triangle.x1 * 256.0f, raster_triangle.y1 * 256.0f };
		raster_triangle.orig_v2 = { raster_triangle.x2 * 256.0f, raster_triangle.y2 * 256.0f };
	}

	// these will not be modified from their original values via clipping;
	// interpolated Z and W depend on the barycentric region, which is, in fact, between the original coordinates 
	// of the original triangle; therefore sub-triangles only need to worry about their 2D coordinates (x,y).
	raster_triangle.z0 = _convert_depth(triangle->v0.z);
	raster_triangle.z1 = _convert_depth(triangle->v1.z);
	raster_triangle.z2 = _convert_depth(triangle->v2.z);
	raster_triangle.w0 = triangle->v0.w;
	raster_triangle.w1 = triangle->v1.w;
	raster_triangle.w2 = triangle->v2.w;
	
	raster_triangle.rgba0.x = triangle->rgba0.x * 65536.0f;
	raster_triangle.rgba0.y = triangle->rgba0.y * 65536.0f;
	raster_triangle.rgba0.z = triangle->rgba0.z * 65536.0f;
	raster_triangle.rgba0.w = triangle->rgba0.w * 65536.0f;
	raster_triangle.rgba1.x = triangle->rgba1.x * 65536.0f;
	raster_triangle.rgba1.y = triangle->rgba1.y * 65536.0f;
	raster_triangle.rgba1.z = triangle->rgba1.z * 65536.0f;
	raster_triangle.rgba1.w = triangle->rgba1.w * 65536.0f;
	raster_triangle.rgba2.x = triangle->rgba2.x * 65536.0f;
	raster_triangle.rgba2.y = triangle->rgba2.y * 65536.0f;
	raster_triangle.rgba2.z = triangle->rgba2.z * 65536.0f;
	raster_triangle.rgba2.w = triangle->rgba2.w * 65536.0f;
	
	_split_raster_triangle(&raster_triangle);
}

// a line ready for post-processing
typedef struct _line_t _line_t;
struct _line_t
{
	// clip-space vertex positions
	brvec4 v0, v1;
	
	// vertex colors
	brvec4 rgba0, rgba1;
	
	// texture coordinates (0-1), origin in bottom left
	brvec2 tcoords0, tcoords1;
};

// a line ready for rasterization
typedef struct _raster_line_t _raster_line_t;
struct _raster_line_t
{
	// raster-space coordinates of vertices
	// will change per sub-line
	float x0, x1;
	float y0, y1;
	// overrides barycentric coordinates of vertices
	// used for sub-lines
	brvec3 bary0;
	brvec3 bary1;
	// vertex z and w; w in clip-space and z in raster-space
	int64_t z0, z1;
	float w0, w1;
	// 16.16 fixed-point vertex colors (*65536)
	brvec4ui rgba0;
	brvec4ui rgba1;
	// 16.16 fixed-point texel coordinates (*65536)
	brvec2ui tx0, tx1;
	// texture unit information @ time of raster
	void* texture;
	uint32_t texture_width;
	uint32_t texture_height;
	uint32_t texture_format;
	bool texture_compressed;
	bool complete_texture_unit;
};

// raster a line
void _raster_line(_raster_line_t* params)
{
	if(!params)
		return;
	if(!_brcontext)
		return;
		
	bool depth_test = (_brcontext->depth_test && _brcontext->db);
	bool plot_color = _brcontext->cb;
	bool plot_depth = (_brcontext->depth_write && _brcontext->db);
	bool textured = (_brcontext->texture && params->complete_texture_unit);
	
	// for fragment passes
	_fragment_t frag_pass;
	if(_brcontext->fshader)
		_init_fragment(&frag_pass);
		
	// 24.8 fixed point
	int x0 = params->x0 * 256.0f;
	int x1 = params->x1 * 256.0f;
	int y0 = params->y0 * 256.0f;
	int y1 = params->y1 * 256.0f;
	// 16.16 fixed point attributes
	uint32_t r0 = params->rgba0.x;
	uint32_t g0 = params->rgba0.y;
	uint32_t b0 = params->rgba0.z;
	uint32_t a0 = params->rgba0.w;
	uint32_t r1 = params->rgba1.x;
	uint32_t g1 = params->rgba1.y;
	uint32_t b1 = params->rgba1.z;
	uint32_t a1 = params->rgba1.w;
	uint32_t tx0 = params->tx0.x;
	uint32_t tx1 = params->tx1.x;
	uint32_t ty0 = params->tx0.y;
	uint32_t ty1 = params->tx1.y;
	
	float length = sqrt(pow(params->x0 - params->x1, 2) + pow(params->y0 - params->y1, 2));
	if(length == 0.0f)
		return;
	float inv_length = 1.0f / length;
	int p = 0;
		
	float inv_v0_w = 0;
	float inv_v1_w = 0;
	if(_brcontext->persp_corr)
	{
		inv_v0_w = _fdiv(1.0f, params->w0);
		inv_v1_w = _fdiv(1.0f, params->w1);
	}
	
	int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;
		
	int x = params->x0;
	int y = params->y0;
	
	uint32_t y_index = y * _brcontext->rb_width;

	for(;;)
	{
		int lengthp = sqrt(pow(x - params->x0, 2) + pow(y - params->y0, 2));
		if(lengthp >= (int)length)
			break;	// this is possibly the least optimal way to do this
		
		if(x >= 0 && x < _brcontext->rb_width && y >= 0 && y < _brcontext->rb_height)
		{
			uint32_t pixel_index = y_index + x;

			brvec3 bc;
			brvec3i bary, linear_bary;
			bc.x = (length - p) * inv_length;	// percent of v0
			bc.y = 1.0f - bc.x;		// percent of v1
			bc.z = 0.0f;
			{
				float bx = bc.x;
				float by = bc.y;
				bc.x = bx * params->bary0.x + by * params->bary1.x;
				bc.y = bx * params->bary0.y + by * params->bary1.y;
			}
			
			// bary to 16.16 fixed point
			bc.x = bc.x*65536.0f;
			bc.y = bc.y*65536.0f;
			bc.z = bc.z*65536.0f;
			
			linear_bary = { bc.x, bc.y, bc.z };
			bary = linear_bary;
			if(_brcontext->persp_corr)
			{
				float w = _fdiv(65536.0f, (bary.x*inv_v0_w + bary.y*inv_v1_w));
				bary.x *= inv_v0_w;
				bary.y *= inv_v1_w;
				bary.x *= w;
				bary.y *= w;
			}
			
			brvec3 flt_bary = { (float)bary.x * _INV_65536, 
				(float)bary.y * _INV_65536, 0 };

			// safest to floating-point interpolate depths; they are in a large range and do not fit nicely to 16.16 fixed-point
			int64_t depth = 0;
			depth = params->z0 * flt_bary.x + params->z1 * flt_bary.y;

			if(depth_test)
			{
				int64_t dst = _get_depth(pixel_index);
				if(!_is_valid_depth(depth) || depth > dst)
				{
					p += 1;
					e2 = err;
					if(e2 > -dx) { err -= dy; x += sx; }
					if(e2 <  dy) { err += dx; y += sy; y_index += sy * _brcontext->rb_width; }
					continue;
				}
			}

			// 16.16 attributes multiplied by 16.16 barycentric coordinates
			uint32_t r = (((uint32_t)r0 * bary.x)>>16) + (((uint32_t)r1 * bary.y)>>16);
			uint32_t g = (((uint32_t)g0 * bary.x)>>16) + (((uint32_t)g1 * bary.y)>>16);
			uint32_t b = (((uint32_t)b0 * bary.x)>>16) + (((uint32_t)b1 * bary.y)>>16);
			uint32_t a = (((uint32_t)a0 * bary.x)>>16) + (((uint32_t)a1 * bary.y)>>16);
			uint32_t tx = 0;
			uint32_t ty = 0;
			if(params->complete_texture_unit)
			{
				tx = (((uint64_t)tx0 * bary.x)>>16) + (((uint64_t)tx1 * bary.y)>>16);
				ty = (((uint64_t)ty0 * bary.x)>>16) + (((uint64_t)ty1 * bary.y)>>16);
			}

			// actual texel coordinates
			tx = tx>>16;
			ty = ty>>16;
		
			// fragment shading operations
			brvec4ui rgba = { r, g, b, a };
			if(_brcontext->fshader || textured)
			{
				brvec4 primary = { r*_INV_65536, g*_INV_65536, b*_INV_65536, a*_INV_65536 };
				brvec4 secondary = { 0,0,0,0 };
				if(textured)
					_get_texel(tx, ty, &secondary, params->texture, params->texture_format, 
						params->texture_width, params->texture_height, params->texture_compressed);
				if(_brcontext->fshader)
				{
					if(textured)	frag_pass.color = secondary;
					else			frag_pass.color = primary;
					frag_pass.primitive_color = primary;
					frag_pass.texture_color = secondary;
					frag_pass.linear_bary.x = linear_bary.x * _INV_65536;
					frag_pass.linear_bary.y = linear_bary.y * _INV_65536;
					frag_pass.linear_bary.z = linear_bary.z * _INV_65536;
					frag_pass.bary = flt_bary;
					frag_pass.position.x = x;
					frag_pass.position.y = y;
					frag_pass.discard = false;

					// convert result fragment to 16.16, setting 'rgba'
					brvec4 color = _fragment_pass(&frag_pass);
					if(frag_pass.discard)
					{
						p += 1;
						e2 = err;
						if(e2 > -dx) { err -= dy; x += sx; }
						if(e2 <  dy) { err += dx; y += sy; y_index += sy * _brcontext->rb_width; }
						continue;
					}
					rgba.x = color.x * 65536.0f;
					rgba.y = color.y * 65536.0f;
					rgba.z = color.z * 65536.0f;
					rgba.w = color.w * 65536.0f;
				}
				else
				{
					// convert secondary color to 16.16, setting 'rgba'
					rgba.x = secondary.x * 65536.0f;
					rgba.y = secondary.y * 65536.0f;	
					rgba.z = secondary.z * 65536.0f;
					rgba.w = secondary.w * 65536.0f;
				}
			}
				
			if(plot_color)
				_plot_pixel(pixel_index, rgba, _brcontext->blend);

			if(plot_depth && _is_valid_depth(depth))
				_plot_depth(pixel_index, depth);
		}
		p += 1;
		e2 = err;
		if(e2 > -dx) { err -= dy; x += sx; }
		if(e2 <  dy) { err += dx; y += sy; y_index += sy * _brcontext->rb_width; }
	}
	
	if(_brcontext->fshader) {
	if(frag_pass.pass_data)
		free(frag_pass.pass_data);
	if(frag_pass.pass_attribs)
		free(frag_pass.pass_attribs);
	}
}

// post-process and raster a line (vertex shader pass, _vertex_pass, not performed here)
// will cause harm to contents of 'line'
void _process_line(_line_t* line)
{
	if(!_brcontext || !line)
		return;
		
	brvec4 orig_v0 = line->v0;
	brvec4 orig_v1 = line->v1;
	
	// perform primitive processing of 'line'
	bool clipped = false;
	
	// clip, if necessary. Will only ever generate one line.
	if(_brcontext->clip)
	{
		// clip 'line' against -w <= (x,y,z) <= w
		// will calculate linear barycentric coordinates relative to the original line,
		// if 'clipped' is true.
		
		if(!in_frustum(line->v0) && !in_frustum(line->v1))
			return;
		
		clip_line(&line->v0, &line->v1);
	}
	
	_raster_line_t raster_line;
	
	// line was not clipped
	if(!clipped)
	{
		raster_line.bary0 = { 1, 0, 0 };
		raster_line.bary1 = { 0, 1, 0 };
	}
	else	// line was clipped
	{
		// calculate LINEAR barycentric coordinates relative to the original line
		float length_v0p;	// where 'p' is a new endpoint and 'v0' is the original v0
		float length = sqrt(pow(orig_v0.x - orig_v1.x, 2) + pow(orig_v0.y - orig_v1.y, 2));

		if(length == 0.0f)
			return;
		float inv_length = 1.0f / length;
		
		length_v0p = sqrt(pow(orig_v0.x - line->v0.x, 2) + pow(orig_v0.y - line->v0.y, 2));

		raster_line.bary0.x = (length - length_v0p) * inv_length;
		raster_line.bary0.y = 1.0f - raster_line.bary0.x;
		raster_line.bary0.z = 0.0f;
		
		length_v0p = sqrt(pow(orig_v0.x - line->v1.x, 2) + pow(orig_v0.y - line->v1.y, 2));
		
		raster_line.bary1.x = (length - length_v0p) * inv_length;
		raster_line.bary1.y = 1.0f - raster_line.bary1.x;
		raster_line.bary1.z = 0.0f;
	}
	
	if(_brcontext->persp_div && line->v0.w != 0.0f && line->v0.w != 1.0f)
	{
		float inv_v0_w = _fdiv(1.0f, line->v0.w);
		line->v0.x *= inv_v0_w;
		line->v0.y *= inv_v0_w;
		line->v0.z *= inv_v0_w;
	}
	if(_brcontext->persp_div && line->v1.w != 0.0f && line->v1.w != 1.0f)
	{
		float inv_v1_w = _fdiv(1.0f, line->v1.w);
		line->v1.x *= inv_v1_w;
		line->v1.y *= inv_v1_w;
		line->v1.z *= inv_v1_w;
	}
	
	if(_brcontext->scale_z)
	{
		line->v0.z *= 0.5f + 0.5f;
		line->v1.z *= 0.5f + 0.5f;
	}
	
	uint32_t tunit = _brcontext->texture_unit;
	raster_line.complete_texture_unit = ( _brcontext->textures[tunit] && _brcontext->texture_widths[tunit] > 0
		&& _brcontext->texture_heights[tunit] > 0 && _is_pixel_format(_brcontext->texture_formats[tunit]) );
	if(raster_line.complete_texture_unit)
	{
		raster_line.texture        = _brcontext->textures[_brcontext->texture_unit];
		raster_line.texture_width  = _brcontext->texture_widths[_brcontext->texture_unit];
		raster_line.texture_height     = _brcontext->texture_heights[_brcontext->texture_unit];
		raster_line.texture_format     = _brcontext->texture_formats[_brcontext->texture_unit];
		raster_line.texture_compressed = _brcontext->texture_compressed_booleans[_brcontext->texture_unit];
		raster_line.tx0.x = line->tcoords0.x * (raster_line.texture_width - 1) * 65536;
		raster_line.tx0.y = (1.0f - line->tcoords0.y) * (raster_line.texture_height - 1) * 65536;
		raster_line.tx1.x = line->tcoords1.x * (raster_line.texture_width - 1) * 65536;
		raster_line.tx1.y = (1.0f - line->tcoords1.y) * (raster_line.texture_height - 1) * 65536;
	}
	
	float half_width  = _brcontext->rb_width  * 0.5f;
	float half_height = _brcontext->rb_height * 0.5f;
	
	raster_line.x0 = half_width  + ( line->v0.x * half_width);
	raster_line.y0 = half_height + (-line->v0.y * half_height);
	raster_line.x1 = half_width  + ( line->v1.x * half_width);
	raster_line.y1 = half_height + (-line->v1.y * half_height);

	raster_line.z0 = _convert_depth(line->v0.z);
	raster_line.z1 = _convert_depth(line->v1.z);
	raster_line.w0 = line->v0.w;
	raster_line.w1 = line->v1.w;
	
	raster_line.rgba0.x = line->rgba0.x * 65536.0f;
	raster_line.rgba0.y = line->rgba0.y * 65536.0f;
	raster_line.rgba0.z = line->rgba0.z * 65536.0f;
	raster_line.rgba0.w = line->rgba0.w * 65536.0f;
	raster_line.rgba1.x = line->rgba1.x * 65536.0f;
	raster_line.rgba1.y = line->rgba1.y * 65536.0f;
	raster_line.rgba1.z = line->rgba1.z * 65536.0f;
	raster_line.rgba1.w = line->rgba1.w * 65536.0f;
	
	_raster_point_t pt;
	pt.x = raster_line.x0;
	pt.y = raster_line.y0;
	pt.rgba = { 65536, 65536, 65536, 65536 };
	pt.r = 2;
	pt.z = 0;
	pt.w = 1;
	_raster_point(&pt);
	pt.x = raster_line.x1;
	pt.y = raster_line.y1;
	pt.rgba = { 65536, 65536, 65536, 65536 };
	pt.r = 2;
	pt.z = 0;
	pt.w = 1;
	_raster_point(&pt);
	
	_raster_line(&raster_line);
}

// a point ready for post-processing
typedef struct _point_t _point_t;
struct _point_t
{
	// clip-space point position
	brvec4 pos;
	
	// point color
	brvec4 rgba;
};

// a point ready for rasterization
/*typedef struct _raster_point_t _raster_point_t;
struct _raster_point_t
{
	// raster-space position of point
	float x, y;
	
	// 16.16 fixed-point point color
	brvec4ui rgba;
	
	// point radius
	uint32_t r;
	
	// point z and w; w in clip-space and z in raster-space
	int64_t z;
	float w;
};*/

// raster a fragment of a point; used by point rasterization algorithm
void _raster_point_fragment(int x, int y, _raster_point_t* point, _fragment_t* frag_pass)
{
	if(!_brcontext || (x < 0 || x >= _brcontext->rb_width || y < 0 || y >= _brcontext->rb_height))
		return;
	
	bool depth_test = (_brcontext->depth_test && _brcontext->db);
	bool plot_color = _brcontext->cb;
	bool plot_depth = (_brcontext->depth_write && _brcontext->db);
	
	uint32_t pixel_index = y * _brcontext->rb_width + x;
	
	int64_t depth = point->z;

	if(depth_test)
	{
		int64_t dst = _get_depth(pixel_index);
		if(!_is_valid_depth(depth) || depth > dst)
			return;
	}

	// 16.16 attributes
	uint32_t r = point->rgba.x;
	uint32_t g = point->rgba.y;
	uint32_t b = point->rgba.z;
	uint32_t a = point->rgba.w;
	uint32_t tx = 0;
	uint32_t ty = 0;
		
	// fragment shading operations
	brvec4ui rgba = { r, g, b, a };
	if(_brcontext->fshader)
	{
		brvec4 primary = { r*_INV_65536, g*_INV_65536, b*_INV_65536, a*_INV_65536 };
		brvec4 secondary = { 0,0,0,0 };
		frag_pass->color = primary;
		frag_pass->primitive_color = primary;
		frag_pass->texture_color = secondary;
		frag_pass->linear_bary = { 0,0,0 };
		frag_pass->bary = { 0,0,0 };
		frag_pass->position.x = x;
		frag_pass->position.y = y;
		frag_pass->discard = false;

		// convert result fragment to 16.16, setting 'rgba'
		brvec4 color = _fragment_pass(frag_pass);
		if(frag_pass->discard)
			return;
		rgba.x = color.x * 65536.0f;
		rgba.y = color.y * 65536.0f;
		rgba.z = color.z * 65536.0f;
		rgba.w = color.w * 65536.0f;
	}
				
	if(plot_color)
		_plot_pixel(pixel_index, rgba, _brcontext->blend);
			
	if(plot_depth && _is_valid_depth(depth))
		_plot_depth(pixel_index, depth);
}

// raster a point
void _raster_point(_raster_point_t* params)
{
	if(!params)
		return;
	if(!_brcontext)
		return;
	
	// for fragment passes
	_fragment_t frag_pass;
	if(_brcontext->fshader)
		_init_fragment(&frag_pass);
	
	uint32_t r = params->r;
	if(r <= 0) 
		return;
		
	int f = 1 - r;
	int dx = 0;
	int dy = -2 * r;
	int x2 = 0;
	int y2 = r;
	
	int point_x = params->x;
	int point_y = params->y;
	
/*	if(point_x - r >= _brcontext->rb_width)
		return;
	if(point_x + r < 0)
		return;
	if(point_y - r >= _brcontext->rb_height)
		return;
	if(point_y + r < 0)
		return;*/
		
	_raster_point_fragment(point_x, point_y + r, params, &frag_pass);
	_raster_point_fragment(point_x, point_y - r, params, &frag_pass);

	for(int x = point_x - r; x <= point_x + r; x += 1)
		_raster_point_fragment(x, point_y, params, &frag_pass);
		
	while(x2 < y2)
	{
		if(f >= 0)
		{
			y2 -= 1;
			dy += 2;
			f += dy;
		}
		x2 += 1;
		dx += 2;
		f += dx + 1;
		int x0 = point_x - x2;
		int x1 = point_x + x2;
		int sx = (x0 < x1) ? 1 : -1; 
		for(int x = x0; x != x1+sx; x += sx)
			_raster_point_fragment(x, point_y + y2, params, &frag_pass);
		for(int x = x0; x != x1+sx; x += sx)
			_raster_point_fragment(x, point_y - y2, params, &frag_pass);
		x0 = point_x - y2;
		x1 = point_x + y2;
		sx = (x0 < x1) ? 1 : -1; 
		for(int x = x0; x != x1+sx; x += sx)
			_raster_point_fragment(x, point_y + x2, params, &frag_pass);
		for(int x = x0; x != x1+sx; x += sx)
			_raster_point_fragment(x, point_y - x2, params, &frag_pass);
	}
	
	if(_brcontext->fshader) {
	if(frag_pass.pass_data)
		free(frag_pass.pass_data);
	if(frag_pass.pass_attribs)
		free(frag_pass.pass_attribs);
	}
}

// post-process and raster a point (vertex shader pass, _vertex_pass, not performed here)
// will cause harm to contents of 'point'
void _process_point(_point_t* point)
{
	if(!_brcontext || !point)
		return;
		
	// clip, if necessary.
	if(_brcontext->clip)
	{
		// clip against -w <= (x,y,z) <= w

		if(-point->pos.w > point->pos.x)
			return;
		if(point->pos.x > point->pos.w)
			return;
		if(-point->pos.w > point->pos.y)
			return;
		if(point->pos.y > point->pos.w)
			return;
		if(-point->pos.w > point->pos.z)
			return;
		if(point->pos.z > point->pos.w)
			return;
	}
	
	_raster_point_t raster_point;
	
	if(_brcontext->persp_div && point->pos.w != 0.0f && point->pos.w != 1.0f)
	{
		float inv_w = _fdiv(1.0f, point->pos.w);
		point->pos.x *= inv_w;
		point->pos.y *= inv_w;
		point->pos.z *= inv_w;
	}
	
	if(_brcontext->scale_z)
		point->pos.z *= 0.5f + 0.5f;
	
	float half_width  = _brcontext->rb_width  * 0.5f;
	float half_height = _brcontext->rb_height * 0.5f;
	
	raster_point.x = half_width  + ( point->pos.x * half_width);
	raster_point.y = half_height + (-point->pos.y * half_height);

	raster_point.z = _convert_depth(point->pos.z);
	raster_point.w = point->pos.w;
	
	raster_point.rgba.x = point->rgba.x * 65536.0f;
	raster_point.rgba.y = point->rgba.y * 65536.0f;
	raster_point.rgba.z = point->rgba.z * 65536.0f;
	raster_point.rgba.w = point->rgba.w * 65536.0f;
	
	raster_point.r = _brcontext->point_radius + .5f;
	
	_raster_point(&raster_point);
}







/* API */
//
// most of the below interfaces use the above functions/types







// allocate, initialize and return a context.
brcontext* brCreateContext()
{
	brcontext* context;
	context = (brcontext*) malloc(sizeof(brcontext));
	
	context->cb = NULL;
	context->db = NULL;
	context->cb2 = NULL;
	context->db2 = NULL;
	context->cb_type = 0;
	context->db_type = 0;
	context->cb2_type = 0;
	context->db2_type = 0;
	context->rb_width = 0;
	context->rb_height = 0;
	context->rb2_width = 0;
	context->rb2_height = 0;
	context->clear_color = {0,0,0,0};
	context->clear_depth = 1;
	context->point_radius = 1;
	context->double_buffer = false;
	context->depth_write = true;
	context->depth_test = true;
	context->persp_corr = true;
	context->texture = true;
	context->blend = false;
	context->cull = false;
	context->cull_winding = BR_CW;
	context->clip = true;
	context->persp_div = true;
	context->scale_z = true;
	context->poly_mode = BR_FILL;
	context->vertex_array = false;
	context->color_array = false;
	context->normal_array = false;
	context->tcoord_array = false;
	context->vertex_stride = 0;
	context->color_stride = 0;
	context->normal_stride = 0;
	context->tcoord_stride = 0;
	context->vertex_offset = NULL;
	context->color_offset = NULL;
	context->normal_offset = NULL;
	context->tcoord_offset = NULL;
	context->vertex_count = 0;
	context->color_count = 0;
	context->texture_unit = 0;
	for(uint32_t i = 0; i < BR_NUM_TEXTURE_UNITS; i += 1)
	{
		context->textures[i] = NULL;
		context->texture_widths[i] = 0;
		context->texture_heights[i] = 0;
		context->texture_formats[i] = 0;
		context->texture_compressed_booleans[i] = false;
	}
	context->vshader = NULL;
	context->fshader = NULL;
	context->sh_vposition = false;
	context->sh_vcolor = false;
	context->sh_vtcoords = false;
	context->sh_vnormals = false;
	context->sh_vtype = false;
	context->sh_prim_color = false;
	context->sh_tex_color = false;
	context->sh_frag_color = false;
	context->sh_bary_linear = false;
	context->sh_bary_persp = false;
	context->sh_fposition = false;
	context->sh_fdepth = false;

	return context;
}

// bind a context.
void brBindContext(brcontext* context)
{
	if(context)
		_brcontext = context;
}

// free the resources allocated by a context, including the context itself.
void brFreeContext(brcontext* context)
{
	if(!context)
		return;

	if(context == _brcontext)
		_brcontext = NULL;

	free(context);
}

// allocate a renderbuffer.
void brCreateRenderbuffer(uint32_t type, uint32_t width, uint32_t height, void** buffer)
{
	if(width < 1 || height < 1)
		return;

	switch(type)
	{
		case BR_R8G8B8A8:
		case BR_R8G8B8:
		case BR_A8B8G8R8:
		case BR_B8G8R8:
		case BR_D32:
			*((uint32_t**)buffer) = (uint32_t*) calloc(width*height, sizeof(uint32_t));
			break;
		case BR_R5G5B5A1:
		case BR_R5G5B5:
		case BR_A1B5G5R5:
		case BR_B5G5R5:
		case BR_D16:
			*((uint16_t**)buffer) = (uint16_t*) calloc(width*height, sizeof(uint16_t));
				break;
		case BR_R3G2B2A1:
		case BR_R3G3B2:
		case BR_A1B2G2R3:
		case BR_B2G3R3:
			*((uint8_t**)buffer) = (uint8_t*) calloc(width*height, sizeof(uint8_t));
				break;
	}
}

// bind a renderbuffer to front set.
void brBindRenderbuffer(uint32_t type, uint32_t width, uint32_t height, void* buffer)
{
	if(!_brcontext || !buffer || width < 1 || height < 1)
		return;

	if(_brcontext->cb || _brcontext->db)
	{
		if(width != _brcontext->rb_width || height != _brcontext->rb_height)
			return;
	}

	switch(type)
	{
		case BR_R8G8B8A8:
		case BR_R8G8B8:
		case BR_A8B8G8R8:
		case BR_B8G8R8:
		case BR_R5G5B5A1:
		case BR_R5G5B5:
		case BR_A1B5G5R5:
		case BR_B5G5R5:
		case BR_R3G2B2A1:
		case BR_R3G3B2:
		case BR_A1B2G2R3:
		case BR_B2G3R3:
			_brcontext->cb = buffer;
			_brcontext->cb_type = type;
			break;
		case BR_D16:
		case BR_D32:
			_brcontext->db = buffer;
			_brcontext->db_type = type;
			break;
		default:
			return;
	}
	_brcontext->rb_width = width;
	_brcontext->rb_height = height;
}

// unbind renderbuffer(s) from front set.
// OR together desired buffer bits. Resets buffer dimensions when neither color nor depth buffers are bound.
void brUnbindRenderbuffer(uint32_t buffers)
{
	if(!_brcontext)
		return;
	
	if(buffers & BR_COLOR_BUFFER_BIT)
	{
		_brcontext->cb = NULL;
		_brcontext->cb_type = 0;
	}
	if(buffers & BR_DEPTH_BUFFER_BIT)
	{
		_brcontext->db = NULL;
		_brcontext->db_type = 0;
	}
	if(!_brcontext->cb && !_brcontext->db)
	{
		_brcontext->rb_width = 0;
		_brcontext->rb_height = 0;
	}
}

// set polygon mode.
void brPolygonMode(uint32_t mode)
{
	if(!_brcontext)
		return;
		
	switch(mode)
	{
		case BR_FILL:
			_brcontext->poly_mode = BR_FILL;
			break;
		case BR_LINE:
			_brcontext->poly_mode = BR_LINE;
			break;
		case BR_POINT:
			_brcontext->poly_mode = BR_POINT;
			break;
	}
}

// set culled winding.
void brCullWinding(uint32_t winding)
{
	if(!_brcontext)
		return;
	
	switch(winding)
	{
		case BR_CW:
		case BR_CCW:
		_brcontext->cull_winding = winding;
	}
}

// set radius of points.
void brPointSize(float radius)
{
	if(!_brcontext)
		return;
	
	if(radius >= 0.0f)
		_brcontext->point_radius = radius;
	else
		_brcontext->point_radius = 0.0f;
}

// enable a toggled state.
void brEnable(uint32_t state)
{
	if(!_brcontext)
		return;

	switch(state)
	{
		case BR_DOUBLE_BUFFER:
			_brcontext->double_buffer = true;
			break;
		case BR_DEPTH_WRITE:
			_brcontext->depth_write = true;
			break;
		case BR_DEPTH_TEST:
			_brcontext->depth_test = true;
			break;
		case BR_PERSPECTIVE_CORRECTION:
			_brcontext->persp_corr = true;
			break;
		case BR_TEXTURE:
			_brcontext->texture = true;
			break;
		case BR_BLEND:
			_brcontext->blend = true;
			break;
		case BR_CULL:
			_brcontext->cull = true;
			break;
		case BR_CLIP:
			_brcontext->clip = true;
			break;
		case BR_PERSPECTIVE_DIVISION:
			_brcontext->persp_div = true;
			break;
		case BR_SCALE_Z:
			_brcontext->scale_z = true;
			break;
		case BR_VERTEX_ARRAY:
			_brcontext->vertex_array = true;
			break;
		case BR_COLOR_ARRAY:
			_brcontext->color_array = true;
			break;
		case BR_NORMAL_ARRAY:
			_brcontext->normal_array = true;
			break;
		case BR_TEXCOORD_ARRAY:
			_brcontext->tcoord_array = true;
			break;
		case BR_VERTEX_TYPE:
			_brcontext->sh_vtype = true;
			break;
		case BR_VERTEX_POSITION:
			_brcontext->sh_vposition = true;
			break;
		case BR_VERTEX_COLOR:
			_brcontext->sh_vcolor = true;
			break;
		case BR_VERTEX_NORMALS:
			_brcontext->sh_vnormals = true;
			break;
		case BR_VERTEX_TEXTURE_COORDINATES:
			_brcontext->sh_vtcoords = true;
			break;
		case BR_PRIMITIVE_COLOR:
			_brcontext->sh_prim_color = true;
			break;
		case BR_TEXTURE_COLOR:
			_brcontext->sh_tex_color = true;
			break;
		case BR_FRAGMENT_COLOR:
			_brcontext->sh_frag_color = true;
			break;
		case BR_BARY_LINEAR:
			_brcontext->sh_bary_linear = true;
			break;
		case BR_BARY_PERSPECTIVE:
			_brcontext->sh_bary_persp = true;
			break;
		case BR_FRAGMENT_POSITION:
			_brcontext->sh_fposition = true;
			break;
		case BR_FRAGMENT_DEPTH:
			_brcontext->sh_fdepth = true;
			break;
	}
}

// disable a toggled state.
void brDisable(uint32_t state)
{
	if(!_brcontext)
		return;

	switch(state)
	{
		case BR_DOUBLE_BUFFER:
			_brcontext->double_buffer = false;
			break;
		case BR_DEPTH_WRITE:
			_brcontext->depth_write = false;
			break;
		case BR_DEPTH_TEST:
			_brcontext->depth_test = false;
			break;
		case BR_PERSPECTIVE_CORRECTION:
			_brcontext->persp_corr = false;
			break;
		case BR_TEXTURE:
			_brcontext->texture = false;
			break;
		case BR_BLEND:
			_brcontext->blend = false;
			break;
		case BR_CULL:
			_brcontext->cull = false;
			break;
		case BR_CLIP:
			_brcontext->clip = false;
			break;
		case BR_PERSPECTIVE_DIVISION:
			_brcontext->persp_div = false;
			break;
		case BR_SCALE_Z:
			_brcontext->scale_z = false;
			break;
		case BR_VERTEX_ARRAY:
			_brcontext->vertex_array = false;
			break;
		case BR_COLOR_ARRAY:
			_brcontext->color_array = false;
			break;
		case BR_NORMAL_ARRAY:
			_brcontext->normal_array = false;
			break;
		case BR_TEXCOORD_ARRAY:
			_brcontext->tcoord_array = false;
			break;
		case BR_VERTEX_TYPE:
			_brcontext->sh_vtype = false;
			break;
		case BR_VERTEX_POSITION:
			_brcontext->sh_vposition = false;
			break;
		case BR_VERTEX_COLOR:
			_brcontext->sh_vcolor = false;
			break;
		case BR_VERTEX_NORMALS:
			_brcontext->sh_vnormals = false;
			break;
		case BR_VERTEX_TEXTURE_COORDINATES:
			_brcontext->sh_vtcoords = false;
			break;
		case BR_PRIMITIVE_COLOR:
			_brcontext->sh_prim_color = false;
			break;
		case BR_TEXTURE_COLOR:
			_brcontext->sh_tex_color = false;
			break;
		case BR_FRAGMENT_COLOR:
			_brcontext->sh_frag_color = false;
			break;
		case BR_BARY_LINEAR:
			_brcontext->sh_bary_linear = false;
			break;
		case BR_BARY_PERSPECTIVE:
			_brcontext->sh_bary_persp = false;
			break;
		case BR_FRAGMENT_POSITION:
			_brcontext->sh_fposition = false;
			break;
		case BR_FRAGMENT_DEPTH:
			_brcontext->sh_fdepth = false;
			break;
	}
}

// query a toggled state.
bool brIsEnabled(uint32_t state)
{
	if(!_brcontext)
		return false;

	switch(state)
	{
		case BR_DOUBLE_BUFFER:
			return _brcontext->double_buffer;
		case BR_DEPTH_WRITE:
			return _brcontext->depth_write;
		case BR_DEPTH_TEST:
			return _brcontext->depth_test;
		case BR_PERSPECTIVE_CORRECTION:
			return _brcontext->persp_corr;
		case BR_TEXTURE:
			return _brcontext->texture;
		case BR_BLEND:
			return _brcontext->blend;
		case BR_CULL:
			return _brcontext->cull;
		case BR_CLIP:
			return _brcontext->clip;
		case BR_PERSPECTIVE_DIVISION:
			return _brcontext->persp_div;
		case BR_SCALE_Z:
			return _brcontext->scale_z;
		case BR_VERTEX_ARRAY:
			return _brcontext->vertex_array;
		case BR_COLOR_ARRAY:
			return _brcontext->color_array;
		case BR_NORMAL_ARRAY:
			return _brcontext->normal_array;
		case BR_TEXCOORD_ARRAY:
			return _brcontext->tcoord_array;
		case BR_VERTEX_TYPE:
			return _brcontext->sh_vtype;
		case BR_VERTEX_POSITION:
			return _brcontext->sh_vposition;
		case BR_VERTEX_COLOR:
			return _brcontext->sh_vcolor;
		case BR_VERTEX_NORMALS:
			return _brcontext->sh_vnormals;
		case BR_VERTEX_TEXTURE_COORDINATES:
			return _brcontext->sh_vtcoords;
		case BR_PRIMITIVE_COLOR:
			return _brcontext->sh_prim_color;
		case BR_TEXTURE_COLOR:
			return _brcontext->sh_tex_color;
		case BR_FRAGMENT_COLOR:
			return _brcontext->sh_frag_color;
		case BR_BARY_LINEAR:
			return _brcontext->sh_bary_linear;
		case BR_BARY_PERSPECTIVE:
			return _brcontext->sh_bary_persp;
		case BR_FRAGMENT_POSITION:
			return _brcontext->sh_fposition;
		case BR_FRAGMENT_DEPTH:
			return _brcontext->sh_fdepth;
	}
}

// bind a shader
void brBindShader(uint32_t type, void* shader)
{
	if(!_brcontext)
		return;

	if(type == BR_VERTEX_SHADER)
	{
		brvec4 (*ptr)(void*, uint32_t*, uint32_t) = (brvec4 (*)(void*, uint32_t*, uint32_t)) shader;
		_brcontext->vshader = ptr;
	}
	if(type == BR_FRAGMENT_SHADER)
	{
		brvec4 (*ptr)(void*, uint32_t*, uint32_t, bool*) = (brvec4 (*)(void*, uint32_t*, uint32_t, bool*)) shader;
		_brcontext->fshader = ptr;
	}
}

// swap back and front renderbuffers, if double-buffering is enabled.
void brSwapBuffers()
{
	if(!_brcontext || !_brcontext->double_buffer)
		return;

	void* cb = _brcontext->cb;
	void* db = _brcontext->db;
	uint32_t cb_type = _brcontext->cb_type;
	uint32_t db_type = _brcontext->db_type;
	uint32_t width = _brcontext->rb_width;
	uint32_t height = _brcontext->rb_height;
	
	_brcontext->cb = _brcontext->cb2;
	_brcontext->db = _brcontext->db2;
	_brcontext->cb_type = _brcontext->cb2_type;
	_brcontext->db_type = _brcontext->db2_type;
	_brcontext->rb_width = _brcontext->rb2_width;
	_brcontext->rb_height = _brcontext->rb2_height;

	_brcontext->cb2 = cb;
	_brcontext->db2 = db;
	_brcontext->cb2_type = cb_type;
	_brcontext->db2_type = db_type;
	_brcontext->rb2_width = width;
	_brcontext->rb2_height = height;
}

// set active texture unit
void brActiveTexture(uint32_t unit)
{
	if(!_brcontext)
		return;
	if(unit < BR_NUM_TEXTURE_UNITS)
		_brcontext->texture_unit = unit;
}

// upload information to texture unit
// pass 0 as data to clear the unit
void brTexture(void* data, uint32_t format, uint32_t width, uint32_t height, bool compressed)
{
	if(!_brcontext)
		return;
	uint32_t unit = _brcontext->texture_unit;
	if(!data || !_is_pixel_format(format) || width < 1 || height < 1)
	{
		_brcontext->textures[unit] = NULL;
		_brcontext->texture_widths[unit] = 0;
		_brcontext->texture_heights[unit] = 0;
		_brcontext->texture_formats[unit] = 0;
		_brcontext->texture_compressed_booleans[unit] = false;
		return;
	}

	_brcontext->textures[unit] = data;
	_brcontext->texture_widths[unit] = width;
	_brcontext->texture_heights[unit] = height;
	_brcontext->texture_formats[unit] = format;
	_brcontext->texture_compressed_booleans[unit] = compressed;
}

// set buffer clear color
// requires color buffer to be bound, and requires update when color buffer type changes
void brClearColor(float r, float g, float b, float a)
{
	if(!_brcontext)
		return;

	if(r < 0.0f)		r = 0.0f;
	if(r > 1.0f)		r = 1.0f;
	if(g < 0.0f)		g = 0.0f;
	if(g > 1.0f)		g = 1.0f;
	if(b < 0.0f)		b = 0.0f;
	if(b > 1.0f)		b = 1.0f;
	if(a < 0.0f)		a = 0.0f;
	if(a > 1.0f)		a = 1.0f;

	_brcontext->clear_color = { r, g, b, a };
}

// set buffer clear depth (0-1)
// requires depth buffer to be bound, and requires update when depth buffer type changes
void brClearDepth(float depth)
{
	if(!_brcontext)
		return;
	
	if(depth > 1.0f)
		depth = 1.0f;
	if(depth < 0.0f)
		depth = 0.0f;
		
	_brcontext->clear_depth = depth;
}

// clear back (if BR_DOUBLE_BUFFER is enabled) or front renderbuffer(s).
// OR together buffer constants.
void brClear(uint32_t buffers)
{
	if(!_brcontext)
		return;

	if(_brcontext->double_buffer)
	{
		bool clear_cb = _brcontext->cb2 && (buffers & BR_COLOR_BUFFER_BIT);
		bool clear_db = _brcontext->db2 && (buffers & BR_DEPTH_BUFFER_BIT);

		uint64_t pixels = _brcontext->rb2_width*_brcontext->rb2_height;

		if(clear_cb && clear_db)
		{
			switch(_brcontext->cb2_type)
			{
				case BR_R8G8B8A8:
				case BR_R8G8B8:
				case BR_A8B8G8R8:
				case BR_B8G8R8:
					{
						uint32_t color;
						if(_brcontext->cb2_type == BR_R8G8B8A8)
							color = _BR_R8G8B8A8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f), (uint8_t)(_brcontext->clear_color.w*255.0f));
						if(_brcontext->cb2_type == BR_R8G8B8)
							color = _BR_R8G8B8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f));
						if(_brcontext->cb2_type == BR_A8B8G8R8)
							color = _BR_A8B8G8R8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f), (uint8_t)(_brcontext->clear_color.w*255.0f));
						if(_brcontext->cb2_type == BR_B8G8R8)
							color = _BR_B8G8R8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f));
						if(_brcontext->db2_type == BR_D16)
						{
							uint32_t* cb = (uint32_t*) _brcontext->cb2;
							uint16_t* db = (uint16_t*) _brcontext->db2;
							uint16_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFF;
							if(d > 0xFFFF) d = 0xFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
						if(_brcontext->db2_type == BR_D32)
						{
							uint32_t* cb = (uint32_t*) _brcontext->cb2;
							uint32_t* db = (uint32_t*) _brcontext->db2;
							uint32_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFFFFFF;
							if(d > 0xFFFFFFFF) d = 0xFFFFFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
					}
					break;
				case BR_R5G5B5A1:
				case BR_R5G5B5:
				case BR_A1B5G5R5:
				case BR_B5G5R5:
					{
						uint16_t color;
						if(_brcontext->cb2_type == BR_R5G5B5A1)
							color = _BR_R5G5B5A1((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb2_type == BR_R5G5B5)
							color = _BR_R5G5B5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f));
						if(_brcontext->cb2_type == BR_A1B5G5R5)
							color = _BR_A1B5G5R5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb2_type == BR_B5G5R5)
							color = _BR_B5G5R5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f));
						if(_brcontext->db2_type == BR_D16)
						{
							uint16_t* cb = (uint16_t*) _brcontext->cb2;
							uint16_t* db = (uint16_t*) _brcontext->db2;
							uint16_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFF;
							if(d > 0xFFFF) d = 0xFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
						if(_brcontext->db2_type == BR_D32)
						{
							uint16_t* cb = (uint16_t*) _brcontext->cb2;
							uint32_t* db = (uint32_t*) _brcontext->db2;
							uint32_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFFFFFF;
							if(d > 0xFFFFFFFF) d = 0xFFFFFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
					}
					break;
				case BR_R3G2B2A1:
				case BR_R3G3B2:
				case BR_A1B2G2R3:
				case BR_B2G3R3:
					{
						uint8_t color;
						if(_brcontext->cb2_type == BR_R3G2B2A1)
							color = _BR_R3G2B2A1((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*4.0f), (uint8_t)(_brcontext->clear_color.z*4.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb2_type == BR_R3G3B2)
							color = _BR_R3G3B2((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*8.0f), (uint8_t)(_brcontext->clear_color.z*4.0f));
						if(_brcontext->cb2_type == BR_A1B2G2R3)
							color = _BR_A1B2G2R3((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*4.0f), (uint8_t)(_brcontext->clear_color.z*4.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb2_type == BR_B2G3R3)
							color = _BR_B2G3R3((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*8.0f), (uint8_t)(_brcontext->clear_color.z*4.0f));
						if(_brcontext->db2_type == BR_D16)
						{
							uint8_t* cb  = (uint8_t* ) _brcontext->cb2;
							uint16_t* db = (uint16_t*) _brcontext->db2;
							uint16_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFF;
							if(d > 0xFFFF) d = 0xFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
						if(_brcontext->db2_type == BR_D32)
						{
							uint8_t* cb  = (uint8_t* ) _brcontext->cb2;
							uint32_t* db = (uint32_t*) _brcontext->db2;
							uint32_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFFFFFF;
							if(d > 0xFFFFFFFF) d = 0xFFFFFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
					}
					break;
			}
		}
		else if(clear_cb)
		{
			switch(_brcontext->cb2_type)
			{
				case BR_R8G8B8A8:
				case BR_R8G8B8:
				case BR_A8B8G8R8:
				case BR_B8G8R8:
					{
						uint32_t color;
						if(_brcontext->cb2_type == BR_R8G8B8A8)
							color = _BR_R8G8B8A8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f), (uint8_t)(_brcontext->clear_color.w*255.0f));
						if(_brcontext->cb2_type == BR_R8G8B8)
							color = _BR_R8G8B8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f));
						if(_brcontext->cb2_type == BR_A8B8G8R8)
							color = _BR_A8B8G8R8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f), (uint8_t)(_brcontext->clear_color.w*255.0f));
						if(_brcontext->cb2_type == BR_B8G8R8)
							color = _BR_B8G8R8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f));
						uint32_t* cb = (uint32_t*) _brcontext->cb2;
						for(uint64_t i = 0; i < pixels; i += 1)
							cb[i] = color;
					}
					break;
				case BR_R5G5B5A1:
				case BR_R5G5B5:
				case BR_A1B5G5R5:
				case BR_B5G5R5:
					{
						uint16_t color;
						if(_brcontext->cb2_type == BR_R5G5B5A1)
							color = _BR_R5G5B5A1((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb2_type == BR_R5G5B5)
							color = _BR_R5G5B5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f));
						if(_brcontext->cb2_type == BR_A1B5G5R5)
							color = _BR_A1B5G5R5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb2_type == BR_B5G5R5)
							color = _BR_B5G5R5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f));
						uint16_t* cb = (uint16_t*) _brcontext->cb2;
						for(uint64_t i = 0; i < pixels; i += 1)
							cb[i] = color;
					}
					break;
				case BR_R3G2B2A1:
				case BR_R3G3B2:
				case BR_A1B2G2R3:
				case BR_B2G3R3:
					{
						uint8_t color;
						if(_brcontext->cb2_type == BR_R3G2B2A1)
							color = _BR_R3G2B2A1((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*4.0f), (uint8_t)(_brcontext->clear_color.z*4.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb2_type == BR_R3G3B2)
							color = _BR_R3G3B2((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*8.0f), (uint8_t)(_brcontext->clear_color.z*4.0f));
						if(_brcontext->cb2_type == BR_A1B2G2R3)
							color = _BR_A1B2G2R3((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*4.0f), (uint8_t)(_brcontext->clear_color.z*4.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb2_type == BR_B2G3R3)
							color = _BR_B2G3R3((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*8.0f), (uint8_t)(_brcontext->clear_color.z*4.0f));
						uint8_t* cb = (uint8_t*) _brcontext->cb2;
						for(uint64_t i = 0; i < pixels; i += 1)
							cb[i] = color;
					}
					break;
			}
		}
		else if(clear_db)
		{
			switch(_brcontext->db2_type)
			{
				case BR_D16:
				{
					uint16_t* db = (uint16_t*) _brcontext->db2;
					uint16_t depth;
					int64_t d = _brcontext->clear_depth * 0xFFFF;
					if(d > 0xFFFF) d = 0xFFFF;
					if(d < 0) d = 0;
					depth = d;
					for(uint64_t i = 0; i < pixels; i += 1)
						db[i] = depth;
				}
				break;
				case BR_D32:
				{
					uint32_t* db = (uint32_t*) _brcontext->db2;
					uint32_t depth;
					int64_t d = _brcontext->clear_depth * 0xFFFFFFFF;
					if(d > 0xFFFFFFFF) d = 0xFFFFFFFF;
					if(d < 0) d = 0;
					depth = d;
					for(uint64_t i = 0; i < pixels; i += 1)
						db[i] = depth;
				}
				break;
			}
		}
	}
	else
	{
		bool clear_cb = _brcontext->cb && (buffers & BR_COLOR_BUFFER_BIT);
		bool clear_db = _brcontext->db && (buffers & BR_DEPTH_BUFFER_BIT);

		uint64_t pixels = _brcontext->rb_width*_brcontext->rb_height;

		if(clear_cb && clear_db)
		{
			switch(_brcontext->cb_type)
			{
				case BR_R8G8B8A8:
				case BR_R8G8B8:
				case BR_A8B8G8R8:
				case BR_B8G8R8:
					{
						uint32_t color;
						if(_brcontext->cb_type == BR_R8G8B8A8)
							color = _BR_R8G8B8A8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f), (uint8_t)(_brcontext->clear_color.w*255.0f));
						if(_brcontext->cb_type == BR_R8G8B8)
							color = _BR_R8G8B8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f));
						if(_brcontext->cb_type == BR_A8B8G8R8)
							color = _BR_A8B8G8R8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f), (uint8_t)(_brcontext->clear_color.w*255.0f));
						if(_brcontext->cb_type == BR_B8G8R8)
							color = _BR_B8G8R8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f));
						if(_brcontext->db_type == BR_D16)
						{
							uint32_t* cb = (uint32_t*) _brcontext->cb;
							uint16_t* db = (uint16_t*) _brcontext->db;
							uint16_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFF;
							if(d > 0xFFFF) d = 0xFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
						if(_brcontext->db_type == BR_D32)
						{
							uint32_t* cb = (uint32_t*) _brcontext->cb;
							uint32_t* db = (uint32_t*) _brcontext->db;
							uint32_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFFFFFF;
							if(d > 0xFFFFFFFF) d = 0xFFFFFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
					}
					break;
				case BR_R5G5B5A1:
				case BR_R5G5B5:
				case BR_A1B5G5R5:
				case BR_B5G5R5:
					{
						uint16_t color;
						if(_brcontext->cb_type == BR_R5G5B5A1)
							color = _BR_R5G5B5A1((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb_type == BR_R5G5B5)
							color = _BR_R5G5B5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f));
						if(_brcontext->cb_type == BR_A1B5G5R5)
							color = _BR_A1B5G5R5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb_type == BR_B5G5R5)
							color = _BR_B5G5R5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f));
						if(_brcontext->db_type == BR_D16)
						{
							uint16_t* cb = (uint16_t*) _brcontext->cb;
							uint16_t* db = (uint16_t*) _brcontext->db;
							uint16_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFF;
							if(d > 0xFFFF) d = 0xFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
						if(_brcontext->db_type == BR_D32)
						{
							uint16_t* cb = (uint16_t*) _brcontext->cb;
							uint32_t* db = (uint32_t*) _brcontext->db;
							uint32_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFFFFFF;
							if(d > 0xFFFFFFFF) d = 0xFFFFFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
					}
					break;
				case BR_R3G2B2A1:
				case BR_R3G3B2:
				case BR_A1B2G2R3:
				case BR_B2G3R3:
					{
						uint8_t color;
						if(_brcontext->cb_type == BR_R3G2B2A1)
							color = _BR_R3G2B2A1((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*4.0f), (uint8_t)(_brcontext->clear_color.z*4.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb_type == BR_R3G3B2)
							color = _BR_R3G3B2((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*8.0f), (uint8_t)(_brcontext->clear_color.z*4.0f));
						if(_brcontext->cb_type == BR_A1B2G2R3)
							color = _BR_A1B2G2R3((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*4.0f), (uint8_t)(_brcontext->clear_color.z*4.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb_type == BR_B2G3R3)
							color = _BR_B2G3R3((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*8.0f), (uint8_t)(_brcontext->clear_color.z*4.0f));
						if(_brcontext->db_type == BR_D16)
						{
							uint8_t* cb  = (uint8_t* ) _brcontext->cb;
							uint16_t* db = (uint16_t*) _brcontext->db;
							uint16_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFF;
							if(d > 0xFFFF) d = 0xFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
						if(_brcontext->db_type == BR_D32)
						{
							uint8_t* cb  = (uint8_t* ) _brcontext->cb;
							uint32_t* db = (uint32_t*) _brcontext->db;
							uint32_t depth;
							int64_t d = _brcontext->clear_depth * 0xFFFFFFFF;
							if(d > 0xFFFFFFFF) d = 0xFFFFFFFF;
							if(d < 0) d = 0;
							depth = d;
							for(uint64_t i = 0; i < pixels; i += 1)
							{
								cb[i] = color;
								db[i] = depth;
							}
						}
					}
					break;
			}
		}
		else if(clear_cb)
		{
			switch(_brcontext->cb_type)
			{
				case BR_R8G8B8A8:
				case BR_R8G8B8:
				case BR_A8B8G8R8:
				case BR_B8G8R8:
					{
						uint32_t color;
						if(_brcontext->cb_type == BR_R8G8B8A8)
							color = _BR_R8G8B8A8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f), (uint8_t)(_brcontext->clear_color.w*255.0f));
						if(_brcontext->cb_type == BR_R8G8B8)
							color = _BR_R8G8B8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f));
						if(_brcontext->cb_type == BR_A8B8G8R8)
							color = _BR_A8B8G8R8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f), (uint8_t)(_brcontext->clear_color.w*255.0f));
						if(_brcontext->cb_type == BR_B8G8R8)
							color = _BR_B8G8R8((uint8_t)(_brcontext->clear_color.x*255.0f), (uint8_t)(_brcontext->clear_color.y*255.0f), (uint8_t)(_brcontext->clear_color.z*255.0f));
						uint32_t* cb = (uint32_t*) _brcontext->cb;
						for(uint64_t i = 0; i < pixels; i += 1)
							cb[i] = color;
					}
					break;
				case BR_R5G5B5A1:
				case BR_R5G5B5:
				case BR_A1B5G5R5:
				case BR_B5G5R5:
					{
						uint16_t color;
						if(_brcontext->cb_type == BR_R5G5B5A1)
							color = _BR_R5G5B5A1((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb_type == BR_R5G5B5)
							color = _BR_R5G5B5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f));
						if(_brcontext->cb_type == BR_A1B5G5R5)
							color = _BR_A1B5G5R5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb_type == BR_B5G5R5)
							color = _BR_B5G5R5((uint8_t)(_brcontext->clear_color.x*31.0f), (uint8_t)(_brcontext->clear_color.y*31.0f), (uint8_t)(_brcontext->clear_color.z*31.0f));
						uint16_t* cb = (uint16_t*) _brcontext->cb;
						for(uint64_t i = 0; i < pixels; i += 1)
							cb[i] = color;
					}
					break;
				case BR_R3G2B2A1:
				case BR_R3G3B2:
				case BR_A1B2G2R3:
				case BR_B2G3R3:
					{
						uint8_t color;
						if(_brcontext->cb_type == BR_R3G2B2A1)
							color = _BR_R3G2B2A1((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*4.0f), (uint8_t)(_brcontext->clear_color.z*4.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb_type == BR_R3G3B2)
							color = _BR_R3G3B2((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*8.0f), (uint8_t)(_brcontext->clear_color.z*4.0f));
						if(_brcontext->cb_type == BR_A1B2G2R3)
							color = _BR_A1B2G2R3((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*4.0f), (uint8_t)(_brcontext->clear_color.z*4.0f), (_brcontext->clear_color.w != 0.0f));
						if(_brcontext->cb_type == BR_B2G3R3)
							color = _BR_B2G3R3((uint8_t)(_brcontext->clear_color.x*8.0f), (uint8_t)(_brcontext->clear_color.y*8.0f), (uint8_t)(_brcontext->clear_color.z*4.0f));
						uint8_t* cb = (uint8_t*) _brcontext->cb;
						for(uint64_t i = 0; i < pixels; i += 1)
							cb[i] = color;
					}
					break;
			}
		}
		else if(clear_db)
		{
			switch(_brcontext->db_type)
			{
				case BR_D16:
				{
					uint16_t* db = (uint16_t*) _brcontext->db;
					uint16_t depth;
					int64_t d = _brcontext->clear_depth * 0xFFFF;
					if(d > 0xFFFF) d = 0xFFFF;
					if(d < 0) d = 0;
					depth = d;
					for(uint64_t i = 0; i < pixels; i += 1)
						db[i] = depth;
				}
				break;
				case BR_D32:
				{
					uint32_t* db = (uint32_t*) _brcontext->db;
					uint32_t depth;
					int64_t d = _brcontext->clear_depth * 0xFFFFFFFF;
					if(d > 0xFFFFFFFF) d = 0xFFFFFFFF;
					if(d < 0) d = 0;
					depth = d;
					for(uint64_t i = 0; i < pixels; i += 1)
						db[i] = depth;
				}
				break;
			}
		}
	}
}

// define where vertex position is located within the vertex layout of arrays.
// count is 2, 3, or 4.
void brVertexPointer(uint32_t count, void* offset, void* stride)
{
	if(count > 4 || count < 2)
		return;
	_brcontext->vertex_count = count;
	_brcontext->vertex_offset = offset;
	_brcontext->vertex_stride = (size_t)stride;
}

// define where vertex color is located within the vertex layout of arrays.
// count is 3 or 4.
void brColorPointer(uint32_t count, void* offset, void* stride)
{
	if(count != 3 && count != 4)
		return;
	_brcontext->color_count = count;
	_brcontext->color_offset = offset;
	_brcontext->color_stride = (size_t)stride;
}

// define where vertex normal is located within the vertex layout of arrays.
void brNormalPointer(void* offset, void* stride)
{
	_brcontext->normal_offset = offset;
	_brcontext->normal_stride = (size_t)stride;
}

// define where vertex texture coordinate in located within the vertex layout of arrays.
void brTexCoordPointer(void* offset, void* stride)
{
	_brcontext->tcoord_offset = offset;
	_brcontext->tcoord_stride = (size_t)stride;
}

// draw an array.
void brDrawArray(uint32_t ptype, uint32_t indices, float* array)
{
	uint32_t v = 0;	// current vertex #
	brvec4 position0;
	brvec4 color0;
	brvec3 normal0;
	brvec2 tcoord0;
	brvec4 position1;
	brvec4 color1;
	brvec3 normal1;
	brvec2 tcoord1;
	brvec4 position2;
	brvec4 color2;
	brvec3 normal2;
	brvec2 tcoord2;
	
	void* vertex_offset = _brcontext->vertex_offset;
	void* color_offset  = _brcontext->color_offset;
	void* normal_offset = _brcontext->normal_offset;
	void* tcoord_offset = _brcontext->tcoord_offset;
	
	for(uint32_t i = 0; i < indices; i += 1)
	{
		// load to vertex
		if(v == 0) {
			position0 = { 0, 0, 0, 1 };
			color0    = { 0, 0, 0, 1 };
			normal0   = { 0, 0, 0 };
			tcoord0   = { 0, 0 };
			
			if(_brcontext->vertex_array) {
				if(_brcontext->vertex_count == 2)
					position0 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						position0.z, position0.w };
				if(_brcontext->vertex_count == 3)
					position0 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						position0.w };
				if(_brcontext->vertex_count == 4)
					position0 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*3) };
			}
			if(_brcontext->color_array) {
				if(_brcontext->color_count == 3)
					color0 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2), 
						color0.w };
				if(_brcontext->color_count == 4)
					color0 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*3) };
			}
			if(_brcontext->normal_array) {
				normal0 = { *(float*)((void*)array + (size_t)normal_offset),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)*2) };
			}
			if(_brcontext->tcoord_array) {
				tcoord0 = { *(float*)((void*)array + (size_t)tcoord_offset),
					*(float*)((void*)array + (size_t)tcoord_offset + sizeof(float)) };
			}
		}
		if(v == 1) {
			position1 = { 0, 0, 0, 1 };
			color1    = { 0, 0, 0, 1 };
			normal1   = { 0, 0, 0 };
			tcoord1   = { 0, 0 };
			
			if(_brcontext->vertex_array) {
				if(_brcontext->vertex_count == 2)
					position1 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						position1.z, position1.w };
				if(_brcontext->vertex_count == 3)
					position1 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						position1.w };
				if(_brcontext->vertex_count == 4)
					position1 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*3) };
			}
			if(_brcontext->color_array) {
				if(_brcontext->color_count == 3)
					color1 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2), 
						color1.w };
				if(_brcontext->color_count == 4)
					color1 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*3) };
			}
			if(_brcontext->normal_array) {
				normal1 = { *(float*)((void*)array + (size_t)normal_offset),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)*2) };
			}
			if(_brcontext->tcoord_array) {
				tcoord1 = { *(float*)((void*)array + (size_t)tcoord_offset),
					*(float*)((void*)array + (size_t)tcoord_offset + sizeof(float)) };
			}
		}
		if(v == 2) {
			position2 = { 0, 0, 0, 1 };
			color2    = { 0, 0, 0, 1 };
			normal2   = { 0, 0, 0 };
			tcoord2   = { 0, 0 };
			
			if(_brcontext->vertex_array) {
				if(_brcontext->vertex_count == 2)
					position2 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						position2.z, position2.w };
				if(_brcontext->vertex_count == 3)
					position2 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						position2.w };
				if(_brcontext->vertex_count == 4)
					position2 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*3) };
			}
			if(_brcontext->color_array) {
				if(_brcontext->color_count == 3)
					color2 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2), 
						color2.w };
				if(_brcontext->color_count == 4)
					color2 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*3) };
			}
			if(_brcontext->normal_array) {
				normal2 = { *(float*)((void*)array + (size_t)normal_offset),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)*2) };
			}
			if(_brcontext->tcoord_array) {
				tcoord2 = { *(float*)((void*)array + (size_t)tcoord_offset),
					*(float*)((void*)array + (size_t)tcoord_offset + sizeof(float)) };
			}
		}
		
		if(ptype == BR_TRIANGLES && v == 2)
		{
			_vertex_t vertex;
			vertex.type = BR_TRIANGLE;
			vertex.position = position0;
			vertex.color = &color0;
			vertex.normals = &normal0;
			vertex.tcoords = &tcoord0;
			position0 = _vertex_pass(&vertex);
			
			vertex.position = position1;
			vertex.color = &color1;
			vertex.normals = &normal1;
			vertex.tcoords = &tcoord1;
			position1 = _vertex_pass(&vertex);
			
			vertex.position = position2;
			vertex.color = &color2;
			vertex.normals = &normal2;
			vertex.tcoords = &tcoord2;
			position2 = _vertex_pass(&vertex);
			
			if(_brcontext->poly_mode == BR_FILL) {
				_triangle_t tri;
				tri.v0 = position0;
				tri.v1 = position1;
				tri.v2 = position2;
				tri.rgba0 = color0;
				tri.rgba1 = color1;
				tri.rgba2 = color2;
				tri.tcoords0 = tcoord0;
				tri.tcoords1 = tcoord1;
				tri.tcoords2 = tcoord2;
				tri.parent = NULL;
				_process_triangle(&tri);
			}
			
			if(_brcontext->poly_mode == BR_LINE) {
				_line_t line;
				line.v0 = position0;
				line.v1 = position1;
				line.rgba0 = color0;
				line.rgba1 = color1;
				line.tcoords0 = tcoord0;
				line.tcoords1 = tcoord1;
				_process_line(&line);
				line.v0 = position1;
				line.v1 = position2;
				line.rgba0 = color1;
				line.rgba1 = color2;
				line.tcoords0 = tcoord1;
				line.tcoords1 = tcoord2;
				_process_line(&line);
				line.v0 = position2;
				line.v1 = position0;
				line.rgba0 = color2;
				line.rgba1 = color0;
				line.tcoords0 = tcoord2;
				line.tcoords1 = tcoord0;
				_process_line(&line);
			}
			
			if(_brcontext->poly_mode == BR_POINT) {
				_point_t point;
				point.pos = position0;
				point.rgba = color0;
				_process_point(&point);
				point.pos = position1;
				point.rgba = color1;
				_process_point(&point);
				point.pos = position2;
				point.rgba = color2;
				_process_point(&point);
			}
		}
		if(ptype == BR_LINES && v == 1)
		{
			_vertex_t vertex;
			vertex.type = BR_LINE;
			vertex.position = position0;
			vertex.color = &color0;
			vertex.normals = &normal0;
			vertex.tcoords = &tcoord0;
			position0 = _vertex_pass(&vertex);
			
			vertex.position = position1;
			vertex.color = &color1;
			vertex.normals = &normal1;
			vertex.tcoords = &tcoord1;
			position1 = _vertex_pass(&vertex);
			
			if(_brcontext->poly_mode == BR_FILL
			|| _brcontext->poly_mode == BR_LINE) {
				_line_t line;
				line.v0 = position0;
				line.v1 = position1;
				line.rgba0 = color0;
				line.rgba1 = color1;
				line.tcoords0 = tcoord0;
				line.tcoords1 = tcoord1;
				_process_line(&line);
			}
			
			if(_brcontext->poly_mode == BR_POINT) {
				_point_t point;
				point.pos = position0;
				point.rgba = color0;
				_process_point(&point);
				point.pos = position1;
				point.rgba = color1;
				_process_point(&point);
			}
		}
		if(ptype == BR_POINTS)
		{
			_vertex_t vertex;
			vertex.type = BR_POINT;
			vertex.position = position0;
			vertex.color = &color0;
			vertex.normals = &normal0;
			vertex.tcoords = &tcoord0;
			position0 = _vertex_pass(&vertex);
			
			_point_t point;
			point.pos = position0;
			point.rgba = color0;
			_process_point(&point);
		}
		
		v += 1;
		if(ptype == BR_TRIANGLES)
		{
			if(v > 2)
				v = 0;
		}
		if(ptype == BR_LINES)
		{
			if(v > 1)
				v = 0;
		}
		if(ptype == BR_POINTS)
			v = 0;
			
		vertex_offset += _brcontext->vertex_stride;
		color_offset  += _brcontext->color_stride;
		normal_offset += _brcontext->normal_stride;
		tcoord_offset += _brcontext->tcoord_stride;
	}
}

// draw an array using elements.
void brDrawElements(uint32_t ptype, uint32_t indices, float* array, uint32_t* elements)
{
	uint32_t v = 0;	// current vertex #
	brvec4 position0;
	brvec4 color0;
	brvec3 normal0;
	brvec2 tcoord0;
	brvec4 position1;
	brvec4 color1;
	brvec3 normal1;
	brvec2 tcoord1;
	brvec4 position2;
	brvec4 color2;
	brvec3 normal2;
	brvec2 tcoord2;
	
	void* vertex_offset;
	void* color_offset;
	void* normal_offset;
	void* tcoord_offset;
	
	for(uint32_t i = 0; i < indices; i += 1)
	{
		vertex_offset = _brcontext->vertex_offset + (_brcontext->vertex_stride*elements[i]);
		color_offset  = _brcontext->color_offset  + (_brcontext->color_stride*elements[i]);
		normal_offset = _brcontext->normal_offset + (_brcontext->normal_stride*elements[i]);
		tcoord_offset = _brcontext->tcoord_offset + (_brcontext->tcoord_stride*elements[i]);
		
		// load to vertex
		if(v == 0) {
			position0 = { 0, 0, 0, 1 };
			color0    = { 0, 0, 0, 1 };
			normal0   = { 0, 0, 0 };
			tcoord0   = { 0, 0 };
			
			if(_brcontext->vertex_array) {
				if(_brcontext->vertex_count == 2)
					position0 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						position0.z, position0.w };
				if(_brcontext->vertex_count == 3)
					position0 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						position0.w };
				if(_brcontext->vertex_count == 4)
					position0 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*3) };
			}
			if(_brcontext->color_array) {
				if(_brcontext->color_count == 3)
					color0 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2), 
						color0.w };
				if(_brcontext->color_count == 4)
					color0 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*3) };
			}
			if(_brcontext->normal_array) {
				normal0 = { *(float*)((void*)array + (size_t)normal_offset),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)*2) };
			}
			if(_brcontext->tcoord_array) {
				tcoord0 = { *(float*)((void*)array + (size_t)tcoord_offset),
					*(float*)((void*)array + (size_t)tcoord_offset + sizeof(float)) };
			}
		}
		if(v == 1) {
			position1 = { 0, 0, 0, 1 };
			color1    = { 0, 0, 0, 1 };
			normal1   = { 0, 0, 0 };
			tcoord1   = { 0, 0 };
			
			if(_brcontext->vertex_array) {
				if(_brcontext->vertex_count == 2)
					position1 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						position1.z, position1.w };
				if(_brcontext->vertex_count == 3)
					position1 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						position1.w };
				if(_brcontext->vertex_count == 4)
					position1 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*3) };
			}
			if(_brcontext->color_array) {
				if(_brcontext->color_count == 3)
					color1 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2), 
						color1.w };
				if(_brcontext->color_count == 4)
					color1 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*3) };
			}
			if(_brcontext->normal_array) {
				normal1 = { *(float*)((void*)array + (size_t)normal_offset),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)*2) };
			}
			if(_brcontext->tcoord_array) {
				tcoord1 = { *(float*)((void*)array + (size_t)tcoord_offset),
					*(float*)((void*)array + (size_t)tcoord_offset + sizeof(float)) };
			}
		}
		if(v == 2) {
			position2 = { 0, 0, 0, 1 };
			color2    = { 0, 0, 0, 1 };
			normal2   = { 0, 0, 0 };
			tcoord2   = { 0, 0 };
			
			if(_brcontext->vertex_array) {
				if(_brcontext->vertex_count == 2)
					position2 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						position2.z, position2.w };
				if(_brcontext->vertex_count == 3)
					position2 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						position2.w };
				if(_brcontext->vertex_count == 4)
					position2 = { *(float*)((void*)array + (size_t)vertex_offset),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)vertex_offset + sizeof(float)*3) };
			}
			if(_brcontext->color_array) {
				if(_brcontext->color_count == 3)
					color2 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2), 
						color2.w };
				if(_brcontext->color_count == 4)
					color2 = { *(float*)((void*)array + (size_t)color_offset),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*2),
						*(float*)((void*)array + (size_t)color_offset + sizeof(float)*3) };
			}
			if(_brcontext->normal_array) {
				normal2 = { *(float*)((void*)array + (size_t)normal_offset),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)),
					*(float*)((void*)array + (size_t)normal_offset + sizeof(float)*2) };
			}
			if(_brcontext->tcoord_array) {
				tcoord2 = { *(float*)((void*)array + (size_t)tcoord_offset),
					*(float*)((void*)array + (size_t)tcoord_offset + sizeof(float)) };
			}
		}
		
		if(ptype == BR_TRIANGLES && v == 2)
		{
			_vertex_t vertex;
			vertex.type = BR_TRIANGLE;
			vertex.position = position0;
			vertex.color = &color0;
			vertex.normals = &normal0;
			vertex.tcoords = &tcoord0;
			position0 = _vertex_pass(&vertex);
			
			vertex.position = position1;
			vertex.color = &color1;
			vertex.normals = &normal1;
			vertex.tcoords = &tcoord1;
			position1 = _vertex_pass(&vertex);
			
			vertex.position = position2;
			vertex.color = &color2;
			vertex.normals = &normal2;
			vertex.tcoords = &tcoord2;
			position2 = _vertex_pass(&vertex);
			
			if(_brcontext->poly_mode == BR_FILL) {
				_triangle_t tri;
				tri.v0 = position0;
				tri.v1 = position1;
				tri.v2 = position2;
				tri.rgba0 = color0;
				tri.rgba1 = color1;
				tri.rgba2 = color2;
				tri.tcoords0 = tcoord0;
				tri.tcoords1 = tcoord1;
				tri.tcoords2 = tcoord2;
				tri.parent = NULL;
				_process_triangle(&tri);
			}
			
			if(_brcontext->poly_mode == BR_LINE) {
				_line_t line;
				line.v0 = position0;
				line.v1 = position1;
				line.rgba0 = color0;
				line.rgba1 = color1;
				line.tcoords0 = tcoord0;
				line.tcoords1 = tcoord1;
				_process_line(&line);
				line.v0 = position1;
				line.v1 = position2;
				line.rgba0 = color1;
				line.rgba1 = color2;
				line.tcoords0 = tcoord1;
				line.tcoords1 = tcoord2;
				_process_line(&line);
				line.v0 = position2;
				line.v1 = position0;
				line.rgba0 = color2;
				line.rgba1 = color0;
				line.tcoords0 = tcoord2;
				line.tcoords1 = tcoord0;
				_process_line(&line);
			}
			
			if(_brcontext->poly_mode == BR_POINT) {
				_point_t point;
				point.pos = position0;
				point.rgba = color0;
				_process_point(&point);
				point.pos = position1;
				point.rgba = color1;
				_process_point(&point);
				point.pos = position2;
				point.rgba = color2;
				_process_point(&point);
			}
		}
		if(ptype == BR_LINES && v == 1)
		{
			_vertex_t vertex;
			vertex.type = BR_LINE;
			vertex.position = position0;
			vertex.color = &color0;
			vertex.normals = &normal0;
			vertex.tcoords = &tcoord0;
			position0 = _vertex_pass(&vertex);
			
			vertex.position = position1;
			vertex.color = &color1;
			vertex.normals = &normal1;
			vertex.tcoords = &tcoord1;
			position1 = _vertex_pass(&vertex);
			
			if(_brcontext->poly_mode == BR_FILL
			|| _brcontext->poly_mode == BR_LINE) {
				_line_t line;
				line.v0 = position0;
				line.v1 = position1;
				line.rgba0 = color0;
				line.rgba1 = color1;
				line.tcoords0 = tcoord0;
				line.tcoords1 = tcoord1;
				_process_line(&line);
			}
			
			if(_brcontext->poly_mode == BR_POINT) {
				_point_t point;
				point.pos = position0;
				point.rgba = color0;
				_process_point(&point);
				point.pos = position1;
				point.rgba = color1;
				_process_point(&point);
			}
		}
		if(ptype == BR_POINTS)
		{
			_vertex_t vertex;
			vertex.type = BR_POINT;
			vertex.position = position0;
			vertex.color = &color0;
			vertex.normals = &normal0;
			vertex.tcoords = &tcoord0;
			position0 = _vertex_pass(&vertex);
			
			_point_t point;
			point.pos = position0;
			point.rgba = color0;
			_process_point(&point);
		}
		
		v += 1;
		if(ptype == BR_TRIANGLES)
		{
			if(v > 2)
				v = 0;
		}
		if(ptype == BR_LINES)
		{
			if(v > 1)
				v = 0;
		}
		if(ptype == BR_POINTS)
			v = 0;
	}
}

// query state.
void brGetState(uint32_t type, uint32_t state, void* ret)
{
	if(type == BR_GLOBAL_STATE)
	{
		if(state == BR_CONTEXT_ADDRESS)
			*(void**)ret = _brcontext;
		return;
	}
	if(!_brcontext)
		return;
		
	if(type == BR_RENDERBUFFER_STATE)
	{
		uint32_t* a;
		
		switch(state)
		{
			case BR_FRONT_COLOR_TYPE:
				if(_brcontext->cb)
					*(uint32_t*)ret = _brcontext->cb_type;
				break;
			case BR_FRONT_COLOR_ADDRESS:
				*(void**)ret = _brcontext->cb;
				break;
			case BR_FRONT_DEPTH_TYPE:
				if(_brcontext->db)
					*(uint32_t*)ret = _brcontext->db_type;
				break;
			case BR_FRONT_DEPTH_ADDRESS:
				*(void**)ret = _brcontext->db;
				break;
			case BR_FRONT_DIMENSIONS:
				a = (uint32_t*)ret;
				a[0] = _brcontext->rb_width;
				a[1] = _brcontext->rb_height;
				break;
			case BR_BACK_COLOR_TYPE:
				if(_brcontext->cb2)
					*(uint32_t*)ret = _brcontext->cb2_type;
				break;
			case BR_BACK_COLOR_ADDRESS:
				*(void**)ret = _brcontext->cb2;
				break;
			case BR_BACK_DEPTH_TYPE:
				if(_brcontext->db2)
					*(uint32_t*)ret = _brcontext->db2_type;
				break;
			case BR_BACK_DEPTH_ADDRESS:
				*(void**)ret = _brcontext->db2;
				break;
			case BR_BACK_DIMENSIONS:
				a = (uint32_t*)ret;
				a[0] = _brcontext->rb2_width;
				a[1] = _brcontext->rb2_height;
				break;
			case BR_CLEAR_COLOR:
				*(brvec4*)ret = _brcontext->clear_color;
				break;
			case BR_CLEAR_DEPTH:
				*(float*)ret = _brcontext->clear_depth;
				break;
		}
	}
	
	if(type == BR_RENDER_STATE)
	{
		switch(state)
		{
			case BR_POINT_SIZE:
				*(float*)ret = _brcontext->point_radius;
				break;
			case BR_CULL_WINDING:
				*(uint32_t*)ret = _brcontext->cull_winding;
				break;
			case BR_POLYGON_MODE:
				*(uint32_t*)ret = _brcontext->poly_mode;
				break;
			case BR_VERTEX_SHADER_ADDRESS:
				*(void**)ret = (void*) _brcontext->vshader;
				break;
			case BR_FRAGMENT_SHADER_ADDRESS:
				*(void**)ret = (void*) _brcontext->fshader;
				break;
		}
	}
	
	if(type == BR_ARRAY_STATE)
	{
		switch(state)
		{
			case BR_VERTEX_STRIDE:
				*(void**)ret = (void*) _brcontext->vertex_stride;
				break;
			case BR_COLOR_STRIDE:
				*(void**)ret = (void*) _brcontext->color_stride;
				break;
			case BR_NORMAL_STRIDE:
				*(void**)ret = (void*) _brcontext->normal_stride;
				break;
			case BR_TEXCOORD_STRIDE:
				*(void**)ret = (void*) _brcontext->tcoord_stride;
				break;
			case BR_VERTEX_OFFSET:
				*(void**)ret = _brcontext->vertex_offset;
				break;
			case BR_COLOR_OFFSET:
				*(void**)ret = _brcontext->color_offset;
				break;
			case BR_NORMAL_OFFSET:
				*(void**)ret = _brcontext->normal_offset;
				break;
			case BR_TEXCOORD_OFFSET:
				*(void**)ret = _brcontext->tcoord_offset;
				break;
			case BR_VERTEX_COUNT:
				*(uint32_t*)ret = _brcontext->vertex_count;
				break;
			case BR_COLOR_COUNT:
				*(uint32_t*)ret = _brcontext->color_count;
				break;
		}
	}
}

// get an identity matrix.
brmat4 brIdentity()
{
	brmat4 id;
	id.m00 = 1.0f;
	id.m01 = 0.0f;
	id.m02 = 0.0f;
	id.m03 = 0.0f;
	id.m10 = 0.0f;
	id.m11 = 1.0f;
	id.m12 = 0.0f;
	id.m13 = 0.0f;
	id.m20 = 0.0f;
	id.m21 = 0.0f;
	id.m22 = 1.0f;
	id.m23 = 0.0f;
	id.m30 = 0.0f;
	id.m31 = 0.0f;
	id.m32 = 0.0f;
	id.m33 = 1.0f;
	return id;
}

// multiply matrix a * b.
brmat4 brMat4Mat4(brmat4 a, brmat4 b)
{
	brmat4 p;
	p.m00 = a.m00*b.m00 + a.m01*b.m10 + a.m02*b.m20 + a.m03*b.m30;
	p.m01 = a.m00*b.m01 + a.m01*b.m11 + a.m02*b.m21 + a.m03*b.m31;
	p.m02 = a.m00*b.m02 + a.m01*b.m12 + a.m02*b.m22 + a.m03*b.m32;
	p.m03 = a.m00*b.m03 + a.m01*b.m13 + a.m02*b.m23 + a.m03*b.m33;
	p.m10 = a.m10*b.m00 + a.m11*b.m10 + a.m12*b.m20 + a.m13*b.m30;
	p.m11 = a.m10*b.m01 + a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	p.m12 = a.m10*b.m02 + a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	p.m13 = a.m10*b.m03 + a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;
	p.m20 = a.m20*b.m00 + a.m21*b.m10 + a.m22*b.m20 + a.m23*b.m30;
	p.m21 = a.m20*b.m01 + a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	p.m22 = a.m20*b.m02 + a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	p.m23 = a.m20*b.m03 + a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;
	p.m30 = a.m30*b.m00 + a.m31*b.m10 + a.m32*b.m20 + a.m33*b.m30;
	p.m31 = a.m30*b.m01 + a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	p.m32 = a.m30*b.m02 + a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	p.m33 = a.m30*b.m03 + a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;	
	return p;
}

// multiply m * v.
brvec4 brMat4Vec4(brmat4 m, brvec4 v)
{
	brvec4 prod;
	prod.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w;
	prod.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w;
	prod.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w;
	prod.w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w;
	return prod;
}

// calculate a symmetrical-frustum projection matrix, foyv provided in degrees.
brmat4 brPerspective(float fovy, float aspect, float near, float far)
{
	fovy *= .01745329251f;			// 1 / (180 / π)
	brmat4 m = brIdentity();
	float f = _fdiv(1.0f, tan(fovy/2.0f));
	
	m.m00 = _fdiv(f, aspect);
	m.m11 = f;
	m.m22 = -_fdiv(far+near, far-near);
	m.m23 = -_fdiv(2*far*near, far-near);
	m.m32 = -1;
	m.m33 = 0;
	return m;
}

// calculate a projection matrix.
brmat4 brFrustum(float left, float right, float top, float bottom, float near, float far)
{
	brmat4 m = brIdentity();
	m.m00 = _fdiv(2.0f*near, right - left);
	m.m02 = _fdiv(right+left, right-left);
	m.m11 = _fdiv(2.0f*near, top-bottom);
	m.m12 = _fdiv(top+bottom, top-bottom);
	m.m22 = _fdiv(-(far+near), far-near);
	m.m23 = _fdiv(-2.0f*far*near, far-near);
	m.m32 = -1;
	m.m33 = 0;
	return m;
}

// calculate a look-at matrix.
brmat4 brLookAt(brvec3 eye, brvec3 center, brvec3 up)
{
	brvec3 f;
	f.x = center.x - eye.x;
	f.y = center.y - eye.y;
	f.z = center.z - eye.z;
	f = _normalize_vec3(f);
		
	brvec3 u = _normalize_vec3(up);
	brvec3 s = _normalize_vec3(_cross_vec3(f,u));
	u = _cross_vec3(s,f);
	
	brmat4 mat = brIdentity();
	mat.m00 = s.x;
	mat.m01 = s.y;
	mat.m02 = s.z;
	mat.m10 = u.x;
	mat.m11 = u.y;
	mat.m12 = u.z;
	mat.m20 = -f.x;
	mat.m21 = -f.y;
	mat.m22 = -f.z;
	mat.m03 = -_dot_vec3(s, eye);
	mat.m13 = -_dot_vec3(u, eye);
	mat.m23 = _dot_vec3(f, eye);
	return mat;
}

// calculate a scale matrix.
brmat4 brScale(brvec3 scale)
{
	brmat4 mscale = brIdentity();
	mscale.m00 = scale.x;
	mscale.m11 = scale.y;
	mscale.m22 = scale.z;
	return mscale;
}

// calculate a rotation matrix (angle given in degrees)
brmat4 brRotate(float angle, brvec3 axis)
{
	angle = fmod(angle, 360.0);
	angle *= .01745329251f;			// 1 / (180 / π)

	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	
	float c = cos(angle);
	float s = sin(angle);
	
	float one_sub_c = 1.0f - c;
	float zs = z*s;
	float ys = y*s;
	float xs = x*s;
	float xz = x*z;
	float yz = y*z;
	
	brmat4 mrotation = brIdentity();
	mrotation.m00 = x*x*(one_sub_c)+c;
	mrotation.m01 = x*y*(one_sub_c)-zs;
	mrotation.m02 = xz *(one_sub_c)+ys;
	mrotation.m10 = y*x*(one_sub_c)+zs;
	mrotation.m11 = y*y*(one_sub_c)+c;
	mrotation.m12 = yz *(one_sub_c)-xs;
	mrotation.m20 = xz *(one_sub_c)-ys;
	mrotation.m21 = yz *(one_sub_c)+xs;
	mrotation.m22 = z*z*(one_sub_c)+c;
	return mrotation;
}

// calculate a translation matrix.
brmat4 brTranslate(brvec3 translation)
{
	brmat4 mtranslation = brIdentity();
	mtranslation.m03 = translation.x;
	mtranslation.m13 = translation.y;
	mtranslation.m23 = translation.z;
	return mtranslation;
}

// convert Euler angles (given in degrees) to quaternion.
brvec4 brEulerToQuat(brvec3 angles)
{
	float c1, c2, c3;
	float s1, s2, s3;
	
	angles.x = fmod(angles.x, 360.0);
	angles.y = fmod(angles.y, 360.0);
	angles.z = fmod(angles.z, 360.0);
	
	float rad_per_degree = .01745329251f;	// 1 / (180 / π)
	angles.x *= rad_per_degree;
	angles.y *= rad_per_degree;
	angles.z *= rad_per_degree;
	
	c1 = cos(angles.y / 2.0);
	c2 = cos(angles.z / 2.0);
	c3 = cos(angles.x / 2.0);
	s1 = sin(angles.y / 2.0);
	s2 = sin(angles.z / 2.0);
	s3 = sin(angles.x / 2.0);
	
	brvec4 quat;
	quat.w = c1*c2*c3 - s1*s2*s3;
	quat.x = s1*s2*c3 + c1*c2*s3;
	quat.y = s1*c2*c3 + c1*s2*s3;
	quat.z = c1*s2*c3 - s1*c2*s3;
	
	float n = sqrt(pow(quat.x, 2) + pow(quat.y, 2) + pow(quat.z, 2) + pow(quat.w, 2));
	float inv = _fdiv(1.0f, n);
	quat.x *= inv;
	quat.y *= inv;
	quat.z *= inv;
	quat.w *= inv;
	
	return quat;
}

// convert quaternion to rotation matrix.
brmat4 brQuatToMat4(brvec4 quat)
{
	float xx = quat.x * quat.x;
	float xy = quat.x * quat.y;
	float xz = quat.x * quat.z;
	float xw = quat.x * quat.w;
	float yy = quat.y * quat.y;
	float yz = quat.y * quat.z;
	float yw = quat.y * quat.w;
	float zz = quat.z * quat.z;
	float zw = quat.z * quat.w;
	
	brmat4 rotation = brIdentity();
	rotation.m00 = 1.0f - 2.0f*yy - 2.0f*zz;
	rotation.m01 = 2.0f * xy - 2.0f * zw;
	rotation.m02 = 2.0f * xz + 2.0f * yw;
	rotation.m10 = 2.0f * xy + 2.0f * zw;
	rotation.m11 = 1.0f - 2.0f*xx - 2.0f*zz;
	rotation.m12 = 2.0f * yz - 2.0f * xw;
	rotation.m20 = 2.0f * xz - 2.0f * yw;
	rotation.m21 = 2.0f * yz + 2.0f * xw;
	rotation.m22 = 1.0f - 2.0f*xx - 2.0f*yy;
	return rotation;
}

#undef _INV_65536
#undef _INV_255
#undef _INV_31
#undef _INV_7
#undef _INV_3

#endif
