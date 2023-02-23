// rl.h
// Rendering Library v1.1 (Development)
// ----------------
// dec 21 2017
// gabriel campbell
//
// Changelog:
// V 1.0                   Feb 25 2018
// - first version of library complete
// - points, lines, triangles
// - vertex and fragment shaders
// - quaternions, 4x4 matrices, vectors
// - perspective corrected and affine interpolation
// - tile-based rasterization
// - alpha-based blending
// - depth testing
// - 16 and 32-bit depth and color buffers
// - culling
// - compressed and non-compressed textures
// - 256 texture units and texturing
// - texture formats RL_RGB16, RL_RGB32, RL_RGBA16, RL_RGBA32
// V 1.1                   Work in progress
// - corrected projection matrices
// - point rasterization switched to bresenham circle algorithm
// - homogeneous clipping
// - automated perspective division (RL_PERSPECTIVE_DIVISION)
// - scaling of Z range [-1,1] to [0,1] (RL_SCALE_Z)
// - RL_FRAG_DEPTH fragment shader attribute
// - RL_V4 array types
// - revisements
//
//
//
//
// See https://github.com/gabecampb/Rendering-Library for documentation and information.
//
//
// Error handling not yet implemented.
// Some of the possible errors were commented with "// unhandled error: ".
//
// If an active texture unit is complete at the time of a draw call, the rastered primitive will be textured 
// and the texture color will be set as it's secondary color.
// Otherwise, the primitive will be drawn as its color and secondary color will be default.
//
// SHADER ATTRIBUTES, IN ORDER THAT THEY ARE PASSED (IF ENABLED):
//
// Vertex attributes:
// RL_PRIMITIVE_TYPE : primitive type. This will be RL_TRIANGLE, RL_POINT or RL_LINE (not affected by polygon mode).
// RL_VERTEX_ARRAY : vertex position, default (0,0,0,1)
// RL_COLOR_ARRAY : vertex RGBA, default (0,0,0,1)
// RL_NORMAL_ARRAY : vertex normals, default (0,0,0)
// RL_TEXCOORD_ARRAY : texture coordinates, default (0,0)
//
// Fragment attributes:
// RL_PRIMITIVE_TYPE : primitive type. This will be RL_TRIANGLE, RL_POINT or RL_LINE (affected by polygon mode).
// RL_COLOR_ARRAY : primitive color after texturing (if texturing occurs, this will be equivalent to the secondary color)
// RL_PRIMARY_COLOR : primitive color, default (0,0,0,1)
// RL_SECONDARY_COLOR : secondary color (texture color), default (0,0,0,0)
// RL_BARY_LINEAR : linearly interpolated barycentric coordinates, default (0,0,0)
// RL_BARY_PERSPECTIVE : perspective corrected barycentric coordinates. defaults to linear if RL_PERSPECTIVE_CORRECTION is not enabled.
// RL_DST_DEPTH : the depth of the destination in the depth buffer (0-1). 0 in absence of a depth buffer.
// RL_FRAG_DEPTH : the depth (Z) coordinate of the fragment (0-1).
// RL_FRAG_X_COORD: the X coordinate (in pixels, where top left is (0,0)) of the fragment.
// RL_FRAG_Y_COORD: the Y coordinate of the fragment.
//
// Attributes are passed to shaders as a void* data array.
// They can be enabled/disabled as states and occur in the ordered they are listed. 
// Additionally, shader functions are passed an array of the contained attribute constants
// describing the order & format of the array and an attribute count describing how many attributes are in the array.
//
// Attribute types are as follows:
//    Vertex shader:
//    RL_PRIMITIVE_TYPE is uint32, which takes up 4 bytes.
//    RL_VERTEX_ARRAY is rlVec4, which takes up 16 bytes.
//    RL_COLOR_ARRAY is rlVec4, which takes up 16 bytes.
//    RL_NORMAL_ARRAY is rlVec3, which takes up 12 bytes.
//    RL_TEXCOORD_ARRAY is rlVec2, which takes up 8 bytes.
//
//    Fragment shader:
//    RL_PRIMITVE_TYPE is uint32, which takes up 4 bytes.
// 	  RL_COLOR_ARRAY is rlVec4, which takes up 16 bytes.
//    RL_PRIMARY_COLOR is rlVec4, which takes up 16 bytes.
//    RL_SECONDARY_COLOR is rlVec4, which takes up 16 bytes.
//    RL_BARY_LINEAR is rlVec3, which takes up 12 bytes.
//    RL_BARY_PERSPECTIVE is rlVec3, which takes up 12 bytes.
//    RL_DST_DEPTH is float, which takes up 4 bytes.
//    RL_FRAG_DEPTH is float, which takes up 4 bytes.
//    RL_FRAG_X_COORD is int, which takes up 4 bytes.
//    RL_FRAG_Y_COORD is int, which takes up 4 bytes.
//
// ------------------
//
// coordinate spaces:
// Object space : vertex coordinates relative to origin
// World space : object space coordinates multplied by a model matrix
// Eye space : world space coordinates multiplied by a view matrix
// Projection space : eye space coordinates multiplied by a projection matrix
// Final coordinates :
// 	Left of screen is (-1,0,0). Right of screen is (1,0,0). Top of screen is (0,1,0). Bottom of screen is (0,-1,0).
// 	Near plane is (0,0,0), and far plane is (0,0,1).


#ifndef RL_H
#define RL_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

// toggled states
#define RL_PERSPECTIVE_CORRECTION	0x01	/* generate perspective corrected barycentric coordinates */
#define RL_BLEND		0x02				/* blend alpha < 1.0 pixels with destination */
#define RL_TEXTURE		0xFF				/* apply texture from active texture unit to primitives */
#define RL_DEPTH_TEST	0x03				/* pixels with depth > than that in depth buffer are discarded */
#define RL_DEPTH_WRITE	0x04				/* pixels write to depth buffer */
#define RL_CULL			0x05				/* cull faces with specified winding */

// vertex layouts: V = vertex, C = color, N = normal, T = texture coordinates
#define RL_V3			0x07
#define RL_V3_C4		0x08
#define RL_V3_N3		0x09
#define RL_V3_T2		0x0A
#define RL_V3_N3_T2		0x0B
#define RL_V3_C4_N3		0x0C
#define RL_V3_C4_T2		0x0D
#define RL_V3_C4_N3_T2	0x0E
#define RL_V4			0x0F
#define RL_V4_C4		0x10
#define RL_V4_N3		0x11
#define RL_V4_T2		0x12
#define RL_V4_N3_T2		0x13
#define RL_V4_C4_N3		0x14
#define RL_V4_C4_T2		0x15
#define RL_V4_C4_N3_T2	0x16

// polygon modes and primitive types
#define RL_FILL		0x17			// denotes polygon mode fill
#define RL_LINE		0x18			// denotes primitive type and polygon mode line
#define RL_POINT	0x19			// denotes primitive type and polygon mode point
#define RL_TRIANGLE 0x1A			// denotes only the primitive type; not to be used as a polygon mode

// primitive types
#define RL_TRIANGLES	0x1B			// denotes triangle descriptions
#define RL_LINES		0x1C			// denotes line descriptions
#define RL_POINTS		0x1D			// denotes point descriptions

// triangle windings
#define RL_CW  0x1E
#define RL_CCW 0x1F

// shaders
#define RL_VERTEX_SHADER	0x20
#define RL_FRAGMENT_SHADER	0x21

// clipping, vertex and primitive post-processing
#define RL_CLIP						0x22
#define RL_PERSPECTIVE_DIVISION		0x23
#define RL_SCALE_Z					0x24

// shader attributes
#define RL_PRIMITIVE_TYPE	0x25
#define RL_VERTEX_ARRAY		0x26
#define RL_COLOR_ARRAY		0x27
#define RL_NORMAL_ARRAY		0x28
#define RL_TEXCOORD_ARRAY	0x29
#define RL_PRIMARY_COLOR	0x2A
#define RL_SECONDARY_COLOR	0x2B
#define RL_BARY_LINEAR		0x2C
#define RL_BARY_PERSPECTIVE	0x2D
#define RL_DST_DEPTH		0x2E
#define RL_FRAG_DEPTH		0x2F
#define RL_FRAG_X_COORD		0x30
#define RL_FRAG_Y_COORD		0x31

// pixel formats
#define RL_RGB16	0x32 	/* 16-bit RGB */
#define RL_RGB32	0x33 	/* 32-bit RGB */
#define RL_RGBA16	0x34 	/* 16-bit RGBA */
#define RL_RGBA32	0x35 	/* 32-bit RGBA */
#define RL_D16		0x36 	/* 16-bit depth */
#define RL_D32		0x37 	/* 32-bit depth */

// buffer sets
#define RL_FRONT_BUFFERS	0x38
#define RL_BACK_BUFFERS 	0x39

// bit flags
#define RL_DEPTH_BUFFER_BIT		0x40000000 
#define RL_COLOR_BUFFER_BIT		0x20000000

// 16-bit R5 G5 B5 A1 read/write macros
#define _RL_RGBA16(r,g,b,a) (uint16_t)(a | (b << 5) | (g << 10) | (r << 15))
#define _RL_RGBA16_A(x) (uint8_t)( x & 0x0001)
#define _RL_RGBA16_B(x) (uint8_t)((x & 0x003E) >> 1)
#define _RL_RGBA16_G(x) (uint8_t)((x & 0x07C0) >> 6)
#define _RL_RGBA16_R(x) (uint8_t)((x & 0xF800) >>11)
// 32-bit R8 G8 B8 A8 read/write macros
#define _RL_RGBA32(r,g,b,a) (uint32_t)(a | (b << 8) | (g << 16) | (r << 24))
#define _RL_RGBA32_A(x) (uint8_t)( x & 0xFF)
#define _RL_RGBA32_B(x) (uint8_t)((x & 0xFF00) >> 8)
#define _RL_RGBA32_G(x) (uint8_t)((x & 0xFF0000) >> 16)
#define _RL_RGBA32_R(x) (uint8_t)((x & 0xFF000000) >> 24)

static const float RL_DEGREES_PER_RADIAN = 180.0 / 3.14159265358979323846264338327950288;
static const float RL_RADIANS_PER_DEGREE = 1.0 / (180.0 / 3.14159265358979323846264338327950288);

typedef struct rlMat4 rlMat4;
typedef struct rlVec4 rlVec4;
typedef struct rlVec3 rlVec3;
typedef struct rlVec2 rlVec2;
typedef struct rlVec4i rlVec4i;
typedef struct rlVec3i rlVec3i;
typedef struct rlVec2i rlVec2i;
typedef struct rlVec4ui rlVec4ui;
typedef struct rlVec3ui rlVec3ui;
typedef struct rlVec2ui rlVec2ui;
typedef struct _rlcore_t _rlcore_t;

/* allocate, initialize and return a context */
_rlcore_t* rlCreateContext();
/* bind a context */
void rlBindContext(_rlcore_t* context);
/* draw primitives described by an array */
void rlDrawArray(uint32_t primitive_type, uint32_t primitive_count, float* data);
/* draw primitives described by an array and an index array */
void rlDrawElements(uint32_t primitive_type, uint32_t primitive_count, float* data, uint32_t* elements);
/* enable a state. */
void rlEnable(uint32_t state);
/* disable a state. */
void rlDisable(uint32_t state);
/* check if a state is enabled. */
bool rlIsEnabled(uint32_t state);
/* set polygon mode. */
void rlPolygonMode(uint32_t mode);
/* specify cull winding */
void rlCullWinding(uint32_t winding);
/* set radius of points. */
void rlPointSize(float radius);
/* allocate a display buffer. */
void rlCreateBuffer(uint32_t type, uint32_t width, uint32_t height, void** buffer);
/* bind a display buffer to front set. Buffer must have same dimensions as any already bound. 'type' is a depth or pixel format. */
void rlBindBuffer(uint32_t type, uint32_t width, uint32_t height, void* buffer);
/* unbind a buffer from the front set, if bound. OR together all desired RL_*_BUFFER_BIT bits. May reset dimensions. */
void rlUnbindBuffer(uint32_t buffers);
/* swap front buffers with back buffers */
void rlSwapBuffers();
/* get dimensions of front or back buffer set in pre-allocated (width, height) array. Note dimensions will be 0 if no buffers are bound. */
void rlGetBufferSize(uint32_t buffer, uint32_t* dimensions);
/* check if there is a specific buffer in the front buffer set. */
bool rlIsBuffer(uint32_t buffer);
/* get the depth range of depth buffer in front set. Returns 0 in absence of depth buffer. */
int64_t rlMaxDepth();
/* set clear depth for rlClear. < 0 is the maximum depth for the depthbuffer  */
void rlClearDepth(float depth);
/* set clear color for rlClear. */
void rlClearColor(float red, float green, float blue);
/* clear back buffers. OR together buffer constants. */
void rlClear(uint32_t buffers);
/* sample currently active texture unit. (0,0) is bottom left and (1,1) is top right. Returns (0,0,0,1) if texture unit incomplete. */
rlVec4 rlSampleTexture(float x, float y);
/* set active texture unit */
void rlActiveTexture(uint8_t unit);
/* provide information for active texture unit. Pass 0 as data to reset all values */
void rlTexture(void* data, uint32_t format, uint32_t width, uint32_t height, bool compressed);
/* bind a shader. */
void rlBindShader(uint32_t type, void* shader);
/* compute a * b */
rlMat4 rlMat4Mat4(rlMat4 a, rlMat4 b);
/* compute m * v */
rlVec4 rlMat4Vec4(rlMat4 m, rlVec4 v);
/* calculate a symmetrical-frustum projection matrix */
rlMat4 rlPerspective(float fovy, float aspect, float near, float far);
/* calculate a projection matrix */
rlMat4 rlFrustum(float left, float right, float top, float bottom, float near, float far);
/* calculate a LookAt matrix */
rlMat4 rlLookAt(rlVec3 eye, rlVec3 center, rlVec3 up);
/* calculate a scale matrix */
rlMat4 rlScale(rlVec3 scale);
/* calculate a rotation matrix (rotation in degrees) */
rlMat4 rlRotate(float angle, rlVec3 axis);
/* convert Euler angles (given in degrees) to quaternion */
rlVec4 rlEulerToQuat(rlVec3 angles);
/* convert quaternion to rotation matrix (assumed to be normalized) */
rlMat4 rlQuatToMat4(rlVec4 quat);
/* calculate a translation matrix */
rlMat4 rlTranslate(rlVec3 translation);
/* get an identity matrix */
rlMat4 rlIdentity();

struct rlVec4
{
	float x,y,z,w;
};
struct rlVec3
{
	float x,y,z;
};
struct rlVec2
{
	float x,y;
};
struct rlVec4i
{
	int x,y,z,w;
};
struct rlVec3i
{
	int x,y,z;
};
struct rlVec2i
{
	int x,y;
};
struct rlVec4ui
{
	uint32_t x,y,z,w;
};
struct rlVec3ui
{
	uint32_t x,y,z;
};
struct rlVec2ui
{
	uint32_t x,y;
};
struct rlMat4
{
	// m{row}{col}
	float m00, m01, m02, m03, 
	      m10, m11, m12, m13,
	      m20, m21, m22, m23, 
	      m30, m31, m32, m33;
};

// the RL context structure
struct _rlcore_t
{
	int64_t _clear_depth;	// rlClear depth; < 0 represents maximum of depth buffer
	uint32_t _clear_color;	// rlClear color

	/* front buffers */
	
	void* _depthbuffer;	// depth buffer
	void* _colorbuffer;	// color buffer
	uint32_t _db_type;		// type of depth buffer
	uint32_t _cb_type;		// type of color buffer
	uint32_t _width;		// width of front buffers
	uint32_t _height;		// height of front buffers

	/* back buffers */
	
	void* _back_depthbuffer;	// back depth buffer type
	void* _back_colorbuffer;	// back color buffer type
	uint32_t _back_db_type;		// back depth buffer type
	uint32_t _back_cb_type;		// back color buffer type
	uint32_t _back_width;		// width of back buffers
	uint32_t _back_height;		// height of back buffers

	uint32_t _vertex_layout;	// current vertex array layout
	uint32_t _mode;	// polygon mode for draw calls
	uint32_t _cull_winding;
	int32_t _point_radius;	// current point radius
	bool _write_depth;	// whether or not to write depth
	bool _depth_test;	// whether or not to perform depth testing
	bool _persp_corr;	// whether or not to generate perspective correct barycentric coordinates
	bool _blend;		// whether or not to blend pixels with their destination
	bool _texture;		// whether or not to apply texture from active texture unit
	bool _cull;			// whether or not to cull faces (winding specified by rlCullWinding)
	bool _clip;			// whether or not to perform clipping against -w <= (x,y,z) <= w during primitive post-processing
	bool _persp_div;	// whether or not to perform perspective (w) division during primitive post-processing
	bool _scale_z;		// whether or not to scale final z from [-1,1] to [0,1] (* .5 + 5) during primitive post-processing
	
	uint8_t _texture_unit;	// current texture unit
	void* _textures[256];		// textures for each unit
	uint32_t _texture_formats[256];	// texture formats
	uint32_t _texture_widths[256];	// texture widths
	uint32_t _texture_heights[256];	// texture heights
	bool _texture_compressed_booleans[256];	// texture is compressed booleans
	
	rlVec4 (*_vshader) (void* data, uint32_t* format, uint32_t attrib_count);	// current vertex shader
	rlVec4 (*_fshader) (void* data, uint32_t* format, uint32_t attrib_count, bool* discard);	// current fragment shader
	bool _sh_primitive_type;	// whether or not to pass primitive type to shaders
	bool _sh_vertex_array;		// whether or not to pass vertex position to _vshader
	bool _sh_color_array;		// whether or not to pass vertex color to _vshader
	bool _sh_normal_array;		// whether or not to pass vertex normals to _vshader
	bool _sh_texcoord_array;	// whether or not to pass vertex texture coordinates to _vshader
	bool _sh_primary_color;		// whether or not to pass primary colors to _fshader
	bool _sh_secondary_color;	// whether or not to pass secondary colors to _fshader
	bool _sh_bary_linear;		// whether or not to pass linear barycentric coordinates to _fshader
	bool _sh_bary_perspective;	// whether or not to pass perspective-corrected barycentric coordinates to _fshader
	bool _sh_dst_depth;		// whether or not to pass destination depth to _fshader
	bool _sh_frag_depth;	// whether or not to pass fragment depth to _fshader
	bool _sh_frag_x_coord;	// whether or not to pass fragment x coordinate to _fshader
	bool _sh_frag_y_coord;	// whether or not to pass fragment y coordinate to _fshader
	
	float _inv_255;
	float _inv_31;
};
_rlcore_t* _rlcore;		// the current context

// safely divide two floats (avoid division-by-zero errors)
float _safedivf(float a, float b)
{
	if(b == 0.0f)
		return 0.0f;
	return a / b;
}

// find min of two integers
int _min(int a, int b)
{
	return (a <= b) ? a : b;
}

// find max of two integers
int _max(int a, int b)
{
	return (a >= b) ? a : b;
}

// find min of unsigned integers
uint32_t _min_u32(uint32_t a, uint32_t b)
{
	return (a <= b) ? a : b;
}
	
// find max of unsigned integers
uint32_t _max_u32(uint32_t a, uint32_t b)
{
	return (a >= b) ? a : b;
}

// find min of 64-bit integers
int64_t _min64(int64_t a, int64_t b)
{
	return (a <= b) ? a : b;
}

// find max of 64-bit integers
int64_t _max64(int64_t a, int64_t b)
{
	return (a >= b) ? a : b;
}
	
// find min of two floats
float _minf(float a, float b)
{
	return (a <= b) ? a : b;
}

// find max of two floats
float _maxf(float a, float b)
{
	return (a >= b) ? a : b;
}

// normalize a vector
rlVec3 _normalize_vec3(rlVec3 v)
{
	float length = sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
	rlVec3 norm;
	norm.x = _safedivf(v.x, length);
	norm.y = _safedivf(v.y, length);
	norm.z = _safedivf(v.z, length);
	return norm;
}
	
// calculate cross product of two vectors
rlVec3 _cross_vec3(rlVec3 a, rlVec3 b)
{
	rlVec3 cross;
	cross.x = a.y*b.z - a.z*b.y;
	cross.y = a.z*b.x - a.x*b.z;
	cross.z = a.x*b.y - a.y*b.x;
	return cross;
}
	
// calculate difference of two vectors
rlVec3 _sub_vec3(rlVec3 a, rlVec3 b)
{
	rlVec3 diff;
	diff.x = a.x - b.x;
	diff.y = a.y - b.y;
	diff.z = a.z - b.z;
	return diff;
}

// calculate dot product of two vectors
float _dot_vec3(rlVec3 a, rlVec3 b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}
	
// reads a single vertex from an array and returns data
// based on current vertex layout.
// position are 4 floats, color 4 floats, normals 3 floats and texcoords 2 floats.
// not to be used directly
void _read_vertex(float* data, uint32_t width, uint32_t vertex, uint32_t vertex_read_count,
	float* v0_position,	float* v1_position,	float* v2_position,
	float* v0_color,		float* v1_color,		float* v2_color, 
	float* v0_normals,		float* v1_normals,		float* v2_normals,
	float* v0_texcoords,	float* v1_texcoords,	float* v2_texcoords)
{
	if(!_rlcore)
		return;

	data += vertex * width;

	if(vertex_read_count == 0)
		return;

	switch(_rlcore->_vertex_layout)
	{
	case RL_V3:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = 1;
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = 1;
	}
	if(vertex_read_count > 2)
	{
		data += width;	
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = 1;
	}
	break;
	case RL_V3_C4:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = 1;
	v0_color[0]    = data[3];
	v0_color[1]    = data[4];
	v0_color[2]    = data[5];
	v0_color[3]    = data[6];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = 1;
		v1_color[0]    = data[3];
		v1_color[1]    = data[4];
		v1_color[2]    = data[5];
		v1_color[3]    = data[6];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = 1;
		v2_color[0]    = data[3];
		v2_color[1]    = data[4];
		v2_color[2]    = data[5];
		v2_color[3]    = data[6];
	}
	break;
	case RL_V3_N3:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = 1;
	v0_normals[0]  = data[3];
	v0_normals[1]  = data[4];
	v0_normals[2]  = data[5];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = 1;
		v1_normals[0]  = data[3];
		v1_normals[1]  = data[4];
		v1_normals[2]  = data[5];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = 1;
		v2_normals[0]  = data[3];
		v2_normals[1]  = data[4];
		v2_normals[2]  = data[5];
	}
	break;
	case RL_V3_T2:
	v0_position[0]  = data[0];
	v0_position[1]  = data[1];
	v0_position[2]  = data[2];
	v0_position[3]  = 1;
	v0_texcoords[0] = data[3];
	v0_texcoords[1] = data[4];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0]  = data[0];
		v1_position[1]  = data[1];
		v1_position[2]  = data[2];
		v1_position[3]  = 1;
		v1_texcoords[0] = data[3];
		v1_texcoords[1] = data[4];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0]  = data[0];
		v2_position[1]  = data[1];
		v2_position[2]  = data[2];
		v2_position[3]  = 1;
		v2_texcoords[0] = data[3];
		v2_texcoords[1] = data[4];
	}
	break;
	case RL_V3_N3_T2:
	v0_position[0]  = data[0];
	v0_position[1]  = data[1];
	v0_position[2]  = data[2];
	v0_position[3]  = 1;
	v0_normals[0]   = data[3];
	v0_normals[1]   = data[4];
	v0_normals[2]   = data[5];
	v0_texcoords[0] = data[6];
	v0_texcoords[1] = data[7];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0]  = data[0];
		v1_position[1]  = data[1];
		v1_position[2]  = data[2];
		v1_position[3]  = 1;
		v1_normals[0]   = data[3];
		v1_normals[1]   = data[4];
		v1_normals[2]   = data[5];
		v1_texcoords[0] = data[6];
		v1_texcoords[1] = data[7];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0]  = data[0];
		v2_position[1]  = data[1];
		v2_position[2]  = data[2];
		v2_position[3]  = 1;
		v2_normals[0]   = data[3];
		v2_normals[1]   = data[4];
		v2_normals[2]   = data[5];
		v2_texcoords[0] = data[6];
		v2_texcoords[1] = data[7];
	}
	break;
	case RL_V3_C4_N3:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = 1;
	v0_color[0]    = data[3];
	v0_color[1]    = data[4];
	v0_color[2]    = data[5];
	v0_color[3]    = data[6];	
	v0_normals[0]  = data[7];
	v0_normals[1]  = data[8];
	v0_normals[2]  = data[9];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = 1;
		v1_color[0]    = data[3];
		v1_color[1]    = data[4];
		v1_color[2]    = data[5];
		v1_color[3]    = data[6];
		v1_normals[0]  = data[7];			
		v1_normals[1]  = data[8];
		v1_normals[2]  = data[9];	
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = 1;
		v2_color[0]    = data[3];
		v2_color[1]    = data[4];
		v2_color[2]    = data[5];
		v2_color[3]    = data[6];
		v2_normals[0]  = data[7];
		v2_normals[1]  = data[8];	
		v2_normals[2]  = data[9];
	}
	break;
	case RL_V3_C4_T2:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = 1;
	v0_color[0] = data[3];
	v0_color[1] = data[4];
	v0_color[2] = data[5];
	v0_color[3] = data[6];
	v0_texcoords[0] = data[7];
	v0_texcoords[1] = data[8];
	if(vertex_read_count > 1)
	{
		data += width;	
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = 1;
		v1_color[0] = data[3];
		v1_color[1] = data[4];
		v1_color[2] = data[5];
		v1_color[3] = data[6];
		v1_texcoords[0] = data[7];
		v1_texcoords[1] = data[8];
	}		
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = 1;
		v2_color[0] = data[3];
		v2_color[1] = data[4];
		v2_color[2] = data[5];
		v2_color[3] = data[6];
		v2_texcoords[0] = data[7];
		v2_texcoords[1] = data[8];
	}
	break;
	case RL_V3_C4_N3_T2:
	v0_position[0]  = data[0];
	v0_position[1]  = data[1];
	v0_position[2]  = data[2];
	v0_position[3]  = 1;
	v0_color[0]     = data[3];
	v0_color[1]     = data[4];
	v0_color[2]     = data[5];
	v0_color[3]     = data[6];
	v0_normals[0]   = data[7];
	v0_normals[1]   = data[8];
	v0_normals[2]   = data[9];
	v0_texcoords[0] = data[10];
	v0_texcoords[1] = data[11];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0]  = data[0];
		v1_position[1]  = data[1];
		v1_position[2]  = data[2];
		v1_position[3]  = 1;
		v1_color[0]     = data[3];
		v1_color[1]     = data[4];
		v1_color[2]     = data[5];
		v1_color[3]     = data[6];
		v1_normals[0]   = data[7];
		v1_normals[1]   = data[8];
		v1_normals[2]   = data[9];
		v1_texcoords[0] = data[10];
		v1_texcoords[1] = data[11];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0]  = data[0];
		v2_position[1]  = data[1];
		v2_position[2]  = data[2];
		v2_position[3]  = 1;
		v2_color[0]     = data[3];
		v2_color[1]     = data[4];
		v2_color[2]     = data[5];
		v2_color[3]     = data[6];
		v2_normals[0]   = data[7];
		v2_normals[1]   = data[8];
		v2_normals[2]   = data[9];
		v2_texcoords[0] = data[10];
		v2_texcoords[1] = data[11];
	}
	break;
	case RL_V4:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = data[3];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = data[3];
	}
	if(vertex_read_count > 2)
	{
		data += width;	
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = data[3];
	}
	break;
	case RL_V4_C4:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = data[3];
	v0_color[0]    = data[4];
	v0_color[1]    = data[5];
	v0_color[2]    = data[6];
	v0_color[3]    = data[7];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = data[3];
		v1_color[0]    = data[4];
		v1_color[1]    = data[5];
		v1_color[2]    = data[6];
		v1_color[3]    = data[7];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = data[3];
		v2_color[0]    = data[4];
		v2_color[1]    = data[5];
		v2_color[2]    = data[6];
		v2_color[3]    = data[7];
	}
	break;
	case RL_V4_N3:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = data[3];
	v0_normals[0]  = data[4];
	v0_normals[1]  = data[5];
	v0_normals[2]  = data[6];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = data[3];
		v1_normals[0]  = data[4];
		v1_normals[1]  = data[5];
		v1_normals[2]  = data[6];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = data[3];
		v2_normals[0]  = data[4];
		v2_normals[1]  = data[5];
		v2_normals[2]  = data[6];
	}
	break;
	case RL_V4_T2:
	v0_position[0]  = data[0];
	v0_position[1]  = data[1];
	v0_position[2]  = data[2];
	v0_position[3]  = data[3];
	v0_texcoords[0] = data[4];
	v0_texcoords[1] = data[5];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0]  = data[0];
		v1_position[1]  = data[1];
		v1_position[2]  = data[2];
		v1_position[3]  = data[3];
		v1_texcoords[0] = data[4];
		v1_texcoords[1] = data[5];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0]  = data[0];
		v2_position[1]  = data[1];
		v2_position[2]  = data[2];
		v2_position[3]  = data[3];
		v2_texcoords[0] = data[4];
		v2_texcoords[1] = data[5];
	}
	break;
	case RL_V4_N3_T2:
	v0_position[0]  = data[0];
	v0_position[1]  = data[1];
	v0_position[2]  = data[2];
	v0_position[3]  = data[3];
	v0_normals[0]   = data[4];
	v0_normals[1]   = data[5];
	v0_normals[2]   = data[6];
	v0_texcoords[0] = data[7];
	v0_texcoords[1] = data[8];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0]  = data[0];
		v1_position[1]  = data[1];
		v1_position[2]  = data[2];
		v1_position[3]  = data[3];
		v1_normals[0]   = data[4];
		v1_normals[1]   = data[5];
		v1_normals[2]   = data[6];
		v1_texcoords[0] = data[7];
		v1_texcoords[1] = data[8];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0]  = data[0];
		v2_position[1]  = data[1];
		v2_position[2]  = data[2];
		v2_position[3]  = data[3];
		v2_normals[0]   = data[4];
		v2_normals[1]   = data[5];
		v2_normals[2]   = data[6];
		v2_texcoords[0] = data[7];
		v2_texcoords[1] = data[8];
	}
	break;
	case RL_V4_C4_N3:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = data[3];
	v0_color[0]    = data[4];
	v0_color[1]    = data[5];
	v0_color[2]    = data[6];
	v0_color[3]    = data[7];	
	v0_normals[0]  = data[8];
	v0_normals[1]  = data[9];
	v0_normals[2]  = data[10];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = data[3];
		v1_color[0]    = data[4];
		v1_color[1]    = data[5];
		v1_color[2]    = data[6];
		v1_color[3]    = data[7];
		v1_normals[0]  = data[8];			
		v1_normals[1]  = data[9];
		v1_normals[2]  = data[10];	
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = data[3];
		v2_color[0]    = data[4];
		v2_color[1]    = data[5];
		v2_color[2]    = data[6];
		v2_color[3]    = data[7];
		v2_normals[0]  = data[8];
		v2_normals[1]  = data[9];	
		v2_normals[2]  = data[10];
	}
	break;
	case RL_V4_C4_T2:
	v0_position[0] = data[0];
	v0_position[1] = data[1];
	v0_position[2] = data[2];
	v0_position[3] = data[3];
	v0_color[0] = data[4];
	v0_color[1] = data[5];
	v0_color[2] = data[6];
	v0_color[3] = data[7];
	v0_texcoords[0] = data[8];
	v0_texcoords[1] = data[9];
	if(vertex_read_count > 1)
	{
		data += width;	
		v1_position[0] = data[0];
		v1_position[1] = data[1];
		v1_position[2] = data[2];
		v1_position[3] = data[3];
		v1_color[0] = data[4];
		v1_color[1] = data[5];
		v1_color[2] = data[6];
		v1_color[3] = data[7];
		v1_texcoords[0] = data[8];
		v1_texcoords[1] = data[9];
	}		
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0] = data[0];
		v2_position[1] = data[1];
		v2_position[2] = data[2];
		v2_position[3] = data[3];
		v2_color[0] = data[4];
		v2_color[1] = data[5];
		v2_color[2] = data[6];
		v2_color[3] = data[7];
		v2_texcoords[0] = data[8];
		v2_texcoords[1] = data[9];
	}
	break;
	case RL_V4_C4_N3_T2:
	v0_position[0]  = data[0];
	v0_position[1]  = data[1];
	v0_position[2]  = data[2];
	v0_position[3]  = data[3];
	v0_color[0]     = data[4];
	v0_color[1]     = data[5];
	v0_color[2]     = data[6];
	v0_color[3]     = data[7];
	v0_normals[0]   = data[8];
	v0_normals[1]   = data[9];
	v0_normals[2]   = data[10];
	v0_texcoords[0] = data[11];
	v0_texcoords[1] = data[12];
	if(vertex_read_count > 1)
	{
		data += width;
		v1_position[0]  = data[0];
		v1_position[1]  = data[1];
		v1_position[2]  = data[2];
		v1_position[3]  = data[3];
		v1_color[0]     = data[4];
		v1_color[1]     = data[5];
		v1_color[2]     = data[6];
		v1_color[3]     = data[7];
		v1_normals[0]   = data[8];
		v1_normals[1]   = data[9];
		v1_normals[2]   = data[10];
		v1_texcoords[0] = data[11];
		v1_texcoords[1] = data[12];
	}
	if(vertex_read_count > 2)
	{
		data += width;
		v2_position[0]  = data[0];
		v2_position[1]  = data[1];
		v2_position[2]  = data[2];
		v2_position[3]  = data[3];
		v2_color[0]     = data[4];
		v2_color[1]     = data[5];
		v2_color[2]     = data[6];
		v2_color[3]     = data[7];
		v2_normals[0]   = data[8];
		v2_normals[1]   = data[9];
		v2_normals[2]   = data[10];
		v2_texcoords[0] = data[11];
		v2_texcoords[1] = data[12];
	}
	break;
	}
}
	
// samples and normalizes a texel from a texture.
// width is the width of the texture. (x,y) relative to top left.
// not to be used directly
void _get_texel(uint32_t x, uint32_t y, rlVec4* col,
	void* texture, uint32_t format, uint32_t width, bool compressed)
{
	if(!_rlcore)
		return;
	
	if(!compressed)
	{
		uint8_t texel_width = 1;
		if(format == RL_RGB16 || format == RL_RGB32)
			texel_width = 3;
		if(format == RL_RGBA16 || format == RL_RGBA32)
			texel_width = 4;
		uint8_t* tex8 = (uint8_t*) texture;
		if(format == RL_RGB16 || format == RL_RGBA16)
		{
			uint8_t* texel = &tex8[(y*width+x)*texel_width];
			col->x = (float)( *(texel) ) * _rlcore->_inv_31;
			col->y = (float)( *(texel+1) ) * _rlcore->_inv_31;
			col->z = (float)( *(texel+2) ) * _rlcore->_inv_31;
			col->w = 1.0f;
			if(format == RL_RGBA16)
				col->w = *(texel + 3) ? 1 : 0;
		}
		if(format == RL_RGB32 || format == RL_RGBA32)
		{
			uint8_t* texel = &tex8[(y*width+x)*texel_width];
			col->x = (float)( *(texel) ) * _rlcore->_inv_255;
			col->y = (float)( *(texel+1) ) * _rlcore->_inv_255;
			col->z = (float)( *(texel+2) ) * _rlcore->_inv_255;
			col->w = 1.0f;
			if(format == RL_RGBA32)
				col->w = (float)( *(texel+3) ) * _rlcore->_inv_255;
		}
	}
	else
	{
		if(format == RL_RGB16 || format == RL_RGBA16)
		{
			uint16_t* tex16 = (uint16_t*) texture;
			uint16_t texel = tex16[y*width+x];
			col->x = (float)( _RL_RGBA16_R(texel) ) * _rlcore->_inv_31;
			col->y = (float)( _RL_RGBA16_G(texel) ) * _rlcore->_inv_31;
			col->z = (float)( _RL_RGBA16_B(texel) ) * _rlcore->_inv_31;
			col->w = 1.0f;
			if(format == RL_RGBA16)
				col->w = _RL_RGBA16_A(texel);
		}
		if(format == RL_RGB32 || format == RL_RGBA32)
		{
			uint32_t* tex32 = (uint32_t*) texture;
			uint32_t texel = tex32[y*width+x];
			col->x = (float)( _RL_RGBA32_R(texel) ) * _rlcore->_inv_255;
			col->y = (float)( _RL_RGBA32_G(texel) ) * _rlcore->_inv_255;
			col->z = (float)( _RL_RGBA32_B(texel) ) * _rlcore->_inv_255;
			col->w = 1.0f;
			if(format == RL_RGBA32)
				col->w = (float)( _RL_RGBA32_A(texel) ) * _rlcore->_inv_255;
		}
	}

	if(col->x > 1.0f) col->x = 1.0f;
	if(col->x < 0.0f) col->x = 0.0f;
	if(col->y > 1.0f) col->y = 1.0f;
	if(col->y < 0.0f) col->y = 0.0f;
	if(col->z > 1.0f) col->z = 1.0f;
	if(col->z < 0.0f) col->z = 0.0f;
	if(col->w > 1.0f) col->w = 1.0f;
	if(col->w < 0.0f) col->w = 0.0f;
}
	
// plot a pixel to the color buffer given a pixel index.
// allows (normalized) color plotting and blending.
// color buffer is assumed to exist.
// not to be used directly
void _plot_pixel(uint32_t pixel_index, rlVec4 rgba, bool blend)	
{
	if(!_rlcore)
		return;

	uint8_t r, g, b, a;
	if(_rlcore->_cb_type == RL_RGB16 || _rlcore->_cb_type == RL_RGBA16)
	{
		uint16_t* cb = (uint16_t*) _rlcore->_colorbuffer;
			
		r = rgba.x * 31.0f;
		g = rgba.y * 31.0f;
		b = rgba.z * 31.0f;
		a = 1;
		
		if(!blend)
			cb[pixel_index] = _RL_RGBA16(r,g,b,a);
		else
		{
			a = (rgba.w) ? 1 : 0;
			if(a)
				cb[pixel_index] = _RL_RGBA16(r,g,b,a);
		}
	}
	if(_rlcore->_cb_type == RL_RGB32 || _rlcore->_cb_type == RL_RGBA32)
	{
		uint32_t* cb = (uint32_t*) _rlcore->_colorbuffer;
		
		r = rgba.x * 255.0f;
		g = rgba.y * 255.0f;
		b = rgba.z * 255.0f;
		if(!blend)
			cb[pixel_index] = _RL_RGBA32(r,g,b,a);
		else
		{
			a = rgba.w * 255.0f;
			
			uint32_t* dst = &cb[pixel_index];
			uint8_t dst_r, dst_g, dst_b;
			dst_r = _RL_RGBA32_R(*dst);
			dst_g = _RL_RGBA32_G(*dst);
			dst_b = _RL_RGBA32_B(*dst);
			if(rgba.w < 1.0f)
			{
				float alpha = 1.0f - rgba.w;
				uint8_t final_r = (r*rgba.w) + (dst_r * alpha);
				uint8_t final_g = (g*rgba.w) + (dst_g * alpha);
				uint8_t final_b = (b*rgba.w) + (dst_b * alpha);
				*dst = _RL_RGBA32(final_r,final_g,final_b,a);
			}
			else
				*dst = _RL_RGBA32(r,g,b,a);
		}
	}
}

// a pointer version of _plot_pixel
// p points to beginning of pixel in color buffer
// not to be used directly
void _plot_pixel_ptr(void* p, rlVec4 rgba, bool blend)	
{
	if(!_rlcore)
		return;

	uint8_t r, g, b, a;
	if(_rlcore->_cb_type == RL_RGB16 || _rlcore->_cb_type == RL_RGBA16)
	{
		uint16_t* p16 = (uint16_t*) p;
		
		r = rgba.x * 31.0f;
		g = rgba.y * 31.0f;
		b = rgba.z * 31.0f;
		a = (rgba.w) ? 1 : 0;
		if(!blend)
			*p16 = _RL_RGBA16(r,g,b,a);
		else
		{
			if(a)
				*p16 = _RL_RGBA16(r,g,b,a);
		}
	}
	if(_rlcore->_cb_type == RL_RGB32 || _rlcore->_cb_type == RL_RGBA32)
	{
		uint32_t* p32 = (uint32_t*) p;
		
		r = rgba.x * 255.0f;
		g = rgba.y * 255.0f;
		b = rgba.z * 255.0f;
		a = rgba.w * 255.0f;
		if(!blend)
			*p32 = _RL_RGBA32(r,g,b,a);
		else
		{
			uint32_t dst = *p32;
			uint8_t dst_r, dst_g, dst_b;
			dst_r = _RL_RGBA32_R(dst);
			dst_g = _RL_RGBA32_G(dst);
			dst_b = _RL_RGBA32_B(dst);
			if(rgba.w < 1.0f)
			{
				float alpha = 1.0f - rgba.w;
				uint8_t final_r = (r*rgba.w) + (dst_r * alpha);
				uint8_t final_g = (g*rgba.w) + (dst_g * alpha);
				uint8_t final_b = (b*rgba.w) + (dst_b * alpha);
				*p32 = _RL_RGBA32(final_r,final_g,final_b,255);
			}
			else
				*p32 = _RL_RGBA32(r,g,b,a);
		}
	}
}
		
// pass a vertex through the vertex shader, if bound
// not to be used directly
rlVec4 _vertex_pass(uint32_t ptype, rlVec4 position, rlVec4 color, rlVec3 normals, rlVec2 texcoords)
{
	if(!_rlcore)
	{
		rlVec4 nil;
		nil.x = 0;
		nil.y = 0;
		nil.z = 0;
		nil.w = 0;
		return nil;
	}

	rlVec4 out;

	if(!_rlcore->_vshader)
	{
		return position;
	}
	else
	{
		void* data = NULL;
		uint32_t enabled_attribs = 0;
		uint32_t size = 0;
		
		if(_rlcore->_sh_primitive_type)		{ enabled_attribs += 1; size += sizeof(uint32_t); }
		if(_rlcore->_sh_vertex_array)		{ enabled_attribs += 1; size += sizeof(rlVec4); }
		if(_rlcore->_sh_color_array)		{ enabled_attribs += 1; size += sizeof(rlVec4); } 
		if(_rlcore->_sh_normal_array)		{ enabled_attribs += 1; size += sizeof(rlVec3); }
		if(_rlcore->_sh_texcoord_array)		{ enabled_attribs += 1; size += sizeof(rlVec2); }
		
		data = malloc(size);
		
		/* ALLOCATE AND FILL DATA WITH ENABLED ATTRIBUTE ARRAYS */
		uint32_t format[enabled_attribs];	// list of attributes in array
		
		uint32_t i = 0;
		uint32_t offset = 0;
		if(_rlcore->_sh_primitive_type)		{ format[i] = RL_PRIMITIVE_TYPE; i += 1; 
			*((uint32_t*)(data + offset)) = ptype; offset += sizeof(uint32_t); }
		if(_rlcore->_sh_vertex_array)		{ format[i] = RL_VERTEX_ARRAY; i += 1;
			*((rlVec4*)(data + offset)) = position; offset += sizeof(rlVec4); }
		if(_rlcore->_sh_color_array)			{ format[i] = RL_COLOR_ARRAY; i += 1;
			*((rlVec4*)(data + offset)) = color; offset += sizeof(rlVec4); }
		if(_rlcore->_sh_normal_array)		{ format[i] = RL_NORMAL_ARRAY; i += 1;
			*((rlVec3*)(data + offset)) = normals; offset += sizeof(rlVec3); }
		if(_rlcore->_sh_texcoord_array)		{ format[i] = RL_TEXCOORD_ARRAY; i += 1;
			*((rlVec2*)(data + offset)) = texcoords; offset += sizeof(rlVec2); }

		if(enabled_attribs)
			out = _rlcore->_vshader(data, format, enabled_attribs);
		else
			out = _rlcore->_vshader(NULL, NULL, 0);
			
		free(data);
	}
		
	return out;
}
	
// allocate data block required for fragment attribute passing
// we do this per-primitive so we don't have to allocate and deallocate it per-fragment
// not to be used directly
void* _alloc_fragment_data(uint32_t* enabled_attrib_count, uint32_t* data_size, uint32_t** data_attrib_format)	
{
	if(!_rlcore)
		return NULL;

	*enabled_attrib_count = 0;
	*data_size = 0;
		
	uint32_t i = 0;
		
	if(_rlcore->_sh_primitive_type)		{ *enabled_attrib_count += 1; *data_size += sizeof(uint32_t); }
	if(_rlcore->_sh_color_array)		{ *enabled_attrib_count += 1; *data_size += sizeof(rlVec4); }
	if(_rlcore->_sh_primary_color)		{ *enabled_attrib_count += 1; *data_size += sizeof(rlVec4); }
	if(_rlcore->_sh_secondary_color)	{ *enabled_attrib_count += 1; *data_size += sizeof(rlVec4); }
	if(_rlcore->_sh_bary_linear)		{ *enabled_attrib_count += 1; *data_size += sizeof(rlVec3); }
	if(_rlcore->_sh_bary_perspective)	{ *enabled_attrib_count += 1; *data_size += sizeof(rlVec3); }
	if(_rlcore->_sh_dst_depth)			{ *enabled_attrib_count += 1; *data_size += sizeof(float); }
	if(_rlcore->_sh_frag_depth)			{ *enabled_attrib_count += 1; *data_size += sizeof(float); }
	if(_rlcore->_sh_frag_x_coord)		{ *enabled_attrib_count += 1; *data_size += sizeof(int); }
	if(_rlcore->_sh_frag_y_coord)		{ *enabled_attrib_count += 1; *data_size += sizeof(int); }

	*data_attrib_format = (uint32_t*) calloc(*enabled_attrib_count, sizeof(uint32_t));
	if(_rlcore->_sh_primitive_type)		{ (*data_attrib_format) [i] = RL_PRIMITIVE_TYPE; i += 1; }
	if(_rlcore->_sh_color_array)		{ (*data_attrib_format) [i] = RL_COLOR_ARRAY; i += 1; }
	if(_rlcore->_sh_primary_color)		{ (*data_attrib_format) [i] = RL_PRIMARY_COLOR; i += 1; }
	if(_rlcore->_sh_secondary_color)	{ (*data_attrib_format) [i] = RL_SECONDARY_COLOR; i += 1; }
	if(_rlcore->_sh_bary_linear)		{ (*data_attrib_format) [i] = RL_BARY_LINEAR; i += 1; }
	if(_rlcore->_sh_bary_perspective)	{ (*data_attrib_format) [i] = RL_BARY_PERSPECTIVE; i += 1; }
	if(_rlcore->_sh_dst_depth)			{ (*data_attrib_format) [i] = RL_DST_DEPTH; i += 1; }
	if(_rlcore->_sh_frag_depth)			{ (*data_attrib_format) [i] = RL_FRAG_DEPTH; i += 1; }
	if(_rlcore->_sh_frag_x_coord)		{ (*data_attrib_format) [i] = RL_FRAG_X_COORD; i += 1; }
	if(_rlcore->_sh_frag_y_coord)		{ (*data_attrib_format) [i] = RL_FRAG_Y_COORD; i += 1; }
			
	if(*data_size)
		return malloc(*data_size);
	return NULL;
}
	
// pass a fragment through the fragment shader, if bound
// not to be used directly
rlVec4 _fragment_pass(void* data, uint32_t enabled_attrib_count, uint32_t data_size, uint32_t* data_attrib_format, 
	uint32_t ptype, rlVec4 primary, rlVec4 secondary, rlVec3 linear_bary, rlVec3 bary, rlVec4 current, float dst_depth,
	float depth, rlVec2i coord, bool* discard)
{
	rlVec4 out;
	
	if(!_rlcore)
		return current;

	if(!_rlcore->_fshader)
		return current;
	else
	{
		/* FILL DATA WITH ENABLED ATTRIBUTE ARRAYS */
		uint32_t i = 0;
		uint32_t offset = 0;
		if(_rlcore->_sh_primitive_type)		{ *((uint32_t*)(data + offset)) = ptype; offset += sizeof(uint32_t); }
		if(_rlcore->_sh_color_array)		{ *((rlVec4*)(data + offset)) = current; offset += sizeof(rlVec4); }
		if(_rlcore->_sh_primary_color)		{ *((rlVec4*)(data + offset)) = primary; offset += sizeof(rlVec4); }
		if(_rlcore->_sh_secondary_color)	{ *((rlVec4*)(data + offset)) = secondary; offset += sizeof(rlVec4); }
		if(_rlcore->_sh_bary_linear)		{ *((rlVec3*)(data + offset)) = linear_bary; offset += sizeof(rlVec3); }
		if(_rlcore->_sh_bary_perspective)	{ *((rlVec3*)(data + offset)) = bary; offset += sizeof(rlVec3); }
		if(_rlcore->_sh_dst_depth)			{ *((float*)(data + offset)) = dst_depth; offset += sizeof(float); }
		if(_rlcore->_sh_frag_depth)			{ *((float*)(data + offset)) = depth; offset += sizeof(float); }
		if(_rlcore->_sh_frag_x_coord)		{ *((int*)(data + offset)) = coord.x; offset += sizeof(int); }
		if(_rlcore->_sh_frag_y_coord)		{ *((int*)(data + offset)) = coord.y; offset += sizeof(int); }

		if(enabled_attrib_count)			
			out = _rlcore->_fshader(data, data_attrib_format, enabled_attrib_count, discard);
		else
			out = _rlcore->_fshader(NULL, NULL, 0, discard);
	}
		
	return out;
}
	
// rasterize a screen-space triangle
// brute force bounding-box method
// not to be used directly
//
// NOTE: outdated.
void _brute_raster(rlVec2 v0, rlVec2 v1, rlVec2 v2, rlVec4 v0_rgba, rlVec4 v1_rgba, rlVec4 v2_rgba,
	rlVec2ui v0_texel, rlVec2ui v1_texel, rlVec2ui v2_texel, int64_t v0_z, int64_t v1_z, int64_t v2_z)
{
	if(!_rlcore)
		return;

	v0_z += 1;
	v1_z += 1;
	v2_z += 1;

	bool plot_color = _rlcore->_colorbuffer;
	bool plot_depth = (_rlcore->_write_depth && _rlcore->_depthbuffer);
	bool can_raster = (_rlcore->_width + _rlcore->_height >= 2);
	bool depth_test = (_rlcore->_depth_test && _rlcore->_depthbuffer);
	
	int64_t db_range = 0;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16) db_range = 0xFFFF;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32) db_range = 0xFFFFFFFF;
	
	float inv_db_range = 0;
	if(_rlcore->_depthbuffer)
		inv_db_range = 1.0f / db_range;

	float inv_v0_z = _safedivf(1.0f, v0_z);
	float inv_v1_z = _safedivf(1.0f, v1_z);
	float inv_v2_z = _safedivf(1.0f, v2_z);
	
	/* USED TO LATER PREVENT PRECISION LOSS */
	int64_t max_z = _max64(v0_z, _max64(v1_z, v2_z));
	int64_t min_z = _min64(v0_z, _min64(v1_z, v2_z));
	float max_r = _maxf(v0_rgba.x, _maxf(v1_rgba.x, v2_rgba.x));
	float min_r = _minf(v0_rgba.x, _minf(v1_rgba.x, v2_rgba.x));
	float max_g = _maxf(v0_rgba.y, _maxf(v1_rgba.y, v2_rgba.y));
	float min_g = _minf(v0_rgba.y, _minf(v1_rgba.y, v2_rgba.y));
	float max_b = _maxf(v0_rgba.z, _maxf(v1_rgba.z, v2_rgba.z));
	float min_b = _minf(v0_rgba.z, _minf(v1_rgba.z, v2_rgba.z));
	float max_a = _maxf(v0_rgba.w, _maxf(v1_rgba.w, v2_rgba.w));
	float min_a = _minf(v0_rgba.w, _minf(v1_rgba.w, v2_rgba.w));
	uint32_t max_texel_x = _max_u32(v0_texel.x, _max_u32(v1_texel.x, v2_texel.x));
	uint32_t min_texel_x = _min_u32(v0_texel.x, _min_u32(v1_texel.x, v2_texel.x));
	uint32_t max_texel_y = _max_u32(v0_texel.y, _max_u32(v1_texel.y, v2_texel.y));
	uint32_t min_texel_y = _min_u32(v0_texel.y, _min_u32(v1_texel.y, v2_texel.y));
	
	rlVec2i a, b;
	a.x = v1.x - v0.x;
	a.y = v1.y - v0.y;
	b.x = v2.x - v0.x;
	b.y = v2.y - v0.y;
	float den = _safedivf(1.0f, (a.x * b.y - b.x * a.y));
	
	/* USED FOR FRAGMENT SHADER PASSES */
	void* attrib_data = 0;
	uint32_t* attrib_format = 0;
	uint32_t enabled_attrib_count = 0;
	uint32_t data_size = 0;
	attrib_data = _alloc_fragment_data(&enabled_attrib_count, &data_size, &attrib_format);
	
	uint32_t format = _rlcore->_texture_formats[_rlcore->_texture_unit];
	bool texture_unit_complete = _rlcore->_textures[_rlcore->_texture_unit]
		&& _rlcore->_texture_widths[_rlcore->_texture_unit] > 0
		&& _rlcore->_texture_heights[_rlcore->_texture_unit] > 0
		&& (format == RL_RGB16 ||
			format == RL_RGBA16 ||
			format == RL_RGB32 ||
			format == RL_RGBA32);
	
	int minx = _min(v0.x, _min(v1.x, v2.x));
	int maxx = _max(v0.x, _max(v1.x, v2.x));
	int miny = _min(v0.y, _min(v1.y, v2.y));
	int maxy = _max(v0.y, _max(v1.y, v2.y));
	uint32_t y_idx = (miny > 0) ? miny * _rlcore->_width : 0;
	if(can_raster)
	for(int y = miny; y < maxy; y += 1)
	{
		if(y < 0)			continue;
		if(y >= _rlcore->_height)	break;
	for(int x = minx; x < maxx; x += 1)
	{
		if(x < 0)		continue;
		if(x >= _rlcore->_width)	break;
		
		rlVec3 bary;
		rlVec2i c;
		c.x = x - v0.x;
		c.y = y - v0.y;
		bary.y = (c.x * b.y - b.x * c.y) * den;
		bary.z = (a.x * c.y - c.x * a.y) * den;
		bary.x = 1.0f - bary.y - bary.z;
		
		if(bary.x >= 0.0f && bary.y >= 0.0f && bary.z >= 0.0f)
		{
			rlVec3 linear_bary;
			linear_bary = bary;
			if(_rlcore->_persp_corr) 
			{
				float z = _safedivf(1.0f, (bary.x*inv_v0_z + bary.y*inv_v1_z + bary.z*inv_v2_z));
				bary.x *= inv_v0_z;
				bary.y *= inv_v1_z;
				bary.z *= inv_v2_z;
				bary.x *= z;
				bary.y *= z;
				bary.z *= z;
			}

			int64_t z = bary.x * v0_z + bary.y * v1_z + bary.z * v2_z;
			float r = bary.x * v0_rgba.x + bary.y * v1_rgba.x + bary.z * v2_rgba.x;
			float g = bary.x * v0_rgba.y + bary.y * v1_rgba.y + bary.z * v2_rgba.y;
			float b = bary.x * v0_rgba.z + bary.y * v1_rgba.z + bary.z * v2_rgba.z;
			float a = bary.x * v0_rgba.w + bary.y * v1_rgba.w + bary.z * v2_rgba.w;
			uint32_t texel_x = bary.x * v0_texel.x + bary.y * v1_texel.x + bary.z * v2_texel.x;
			uint32_t texel_y = bary.x * v0_texel.y + bary.y * v1_texel.y + bary.z * v2_texel.y;
			float dst_depth = 0;
			
			/* prevent precision loss */
			if(z > max_z)	z = max_z;
			if(z < min_z)	z = min_z;
			if(r > max_r)	r = max_r;
			if(r < min_r)	r = min_r;
			if(g > max_g)	g = max_g;
			if(g < min_g)	g = min_g;
			if(b > max_b)	b = max_b;
			if(b < min_b)	b = min_b;
			if(a > max_a)	a = max_a;
			if(a < min_a)	a = min_a;
			if(texel_x > max_texel_x)	texel_x = max_texel_x;
			if(texel_x < min_texel_x)	texel_x = min_texel_x;
			if(texel_y > max_texel_y)	texel_y = max_texel_y;
			if(texel_y < min_texel_y)	texel_y = min_texel_y;
			
			if(z < 0)
				continue;
			if(_rlcore->_depthbuffer && z > db_range)
				continue;
			
			uint32_t pixel_index = 0;
			pixel_index = y_idx + x;
			
			if(depth_test)
			{
				if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16)
				{
					if(z > ((uint16_t*)_rlcore->_depthbuffer) [pixel_index])
						continue;
					dst_depth = ((uint16_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
				}
				if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32)
				{
					if(z > ((uint32_t*)_rlcore->_depthbuffer) [pixel_index])
						continue;
					dst_depth = ((uint32_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
				}
			}
			
			if(plot_color)
			{
				rlVec4 primary;		// primary (primitive) color
				rlVec4 secondary;	// secondary (texture) color
				primary.x = r, secondary.x = 0.0f;
				primary.y = g, secondary.y = 0.0f;
				primary.z = b, secondary.z = 0.0f;
				primary.w = a, secondary.w = 0.0f;
				rlVec4 color = primary;	// color to draw with
				if(texture_unit_complete && _rlcore->_texture)
				{
					_get_texel(texel_x, texel_y, &secondary, _rlcore->_textures[_rlcore->_texture_unit], 
						_rlcore->_texture_formats[_rlcore->_texture_unit], 
						_rlcore->_texture_widths[_rlcore->_texture_unit],
						_rlcore->_texture_compressed_booleans[_rlcore->_texture_unit]);
					color = secondary;
				}
				
				rlVec2i coord;
				coord.x = x;
				coord.y = y;
				
				bool discard = false;
				color = _fragment_pass(attrib_data, enabled_attrib_count, data_size, attrib_format, 
					RL_TRIANGLE, primary, secondary, linear_bary, bary, color, dst_depth, 1, coord, &discard);
					
				if(discard)
					continue;

				if(color.x > 1.0f) color.x = 1.0f;
				if(color.x < 0.0f) color.x = 0.0f;
				if(color.y > 1.0f) color.y = 1.0f;
				if(color.y < 0.0f) color.y = 0.0f;
				if(color.z > 1.0f) color.z = 1.0f;
				if(color.z < 0.0f) color.z = 0.0f;
				if(color.w > 1.0f) color.w = 1.0f;
				if(color.w < 0.0f) color.w = 0.0f;
					
				_plot_pixel(pixel_index, color, _rlcore->_blend);
			}
			
			if(plot_depth)
			{
				if(_rlcore->_db_type == RL_D16)
				((uint16_t*)_rlcore->_depthbuffer) [pixel_index] = z;
				if(_rlcore->_db_type == RL_D32)
					((uint32_t*)_rlcore->_depthbuffer) [pixel_index] = z;
			}
		}
	}
		y_idx += _rlcore->_width;
	}
		
	/* these were used for fragment shader passes */
	free(attrib_data);
	free(attrib_format);
}

// a tile-based rasterizer with 4 bits of sub-pixel precision
// vertices must be counter-clockwise (culling & sorting automatically handled)
// v0_bary, v1_bary, and v2_bary used for sub-triangles
// not to be used directly
void _raster(rlVec2 v0, rlVec2 v1, rlVec2 v2, rlVec4 v0_rgba, rlVec4 v1_rgba, rlVec4 v2_rgba,
	rlVec2ui v0_texel, rlVec2ui v1_texel, rlVec2ui v2_texel, int64_t v0_z, int64_t v1_z, int64_t v2_z,
	float v0_w, float v1_w, float v2_w, rlVec3 v0_bary, rlVec3 v1_bary, rlVec3 v2_bary)
{
	if(!_rlcore)
		return;

	int x0 = 16.0f * v0.x + 0.5f;
	int x1 = 16.0f * v1.x + 0.5f;
	int x2 = 16.0f * v2.x + 0.5f;
	int y0 = 16.0f * v0.y + 0.5f;
	int y1 = 16.0f * v1.y + 0.5f;
	int y2 = 16.0f * v2.y + 0.5f;

	// determine winding
	rlVec3 n;
	bool cw = false;
	rlVec3 w_v0 = { v0.x, v0.y, 0 };
	rlVec3 w_v1 = { v1.x, v1.y, 0 };
	rlVec3 w_v2 = { v2.x, v2.y, 0 };
	n = _cross_vec3(_sub_vec3(w_v1, w_v0), _sub_vec3(w_v2, w_v0));
	if(n.z > 0)
		cw = true;
		
	if(cw && _rlcore->_cull && _rlcore->_cull_winding == RL_CW)
		return;
	if(!cw && _rlcore->_cull && _rlcore->_cull_winding == RL_CCW)
		return;
			
	if(cw)
	{
		int tmpx = x1;
		int tmpy = y1;
		x1 = x2;
		y1 = y2;
		x2 = tmpx;
		y2 = tmpy;
	}
		
	// deltas
	int dx01 = x0 - x1;
	int dx12 = x1 - x2;
	int dx20 = x2 - x0;
	int dy01 = y0 - y1;
	int dy12 = y1 - y2;
	int dy20 = y2 - y0;

	// fixed-point deltas
	int fdx01 = dx01 << 4;
	int fdx12 = dx12 << 4;
	int fdx20 = dx20 << 4;
	int fdy01 = dy01 << 4;
	int fdy12 = dy12 << 4;
	int fdy20 = dy20 << 4;
		
	v0_z += 1;
	v1_z += 1;
	v2_z += 1;

	bool plot_color = _rlcore->_colorbuffer;
	bool plot_depth = (_rlcore->_write_depth && _rlcore->_depthbuffer);
	bool can_raster = (_rlcore->_width + _rlcore->_height >= 2);
	bool depth_test = (_rlcore->_depth_test && _rlcore->_depthbuffer);
	
	int64_t db_range = 0;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16) db_range = 0xFFFF;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32) db_range = 0xFFFFFFFF;
	
	float inv_db_range = 0;
	if(db_range)
		inv_db_range = 1.0f / db_range;

	float inv_v0_w = _safedivf(1.0f, v0_w);
	float inv_v1_w = _safedivf(1.0f, v1_w);
	float inv_v2_w = _safedivf(1.0f, v2_w);
	
	/* USED TO LATER PREVENT PRECISION LOSS */
	int64_t max_z = _max64(v0_z, _max64(v1_z, v2_z));
	int64_t min_z = _min64(v0_z, _min64(v1_z, v2_z));
	float max_r = _maxf(v0_rgba.x, _maxf(v1_rgba.x, v2_rgba.x));
	float min_r = _minf(v0_rgba.x, _minf(v1_rgba.x, v2_rgba.x));
	float max_g = _maxf(v0_rgba.y, _maxf(v1_rgba.y, v2_rgba.y));
	float min_g = _minf(v0_rgba.y, _minf(v1_rgba.y, v2_rgba.y));
	float max_b = _maxf(v0_rgba.z, _maxf(v1_rgba.z, v2_rgba.z));
	float min_b = _minf(v0_rgba.z, _minf(v1_rgba.z, v2_rgba.z));
	float max_a = _maxf(v0_rgba.w, _maxf(v1_rgba.w, v2_rgba.w));
	float min_a = _minf(v0_rgba.w, _minf(v1_rgba.w, v2_rgba.w));
	uint32_t max_texel_x = _max_u32(v0_texel.x, _max_u32(v1_texel.x, v2_texel.x));
	uint32_t min_texel_x = _min_u32(v0_texel.x, _min_u32(v1_texel.x, v2_texel.x));
	uint32_t max_texel_y = _max_u32(v0_texel.y, _max_u32(v1_texel.y, v2_texel.y));
	uint32_t min_texel_y = _min_u32(v0_texel.y, _min_u32(v1_texel.y, v2_texel.y));
	
	rlVec2 a, b;
	a.x = v1.x - v0.x;
	a.y = v1.y - v0.y;
	b.x = v2.x - v0.x;
	b.y = v2.y - v0.y;
	float den = _safedivf(1.0f, (a.x * b.y - b.x * a.y));
	
	/* USED FOR FRAGMENT SHADER PASSES */
	void* attrib_data = 0;
	uint32_t* attrib_format = 0;
	uint32_t enabled_attrib_count = 0;
	uint32_t data_size = 0;
	attrib_data = _alloc_fragment_data(&enabled_attrib_count, &data_size, &attrib_format);
	
	uint32_t format = _rlcore->_texture_formats[_rlcore->_texture_unit];
	bool texture_unit_complete = _rlcore->_textures[_rlcore->_texture_unit]
		&& _rlcore->_texture_widths[_rlcore->_texture_unit] > 0
		&& _rlcore->_texture_heights[_rlcore->_texture_unit] > 0
		&& (format == RL_RGB16 ||
			format == RL_RGBA16 ||
			format == RL_RGB32 ||
			format == RL_RGBA32);

	// calculate bounding box
	int minx = _min(v0.x+0.5f, _min(v1.x+0.5f, v2.x+0.5f));
	int maxx = _max(v0.x+0.5f, _max(v1.x+0.5f, v2.x+0.5f));
	int miny = _min(v0.y+0.5f, _min(v1.y+0.5f, v2.y+0.5f));
	int maxy = _max(v0.y+0.5f, _max(v1.y+0.5f, v2.y+0.5f));

	// clip
	minx = _max(minx, 0);
	miny = _max(miny, 0);
	maxx = _min(maxx, _rlcore->_width-1);
	maxy = _min(maxy, _rlcore->_width-1);
	if(minx >= _rlcore->_width || miny >= _rlcore->_height || maxx < 0 || maxy < 0 || minx == maxx || miny == maxy)
		return;

	// block size (must be power of 2)
	int q = 8;

	// round to tile grid
	minx &= ~(q-1);
	miny &= ~(q-1);


	// half-edge constants
	int c1 = dy01 * x0 - dx01 * y0;
	int c2 = dy12 * x1 - dx12 * y1;
	int c3 = dy20 * x2 - dx20 * y2;

	// correct for fill conventions
	if(dy01 < 0 || (dy01 == 0 && dx01 > 0))
		c1 += 1;
	if(dy12 < 0 || (dy12 == 0 && dx12 > 0))
		c2 += 1;
	if(dy20 < 0 || (dy20 == 0 && dx20 > 0))
		c3 += 1;

	if(can_raster)
	for(int ty = miny; ty < maxy; ty += q) 
	{
		for(int tx = minx; tx < maxx; tx += q)
		{
			// corners
			int tx0 = tx << 4;
			int tx1 = (tx + q - 1) << 4;
			int ty0 = ty << 4;
			int ty1 = (ty + q - 1) << 4;

			// evaluate half-space functions
			bool a00, a10, a01, a11;
			bool b00, b10, b01, b11;
			bool c00, c10, c01, c11;

			a00 = c1 + dx01 * ty0 - dy01 * tx0 > 0;
			a10 = c1 + dx01 * ty0 - dy01 * tx1 > 0;
			a01 = c1 + dx01 * ty1 - dy01 * tx0 > 0;
			a11 = c1 + dx01 * ty1 - dy01 * tx1 > 0;
			b00 = c2 + dx12 * ty0 - dy12 * tx0 > 0;
			b10 = c2 + dx12 * ty0 - dy12 * tx1 > 0;
			b01 = c2 + dx12 * ty1 - dy12 * tx0 > 0;
			b11 = c2 + dx12 * ty1 - dy12 * tx1 > 0;
			c00 = c3 + dx20 * ty0 - dy20 * tx0 > 0;
			c10 = c3 + dx20 * ty0 - dy20 * tx1 > 0;
			c01 = c3 + dx20 * ty1 - dy20 * tx0 > 0;
			c11 = c3 + dx20 * ty1 - dy20 * tx1 > 0;
			int edge_a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);
			int edge_b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);
			int edge_c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

			// skip block when outside edge
			if(edge_a == 0x0 || edge_b == 0x0 || edge_c == 0x0)
				continue;

			// entire block covered
			if(edge_a == 0xF && edge_b == 0xF && edge_c == 0xF)
			{
				uint32_t y_idx = ty * _rlcore->_width;
					
				for(int y = ty; y < ty+q; y += 1)
				{
					if(y >= _rlcore->_height)
						break;
					if(y < 0)
					{
						y_idx += _rlcore->_width;
						continue;
					}
				for(int x = tx; x < tx+q; x += 1)
				{
					if(x >= _rlcore->_width)
						break;
					if(x < 0)
						continue;
					rlVec3 bary;
					rlVec2 c;
					c.x = x - v0.x;
					c.y = y - v0.y;
					bary.y = (c.x * b.y - b.x * c.y) * den;
					bary.z = (a.x * c.y - c.x * a.y) * den;
					bary.x = 1.0f - bary.y - bary.z;
					{
						// apply barycentric offsets
						float bx = bary.x;
						float by = bary.y;
						float bz = bary.z;
						bary.x = bx * v0_bary.x + by * v1_bary.x + bz * v2_bary.x;
						bary.y = bx * v0_bary.y + by * v1_bary.y + bz * v2_bary.y;
						bary.z = bx * v0_bary.z + by * v1_bary.z + bz * v2_bary.z;
					}
					rlVec3 linear_bary;
					linear_bary = bary;
					if(_rlcore->_persp_corr) 
					{
						float w = _safedivf(1.0f, (bary.x*inv_v0_w + bary.y*inv_v1_w + bary.z*inv_v2_w));
						bary.x *= inv_v0_w;
						bary.y *= inv_v1_w;
						bary.z *= inv_v2_w;
						bary.x *= w;
						bary.y *= w;
						bary.z *= w;
					}

					uint32_t pixel_index = 0;
					pixel_index = y_idx + x;

					float dst_depth = 0;
					float depth = 0;
					int64_t z = bary.x * v0_z + bary.y * v1_z + bary.z * v2_z;

					if(z < 0)
						continue;
					if(_rlcore->_depthbuffer && z > db_range)
						continue;

					if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16)
					{
						if(depth_test && z > ((uint16_t*)_rlcore->_depthbuffer) [pixel_index])
							continue;
						dst_depth = ((uint16_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
						depth = z * inv_db_range;
					}
					if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32)
					{
						if(depth_test && z > ((uint32_t*)_rlcore->_depthbuffer) [pixel_index])
							continue;
						dst_depth = ((uint32_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
						depth = z * inv_db_range;
					}					

					float r = bary.x * v0_rgba.x + bary.y * v1_rgba.x + bary.z * v2_rgba.x;
					float g = bary.x * v0_rgba.y + bary.y * v1_rgba.y + bary.z * v2_rgba.y;
					float b = bary.x * v0_rgba.z + bary.y * v1_rgba.z + bary.z * v2_rgba.z;
					float a = bary.x * v0_rgba.w + bary.y * v1_rgba.w + bary.z * v2_rgba.w;
					uint32_t texel_x = bary.x * v0_texel.x + bary.y * v1_texel.x + bary.z * v2_texel.x;
					uint32_t texel_y = bary.x * v0_texel.y + bary.y * v1_texel.y + bary.z * v2_texel.y;
					
					/* prevent precision loss */
					if(z > max_z)	z = max_z;
					if(z < min_z)	z = min_z;
					if(r > max_r)	r = max_r;
					if(r < min_r)	r = min_r;
					if(g > max_g)	g = max_g;
					if(g < min_g)	g = min_g;
					if(b > max_b)	b = max_b;
					if(b < min_b)	b = min_b;
					if(a > max_a)	a = max_a;
					if(a < min_a)	a = min_a;
					if(texel_x > max_texel_x)	texel_x = max_texel_x;
					if(texel_x < min_texel_x)	texel_x = min_texel_x;
					if(texel_y > max_texel_y)	texel_y = max_texel_y;
					if(texel_y < min_texel_y)	texel_y = min_texel_y;
					
					if(plot_color)
					{
						rlVec4 primary;		// primary (primitive) color
						rlVec4 secondary;	// secondary (texture) color
						primary.x = r, secondary.x = 0.0f;
						primary.y = g, secondary.y = 0.0f;
						primary.z = b, secondary.z = 0.0f;
						primary.w = a, secondary.w = 0.0f;
						rlVec4 color = primary;	// color to draw with
						if(texture_unit_complete && _rlcore->_texture)
						{
							_get_texel(texel_x, texel_y, &secondary, _rlcore->_textures[_rlcore->_texture_unit], 
								_rlcore->_texture_formats[_rlcore->_texture_unit], 
								_rlcore->_texture_widths[_rlcore->_texture_unit],
								_rlcore->_texture_compressed_booleans[_rlcore->_texture_unit]);
							color = secondary;
						}
						
						rlVec2i coord;
						coord.x = x;
						coord.y = y;
						
						bool discard = false;
						color = _fragment_pass(attrib_data, enabled_attrib_count, data_size, attrib_format, 
							RL_TRIANGLE, primary, secondary, linear_bary, bary, color, dst_depth, depth, coord, &discard);
								
						if(discard)
							continue;

						if(color.x > 1.0f) color.x = 1.0f;
						if(color.x < 0.0f) color.x = 0.0f;
						if(color.y > 1.0f) color.y = 1.0f;
						if(color.y < 0.0f) color.y = 0.0f;
						if(color.z > 1.0f) color.z = 1.0f;
						if(color.z < 0.0f) color.z = 0.0f;
						if(color.w > 1.0f) color.w = 1.0f;
						if(color.w < 0.0f) color.w = 0.0f;
						
						_plot_pixel(pixel_index, color, _rlcore->_blend);
					}
					
					if(plot_depth)
					{
						if(_rlcore->_db_type == RL_D16)
							((uint16_t*)_rlcore->_depthbuffer) [pixel_index] = z;
						if(_rlcore->_db_type == RL_D32)
							((uint32_t*)_rlcore->_depthbuffer) [pixel_index] = z;
					}
				}	// cycle x in tile
					y_idx += _rlcore->_width;
				}	// cycle y in tile
			}
			else
			{
				int cy1 = c1 + dx01 * ty0 - dy01 * tx0;
				int cy2 = c2 + dx12 * ty0 - dy12 * tx0;
				int cy3 = c3 + dx20 * ty0 - dy20 * tx0;

				uint32_t y_idx = ty * _rlcore->_width;
				for(int y = ty; y < ty+q; y += 1)
				{
					if(y >= _rlcore->_height)
						break;
					if(y < 0)
					{
						cy1 += fdx01;
						cy2 += fdx12;
						cy3 += fdx20;
						y_idx += _rlcore->_width;
						continue;
					}
					
					int cx1 = cy1;
					int cx2 = cy2;
					int cx3 = cy3;
					for(int x = tx; x < tx+q; x += 1)
					{
						if(x < 0)
						{
							cx1 -= fdy01;
							cx2 -= fdy12;
							cx3 -= fdy20;
							continue;
						}
						if(x >= _rlcore->_width)
							break;
						if(cx1 > 0 && cx2 > 0 && cx3 > 0)
						{
							rlVec3 bary;
							rlVec2 c;
							c.x = x - v0.x;
							c.y = y - v0.y;
							bary.y = (c.x * b.y - b.x * c.y) * den;
							bary.z = (a.x * c.y - c.x * a.y) * den;
							bary.x = 1.0f - bary.y - bary.z;
							{
								// apply barycentric offsets
								float bx = bary.x;
								float by = bary.y;
								float bz = bary.z;
								bary.x = bx * v0_bary.x + by * v1_bary.x + bz * v2_bary.x;
								bary.y = bx * v0_bary.y + by * v1_bary.y + bz * v2_bary.y;
								bary.z = bx * v0_bary.z + by * v1_bary.z + bz * v2_bary.z;
							}

							if(bary.x >= 0.0 && bary.y >= 0.0 && bary.z >= 0.0) {
							rlVec3 linear_bary;
							linear_bary = bary;
							if(_rlcore->_persp_corr) 
							{
								float w = _safedivf(1.0f, (bary.x*inv_v0_w + bary.y*inv_v1_w + bary.z*inv_v2_w));
								bary.x *= inv_v0_w;
								bary.y *= inv_v1_w;
								bary.z *= inv_v2_w;
								bary.x *= w;
								bary.y *= w;
								bary.z *= w;
							}

							uint32_t pixel_index = 0;
							pixel_index = y_idx + x;

							float dst_depth = 0;
							float depth = 0;
							int64_t z = bary.x * v0_z + bary.y * v1_z + bary.z * v2_z;
	
							if(z < 0)
								continue;
							if(_rlcore->_depthbuffer && z > db_range)
								continue;

							if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16)
							{
								if(depth_test && z > ((uint16_t*)_rlcore->_depthbuffer) [pixel_index])
									continue;
								dst_depth = ((uint16_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
								depth = z * inv_db_range;
							}
							if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32)
							{
								if(depth_test && z > ((uint32_t*)_rlcore->_depthbuffer) [pixel_index])
									continue;
								dst_depth = ((uint32_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
								depth = z * inv_db_range;
							}

							float r = bary.x * v0_rgba.x + bary.y * v1_rgba.x + bary.z * v2_rgba.x;
							float g = bary.x * v0_rgba.y + bary.y * v1_rgba.y + bary.z * v2_rgba.y;
							float b = bary.x * v0_rgba.z + bary.y * v1_rgba.z + bary.z * v2_rgba.z;
							float a = bary.x * v0_rgba.w + bary.y * v1_rgba.w + bary.z * v2_rgba.w;
							uint32_t texel_x = bary.x * v0_texel.x + bary.y * v1_texel.x + bary.z * v2_texel.x;
							uint32_t texel_y = bary.x * v0_texel.y + bary.y * v1_texel.y + bary.z * v2_texel.y;
							
							/* prevent precision loss */
							if(z > max_z)	z = max_z;
							if(z < min_z)	z = min_z;
							if(r > max_r)	r = max_r;
							if(r < min_r)	r = min_r;
							if(g > max_g)	g = max_g;
							if(g < min_g)	g = min_g;
							if(b > max_b)	b = max_b;
							if(b < min_b)	b = min_b;
							if(a > max_a)	a = max_a;
							if(a < min_a)	a = min_a;
							if(texel_x > max_texel_x)	texel_x = max_texel_x;
							if(texel_x < min_texel_x)	texel_x = min_texel_x;
							if(texel_y > max_texel_y)	texel_y = max_texel_y;
							if(texel_y < min_texel_y)	texel_y = min_texel_y;
							
							if(plot_color)
							{
								rlVec4 primary;		// primary (primitive) color
								rlVec4 secondary;	// secondary (texture) color
								primary.x = r, secondary.x = 0.0f;
								primary.y = g, secondary.y = 0.0f;
								primary.z = b, secondary.z = 0.0f;
								primary.w = a, secondary.w = 0.0f;
								rlVec4 color = primary;	// color to draw with
								if(texture_unit_complete && _rlcore->_texture)
								{
									_get_texel(texel_x, texel_y, &secondary, _rlcore->_textures[_rlcore->_texture_unit], 
										_rlcore->_texture_formats[_rlcore->_texture_unit], 
										_rlcore->_texture_widths[_rlcore->_texture_unit],
										_rlcore->_texture_compressed_booleans[_rlcore->_texture_unit]);
									color = secondary;
								}
								
								rlVec2i coord;
								coord.x = x;
								coord.y = y;
									
								bool discard = false;
								color = _fragment_pass(attrib_data, enabled_attrib_count, data_size, attrib_format, 
										RL_TRIANGLE, primary, secondary, linear_bary, bary, color, dst_depth, depth, coord, &discard);
									
								if(discard)
									continue;

								if(color.x > 1.0f) color.x = 1.0f;
								if(color.x < 0.0f) color.x = 0.0f;
								if(color.y > 1.0f) color.y = 1.0f;
								if(color.y < 0.0f) color.y = 0.0f;
								if(color.z > 1.0f) color.z = 1.0f;
								if(color.z < 0.0f) color.z = 0.0f;
								if(color.w > 1.0f) color.w = 1.0f;
								if(color.w < 0.0f) color.w = 0.0f;
									
								_plot_pixel(pixel_index, color, _rlcore->_blend);
							}
								
							if(plot_depth)
							{
								if(_rlcore->_db_type == RL_D16)
									((uint16_t*)_rlcore->_depthbuffer) [pixel_index] = z;
								if(_rlcore->_db_type == RL_D32)
									((uint32_t*)_rlcore->_depthbuffer) [pixel_index] = z;
							}
						}
						}
						cx1 -= fdy01;
						cx2 -= fdy12;
						cx3 -= fdy20;
					}	// cycle x in tile
					cy1 += fdx01;
					cy2 += fdx12;
					cy3 += fdx20;
					y_idx += _rlcore->_width;
				}	// cycle y in tile
			}	// tile partially covered
		}	// cycle tile x
	}	// cycle tile y
		
	free(attrib_data);
	free(attrib_format);
}
	
// rasterize a screen-space line
// v0_bary and v1_bary used for sub-lines
// not to be used directly
void _raster_line(rlVec2 v0, rlVec2 v1, rlVec4 v0_rgba, rlVec4 v1_rgba,
	rlVec2ui v0_texel, rlVec2ui v1_texel, int64_t v0_z, int64_t v1_z,
	float v0_w, float v1_w, rlVec2 v0_bary, rlVec2 v1_bary)
{
	if(!_rlcore)
		return;

	if((v0.x < 0 && v1.x < 0)
	|| (v0.x >= _rlcore->_width && v1.x >= _rlcore->_width)
	|| (v0.y < 0 && v1.y < 0)
	|| (v0.y >= _rlcore->_height && v1.y >= _rlcore->_height))
		return;


	v0_z += 1;
	v1_z += 1;
		
	bool plot_color = _rlcore->_colorbuffer;
	bool plot_depth = (_rlcore->_write_depth && _rlcore->_depthbuffer);
	bool can_raster = (_rlcore->_width + _rlcore->_height >= 2);
	bool depth_test = (_rlcore->_depth_test && _rlcore->_depthbuffer);
	
	int64_t db_range = 0;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16) db_range = 0xFFFF;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32) db_range = 0xFFFFFFFF;
	
	float inv_db_range = 0;
	if(db_range)
		inv_db_range = 1.0f / db_range;

	float inv_v0_w = _safedivf(1.0f, v0_w);
	float inv_v1_w = _safedivf(1.0f, v1_w);
		
	/* USED TO LATER PREVENT PRECISION LOSS */
	int64_t max_z = _max64(v0_z, v1_z);
	int64_t min_z = _min64(v0_z, v1_z);
	float max_r = _maxf(v0_rgba.x, v1_rgba.x);
	float min_r = _minf(v0_rgba.x, v1_rgba.x);
	float max_g = _maxf(v0_rgba.y, v1_rgba.y);
	float min_g = _minf(v0_rgba.y, v1_rgba.y);
	float max_b = _maxf(v0_rgba.z, v1_rgba.z);
	float min_b = _minf(v0_rgba.z, v1_rgba.z);
	float max_a = _maxf(v0_rgba.w, v1_rgba.w);
	float min_a = _minf(v0_rgba.w, v1_rgba.w);
	uint32_t max_texel_x = _max_u32(v0_texel.x, v1_texel.x);
	uint32_t min_texel_x = _min_u32(v0_texel.x, v1_texel.x);
	uint32_t max_texel_y = _max_u32(v0_texel.y, v1_texel.y);
	uint32_t min_texel_y = _min_u32(v0_texel.y, v1_texel.y);
		
	/* USED FOR FRAGMENT SHADER PASSES */
	void* attrib_data = 0;
	uint32_t* attrib_format = 0;
	uint32_t enabled_attrib_count = 0;
	uint32_t data_size = 0;
	attrib_data = _alloc_fragment_data(&enabled_attrib_count, &data_size, &attrib_format);
		
	uint32_t format = _rlcore->_texture_formats[_rlcore->_texture_unit];
	bool texture_unit_complete = _rlcore->_textures[_rlcore->_texture_unit]
		&& _rlcore->_texture_widths[_rlcore->_texture_unit] > 0
		&& _rlcore->_texture_heights[_rlcore->_texture_unit] > 0
		&& (format == RL_RGB16 ||
			format == RL_RGBA16 ||
			format == RL_RGB32 ||
			format == RL_RGBA32);
				
	float length = sqrt(pow(v0.x - v1.x, 2) + pow(v0.y - v1.y, 2));
	if(length == 0.0f)
		return;
	float inv_length = 1.0f / length;
	int p = 0;
	
	int x0 = v0.x;
	int x1 = v1.x;
	int y0 = v0.y;
	int y1 = v1.y;
		
	int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;
		
	int x = x0;
	int y = y0;
	
	int32_t y_idx = y * _rlcore->_width;
	if(can_raster)
	for(;;)
	{
		if(x == x1 && y == y1)
			break;
		
		if(x >= 0 && x < _rlcore->_width && y >= 0 && y < _rlcore->_height)
		{
			rlVec3 bary, linear_bary;
			bary.x = (length - p) * inv_length;	// percent of v0
			bary.y = 1.0f - bary.x;		// percent of v1
			bary.z = 0.0f;
			{
				float bx = bary.x;
				float by = bary.y;
				bary.x = bx * v0_bary.x + by * v1_bary.x;
				bary.y = bx * v0_bary.y + by * v1_bary.y;
			}
			linear_bary = bary;
			if(_rlcore->_persp_corr) 
			{
				float w = _safedivf(1.0f, (bary.x*inv_v0_w + bary.y*inv_v1_w));
				bary.x *= inv_v0_w;
				bary.y *= inv_v1_w;
				bary.x *= w;
				bary.y *= w;
			}
			
			int64_t z = bary.x * v0_z + bary.y * v1_z;
			float r = bary.x * v0_rgba.x + bary.y * v1_rgba.x;
			float g = bary.x * v0_rgba.y + bary.y * v1_rgba.y;
			float b = bary.x * v0_rgba.z + bary.y * v1_rgba.z;
			float a = bary.x * v0_rgba.w + bary.y * v1_rgba.w;
			uint32_t texel_x = bary.x * v0_texel.x + bary.y * v1_texel.x;
			uint32_t texel_y = bary.x * v0_texel.y + bary.y * v1_texel.y;
			float dst_depth = 0;
			float depth = 0;
			
			/* prevent precision loss */
			if(z > max_z)	z = max_z;
			if(z < min_z)	z = min_z;
			if(r > max_r)	r = max_r;
			if(r < min_r)	r = min_r;
			if(g > max_g)	g = max_g;
			if(g < min_g)	g = min_g;
			if(b > max_b)	b = max_b;
			if(b < min_b)	b = min_b;
			if(a > max_a)	a = max_a;
			if(a < min_a)	a = min_a;
			if(texel_x > max_texel_x)	texel_x = max_texel_x;
			if(texel_x < min_texel_x)	texel_x = min_texel_x;
			if(texel_y > max_texel_y)	texel_y = max_texel_y;
			if(texel_y < min_texel_y)	texel_y = min_texel_y;
			
			if(z < 0)
			{
				p += 1;
				e2 = err;
				if(e2 > -dx) { err -= dy; x += sx; }
				if(e2 <  dy) { err += dx; y += sy; y_idx += sy * _rlcore->_width; }
				continue;
			}
			if(_rlcore->_depthbuffer && z > db_range)
			{
				p += 1;
				e2 = err;
				if(e2 > -dx) { err -= dy; x += sx; }
				if(e2 <  dy) { err += dx; y += sy; y_idx += sy * _rlcore->_width; }
				continue;
			}
			
			uint32_t pixel_index = 0;
			pixel_index = y_idx + x;
			
			if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16)
			{
				if(depth_test && z > ((uint16_t*)_rlcore->_depthbuffer) [pixel_index])
				{
					p += 1;
					e2 = err;
					if(e2 > -dx) { err -= dy; x += sx; }
					if(e2 <  dy) { err += dx; y += sy; y_idx += sy * _rlcore->_width; }
					continue;
				}
				dst_depth = ((uint16_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
				depth = z * inv_db_range;
			}
			if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32)
			{
				if(depth_test && z > ((uint32_t*)_rlcore->_depthbuffer) [pixel_index])
				{
					p += 1;
					e2 = err;
					if(e2 > -dx) { err -= dy; x += sx; }
					if(e2 <  dy) { err += dx; y += sy; y_idx += sy * _rlcore->_width; }
					continue;
				}
				dst_depth = ((uint32_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
				depth = z * inv_db_range;
			}
			
			if(plot_color)
			{
				rlVec4 primary;		// primary (primitive) color
				rlVec4 secondary;	// secondary (texture) color
				primary.x = r, secondary.x = 0.0f;
				primary.y = g, secondary.y = 0.0f;
				primary.z = b, secondary.z = 0.0f;
				primary.w = a, secondary.w = 0.0f;
				rlVec4 color = primary;	// color to draw with
				if(texture_unit_complete && _rlcore->_texture)
				{
					_get_texel(texel_x, texel_y, &secondary, _rlcore->_textures[_rlcore->_texture_unit], 
						_rlcore->_texture_formats[_rlcore->_texture_unit], 
						_rlcore->_texture_widths[_rlcore->_texture_unit], _rlcore->_texture_compressed_booleans[_rlcore->_texture_unit]);
					color = secondary;
				}
				
				rlVec2i coord;
				coord.x = x;
				coord.y = y;
				
				bool discard = false;
				color = _fragment_pass(attrib_data, enabled_attrib_count, data_size, attrib_format,
					RL_LINE, primary, secondary, linear_bary, bary, color, dst_depth, depth, coord, &discard);
				
				if(discard)
				{
					p += 1;
					e2 = err;
					if(e2 > -dx) { err -= dy; x += sx; }
					if(e2 <  dy) { err += dx; y += sy; y_idx += sy * _rlcore->_width; }
					continue;
				}

				if(color.x > 1.0f) color.x = 1.0f;
				if(color.x < 0.0f) color.x = 0.0f;
				if(color.y > 1.0f) color.y = 1.0f;
				if(color.y < 0.0f) color.y = 0.0f;
				if(color.z > 1.0f) color.z = 1.0f;
				if(color.z < 0.0f) color.z = 0.0f;
				if(color.w > 1.0f) color.w = 1.0f;
				if(color.w < 0.0f) color.w = 0.0f;
				
				_plot_pixel(pixel_index, color, _rlcore->_blend);
			}

			if(plot_depth)
			{
				if(_rlcore->_db_type == RL_D16)
					((uint16_t*)_rlcore->_depthbuffer) [pixel_index] = z;
				if(_rlcore->_db_type == RL_D32)
					((uint32_t*)_rlcore->_depthbuffer) [pixel_index] = z;
			}
		}
				
		p += 1;
				
		e2 = err;
		if(e2 > -dx) { err -= dy; x += sx; }
		if(e2 <  dy) { err += dx; y += sy; y_idx += sy * _rlcore->_width; }
	}

	free(attrib_data);
	free(attrib_format);
}
	
// rasters a fragment of a point
// for use exclusively only in _raster_point
void _raster_point_fragment(int x, int y, rlVec4 rgba, int64_t z, void* attrib_data, uint32_t* attrib_format,
	uint32_t enabled_attrib_count, uint32_t data_size, int64_t db_range, float inv_db_range)
{
	float dst_depth = 0;
	float depth = 0;
	
	if(x < 0 || x >= _rlcore->_width || y < 0 || y >= _rlcore->_height)
		return;
			
	rlVec3 bary, linear_bary;
	bary.x = 0.0f;
	bary.y = 0.0f;
	bary.z = 0.0f;
	linear_bary = bary;
		
	uint32_t pixel_index = 0;
	pixel_index = y * _rlcore->_width + x;
		
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16)
	{
		if(_rlcore->_depth_test && z > ((uint16_t*)_rlcore->_depthbuffer) [pixel_index])
			return;
		dst_depth = ((uint16_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
		depth = z * inv_db_range;
	}
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32)
	{
		if(_rlcore->_depth_test && z > ((uint32_t*)_rlcore->_depthbuffer) [pixel_index])
			return;
		dst_depth = ((uint32_t*)_rlcore->_depthbuffer) [pixel_index] * inv_db_range;
		depth = z * inv_db_range;
	}
	
	if(_rlcore->_colorbuffer)
	{
		rlVec4 primary;		// primary (primitive) color
		rlVec4 secondary;	// secondary (texture) color
		primary.x = rgba.x, secondary.x = 0.0f;
		primary.y = rgba.y, secondary.y = 0.0f;
		primary.z = rgba.z, secondary.z = 0.0f;
		primary.w = rgba.w, secondary.w = 0.0f;
		rlVec4 color = primary;	// color to draw with
		
		rlVec2i coord;
		coord.x = x;
		coord.y = y;
		
		bool discard = false;
		color = _fragment_pass(attrib_data, enabled_attrib_count, data_size, attrib_format,
			RL_POINT, primary, secondary, linear_bary, bary, color, dst_depth, depth, coord, &discard);
		
		if(discard)
			return;
		if(color.x > 1.0f) color.x = 1.0f;			
		if(color.x < 0.0f) color.x = 0.0f;
		if(color.y > 1.0f) color.y = 1.0f;
		if(color.y < 0.0f) color.y = 0.0f;
		if(color.z > 1.0f) color.z = 1.0f;
		if(color.z < 0.0f) color.z = 0.0f;
		if(color.w > 1.0f) color.w = 1.0f;
		if(color.w < 0.0f) color.w = 0.0f;
			
		_plot_pixel(pixel_index, color, _rlcore->_blend);
	}

	if(_rlcore->_write_depth && _rlcore->_depthbuffer)
	{
		if(_rlcore->_db_type == RL_D16)
			((uint16_t*)_rlcore->_depthbuffer) [pixel_index] = z;
		if(_rlcore->_db_type == RL_D32)
			((uint32_t*)_rlcore->_depthbuffer) [pixel_index] = z;
	}
}
	
void _raster_point(rlVec2 pos, rlVec4 rgba, int64_t z)
{
	if(!_rlcore)
		return;
		
	z += 1;
	
	bool plot_color = _rlcore->_colorbuffer;
	bool plot_depth = (_rlcore->_write_depth && _rlcore->_depthbuffer);
	bool can_raster = (_rlcore->_width + _rlcore->_height >= 2);
	bool depth_test = (_rlcore->_depth_test && _rlcore->_depthbuffer);
	
	int64_t db_range = 0;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16) db_range = 0xFFFF;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32) db_range = 0xFFFFFFFF;
	
	float inv_db_range = 0;
	if(db_range)
		inv_db_range = 1.0f / db_range;
		
	uint32_t r = _rlcore->_point_radius;
	if(_rlcore->_depthbuffer)
	{
		float pt_depth = z * inv_db_range;
		if(pt_depth > 1.0f)
			return;
		if(pt_depth < 0.0f)
			return;
	}
	
	/* USED FOR FRAGMENT SHADER PASSES */
	void* attrib_data = 0;
	uint32_t* attrib_format = 0;
	uint32_t enabled_attrib_count = 0;
	uint32_t data_size = 0;
	attrib_data = _alloc_fragment_data(&enabled_attrib_count, &data_size, &attrib_format);
	
	float dst_depth = 0;
	if(z < 0)
		return;
	if(_rlcore->_depthbuffer && z > db_range)
		return;
		
	int f = 1 - r;
	int dx = 0;
	int dy = -2 * r;
	int x2 = 0;
	int y2 = r;

	if(can_raster)
	{
		if(pos.x - r >= _rlcore->_width)
			return;
		if(pos.x + r < 0)
			return;
		if(pos.y - r >= _rlcore->_height)
			return;
		if(pos.y + r < 0)
			return;

		_raster_point_fragment(pos.x, pos.y + r, rgba, z, attrib_data, 
			attrib_format, enabled_attrib_count, data_size, db_range, inv_db_range);
		_raster_point_fragment(pos.x, pos.y - r, rgba, z, attrib_data,
			attrib_format, enabled_attrib_count, data_size, db_range, inv_db_range);
		_raster_point_fragment(pos.x + r, pos.y, rgba, z, attrib_data,
			attrib_format, enabled_attrib_count, data_size, db_range, inv_db_range);
		_raster_point_fragment(pos.x - r, pos.y, rgba, z, attrib_data,
			attrib_format, enabled_attrib_count, data_size, db_range, inv_db_range);
		for(int x = pos.x - r; x < pos.x + r; x += 1)
			_raster_point_fragment(x, pos.y, rgba, z, attrib_data, attrib_format, 
				enabled_attrib_count, data_size, db_range, inv_db_range);
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
		int x0 = pos.x - x2;
		int x1 = pos.x + x2;
		int sx = (x0 < x1) ? 1 : -1; 
		for(int x = x0; x != x1; x += sx)
			_raster_point_fragment(x, pos.y + y2, rgba, z, attrib_data, attrib_format, 
				enabled_attrib_count, data_size, db_range, inv_db_range);
		for(int x = x0; x != x1; x += sx)
			_raster_point_fragment(x, pos.y - y2, rgba, z, attrib_data, attrib_format, 
				enabled_attrib_count, data_size, db_range, inv_db_range);
		x0 = pos.x - y2;
		x1 = pos.x + y2;
		sx = (x0 < x1) ? 1 : -1; 
		for(int x = x0; x != x1; x += sx)
			_raster_point_fragment(x, pos.y + x2, rgba, z, attrib_data, attrib_format, 
				enabled_attrib_count, data_size, db_range, inv_db_range);
		for(int x = x0; x != x1; x += sx)
			_raster_point_fragment(x, pos.y - x2, rgba, z, attrib_data, attrib_format, 
				enabled_attrib_count, data_size, db_range, inv_db_range);
	}
	}
	
	free(attrib_data);
	free(attrib_format);
}

// clip a line against an x plane. Returns coordinates of clipped end point.
// only to be used when the line is not vertical and has end to be clipped.
//
rlVec2 _clip_line_x(rlVec3 v0, rlVec3 v1, float x)
{
	float length_v0v1 = fabs(v0.x - v1.x);
	float length_v0x  = fabs(v0.x - x);
	
	rlVec2 bc;
	bc.x = 1.0f - _safedivf(length_v0x, length_v0v1);
	bc.y = 1.0f - bc.x;

	rlVec2 pt;
	pt.x = x;
	pt.y = bc.x * v0.y + bc.y * v1.y;
	return pt;
}

// clip a line against a y plane. Returns coordinates of clipped end point.
// only to be used when the line is not horizontal and has end to be clipped.
//
rlVec2 _clip_line_y(rlVec3 v0, rlVec3 v1, float y)
{
	float length_v0v1 = fabs(v0.y - v1.y);
	float length_v0y  = fabs(v0.y - y);
	
	rlVec2 bc;
	bc.x = 1.0f - _safedivf(length_v0y, length_v0v1);
	bc.y = 1.0f - bc.x;

	rlVec2 pt;
	pt.x = bc.x * v0.x + bc.y * v1.x;
	pt.y = y;
	return pt;
}

// find a point on line with Z value 'z' and return the point's coordinates.
// v0 and v1 in clip-space (w should retain z prior to projection)
//
rlVec2 _clip_line_z(rlVec4 v0, rlVec4 v1, float z)
{
	float length_v0v1 = fabs(v0.z - v1.z);
	float length_v0z  = fabs(v0.z - z);
	
	rlVec2 bc;		// barycentric coordinates of point on line
	bc.x = 1.0f - _safedivf(length_v0z, length_v0v1);
	bc.y = 1.0f - bc.x;

	rlVec2 pt;

	if(_rlcore->_persp_corr)
	{			
		float inv_v0_w = _safedivf(1.0f, v0.w);
		float inv_v1_w = _safedivf(1.0f, v1.w);
		float w = _safedivf(1.0f, bc.x*inv_v0_w + bc.y*inv_v1_w);
		bc.x *= inv_v0_w;
		bc.y *= inv_v1_w;
		bc.x *= w;
		bc.y *= w;
		
		pt.x = bc.x * v0.x + bc.y * v1.x;
		pt.y = bc.x * v0.y + bc.y * v1.y;
		
	}
	else
	{
		pt.x = bc.x * v0.x + bc.y * v1.x;
		pt.y = bc.x * v0.y + bc.y * v1.y;
	}
	return pt;
}

// get linear barycentric coordinates of point on a line
//
rlVec2 _calc_line_bary(rlVec2 v0, rlVec2 v1, rlVec2 p)
{
	float distance_v0v1 = (v1.x-v0.x)*(v1.x-v0.x) + (v1.y-v0.y)*(v1.y-v0.y);
	float distance_v0p  = (p.x-v0.x)*(p.x-v0.x) + (p.y-v0.y)*(p.y-v0.y);

	if(distance_v0v1 != 0.0f)
		 distance_v0v1 = sqrt(distance_v0v1);
	else distance_v0v1 = 0.0f;
	if(distance_v0p != 0.0f)
		 distance_v0p = sqrt(distance_v0p);
	else distance_v0p = 0.0f;

	rlVec2 bary;
	bary.x = 1.0f - _safedivf(distance_v0p, distance_v0v1);
	bary.y = 1.0f - bary.x;
	return bary;
}

rlVec3 _vec2_to_vec3(rlVec2 v, float z)
{
	rlVec3 result = { v.x, v.y, z };
	return result;
}

// post-process and raster a point
//
void _process_point(rlVec4 pos, rlVec4 rgba, float width_div_2, float height_div_2)
{
	if(_rlcore->_clip)
	{
		if(!(-pos.w <= pos.x <= pos.w))
			return;
		if(!(-pos.w <= pos.y <= pos.w))
			return;
		if(!(-pos.w <= pos.z <= pos.w))
			return;
	}
	
	if(_rlcore->_persp_div && pos.w != 0.0f && pos.w != 1.0f)
	{
		pos.x = _safedivf(pos.x, pos.w);
		pos.y = _safedivf(pos.y, pos.w);
		pos.z = _safedivf(pos.z, pos.w);
	}
	
	if(_rlcore->_scale_z)
		pos.z = pos.z * 0.5f + 0.5f;
	
	if(pos.z > 1.0f || pos.z < 0.0f)
		return;

	rlVec2 raster_v0;
	raster_v0.x = width_div_2  + ( pos.x * ((float)(_rlcore->_width)  - width_div_2 ));
	raster_v0.y = height_div_2 + (-pos.y * ((float)(_rlcore->_height) - height_div_2));
	int64_t v0_z = 0;
	if(_rlcore->_depthbuffer)
	{
		if(_rlcore->_db_type == RL_D16)
			v0_z = pos.z * 0xFFFF;
		if(_rlcore->_db_type == RL_D32)
			v0_z = pos.z * 0xFFFFFFFF;
	}
	
	_raster_point(raster_v0, rgba, v0_z);
}

// return the cohen-sutherland outcode for a point in clip-space
// 0 0 X1 X2 Y1 Y2 Z1 Z2
// if bit 1: x < xmin
// if bit 2: x > xmax
// if bit 3: y < ymin
// if bit 4: y > ymax
// if bit 5: z < zmin
// if bit 6: z > zmax
//
uint8_t _cohen_sutherland(rlVec3 p, float xmin, float xmax,
	float ymin, float ymax, float zmin, float zmax)
{
	uint8_t result = 0;
	result |= (p.x < xmin);
	result |= ((p.x > xmax) << 1);
	result |= ((p.y < ymin) << 2);
	result |= ((p.y > ymax) << 3);
	result |= ((p.z < zmin) << 4);
	result |= ((p.z > zmax) << 5);
	return result;
}

// post-process and raster a line
//
void _process_line(rlVec4 v0, rlVec4 v1, rlVec4 rgba_v0, rlVec4 rgba_v1,
	rlVec2 tcoords_v0, rlVec2 tcoords_v1, float width_div_2, float height_div_2)
{
	rlVec3 clipped_v0 = { v0.x, v0.y, v0.z };
	rlVec3 clipped_v1 = { v1.x, v1.y, v1.z };
	
	rlVec2 v0_bary = { 1, 0 };
	rlVec2 v1_bary = { 0, 1 };

	/* LINE CLIPPING */
	// (much simpler than triangle clipping!)

	if(_rlcore->_clip)
	{	
		// use _clip_line_x, _clip_line_y, and _clip_line_z

		// clip so that -w <= (x,y,z) <= w
 
		rlVec2 v0_2d = { v0.x, v0.y };
		rlVec2 v1_2d = { v1.x, v1.y };
		rlVec3 v0_3d = { v0.x, v0.y, v0.z };
		rlVec3 v1_3d = { v1.x, v1.y, v1.z };

		// first, clip vertices against Z.
		// then, clip resulting vertices against X, if necessary.
		// finally, clip those resulting vertices against Y if necessary.

		// CLIP VERTICES
		
		uint8_t v0_outcode = _cohen_sutherland(clipped_v0, -v0.w, v0.w, -v0.w, v0.w, -v0.w, v0.w);
		uint8_t v1_outcode = _cohen_sutherland(clipped_v1, -v1.w, v1.w, -v1.w, v1.w, -v1.w, v1.w);

		for(;;)
		{
			if((v0_outcode | v1_outcode) == 0x0)
				break;	// accept
				
			if(v0_outcode & v1_outcode)
				return;	// reject
					
			float x, y, z, w;

			uint8_t outcode = v0_outcode ? v0_outcode : v1_outcode;
			if(outcode == v0_outcode)
			{
				z = clipped_v0.z;
				w = v0.w;
			}
			else
			{
				z = clipped_v1.z;
				w = v1.w;
			}

			if(outcode & 0x10)
			{
				rlVec2 pt = _clip_line_z(v0, v1, -w);
				x = pt.x;
				y = pt.y;
				z = -w;
			}
			else if(outcode & 0x20)
			{
				rlVec2 pt = _clip_line_z(v0, v1, w);
				x = pt.x;
				y = pt.y;
				z = w;
			}
			else if(outcode & 0x4)	// y < -w
			{
				x = clipped_v0.x + (clipped_v1.x - clipped_v0.x) * (-w - clipped_v0.y) / (clipped_v1.y - clipped_v0.y);
				y = -w;
			}
			else if(outcode & 0x8)	// y > w
			{
				x = clipped_v0.x + (clipped_v1.x - clipped_v0.x) * (w - clipped_v0.y) / (clipped_v1.y - clipped_v0.y);
				y = w;
			}
			else if(outcode & 0x1) // x < -w
			{
				y = clipped_v0.y + (clipped_v1.y - clipped_v0.y) * (-w - clipped_v0.x) / (clipped_v1.x - clipped_v0.x);
				x = -w;
			}
			else if(outcode & 0x2)	// x > w
			{
				y = clipped_v0.y + (clipped_v1.y - clipped_v0.y) * (w - clipped_v0.x) / (clipped_v1.x - clipped_v0.x);
				x = w;
			}

			if(outcode == v0_outcode)
			{
				clipped_v0.x = x;
				clipped_v0.y = y;
				clipped_v0.z = z;
				v0_outcode = _cohen_sutherland(clipped_v0, -w, w, -w, w, -w, w);
			}
			else
			{
				clipped_v1.x = x;
				clipped_v1.y = y;
				clipped_v1.z = z;
				v1_outcode = _cohen_sutherland(clipped_v1, -w, w, -w, w, -w, w);
			}
		}
		
		rlVec2 cv0_2d = { clipped_v0.x, clipped_v0.y };
		rlVec2 cv1_2d = { clipped_v1.x, clipped_v1.y };
		v0_bary = _calc_line_bary(v0_2d, v1_2d, cv0_2d);
		v1_bary = _calc_line_bary(v0_2d, v1_2d, cv1_2d);
	}
	
	if(_rlcore->_persp_div && v0.w != 0.0f && v0.w != 1.0f)
	{
		float inv_v0_w = _safedivf(1.0f, v0.w);
		clipped_v0.x *= inv_v0_w;
		clipped_v0.y *= inv_v0_w;
		v0.z *= inv_v0_w;
	}
	if(_rlcore->_persp_div && v1.w != 0.0f && v1.w != 1.0f)
	{
		float inv_v1_w = _safedivf(1.0f, v1.w);
		clipped_v1.x *= inv_v1_w;
		clipped_v1.y *= inv_v1_w;
		v1.z *= inv_v1_w;
	}
	
	if(_rlcore->_scale_z)
	{
		v0.z *= 0.5f + 0.5f;
		v1.z *= 0.5f + 0.5f;
	}
	
	rlVec2 raster_v0, raster_v1;
	raster_v0.x = width_div_2  + ( clipped_v0.x * ((float)(_rlcore->_width)  - width_div_2 ));
	raster_v0.y = height_div_2 + (-clipped_v0.y * ((float)(_rlcore->_height) - height_div_2));
	raster_v1.x = width_div_2  + ( clipped_v1.x * ((float)(_rlcore->_width)  - width_div_2 ));
	raster_v1.y = height_div_2 + (-clipped_v1.y * ((float)(_rlcore->_height) - height_div_2));
	int64_t v0_z = 0, v1_z = 0;
	if(_rlcore->_depthbuffer)
	{
		if(_rlcore->_db_type == RL_D16)
		{
			v0_z = v0.z * 0xFFFF;
			v1_z = v1.z * 0xFFFF;
		}
		if(_rlcore->_db_type == RL_D32)
		{
			v0_z = v0.z * 0xFFFFFFFF;
			v1_z = v1.z * 0xFFFFFFFF;
		}
	}
			
	uint32_t format = _rlcore->_texture_formats[_rlcore->_texture_unit];
	rlVec2ui texel_v0, texel_v1;
	if(!_rlcore->_textures[_rlcore->_texture_unit]
	|| _rlcore->_texture_widths[_rlcore->_texture_unit] == 0
	|| _rlcore->_texture_heights[_rlcore->_texture_unit] == 0
	|| (format != RL_RGB16 && 
		format != RL_RGBA16 &&
		format != RL_RGB32 &&
		format != RL_RGBA32))
	{
		texel_v0.x = 0, texel_v0.y = 0;
		texel_v1.x = 0, texel_v1.y = 0;
	}
	else
	{
		texel_v0.x = tcoords_v0.x * (_rlcore->_texture_widths[_rlcore->_texture_unit] - 1);
		texel_v0.y = (1.0f - tcoords_v0.y) * (_rlcore->_texture_heights[_rlcore->_texture_unit] - 1);
		texel_v1.x = tcoords_v1.x * (_rlcore->_texture_widths[_rlcore->_texture_unit] - 1);
		texel_v1.y = (1.0f - tcoords_v1.y) * (_rlcore->_texture_heights[_rlcore->_texture_unit] - 1);
	}
	
	_raster_line(raster_v0, raster_v1, rgba_v0, rgba_v1, texel_v0, texel_v1, v0_z, v1_z, v0.w, v1.w, v0_bary, v1_bary);
}

// post-process and raster a triangle
//
void _process_triangle(rlVec4 v0, rlVec4 v1, rlVec4 v2, rlVec4 rgba_v0, rlVec4 rgba_v1, rlVec4 rgba_v2,
	rlVec2 tcoords_v0, rlVec2 tcoords_v1, rlVec2 tcoords_v2, float width_div_2, float height_div_2)
{
	rlVec3 clipped_v0 = { v0.x, v0.y, v0.z };
	rlVec3 clipped_v1 = { v1.x, v1.y, v1.z };
	rlVec3 clipped_v2 = { v2.x, v2.y, v2.z };
	
	rlVec3 v0_bary = { 1, 0, 0 };
	rlVec3 v1_bary = { 0, 1, 0 };
	rlVec3 v2_bary = { 0, 0, 1 };
	
	/* TRIANGLE CLIPPING*/
	
	if(_rlcore->_clip)
	{
		if(!(-v0.w <= v0.x <= v0.w) || !(-v1.w <= v1.x <= v1.w) || !(-v2.w <= v2.x <= v2.w))
			return;
		if(!(-v0.w <= v0.y <= v0.w) || !(-v1.w <= v1.y <= v1.w) || !(-v2.w <= v2.y <= v2.w))
			return;
		if(!(-v0.w <= v0.z <= v0.w) || !(-v1.w <= v1.z <= v1.w) || !(-v2.w <= v2.z <= v2.w))
			return;
			
		// clip & calculate bary for each vertex of each new triangle
	}
	
	if(_rlcore->_persp_div && v0.w != 0.0f && v0.w != 1.0f)
	{
		float inv_v0_w = _safedivf(1.0f, v0.w);
		clipped_v0.x *= inv_v0_w;
		clipped_v0.y *= inv_v0_w;
		v0.z *= inv_v0_w;
	}
	if(_rlcore->_persp_div && v1.w != 0.0f && v1.w != 1.0f)
	{
		float inv_v1_w = _safedivf(1.0f, v1.w);
		clipped_v1.x *= inv_v1_w;
		clipped_v1.y *= inv_v1_w;
		v1.z *= inv_v1_w;
	}
	if(_rlcore->_persp_div && v2.w != 0.0f && v2.w != 1.0f)
	{
		float inv_v2_w = _safedivf(1.0f, v2.w);
		clipped_v2.x *= inv_v2_w;
		clipped_v2.y *= inv_v2_w;
		v2.z *= inv_v2_w;
	}
	
	if(_rlcore->_scale_z)
	{
		v0.z *= 0.5f + 0.5f;
		v1.z *= 0.5f + 0.5f;
		v2.z *= 0.5f + 0.5f;
	}

	rlVec2 raster_v0, raster_v1, raster_v2;
	raster_v0.x = width_div_2  + ( clipped_v0.x * ((float)(_rlcore->_width)  - width_div_2 ));
	raster_v0.y = height_div_2 + (-clipped_v0.y * ((float)(_rlcore->_height) - height_div_2));
	raster_v1.x = width_div_2  + ( clipped_v1.x * ((float)(_rlcore->_width)  - width_div_2 ));
	raster_v1.y = height_div_2 + (-clipped_v1.y * ((float)(_rlcore->_height) - height_div_2));
	raster_v2.x = width_div_2  + ( clipped_v2.x * ((float)(_rlcore->_width)  - width_div_2 ));
	raster_v2.y = height_div_2 + (-clipped_v2.y * ((float)(_rlcore->_height) - height_div_2));
	int64_t v0_z = 0, v1_z = 0, v2_z = 0;
	if(_rlcore->_depthbuffer)
	{
		if(_rlcore->_db_type == RL_D16)
		{
			v0_z = v0.z * 0xFFFF;
			v1_z = v1.z * 0xFFFF;
			v2_z = v2.z * 0xFFFF;
		}
		if(_rlcore->_db_type == RL_D32)
		{
			v0_z = v0.z * 0xFFFFFFFF;
			v1_z = v1.z * 0xFFFFFFFF;
			v2_z = v2.z * 0xFFFFFFFF;
		}
	}
	
	uint32_t format = _rlcore->_texture_formats[_rlcore->_texture_unit];
	rlVec2ui texel_v0, texel_v1, texel_v2;
	if(!_rlcore->_textures[_rlcore->_texture_unit]
	|| _rlcore->_texture_widths[_rlcore->_texture_unit] == 0
	|| _rlcore->_texture_heights[_rlcore->_texture_unit] == 0
	|| (format != RL_RGB16 && 
		format != RL_RGBA16 &&
		format != RL_RGB32 &&
		format != RL_RGBA32))
	{
		texel_v0.x = 0, texel_v0.y = 0;
		texel_v1.x = 0, texel_v1.y = 0;
		texel_v2.x = 0, texel_v2.y = 0;
	}
	else
	{
		texel_v0.x = tcoords_v0.x * (_rlcore->_texture_widths[_rlcore->_texture_unit] - 1);
		texel_v0.y = (1.0f - tcoords_v0.y) * (_rlcore->_texture_heights[_rlcore->_texture_unit] - 1);
		texel_v1.x = tcoords_v1.x * (_rlcore->_texture_widths[_rlcore->_texture_unit] - 1);
		texel_v1.y = (1.0f - tcoords_v1.y) * (_rlcore->_texture_heights[_rlcore->_texture_unit] - 1);
		texel_v2.x = tcoords_v2.x * (_rlcore->_texture_widths[_rlcore->_texture_unit] - 1);
		texel_v2.y = (1.0f - tcoords_v2.y) * (_rlcore->_texture_heights[_rlcore->_texture_unit] - 1);
	}
		
	_raster(raster_v0, raster_v1, raster_v2, rgba_v0, rgba_v1, rgba_v2, texel_v0, texel_v1, texel_v2,
		v0_z, v1_z, v2_z, v0.w, v1.w, v2.w, v0_bary, v1_bary, v2_bary);
}

/* allocate, initialize and return a context */
_rlcore_t* rlCreateContext()
{
	_rlcore_t* context = (_rlcore_t*) malloc(sizeof(_rlcore_t));
	context->_clear_depth = -1;
	context->_clear_color = 0;
	context->_depthbuffer = NULL;
	context->_colorbuffer = NULL;
	context->_db_type = 0;
	context->_cb_type = 0;
	context->_width = 0;
	context->_height = 0;
	context->_back_depthbuffer = NULL;
	context->_back_colorbuffer = NULL;
	context->_back_db_type = 0;
	context->_back_cb_type = 0;
	context->_back_width = 0;
	context->_back_height = 0;
	context->_vertex_layout = RL_V3;
	context->_mode = RL_FILL;
	context->_cull_winding = RL_CW;
	context->_point_radius = 1;
	context->_write_depth = true;
	context->_depth_test = true;
	context->_persp_corr = true;
	context->_blend = false;
	context->_texture = true;
	context->_cull = false;
	context->_clip = true;
	context->_persp_div = true;
	context->_scale_z = true;
	context->_texture_unit = 0;
	for(uint8_t i = 0; i < 255; i += 1) {
		context->_textures[i] = NULL;
		context->_texture_formats[i] = 0;
		context->_texture_widths[i] = 0;
		context->_texture_heights[i] = 0;
		context->_texture_compressed_booleans[i] = false; }
	context->_vshader = NULL;
	context->_fshader = NULL;
	context->_sh_primitive_type = false;
	context->_sh_vertex_array = false;
	context->_sh_color_array = false;
	context->_sh_normal_array = false;
	context->_sh_texcoord_array = false;
	context->_sh_primary_color = false;
	context->_sh_secondary_color = false;
	context->_sh_bary_linear = false;
	context->_sh_bary_perspective = false;
	context->_sh_dst_depth = false;
	context->_sh_frag_depth = false;
	context->_sh_frag_x_coord = false;
	context->_sh_frag_y_coord = false;
	context->_inv_255 = 1.0f / 255.0f;
	context->_inv_31 = 1.0f / 31.0f;

	return context;
}

/* bind a context */
void rlBindContext(_rlcore_t* context)
{
	_rlcore = context;
}

/* draw primitives described by an array */
void rlDrawArray(uint32_t primitive_type, uint32_t primitive_count, float* data)
{	
	if(!_rlcore)
		return;

	uint32_t vertex_count = 0;
	switch(primitive_type)
	{
		case RL_POINTS:
		vertex_count = primitive_count;
		break;
		case RL_LINES:
		vertex_count = primitive_count * 2;
		break;
		case RL_TRIANGLES:
		vertex_count = primitive_count * 3;
		break;
		default:
		// unhandled error: invalid parameter
		return;
	}
	
	uint32_t vertex_width = 0;
	switch(_rlcore->_vertex_layout)
	{
		case RL_V3:		vertex_width = 3;	break;
		case RL_V3_C4:	vertex_width = 7;	break;
		case RL_V3_N3:	vertex_width = 6;	break;
		case RL_V3_T2:	vertex_width = 5;	break;
		case RL_V3_N3_T2:		vertex_width = 8;	break;
		case RL_V3_C4_N3:		vertex_width = 10;	break;
		case RL_V3_C4_T2:		vertex_width = 9;	break;
		case RL_V3_C4_N3_T2:	vertex_width = 12;	break;
		case RL_V4:		vertex_width = 4;	break;
		case RL_V4_C4:	vertex_width = 8;	break;
		case RL_V4_N3:	vertex_width = 7;	break;
		case RL_V4_T2:	vertex_width = 6;	break;
		case RL_V4_N3_T2:		vertex_width = 9;	break;
		case RL_V4_C4_N3:		vertex_width = 11;	break;
		case RL_V4_C4_T2:		vertex_width = 10;	break;
		case RL_V4_C4_N3_T2:	vertex_width = 13;	break;
		default:
		// unhandled error: unknown vertex layout
		return;
	}
	
	bool mode_valid = (_rlcore->_mode == RL_FILL || _rlcore->_mode == RL_POINT || _rlcore->_mode == RL_LINE);
	
	uint32_t v = 0;
	
	float v0_position[4], v1_position[4], v2_position[4];
	float v0_color[4], v1_color[4], v2_color[4];
	float v0_normals[3], v1_normals[3], v2_normals[3];
	float v0_tcoords[2], v1_tcoords[2], v2_tcoords[2];
	
	float width_div_2  = _rlcore->_width / 2.0f;
	float height_div_2 = _rlcore->_height / 2.0f;

	bool has_position = false;
	bool has_color = false;
	bool has_normals = false;
	bool has_tcoords = false;
	
	for(uint32_t p = 0; p < primitive_count; p += 1)
	{
		if(primitive_type == RL_POINTS)
		{
			switch(_rlcore->_vertex_layout)
			{
				case RL_V3:
				case RL_V4:
				_read_vertex(data, vertex_width, v, 1, v0_position, NULL, NULL, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				break;
				case RL_V3_C4:
				case RL_V4_C4:
				_read_vertex(data, vertex_width, v, 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				break;
				case RL_V3_N3:
				case RL_V4_N3:
				_read_vertex(data, vertex_width, v, 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, v0_normals, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_normals = true;
				break;
				case RL_V3_T2:
				case RL_V4_T2:
				_read_vertex(data, vertex_width, v, 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, v0_tcoords, NULL, NULL);
				has_position = true;
				has_tcoords = true;
				break;
				case RL_V3_N3_T2:
				case RL_V4_N3_T2:
				_read_vertex(data, vertex_width, v, 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, v0_normals, NULL, NULL, v0_tcoords, NULL, NULL);
				has_position = true;
				has_normals = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3:
				case RL_V4_C4_N3:
				_read_vertex(data, vertex_width, v, 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, v0_normals, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				break;
				case RL_V3_C4_T2:
				case RL_V4_C4_T2:
				_read_vertex(data, vertex_width, v, 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, NULL, NULL, NULL, v0_tcoords, NULL, NULL);
				has_position = true;
				has_color = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3_T2:
				case RL_V4_C4_N3_T2:
				_read_vertex(data, vertex_width, v, 1, v0_position, NULL, NULL, v0_color, NULL,
					NULL, v0_normals, NULL, NULL, v0_tcoords, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				has_tcoords = true;
				break;
			}
			
			// process v0
			// if not has_attrib, default the attribute.
			
			rlVec4 color_v0;
			rlVec3 normals_v0;
			rlVec2 tcoords_v0;
			
			rlVec4 v0;
			v0.x = v0_position[0];
			v0.y = v0_position[1];
			v0.z = v0_position[2];
			v0.w = v0_position[3];
			
			if(has_color)
			{
				if(v0_color[0] < 0.0f) v0_color[0] = 0.0f;
				if(v0_color[0] > 1.0f) v0_color[0] = 1.0f;
				if(v0_color[1] < 0.0f) v0_color[1] = 0.0f;
				if(v0_color[1] > 1.0f) v0_color[1] = 1.0f;
				if(v0_color[2] < 0.0f) v0_color[2] = 0.0f;
				if(v0_color[2] > 1.0f) v0_color[2] = 1.0f;
				if(v0_color[3] < 0.0f) v0_color[3] = 0.0f;
				if(v0_color[3] > 1.0f) v0_color[3] = 1.0f;
				color_v0.x = v0_color[0];
				color_v0.y = v0_color[1];
				color_v0.z = v0_color[2];
				color_v0.w = v0_color[3];
			}
			else
				color_v0.x = 0, color_v0.y = 0, color_v0.z = 0, color_v0.w = 1;
			
			if(has_normals)
			{
				if(v0_normals[0] < 0.0f) v0_normals[0] = 0.0f;
				if(v0_normals[0] > 1.0f) v0_normals[0] = 1.0f;
				if(v0_normals[1] < 0.0f) v0_normals[1] = 0.0f;
				if(v0_normals[1] > 1.0f) v0_normals[1] = 1.0f;
				if(v0_normals[2] < 0.0f) v0_normals[2] = 0.0f;
				if(v0_normals[2] > 1.0f) v0_normals[2] = 1.0f;
				normals_v0.x = v0_normals[0];
				normals_v0.y = v0_normals[1];
				normals_v0.z = v0_normals[2];
			}
			else
				normals_v0.x = 0, normals_v0.y = 0, normals_v0.z = 0;
			
			if(has_tcoords)
			{
				if(v0_tcoords[0] < 0.0f) v0_tcoords[0] = 0.0f;
				if(v0_tcoords[0] > 1.0f) v0_tcoords[0] = 1.0f;
				if(v0_tcoords[1] < 0.0f) v0_tcoords[1] = 0.0f;
				if(v0_tcoords[1] > 1.0f) v0_tcoords[1] = 1.0f;
				tcoords_v0.x = v0_tcoords[0];
				tcoords_v0.y = v0_tcoords[1];
			}
			
			v0 = _vertex_pass(RL_POINT, v0, color_v0, normals_v0, tcoords_v0);
			
			if(mode_valid)
			{
				_process_point(v0, color_v0, width_div_2, height_div_2);
			}
			
			v += 1;
		}
		if(primitive_type == RL_LINES)
		{
			switch(_rlcore->_vertex_layout)
			{
				case RL_V3:
				case RL_V4:
				_read_vertex(data, vertex_width, v, 2, v0_position, v1_position, NULL, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				break;
				case RL_V3_C4:
				case RL_V4_C4:
				_read_vertex(data, vertex_width, v, 2, v0_position, v1_position, NULL, v0_color, 
					v1_color, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				break;
				case RL_V3_N3:
				case RL_V4_N3:
				_read_vertex(data, vertex_width, v, 2, v0_position, v1_position, NULL, NULL, NULL, 
					NULL, v0_normals, v1_normals, NULL, NULL, NULL, NULL);
				has_position = true;
				has_normals = true;
				break;
				case RL_V3_T2:
				case RL_V4_T2:
				_read_vertex(data, vertex_width, v, 2, v0_position, v1_position, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, v0_tcoords, v1_tcoords, NULL);
				has_position = true;
				has_tcoords = true;
				break;
				case RL_V3_N3_T2:
				case RL_V4_N3_T2:
				_read_vertex(data, vertex_width, v, 2, v0_position, v1_position, NULL, NULL, NULL, 
					NULL, v0_normals, v1_normals, NULL, v0_tcoords, v1_tcoords, NULL);
				has_position = true;
				has_normals = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3:
				case RL_V4_C4_N3:
				_read_vertex(data, vertex_width, v, 2, v0_position, v1_position, NULL, v0_color, 
					v1_color, NULL, v0_normals, v1_normals, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				break;
				case RL_V3_C4_T2:
				case RL_V4_C4_T2:
				_read_vertex(data, vertex_width, v, 2, v0_position, v1_position, NULL, v0_color, 
					v1_color, NULL, NULL, NULL, NULL, v0_tcoords, v1_tcoords, NULL);
				has_position = true;
				has_color = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3_T2:
				case RL_V4_C4_N3_T2:
				_read_vertex(data, vertex_width, v, 2, v0_position, v1_position, NULL, v0_color, v1_color,
					NULL, v0_normals, v1_normals, NULL, v0_tcoords, v1_tcoords, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				has_tcoords = true;
				break;
			}
			
			// process v0 & v1
			// if not has_attrib, default the attribute.
			
			rlVec2 raster_v0, raster_v1;
			rlVec4 color_v0, color_v1;
			rlVec3 normals_v0, normals_v1;
			rlVec2 tcoords_v0, tcoords_v1;

			rlVec4 v0, v1;
			v0.x = v0_position[0];
			v0.y = v0_position[1];
			v0.z = v0_position[2];
			v0.w = v0_position[3];
			v1.x = v1_position[0];
			v1.y = v1_position[1];
			v1.z = v1_position[2];
			v1.w = v1_position[3];
			
			if(has_color)
			{
				if(v0_color[0] < 0.0f) v0_color[0] = 0.0f;
				if(v0_color[0] > 1.0f) v0_color[0] = 1.0f;
				if(v0_color[1] < 0.0f) v0_color[1] = 0.0f;
				if(v0_color[1] > 1.0f) v0_color[1] = 1.0f;
				if(v0_color[2] < 0.0f) v0_color[2] = 0.0f;
				if(v0_color[2] > 1.0f) v0_color[2] = 1.0f;
				if(v0_color[3] < 0.0f) v0_color[3] = 0.0f;
				if(v0_color[3] > 1.0f) v0_color[3] = 1.0f;
				if(v1_color[0] < 0.0f) v1_color[0] = 0.0f;
				if(v1_color[0] > 1.0f) v1_color[0] = 1.0f;
				if(v1_color[1] < 0.0f) v1_color[1] = 0.0f;
				if(v1_color[1] > 1.0f) v1_color[1] = 1.0f;
				if(v1_color[2] < 0.0f) v1_color[2] = 0.0f;
				if(v1_color[2] > 1.0f) v1_color[2] = 1.0f;
				if(v1_color[3] < 0.0f) v1_color[3] = 0.0f;
				if(v1_color[3] > 1.0f) v1_color[3] = 1.0f;
				color_v0.x = v0_color[0], color_v1.x = v1_color[0];
				color_v0.y = v0_color[1], color_v1.y = v1_color[1];
				color_v0.z = v0_color[2], color_v1.z = v1_color[2];
				color_v0.w = v0_color[3], color_v1.w = v1_color[3];
			}
			else
			{
				color_v0.x = 0, color_v0.y = 0, color_v0.z = 0, color_v0.w = 1;
				color_v1.x = 0, color_v1.y = 0, color_v1.z = 0, color_v1.w = 1;
			}
			
			if(has_normals)
			{
				if(v0_normals[0] < 0.0f) v0_normals[0] = 0.0f;
				if(v0_normals[0] > 1.0f) v0_normals[0] = 1.0f;
				if(v0_normals[1] < 0.0f) v0_normals[1] = 0.0f;
				if(v0_normals[1] > 1.0f) v0_normals[1] = 1.0f;
				if(v0_normals[2] < 0.0f) v0_normals[2] = 0.0f;
				if(v0_normals[2] > 1.0f) v0_normals[2] = 1.0f;
				if(v1_normals[0] < 0.0f) v1_normals[0] = 0.0f;
				if(v1_normals[0] > 1.0f) v1_normals[0] = 1.0f;
				if(v1_normals[1] < 0.0f) v1_normals[1] = 0.0f;
				if(v1_normals[1] > 1.0f) v1_normals[1] = 1.0f;
				if(v1_normals[2] < 0.0f) v1_normals[2] = 0.0f;
				if(v1_normals[2] > 1.0f) v1_normals[2] = 1.0f;
				normals_v0.x = v0_normals[0], normals_v1.x = v1_normals[0];
				normals_v0.y = v0_normals[1], normals_v1.y = v1_normals[1];
				normals_v0.z = v0_normals[2], normals_v1.z = v1_normals[2];
			}
			else
			{
				normals_v0.x = 0, normals_v0.y = 0, normals_v0.z = 0;
				normals_v1.x = 0, normals_v1.y = 0, normals_v1.z = 0;
			}
			
			if(has_tcoords)
			{
				if(v0_tcoords[0] < 0.0f) v0_tcoords[0] = 0.0f;
				if(v0_tcoords[0] > 1.0f) v0_tcoords[0] = 1.0f;
				if(v0_tcoords[1] < 0.0f) v0_tcoords[1] = 0.0f;
				if(v0_tcoords[1] > 1.0f) v0_tcoords[1] = 1.0f;
				if(v1_tcoords[0] < 0.0f) v1_tcoords[0] = 0.0f;
				if(v1_tcoords[0] > 1.0f) v1_tcoords[0] = 1.0f;
				if(v1_tcoords[1] < 0.0f) v1_tcoords[1] = 0.0f;
				if(v1_tcoords[1] > 1.0f) v1_tcoords[1] = 1.0f;
				tcoords_v0.x = v0_tcoords[0], tcoords_v1.x = v1_tcoords[0];
				tcoords_v0.y = v0_tcoords[1], tcoords_v1.y = v1_tcoords[1];
			}
			else
			{
				tcoords_v0.x = 0, tcoords_v1.x = 0;
				tcoords_v0.y = 0, tcoords_v1.y = 0;
			}
			
			v0 = _vertex_pass(RL_LINE, v0, color_v0, normals_v0, tcoords_v0);
			v1 = _vertex_pass(RL_LINE, v1, color_v1, normals_v1, tcoords_v1);
			
			if(_rlcore->_mode == RL_LINE || _rlcore->_mode == RL_FILL)
			{
				_process_line(v0, v1, color_v0, color_v1, tcoords_v0, tcoords_v1, width_div_2, height_div_2);
			}
			else if(_rlcore->_mode == RL_POINT)
			{
				_process_point(v0, color_v0, width_div_2, height_div_2);
				_process_point(v1, color_v1, width_div_2, height_div_2);
			}
			
			v += 2;
		}
		if(primitive_type == RL_TRIANGLES)
		{
			switch(_rlcore->_vertex_layout)
			{
				case RL_V3:
				case RL_V4:
				_read_vertex(data, vertex_width, v, 3, v0_position, v1_position, v2_position, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				break;
				case RL_V3_C4:
				case RL_V4_C4:
				_read_vertex(data, vertex_width, v, 3, v0_position, v1_position, v2_position, v0_color, 
					v1_color, v2_color, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				break;
				case RL_V3_N3:
				case RL_V4_N3:
				_read_vertex(data, vertex_width, v, 3, v0_position, v1_position, v2_position, NULL, NULL, 
					NULL, v0_normals, v1_normals, v2_normals, NULL, NULL, NULL);
				has_position = true;
				has_normals = true;
				break;
				case RL_V3_T2:
				case RL_V4_T2:
				_read_vertex(data, vertex_width, v, 3, v0_position, v1_position, v2_position, NULL, NULL, 
					NULL, NULL, NULL, NULL, v0_tcoords, v1_tcoords, v2_tcoords);
				has_position = true;
				has_tcoords = true;
				break;
				case RL_V3_N3_T2:
				case RL_V4_N3_T2:
				_read_vertex(data, vertex_width, v, 3, v0_position, v1_position, v2_position, NULL, NULL, 
					NULL, v0_normals, v1_normals, v2_normals, v0_tcoords, v1_tcoords, v2_tcoords);
				has_position = true;
				has_normals = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3:
				case RL_V4_C4_N3:
				_read_vertex(data, vertex_width, v, 3, v0_position, v1_position, v2_position, v0_color, 
					v1_color, v2_color, v0_normals, v1_normals, v2_normals, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				break;
				case RL_V3_C4_T2:
				case RL_V4_C4_T2:
				_read_vertex(data, vertex_width, v, 3, v0_position, v1_position, v2_position, v0_color, 
					v1_color, v2_color, NULL, NULL, NULL, v0_tcoords, v1_tcoords, v2_tcoords);
				has_position = true;
				has_color = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3_T2:
				case RL_V4_C4_N3_T2:
				_read_vertex(data, vertex_width, v, 3, v0_position, v1_position, v2_position, v0_color, v1_color,
					v2_color, v0_normals, v1_normals, v2_normals, v0_tcoords, v1_tcoords, v2_tcoords);
				has_position = true;
				has_color = true;
				has_normals = true;
				has_tcoords = true;
				break;
			}
			
			// process v0, v1 & v2
			// if not has_attrib, default the attribute.
			
			rlVec4 color_v0, color_v1, color_v2;
			rlVec3 normals_v0, normals_v1, normals_v2;
			rlVec2 tcoords_v0, tcoords_v1, tcoords_v2;
			
			rlVec4 v0, v1, v2;
			v0.x = v0_position[0];
			v0.y = v0_position[1];
			v0.z = v0_position[2];
			v0.w = v0_position[3];
			v1.x = v1_position[0];
			v1.y = v1_position[1];
			v1.z = v1_position[2];
			v1.w = v1_position[3];
			v2.x = v2_position[0];
			v2.y = v2_position[1];
			v2.z = v2_position[2];
			v2.w = v2_position[3];
			
			if(has_color)
			{
				if(v0_color[0] < 0.0f) v0_color[0] = 0.0f;
				if(v0_color[0] > 1.0f) v0_color[0] = 1.0f;
				if(v0_color[1] < 0.0f) v0_color[1] = 0.0f;
				if(v0_color[1] > 1.0f) v0_color[1] = 1.0f;
				if(v0_color[2] < 0.0f) v0_color[2] = 0.0f;
				if(v0_color[2] > 1.0f) v0_color[2] = 1.0f;
				if(v0_color[3] < 0.0f) v0_color[3] = 0.0f;
				if(v0_color[3] > 1.0f) v0_color[3] = 1.0f;
				if(v1_color[0] < 0.0f) v1_color[0] = 0.0f;
				if(v1_color[0] > 1.0f) v1_color[0] = 1.0f;
				if(v1_color[1] < 0.0f) v1_color[1] = 0.0f;
				if(v1_color[1] > 1.0f) v1_color[1] = 1.0f;
				if(v1_color[2] < 0.0f) v1_color[2] = 0.0f;
				if(v1_color[2] > 1.0f) v1_color[2] = 1.0f;
				if(v1_color[3] < 0.0f) v1_color[3] = 0.0f;
				if(v1_color[3] > 1.0f) v1_color[3] = 1.0f;
				if(v2_color[0] < 0.0f) v2_color[0] = 0.0f;
				if(v2_color[0] > 1.0f) v2_color[0] = 1.0f;
				if(v2_color[1] < 0.0f) v2_color[1] = 0.0f;
				if(v2_color[1] > 1.0f) v2_color[1] = 1.0f;
				if(v2_color[2] < 0.0f) v2_color[2] = 0.0f;
				if(v2_color[2] > 1.0f) v2_color[2] = 1.0f;
				if(v2_color[3] < 0.0f) v2_color[3] = 0.0f;
				if(v2_color[3] > 1.0f) v2_color[3] = 1.0f;
				color_v0.x = v0_color[0], color_v1.x = v1_color[0], color_v2.x = v2_color[0];
				color_v0.y = v0_color[1], color_v1.y = v1_color[1], color_v2.y = v2_color[1];
				color_v0.z = v0_color[2], color_v1.z = v1_color[2], color_v2.z = v2_color[2];
				color_v0.w = v0_color[3], color_v1.w = v1_color[3], color_v2.w = v2_color[3];
			}
			else
			{
				color_v0.x = 0, color_v0.y = 0, color_v0.z = 0, color_v0.w = 1;
				color_v1.x = 0, color_v1.y = 0, color_v1.z = 0, color_v1.w = 1;
				color_v2.x = 0, color_v2.y = 0, color_v2.z = 0, color_v2.w = 1;
			}
			
			if(has_normals)
			{
				if(v0_normals[0] < 0.0f) v0_normals[0] = 0.0f;
				if(v0_normals[0] > 1.0f) v0_normals[0] = 1.0f;
				if(v0_normals[1] < 0.0f) v0_normals[1] = 0.0f;
				if(v0_normals[1] > 1.0f) v0_normals[1] = 1.0f;
				if(v0_normals[2] < 0.0f) v0_normals[2] = 0.0f;
				if(v0_normals[2] > 1.0f) v0_normals[2] = 1.0f;
				if(v1_normals[0] < 0.0f) v1_normals[0] = 0.0f;
				if(v1_normals[0] > 1.0f) v1_normals[0] = 1.0f;
				if(v1_normals[1] < 0.0f) v1_normals[1] = 0.0f;
				if(v1_normals[1] > 1.0f) v1_normals[1] = 1.0f;
				if(v1_normals[2] < 0.0f) v1_normals[2] = 0.0f;
				if(v1_normals[2] > 1.0f) v1_normals[2] = 1.0f;
				if(v2_normals[0] < 0.0f) v2_normals[0] = 0.0f;
				if(v2_normals[0] > 1.0f) v2_normals[0] = 1.0f;
				if(v2_normals[1] < 0.0f) v2_normals[1] = 0.0f;
				if(v2_normals[1] > 1.0f) v2_normals[1] = 1.0f;
				if(v2_normals[2] < 0.0f) v2_normals[2] = 0.0f;
				if(v2_normals[2] > 1.0f) v2_normals[2] = 1.0f;
				normals_v0.x = v0_normals[0], normals_v1.x = v1_normals[0], normals_v2.x = v2_normals[0];
				normals_v0.y = v0_normals[1], normals_v1.y = v1_normals[1], normals_v2.y = v2_normals[1];
				normals_v0.z = v0_normals[2], normals_v1.z = v1_normals[2], normals_v2.z = v2_normals[2];
			}
			else
			{
				normals_v0.x = 0, normals_v0.y = 0, normals_v0.z = 0;
				normals_v1.x = 0, normals_v1.y = 0, normals_v1.z = 0;
				normals_v2.x = 0, normals_v2.y = 0, normals_v2.z = 0;
			}
			
			if(has_tcoords)
			{
				if(v0_tcoords[0] < 0.0f) v0_tcoords[0] = 0.0f;
				if(v0_tcoords[0] > 1.0f) v0_tcoords[0] = 1.0f;
				if(v0_tcoords[1] < 0.0f) v0_tcoords[1] = 0.0f;
				if(v0_tcoords[1] > 1.0f) v0_tcoords[1] = 1.0f;
				if(v1_tcoords[0] < 0.0f) v1_tcoords[0] = 0.0f;
				if(v1_tcoords[0] > 1.0f) v1_tcoords[0] = 1.0f;
				if(v1_tcoords[1] < 0.0f) v1_tcoords[1] = 0.0f;
				if(v1_tcoords[1] > 1.0f) v1_tcoords[1] = 1.0f;
				if(v2_tcoords[0] < 0.0f) v2_tcoords[0] = 0.0f;
				if(v2_tcoords[0] > 1.0f) v2_tcoords[0] = 1.0f;
				if(v2_tcoords[1] < 0.0f) v2_tcoords[1] = 0.0f;
				if(v2_tcoords[1] > 1.0f) v2_tcoords[1] = 1.0f;
				tcoords_v0.x = v0_tcoords[0], tcoords_v1.x = v1_tcoords[0], tcoords_v2.x = v2_tcoords[0];
				tcoords_v0.y = v0_tcoords[1], tcoords_v1.y = v1_tcoords[1], tcoords_v2.y = v2_tcoords[1];
			}
			else
			{
				tcoords_v0.x = 0, tcoords_v1.x = 0, tcoords_v2.x = 0;
				tcoords_v0.y = 0, tcoords_v1.y = 0, tcoords_v2.y = 0;
			}
			
			v0 = _vertex_pass(RL_TRIANGLE, v0, color_v0, normals_v0, tcoords_v0);
			v1 = _vertex_pass(RL_TRIANGLE, v1, color_v1, normals_v1, tcoords_v1);
			v2 = _vertex_pass(RL_TRIANGLE, v2, color_v2, normals_v2, tcoords_v2);
			
			if(_rlcore->_mode == RL_FILL)
				_process_triangle(v0, v1, v2, color_v0, color_v1, color_v2, tcoords_v0, tcoords_v1, tcoords_v2, width_div_2, height_div_2);
			if(_rlcore->_mode == RL_LINE)
			{
				_process_line(v0, v1, color_v0, color_v1, tcoords_v0, tcoords_v1, width_div_2, height_div_2);
				_process_line(v1, v2, color_v1, color_v2, tcoords_v1, tcoords_v2, width_div_2, height_div_2);
				_process_line(v2, v0, color_v2, color_v0, tcoords_v2, tcoords_v0, width_div_2, height_div_2);
			}
			else if(_rlcore->_mode == RL_POINT)
			{
				_process_point(v0, color_v0, width_div_2, height_div_2);
				_process_point(v1, color_v1, width_div_2, height_div_2);
				_process_point(v2, color_v2, width_div_2, height_div_2);
			}
			
			v += 3;
		}
	}
}

/* draw primitives described by an array and an index array */
void rlDrawElements(uint32_t primitive_type, uint32_t primitive_count, float* data, uint32_t* elements)
{	
	if(!_rlcore)
		return;
	
	uint32_t vertex_count = 0;
	switch(primitive_type)
	{
		case RL_POINTS:
		vertex_count = primitive_count;
		break;
		case RL_LINES:
		vertex_count = primitive_count * 2;
		break;
		case RL_TRIANGLES:
		vertex_count = primitive_count * 3;
		break;
		default:
		// unhandled error: invalid parameter
		return;
	}
	
	uint32_t vertex_width = 0;
	switch(_rlcore->_vertex_layout)
	{
		case RL_V3:		vertex_width = 3;	break;
		case RL_V3_C4:	vertex_width = 7;	break;
		case RL_V3_N3:	vertex_width = 6;	break;
		case RL_V3_T2:	vertex_width = 5;	break;
		case RL_V3_N3_T2:		vertex_width = 8;	break;
		case RL_V3_C4_N3:		vertex_width = 10;	break;
		case RL_V3_C4_T2:		vertex_width = 9;	break;
		case RL_V3_C4_N3_T2:	vertex_width = 12;	break;
		case RL_V4:		vertex_width = 4;	break;
		case RL_V4_C4:	vertex_width = 8;	break;
		case RL_V4_N3:	vertex_width = 7;	break;
		case RL_V4_T2:	vertex_width = 6;	break;
		case RL_V4_N3_T2:		vertex_width = 9;	break;
		case RL_V4_C4_N3:		vertex_width = 11;	break;
		case RL_V4_C4_T2:		vertex_width = 10;	break;
		case RL_V4_C4_N3_T2:	vertex_width = 13;	break;
		default:
		// unhandled error: unknown vertex layout
		return;
	}
	
	bool mode_valid = (_rlcore->_mode == RL_FILL || _rlcore->_mode == RL_POINT || _rlcore->_mode == RL_LINE);
	
	uint32_t v = 0;
	
	float v0_position[4], v1_position[4], v2_position[4];
	float v0_color[4], v1_color[4], v2_color[4];
	float v0_normals[3], v1_normals[3], v2_normals[3];
	float v0_tcoords[2], v1_tcoords[2], v2_tcoords[2];
	
	float width_div_2  = _rlcore->_width / 2.0f;
	float height_div_2 = _rlcore->_height / 2.0f;

	bool has_position = false;
	bool has_color = false;
	bool has_normals = false;
	bool has_tcoords = false;
	
	for(uint32_t p = 0; p < primitive_count; p += 1)
	{
		if(primitive_type == RL_POINTS)
		{
			switch(_rlcore->_vertex_layout)
			{
				case RL_V3:
				case RL_V4:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				break;
				case RL_V3_C4:
				case RL_V4_C4:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				break;
				case RL_V3_N3:
				case RL_V4_N3:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, v0_normals, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_normals = true;
				break;
				case RL_V3_T2:
				case RL_V4_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, v0_tcoords, NULL, NULL);
				has_position = true;
				has_tcoords = true;
				break;
				case RL_V3_N3_T2:
				case RL_V4_N3_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, v0_normals, NULL, NULL, v0_tcoords, NULL, NULL);
				has_position = true;
				has_normals = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3:
				case RL_V4_C4_N3:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, v0_normals, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				break;
				case RL_V3_C4_T2:
				case RL_V4_C4_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, NULL, NULL, NULL, v0_tcoords, NULL, NULL);
				has_position = true;
				has_color = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3_T2:
				case RL_V4_C4_N3_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, NULL,
					NULL, v0_normals, NULL, NULL, v0_tcoords, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				has_tcoords = true;
				break;
			}
			
			// process v0
			// if not has_attrib, default the attribute.
			
			rlVec4 color_v0;
			rlVec3 normals_v0;
			rlVec2 tcoords_v0;
			
			rlVec4 v0;
			v0.x = v0_position[0];
			v0.y = v0_position[1];
			v0.z = v0_position[2];
			v0.w = v0_position[3];
			
			if(has_color)
			{
				if(v0_color[0] < 0.0f) v0_color[0] = 0.0f;
				if(v0_color[0] > 1.0f) v0_color[0] = 1.0f;
				if(v0_color[1] < 0.0f) v0_color[1] = 0.0f;
				if(v0_color[1] > 1.0f) v0_color[1] = 1.0f;
				if(v0_color[2] < 0.0f) v0_color[2] = 0.0f;
				if(v0_color[2] > 1.0f) v0_color[2] = 1.0f;
				if(v0_color[3] < 0.0f) v0_color[3] = 0.0f;
				if(v0_color[3] > 1.0f) v0_color[3] = 1.0f;
				color_v0.x = v0_color[0];
				color_v0.y = v0_color[1];
				color_v0.z = v0_color[2];
				color_v0.w = v0_color[3];
			}
			else
				color_v0.x = 0, color_v0.y = 0, color_v0.z = 0, color_v0.w = 1;
			
			if(has_normals)
			{
				if(v0_normals[0] < 0.0f) v0_normals[0] = 0.0f;
				if(v0_normals[0] > 1.0f) v0_normals[0] = 1.0f;
				if(v0_normals[1] < 0.0f) v0_normals[1] = 0.0f;
				if(v0_normals[1] > 1.0f) v0_normals[1] = 1.0f;
				if(v0_normals[2] < 0.0f) v0_normals[2] = 0.0f;
				if(v0_normals[2] > 1.0f) v0_normals[2] = 1.0f;
				normals_v0.x = v0_normals[0];
				normals_v0.y = v0_normals[1];
				normals_v0.z = v0_normals[2];
			}
			else
				normals_v0.x = 0, normals_v0.y = 0, normals_v0.z = 0;
			
			if(has_tcoords)
			{
				if(v0_tcoords[0] < 0.0f) v0_tcoords[0] = 0.0f;
				if(v0_tcoords[0] > 1.0f) v0_tcoords[0] = 1.0f;
				if(v0_tcoords[1] < 0.0f) v0_tcoords[1] = 0.0f;
				if(v0_tcoords[1] > 1.0f) v0_tcoords[1] = 1.0f;
				tcoords_v0.x = v0_tcoords[0];
				tcoords_v0.y = v0_tcoords[1];
			}
			
			v0 = _vertex_pass(RL_POINT, v0, color_v0, normals_v0, tcoords_v0);
			
			if(mode_valid)
			{
				_process_point(v0, color_v0, width_div_2, height_div_2);
			}
			
			v += 1;
		}
		if(primitive_type == RL_LINES)
		{
			switch(_rlcore->_vertex_layout)
			{
				case RL_V3:
				case RL_V4:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				break;
				case RL_V3_C4:
				case RL_V4_C4:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, v1_color, 
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				break;
				case RL_V3_N3:
				case RL_V4_N3:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, v0_normals, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, NULL, NULL, 
					NULL, v1_normals, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_normals = true;
				break;
				case RL_V3_T2:
				case RL_V4_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, v0_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, v1_tcoords, NULL, NULL);
				has_position = true;
				has_tcoords = true;
				break;
				case RL_V3_N3_T2:
				case RL_V4_N3_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, v0_normals, NULL, NULL, v0_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, NULL, NULL, 
					NULL, v1_normals, NULL, NULL, v1_tcoords, NULL, NULL);
				has_position = true;
				has_normals = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3:
				case RL_V4_C4_N3:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, v0_normals, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, v1_color, 
					NULL, NULL, v1_normals, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				break;
				case RL_V3_C4_T2:
				case RL_V4_C4_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, NULL, NULL, NULL, v0_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, v1_color, 
					NULL, NULL, NULL, NULL, NULL, v1_tcoords, NULL, NULL);
				has_position = true;
				has_color = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3_T2:
				case RL_V4_C4_N3_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, NULL,
					NULL, v0_normals, NULL, NULL, v0_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, v1_color, NULL,
					NULL, v1_normals, NULL, NULL, v1_tcoords, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				has_tcoords = true;
				break;
			}
			
			// process v0 & v1
			// if not has_attrib, default the attribute.
			
			rlVec2 raster_v0, raster_v1;
			rlVec4 color_v0, color_v1;
			rlVec3 normals_v0, normals_v1;
			rlVec2 tcoords_v0, tcoords_v1;

			rlVec4 v0, v1;
			v0.x = v0_position[0];
			v0.y = v0_position[1];
			v0.z = v0_position[2];
			v0.w = v0_position[3];
			v1.x = v1_position[0];
			v1.y = v1_position[1];
			v1.z = v1_position[2];
			v1.w = v1_position[3];
			
			if(has_color)
			{
				if(v0_color[0] < 0.0f) v0_color[0] = 0.0f;
				if(v0_color[0] > 1.0f) v0_color[0] = 1.0f;
				if(v0_color[1] < 0.0f) v0_color[1] = 0.0f;
				if(v0_color[1] > 1.0f) v0_color[1] = 1.0f;
				if(v0_color[2] < 0.0f) v0_color[2] = 0.0f;
				if(v0_color[2] > 1.0f) v0_color[2] = 1.0f;
				if(v0_color[3] < 0.0f) v0_color[3] = 0.0f;
				if(v0_color[3] > 1.0f) v0_color[3] = 1.0f;
				if(v1_color[0] < 0.0f) v1_color[0] = 0.0f;
				if(v1_color[0] > 1.0f) v1_color[0] = 1.0f;
				if(v1_color[1] < 0.0f) v1_color[1] = 0.0f;
				if(v1_color[1] > 1.0f) v1_color[1] = 1.0f;
				if(v1_color[2] < 0.0f) v1_color[2] = 0.0f;
				if(v1_color[2] > 1.0f) v1_color[2] = 1.0f;
				if(v1_color[3] < 0.0f) v1_color[3] = 0.0f;
				if(v1_color[3] > 1.0f) v1_color[3] = 1.0f;
				color_v0.x = v0_color[0], color_v1.x = v1_color[0];
				color_v0.y = v0_color[1], color_v1.y = v1_color[1];
				color_v0.z = v0_color[2], color_v1.z = v1_color[2];
				color_v0.w = v0_color[3], color_v1.w = v1_color[3];
			}
			else
			{
				color_v0.x = 0, color_v0.y = 0, color_v0.z = 0, color_v0.w = 1;
				color_v1.x = 0, color_v1.y = 0, color_v1.z = 0, color_v1.w = 1;
			}
			
			if(has_normals)
			{
				if(v0_normals[0] < 0.0f) v0_normals[0] = 0.0f;
				if(v0_normals[0] > 1.0f) v0_normals[0] = 1.0f;
				if(v0_normals[1] < 0.0f) v0_normals[1] = 0.0f;
				if(v0_normals[1] > 1.0f) v0_normals[1] = 1.0f;
				if(v0_normals[2] < 0.0f) v0_normals[2] = 0.0f;
				if(v0_normals[2] > 1.0f) v0_normals[2] = 1.0f;
				if(v1_normals[0] < 0.0f) v1_normals[0] = 0.0f;
				if(v1_normals[0] > 1.0f) v1_normals[0] = 1.0f;
				if(v1_normals[1] < 0.0f) v1_normals[1] = 0.0f;
				if(v1_normals[1] > 1.0f) v1_normals[1] = 1.0f;
				if(v1_normals[2] < 0.0f) v1_normals[2] = 0.0f;
				if(v1_normals[2] > 1.0f) v1_normals[2] = 1.0f;
				normals_v0.x = v0_normals[0], normals_v1.x = v1_normals[0];
				normals_v0.y = v0_normals[1], normals_v1.y = v1_normals[1];
				normals_v0.z = v0_normals[2], normals_v1.z = v1_normals[2];
			}
			else
			{
				normals_v0.x = 0, normals_v0.y = 0, normals_v0.z = 0;
				normals_v1.x = 0, normals_v1.y = 0, normals_v1.z = 0;
			}
			
			if(has_tcoords)
			{
				if(v0_tcoords[0] < 0.0f) v0_tcoords[0] = 0.0f;
				if(v0_tcoords[0] > 1.0f) v0_tcoords[0] = 1.0f;
				if(v0_tcoords[1] < 0.0f) v0_tcoords[1] = 0.0f;
				if(v0_tcoords[1] > 1.0f) v0_tcoords[1] = 1.0f;
				if(v1_tcoords[0] < 0.0f) v1_tcoords[0] = 0.0f;
				if(v1_tcoords[0] > 1.0f) v1_tcoords[0] = 1.0f;
				if(v1_tcoords[1] < 0.0f) v1_tcoords[1] = 0.0f;
				if(v1_tcoords[1] > 1.0f) v1_tcoords[1] = 1.0f;
				tcoords_v0.x = v0_tcoords[0], tcoords_v1.x = v1_tcoords[0];
				tcoords_v0.y = v0_tcoords[1], tcoords_v1.y = v1_tcoords[1];
			}
			else
			{
				tcoords_v0.x = 0, tcoords_v1.x = 0;
				tcoords_v0.y = 0, tcoords_v1.y = 0;
			}
			
			v0 = _vertex_pass(RL_LINE, v0, color_v0, normals_v0, tcoords_v0);
			v1 = _vertex_pass(RL_LINE, v1, color_v1, normals_v1, tcoords_v1);
			
			if(_rlcore->_mode == RL_LINE || _rlcore->_mode == RL_FILL)
			{
				_process_line(v0, v1, color_v0, color_v1, tcoords_v0, tcoords_v1, width_div_2, height_div_2);
			}
			else if(_rlcore->_mode == RL_POINT)
			{
				_process_point(v0, color_v0, width_div_2, height_div_2);
				_process_point(v1, color_v1, width_div_2, height_div_2);
			}
			
			v += 2;
		}
		if(primitive_type == RL_TRIANGLES)
		{
			switch(_rlcore->_vertex_layout)
			{
				case RL_V3:
				case RL_V4:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+2], 1, v2_position, NULL, NULL, NULL, NULL,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				break;
				case RL_V3_C4:
				case RL_V4_C4:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, v1_color, 
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+2], 1, v2_position, NULL, NULL, v2_color, 
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				break;
				case RL_V3_N3:
				case RL_V4_N3:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, v0_normals, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, NULL, NULL, 
					NULL, v1_normals, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+2], 1, v2_position, NULL, NULL, NULL, NULL, 
					NULL, v2_normals, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_normals = true;
				break;
				case RL_V3_T2:
				case RL_V4_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, v0_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, v1_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+2], 1, v2_position, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, v2_tcoords, NULL, NULL);
				has_position = true;
				has_tcoords = true;
				break;
				case RL_V3_N3_T2:
				case RL_V4_N3_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, NULL, NULL, 
					NULL, v0_normals, NULL, NULL, v0_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, NULL, NULL, 
					NULL, v1_normals, NULL, NULL, v1_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+2], 1, v2_position, NULL, NULL, NULL, NULL, 
					NULL, v2_normals, NULL, NULL, v2_tcoords, NULL, NULL);
				has_position = true;
				has_normals = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3:
				case RL_V4_C4_N3:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, v0_normals, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, v1_color, 
					NULL, NULL, v1_normals, NULL, NULL, NULL, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+2], 1, v2_position, NULL, NULL, v2_color, 
					NULL, NULL, v2_normals, NULL, NULL, NULL, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				break;
				case RL_V3_C4_T2:
				case RL_V4_C4_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, 
					NULL, NULL, NULL, NULL, NULL, v0_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, v1_color, 
					NULL, NULL, NULL, NULL, NULL, v1_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+2], 1, v2_position, NULL, NULL, v2_color, 
					NULL, NULL, NULL, NULL, NULL, v2_tcoords, NULL, NULL);
				has_position = true;
				has_color = true;
				has_tcoords = true;
				break;
				case RL_V3_C4_N3_T2:
				case RL_V4_C4_N3_T2:
				_read_vertex(data, vertex_width, elements[v], 1, v0_position, NULL, NULL, v0_color, NULL,
					NULL, v0_normals, NULL, NULL, v0_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+1], 1, v1_position, NULL, NULL, v1_color, NULL,
					NULL, v1_normals, NULL, NULL, v1_tcoords, NULL, NULL);
				_read_vertex(data, vertex_width, elements[v+2], 1, v2_position, NULL, NULL, v2_color, NULL,
					NULL, v2_normals, NULL, NULL, v2_tcoords, NULL, NULL);
				has_position = true;
				has_color = true;
				has_normals = true;
				has_tcoords = true;
				break;
			}
			
			// process v0, v1 & v2
			// if not has_attrib, default the attribute.
			
			rlVec4 color_v0, color_v1, color_v2;
			rlVec3 normals_v0, normals_v1, normals_v2;
			rlVec2 tcoords_v0, tcoords_v1, tcoords_v2;
			
			rlVec4 v0, v1, v2;
			v0.x = v0_position[0];
			v0.y = v0_position[1];
			v0.z = v0_position[2];
			v0.w = v0_position[3];
			v1.x = v1_position[0];
			v1.y = v1_position[1];
			v1.z = v1_position[2];
			v1.w = v1_position[3];
			v2.x = v2_position[0];
			v2.y = v2_position[1];
			v2.z = v2_position[2];
			v2.w = v2_position[3];
			
			if(has_color)
			{
				if(v0_color[0] < 0.0f) v0_color[0] = 0.0f;
				if(v0_color[0] > 1.0f) v0_color[0] = 1.0f;
				if(v0_color[1] < 0.0f) v0_color[1] = 0.0f;
				if(v0_color[1] > 1.0f) v0_color[1] = 1.0f;
				if(v0_color[2] < 0.0f) v0_color[2] = 0.0f;
				if(v0_color[2] > 1.0f) v0_color[2] = 1.0f;
				if(v0_color[3] < 0.0f) v0_color[3] = 0.0f;
				if(v0_color[3] > 1.0f) v0_color[3] = 1.0f;
				if(v1_color[0] < 0.0f) v1_color[0] = 0.0f;
				if(v1_color[0] > 1.0f) v1_color[0] = 1.0f;
				if(v1_color[1] < 0.0f) v1_color[1] = 0.0f;
				if(v1_color[1] > 1.0f) v1_color[1] = 1.0f;
				if(v1_color[2] < 0.0f) v1_color[2] = 0.0f;
				if(v1_color[2] > 1.0f) v1_color[2] = 1.0f;
				if(v1_color[3] < 0.0f) v1_color[3] = 0.0f;
				if(v1_color[3] > 1.0f) v1_color[3] = 1.0f;
				if(v2_color[0] < 0.0f) v2_color[0] = 0.0f;
				if(v2_color[0] > 1.0f) v2_color[0] = 1.0f;
				if(v2_color[1] < 0.0f) v2_color[1] = 0.0f;
				if(v2_color[1] > 1.0f) v2_color[1] = 1.0f;
				if(v2_color[2] < 0.0f) v2_color[2] = 0.0f;
				if(v2_color[2] > 1.0f) v2_color[2] = 1.0f;
				if(v2_color[3] < 0.0f) v2_color[3] = 0.0f;
				if(v2_color[3] > 1.0f) v2_color[3] = 1.0f;
				color_v0.x = v0_color[0], color_v1.x = v1_color[0], color_v2.x = v2_color[0];
				color_v0.y = v0_color[1], color_v1.y = v1_color[1], color_v2.y = v2_color[1];
				color_v0.z = v0_color[2], color_v1.z = v1_color[2], color_v2.z = v2_color[2];
				color_v0.w = v0_color[3], color_v1.w = v1_color[3], color_v2.w = v2_color[3];
			}
			else
			{
				color_v0.x = 0, color_v0.y = 0, color_v0.z = 0, color_v0.w = 1;
				color_v1.x = 0, color_v1.y = 0, color_v1.z = 0, color_v1.w = 1;
				color_v2.x = 0, color_v2.y = 0, color_v2.z = 0, color_v2.w = 1;
			}
			
			if(has_normals)
			{
				if(v0_normals[0] < 0.0f) v0_normals[0] = 0.0f;
				if(v0_normals[0] > 1.0f) v0_normals[0] = 1.0f;
				if(v0_normals[1] < 0.0f) v0_normals[1] = 0.0f;
				if(v0_normals[1] > 1.0f) v0_normals[1] = 1.0f;
				if(v0_normals[2] < 0.0f) v0_normals[2] = 0.0f;
				if(v0_normals[2] > 1.0f) v0_normals[2] = 1.0f;
				if(v1_normals[0] < 0.0f) v1_normals[0] = 0.0f;
				if(v1_normals[0] > 1.0f) v1_normals[0] = 1.0f;
				if(v1_normals[1] < 0.0f) v1_normals[1] = 0.0f;
				if(v1_normals[1] > 1.0f) v1_normals[1] = 1.0f;
				if(v1_normals[2] < 0.0f) v1_normals[2] = 0.0f;
				if(v1_normals[2] > 1.0f) v1_normals[2] = 1.0f;
				if(v2_normals[0] < 0.0f) v2_normals[0] = 0.0f;
				if(v2_normals[0] > 1.0f) v2_normals[0] = 1.0f;
				if(v2_normals[1] < 0.0f) v2_normals[1] = 0.0f;
				if(v2_normals[1] > 1.0f) v2_normals[1] = 1.0f;
				if(v2_normals[2] < 0.0f) v2_normals[2] = 0.0f;
				if(v2_normals[2] > 1.0f) v2_normals[2] = 1.0f;
				normals_v0.x = v0_normals[0], normals_v1.x = v1_normals[0], normals_v2.x = v2_normals[0];
				normals_v0.y = v0_normals[1], normals_v1.y = v1_normals[1], normals_v2.y = v2_normals[1];
				normals_v0.z = v0_normals[2], normals_v1.z = v1_normals[2], normals_v2.z = v2_normals[2];
			}
			else
			{
				normals_v0.x = 0, normals_v0.y = 0, normals_v0.z = 0;
				normals_v1.x = 0, normals_v1.y = 0, normals_v1.z = 0;
				normals_v2.x = 0, normals_v2.y = 0, normals_v2.z = 0;
			}
			
			if(has_tcoords)
			{
				if(v0_tcoords[0] < 0.0f) v0_tcoords[0] = 0.0f;
				if(v0_tcoords[0] > 1.0f) v0_tcoords[0] = 1.0f;
				if(v0_tcoords[1] < 0.0f) v0_tcoords[1] = 0.0f;
				if(v0_tcoords[1] > 1.0f) v0_tcoords[1] = 1.0f;
				if(v1_tcoords[0] < 0.0f) v1_tcoords[0] = 0.0f;
				if(v1_tcoords[0] > 1.0f) v1_tcoords[0] = 1.0f;
				if(v1_tcoords[1] < 0.0f) v1_tcoords[1] = 0.0f;
				if(v1_tcoords[1] > 1.0f) v1_tcoords[1] = 1.0f;
				if(v2_tcoords[0] < 0.0f) v2_tcoords[0] = 0.0f;
				if(v2_tcoords[0] > 1.0f) v2_tcoords[0] = 1.0f;
				if(v2_tcoords[1] < 0.0f) v2_tcoords[1] = 0.0f;
				if(v2_tcoords[1] > 1.0f) v2_tcoords[1] = 1.0f;
				tcoords_v0.x = v0_tcoords[0], tcoords_v1.x = v1_tcoords[0], tcoords_v2.x = v2_tcoords[0];
				tcoords_v0.y = v0_tcoords[1], tcoords_v1.y = v1_tcoords[1], tcoords_v2.y = v2_tcoords[1];
			}
			else
			{
				tcoords_v0.x = 0, tcoords_v1.x = 0, tcoords_v2.x = 0;
				tcoords_v0.y = 0, tcoords_v1.y = 0, tcoords_v2.y = 0;
			}
			
			v0 = _vertex_pass(RL_TRIANGLE, v0, color_v0, normals_v0, tcoords_v0);
			v1 = _vertex_pass(RL_TRIANGLE, v1, color_v1, normals_v1, tcoords_v1);
			v2 = _vertex_pass(RL_TRIANGLE, v2, color_v2, normals_v2, tcoords_v2);
			
			if(_rlcore->_mode == RL_FILL)
				_process_triangle(v0, v1, v2, color_v0, color_v1, color_v2, tcoords_v0, tcoords_v1, tcoords_v2, width_div_2, height_div_2);
			if(_rlcore->_mode == RL_LINE)
			{
				_process_line(v0, v1, color_v0, color_v1, tcoords_v0, tcoords_v1, width_div_2, height_div_2);
				_process_line(v1, v2, color_v1, color_v2, tcoords_v1, tcoords_v2, width_div_2, height_div_2);
				_process_line(v2, v0, color_v2, color_v0, tcoords_v2, tcoords_v0, width_div_2, height_div_2);
			}
			else if(_rlcore->_mode == RL_POINT)
			{
				_process_point(v0, color_v0, width_div_2, height_div_2);
				_process_point(v1, color_v1, width_div_2, height_div_2);
				_process_point(v2, color_v2, width_div_2, height_div_2);
			}
			
			v += 3;
		}
	}
}

/* enable a state. */
void rlEnable(uint32_t state)
{
	if(!_rlcore)
		return;
	
	switch(state)
	{
		case RL_PERSPECTIVE_CORRECTION:
			_rlcore->_persp_corr = true;
			break;
		case RL_BLEND:
			_rlcore->_blend = true;
			break;
		case RL_TEXTURE:
			_rlcore->_texture = true;
			break;
		case RL_DEPTH_TEST:
			_rlcore->_depth_test = true;
			break;
		case RL_DEPTH_WRITE:
			_rlcore->_write_depth = true;
			break;
		case RL_CULL:
			_rlcore->_cull = true;
			break;
		case RL_CLIP:
			_rlcore->_clip = true;
			break;
		case RL_PERSPECTIVE_DIVISION:
			_rlcore->_persp_div = true;
			break;
		case RL_SCALE_Z:
			_rlcore->_scale_z = true;
			break;
		case RL_PRIMITIVE_TYPE:
			_rlcore->_sh_primitive_type = true;
			break;
		case RL_VERTEX_ARRAY:
			_rlcore->_sh_vertex_array = true;
			break;
		case RL_COLOR_ARRAY:
			_rlcore->_sh_color_array = true;
			break;
		case RL_NORMAL_ARRAY:
			_rlcore->_sh_normal_array = true;
			break;
		case RL_TEXCOORD_ARRAY:
			_rlcore->_sh_texcoord_array = true;
			break;
		case RL_PRIMARY_COLOR:
			_rlcore->_sh_primary_color = true;
			break;
		case RL_SECONDARY_COLOR:
			_rlcore->_sh_secondary_color = true;
			break;
		case RL_BARY_LINEAR:
			_rlcore->_sh_bary_linear = true;
			break;
		case RL_BARY_PERSPECTIVE:
			_rlcore->_sh_bary_perspective = true;
			break;
		case RL_DST_DEPTH:
			_rlcore->_sh_dst_depth = true;
			break;
		case RL_FRAG_DEPTH:
			_rlcore->_sh_frag_depth = true;
			break;
		case RL_FRAG_X_COORD:
			_rlcore->_sh_frag_x_coord = true;
			break;
		case RL_FRAG_Y_COORD:
			_rlcore->_sh_frag_y_coord = true;
			break;
		case RL_V3:
		case RL_V3_C4:
		case RL_V3_N3:
		case RL_V3_T2:
		case RL_V3_N3_T2:
		case RL_V3_C4_N3:
		case RL_V3_C4_T2:
		case RL_V3_C4_N3_T2:
		case RL_V4:
		case RL_V4_C4:
		case RL_V4_N3:
		case RL_V4_T2:
		case RL_V4_N3_T2:
		case RL_V4_C4_N3:
		case RL_V4_C4_T2:
		case RL_V4_C4_N3_T2:
			_rlcore->_vertex_layout = state;
			break;
	}
}

/* disable a state. */
void rlDisable(uint32_t state)
{
	if(!_rlcore)
		return;
	
	switch(state)
	{
		case RL_PERSPECTIVE_CORRECTION:
			_rlcore->_persp_corr = false;
			break;
		case RL_BLEND:
			_rlcore->_blend = false;
			break;
		case RL_TEXTURE:
			_rlcore->_texture = false;
			break;
		case RL_DEPTH_TEST:
			_rlcore->_depth_test = false;
			break;
		case RL_DEPTH_WRITE:
			_rlcore->_write_depth = false;
			break;
		case RL_CULL:
			_rlcore->_cull = false;
			break;
		case RL_CLIP:
			_rlcore->_clip = false;
			break;
		case RL_PERSPECTIVE_DIVISION:
			_rlcore->_persp_div = false;
			break;
		case RL_SCALE_Z:
			_rlcore->_scale_z = false;
			break;
		case RL_PRIMITIVE_TYPE:
			_rlcore->_sh_primitive_type = false;
			break;
		case RL_VERTEX_ARRAY:
			_rlcore->_sh_vertex_array = false;
			break;
		case RL_COLOR_ARRAY:
			_rlcore->_sh_color_array = false;
			break;
		case RL_NORMAL_ARRAY:
			_rlcore->_sh_normal_array = false;
			break;
		case RL_TEXCOORD_ARRAY:
			_rlcore->_sh_texcoord_array = false;
			break;
		case RL_PRIMARY_COLOR:
			_rlcore->_sh_primary_color = false;
			break;
		case RL_SECONDARY_COLOR:
			_rlcore->_sh_secondary_color = false;
			break;
		case RL_BARY_LINEAR:
			_rlcore->_sh_bary_linear = false;
			break;
		case RL_BARY_PERSPECTIVE:
			_rlcore->_sh_bary_perspective = false;
			break;
		case RL_DST_DEPTH:
			_rlcore->_sh_dst_depth = false;
			break;
		case RL_FRAG_DEPTH:
			_rlcore->_sh_frag_depth = false;
			break;
		case RL_FRAG_X_COORD:
			_rlcore->_sh_frag_x_coord = false;
			break;
		case RL_FRAG_Y_COORD:
			_rlcore->_sh_frag_y_coord = false;
			break;
		case RL_V3:
		case RL_V3_C4:
		case RL_V3_N3:
		case RL_V3_T2:
		case RL_V3_N3_T2:
		case RL_V3_C4_N3:
		case RL_V3_C4_T2:
		case RL_V3_C4_N3_T2:
		case RL_V4:
		case RL_V4_C4:
		case RL_V4_N3:
		case RL_V4_T2:
		case RL_V4_N3_T2:
		case RL_V4_C4_N3:
		case RL_V4_C4_T2:
		case RL_V4_C4_N3_T2:
			_rlcore->_vertex_layout = RL_V3;
			break;
	}
}

/* check if a state is enabled. */
bool rlIsEnabled(uint32_t state)
{
	if(!_rlcore)
		return false;
	
	switch(state)
	{
		case RL_PERSPECTIVE_CORRECTION:
			return _rlcore->_persp_corr;
		case RL_BLEND:
			return _rlcore->_blend;
		case RL_TEXTURE:
			return _rlcore->_texture;
		case RL_DEPTH_TEST:
			return _rlcore->_depth_test;
		case RL_DEPTH_WRITE:
			return _rlcore->_write_depth;
		case RL_CULL:
			return _rlcore->_cull;
		case RL_CLIP:
			return _rlcore->_clip;
		case RL_PERSPECTIVE_DIVISION:
			return _rlcore->_persp_div;
		case RL_SCALE_Z:
			return _rlcore->_scale_z;
		case RL_PRIMITIVE_TYPE:
			return _rlcore->_sh_primitive_type;
		case RL_VERTEX_ARRAY:
			return _rlcore->_sh_vertex_array;
		case RL_COLOR_ARRAY:
			return _rlcore->_sh_color_array;
		case RL_NORMAL_ARRAY:
			return _rlcore->_sh_normal_array;
		case RL_TEXCOORD_ARRAY:
			return _rlcore->_sh_texcoord_array;
		case RL_PRIMARY_COLOR:
			return _rlcore->_sh_primary_color;
		case RL_SECONDARY_COLOR:
			return _rlcore->_sh_secondary_color;
		case RL_BARY_LINEAR:
			return _rlcore->_sh_bary_linear;
		case RL_BARY_PERSPECTIVE:
			return _rlcore->_sh_bary_perspective;
		case RL_DST_DEPTH:
			return _rlcore->_sh_dst_depth;
		case RL_FRAG_DEPTH:
			return _rlcore->_sh_frag_depth;
		case RL_FRAG_X_COORD:
			return _rlcore->_sh_frag_x_coord;
		case RL_FRAG_Y_COORD:
			return _rlcore->_sh_frag_y_coord;
		case RL_V3:
			return (_rlcore->_vertex_layout == RL_V3);
		case RL_V3_C4:
			return (_rlcore->_vertex_layout == RL_V3_C4);
		case RL_V3_N3:
			return (_rlcore->_vertex_layout == RL_V3_N3);
		case RL_V3_T2:
			return (_rlcore->_vertex_layout == RL_V3_T2);
		case RL_V3_N3_T2:
			return (_rlcore->_vertex_layout == RL_V3_N3_T2);
		case RL_V3_C4_N3:
			return (_rlcore->_vertex_layout == RL_V3_C4_N3);
		case RL_V3_C4_T2:
			return (_rlcore->_vertex_layout == RL_V3_C4_T2);
		case RL_V3_C4_N3_T2:
			return (_rlcore->_vertex_layout == RL_V3_C4_N3_T2);
		case RL_V4:
			return (_rlcore->_vertex_layout == RL_V4);
		case RL_V4_C4:
			return (_rlcore->_vertex_layout == RL_V4_C4);
		case RL_V4_N3:
			return (_rlcore->_vertex_layout == RL_V4_N3);
		case RL_V4_T2:
			return (_rlcore->_vertex_layout == RL_V4_T2);
		case RL_V4_N3_T2:
			return (_rlcore->_vertex_layout == RL_V4_N3_T2);
		case RL_V4_C4_N3:
			return (_rlcore->_vertex_layout == RL_V4_C4_N3);
		case RL_V4_C4_T2:
			return (_rlcore->_vertex_layout == RL_V4_C4_T2);
		case RL_V4_C4_N3_T2:
			return (_rlcore->_vertex_layout == RL_V4_C4_N3_T2);
	}
}

/* set polygon mode. */
void rlPolygonMode(uint32_t mode)
{
	if(!_rlcore)
		return;
	
	switch(mode)
	{
		case RL_POINT:
			_rlcore->_mode = RL_POINT;
			break;
		case RL_LINE:
			_rlcore->_mode = RL_LINE;
			break;
		case RL_FILL:
			_rlcore->_mode = RL_FILL;
			break;
	}
}

/* specify cull winding */
void rlCullWinding(uint32_t winding)
{
	if(!_rlcore)
		return;
	
	if(winding == RL_CW)
		_rlcore->_cull_winding = RL_CW;
	if(winding == RL_CCW)
		_rlcore->_cull_winding = RL_CCW;
		
	// unhandled error: invalid parameter
}

/* set radius of points. */
void rlPointSize(float radius)
{
	if(!_rlcore)
		return;
	
	if(radius >= 0.0f)
		_rlcore->_point_radius = radius;
	else
		_rlcore->_point_radius = 0.0f;
}

/* allocate a display buffer. */
void rlCreateBuffer(uint32_t type, uint32_t width, uint32_t height, void** buffer)
{
	if(width + height < 2)
		// unhandled error: invalid buffer dimensions
		return;
	switch(type)
	{
		case RL_RGB16:
		case RL_RGBA16:
			*((uint16_t**)buffer) = (uint16_t*) calloc(width*height, sizeof(uint16_t));
			break;
		case RL_RGB32:
		case RL_RGBA32:
			*((uint32_t**)buffer) = (uint32_t*) calloc(width*height, sizeof(uint32_t));
			break;
		case RL_D16:
			*((uint16_t**)buffer) = (uint16_t*) calloc(width*height, sizeof(uint16_t));
			break;
		case RL_D32:
			*((uint32_t**)buffer) = (uint32_t*) calloc(width*height, sizeof(uint32_t));
			break;
		default:
			// unhandled error: invalid buffer type
			buffer = NULL;
	}
}

/* bind a display buffer to front set. Buffer must have same dimensions as any already bound. 'type' is a depth or pixel format. */
void rlBindBuffer(uint32_t type, uint32_t width, uint32_t height, void* buffer)
{
	if(!_rlcore)
		return;
	
	if(!buffer)
	{
		// unhandled error: no buffer passed
		return;
	}
	if(_rlcore->_colorbuffer || _rlcore->_depthbuffer)
	{
		if(width != _rlcore->_width || height != _rlcore->_height)
		{
			// unhandled error: invalid buffer dimensions
			return;
		}
	}
	switch(type)
	{
		case RL_RGB16:	
		case RL_RGB32:
		case RL_RGBA16:	
		case RL_RGBA32:
			_rlcore->_colorbuffer = buffer;
			_rlcore->_cb_type = type;
			break;
		case RL_D16:
		case RL_D32:
			_rlcore->_depthbuffer = buffer;
			_rlcore->_db_type = type;
			break;
		default:
			// unhandled error: no buffer passed 
			return;
	}
	_rlcore->_width = width;
	_rlcore->_height = height;
}

/* unbind a buffer from the front set, if bound. OR together all desired RL_*_BUFFER_BIT bits. May reset dimensions. */
void rlUnbindBuffer(uint32_t buffers)
{
	if(!_rlcore)
		return;
	
	if(buffers & RL_COLOR_BUFFER_BIT)
		_rlcore->_colorbuffer = NULL;
	if(buffers & RL_DEPTH_BUFFER_BIT)
		_rlcore->_depthbuffer = NULL;
	if(!_rlcore->_colorbuffer && !_rlcore->_depthbuffer)
	{
		_rlcore->_width = 0;
		_rlcore->_height = 0;
	}
}

/* swap front buffers with back buffers */
void rlSwapBuffers()
{
	if(!_rlcore)
		return;
	
	void* cb = _rlcore->_colorbuffer;
	void* db = _rlcore->_depthbuffer;
	uint32_t cb_type = _rlcore->_cb_type;
	uint32_t db_type = _rlcore->_db_type;
	uint32_t width = _rlcore->_width;
	uint32_t height = _rlcore->_height;
	
	_rlcore->_colorbuffer = _rlcore->_back_colorbuffer;
	_rlcore->_depthbuffer = _rlcore->_back_depthbuffer;
	_rlcore->_cb_type = _rlcore->_back_cb_type;
	_rlcore->_db_type = _rlcore->_back_db_type;
	_rlcore->_width = _rlcore->_back_width;
	_rlcore->_height = _rlcore->_back_height;

	_rlcore->_back_colorbuffer = cb;
	_rlcore->_back_depthbuffer = db;
	_rlcore->_back_cb_type = cb_type;
	_rlcore->_back_db_type = db_type;
	_rlcore->_back_width = width;
	_rlcore->_back_height = height;
}

/* get dimensions of front or back buffer set in pre-allocated (width, height) array. Note dimensions will be 0 if no buffers are bound. */
void rlGetBufferSize(uint32_t buffer, uint32_t* dimensions)
{
	if(!_rlcore)
		return;
	
	if(buffer == RL_FRONT_BUFFERS)
	{
		dimensions[0] = _rlcore->_width;
		dimensions[1] = _rlcore->_height;
		return;
	}
	if(buffer == RL_BACK_BUFFERS)
	{
		dimensions[0] = _rlcore->_back_width;
		dimensions[1] = _rlcore->_back_height;
		return;
	}
	// unhandled error: invalid parameter
}

/* check if there is a specific buffer in the front buffer set. */
bool rlIsBuffer(uint32_t buffer)
{
	if(!_rlcore)
		return false;
	
	if(buffer & RL_COLOR_BUFFER_BIT)
	{
		if(_rlcore->_colorbuffer)
			return true;
		return false;
	}
	if(buffer & RL_DEPTH_BUFFER_BIT)
	{
		if(_rlcore->_depthbuffer)
			return true;
		return false;
	}
	// unhandled error: invalid parameter
	return false;
}

/* get the depth range of depth buffer in front set. Returns 0 in absence of depth buffer. */
int64_t rlMaxDepth()
{
	if(!_rlcore)
		return 0;
	
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D16)
		return 0xFFFF;
	if(_rlcore->_depthbuffer && _rlcore->_db_type == RL_D32)
		return 0xFFFFFFFF;
	return 0;
}

/* set clear depth for rlClear.  */
void rlClearDepth(float depth)
{
	if(!_rlcore)
		return;
	
	if(depth > 1.0f)
		depth = 1.0f;
	if(depth < 0.0f)
		depth = 0.0f;
	
	if(_rlcore->_db_type == RL_D16)
		_rlcore->_clear_depth = depth * 0xFFFF;
	if(_rlcore->_db_type == RL_D32)
		_rlcore->_clear_depth = depth * 0xFFFFFFFF;
		
	// unhandled error: no depth buffer bound
}

/* set clear color for rlClear. */
void rlClearColor(float red, float green, float blue)
{
	if(!_rlcore)
		return;
	
	if(red < 0.0f)		red = 0.0f;
	if(red > 1.0f)		red = 1.0f;
	if(green < 0.0f)	green = 0.0f;
	if(green > 1.0f)	green = 1.0f;
	if(blue < 0.0f)		blue = 0.0f;
	if(blue > 1.0f)		blue = 1.0f;
	
	if(_rlcore->_cb_type == RL_RGB16 || _rlcore->_cb_type == RL_RGBA16)
		_rlcore->_clear_color = _RL_RGBA16((uint8_t)(red*31.0f), (uint8_t)(green*31.0f), (uint8_t)(blue*31.0f), 1);
	if(_rlcore->_cb_type == RL_RGB32 || _rlcore->_cb_type == RL_RGBA32)
		_rlcore->_clear_color = _RL_RGBA32((uint8_t)(red*255.0f), (uint8_t)(green*255.0f), (uint8_t)(blue*255.0f), 255);
	
	// unhandled error: no color buffer bound
}

/* clear back buffers. OR together buffer constants. */
void rlClear(uint32_t buffers)
{
	if(!_rlcore)
		return;
	
	uint64_t pcount = _rlcore->_width * _rlcore->_height;
	if(buffers & RL_COLOR_BUFFER_BIT && buffers & RL_DEPTH_BUFFER_BIT && _rlcore->_back_colorbuffer && _rlcore->_back_depthbuffer)
	{
		if((_rlcore->_back_cb_type == RL_RGB16 || _rlcore->_back_cb_type == RL_RGBA16) && _rlcore->_back_db_type == RL_D16)
		{
			uint16_t* cb = (uint16_t*) _rlcore->_back_colorbuffer;
			uint16_t* db = (uint16_t*) _rlcore->_back_depthbuffer;
			uint16_t color = _rlcore->_clear_color;
			uint16_t depth = (_rlcore->_clear_depth > 0 && _rlcore->_clear_depth <= 0xFFFF) ? _rlcore->_clear_depth : 0xFFFF;
			for(uint64_t p = 0; p < pcount; p += 1)
			{
				cb[p] = color;
				db[p] = depth;
			}
		}
		if((_rlcore->_back_cb_type == RL_RGB32 || _rlcore->_back_cb_type == RL_RGBA32) && _rlcore->_back_db_type == RL_D16)
		{
			uint32_t* cb = (uint32_t*) _rlcore->_back_colorbuffer;
			uint16_t* db = (uint16_t*) _rlcore->_back_depthbuffer;
			uint32_t color = _rlcore->_clear_color;
			uint16_t depth = (_rlcore->_clear_depth > 0 && _rlcore->_clear_depth <= 0xFFFF) ? _rlcore->_clear_depth : 0xFFFF;
			for(uint64_t p = 0; p < pcount; p += 1)
			{
				cb[p] = color;
				db[p] = depth;
			}
		}
		if((_rlcore->_back_cb_type == RL_RGB16 || _rlcore->_back_cb_type == RL_RGBA16) && _rlcore->_back_db_type == RL_D32)
		{
			uint16_t* cb = (uint16_t*) _rlcore->_back_colorbuffer;
			uint32_t* db = (uint32_t*) _rlcore->_back_depthbuffer;
			uint16_t color = _rlcore->_clear_color;
			uint32_t depth = (_rlcore->_clear_depth > 0 && _rlcore->_clear_depth <= 0xFFFFFFFF) ? _rlcore->_clear_depth : 0xFFFFFFFF;
			for(uint64_t p = 0; p < pcount; p += 1)
			{
				cb[p] = color;
				db[p] = depth;
			}
		}
		if((_rlcore->_back_cb_type == RL_RGB32 || _rlcore->_back_cb_type == RL_RGBA32) && _rlcore->_back_db_type == RL_D32)
		{
			uint32_t* cb = (uint32_t*) _rlcore->_back_colorbuffer;
			uint32_t* db = (uint32_t*) _rlcore->_back_depthbuffer;
			uint32_t color = _rlcore->_clear_color;
			uint32_t depth = (_rlcore->_clear_depth > 0 && _rlcore->_clear_depth <= 0xFFFFFFFF) ? _rlcore->_clear_depth : 0xFFFFFFFF;
			for(uint64_t p = 0; p < pcount; p += 1)
			{
				cb[p] = color;
				db[p] = depth;
			}
		}
	}
	else if(buffers & RL_COLOR_BUFFER_BIT && _rlcore->_back_colorbuffer)
	{
		if(_rlcore->_back_cb_type == RL_RGB16 || _rlcore->_back_cb_type == RL_RGBA16)
		{
			uint16_t* cb = (uint16_t*) _rlcore->_back_colorbuffer;
			uint16_t color = _rlcore->_clear_color;
			for(uint64_t p = 0; p < pcount; p += 1)
				cb[p] = color;
		}
		if(_rlcore->_back_cb_type == RL_RGB32 || _rlcore->_back_cb_type == RL_RGBA32)
		{
			uint32_t* cb = (uint32_t*) _rlcore->_back_colorbuffer;
			uint32_t color = _rlcore->_clear_color;
			for(uint64_t p = 0; p < pcount; p += 1)
				cb[p] = color;
		}
	}
	else if(buffers & RL_DEPTH_BUFFER_BIT && _rlcore->_back_depthbuffer)
	{
		if(_rlcore->_back_db_type == RL_D16)
		{
			uint16_t* db = (uint16_t*) _rlcore->_back_depthbuffer;
			uint16_t depth = (_rlcore->_clear_depth > 0 && _rlcore->_clear_depth <= 0xFFFF) ? _rlcore->_clear_depth : 0xFFFF;
			for(uint64_t p = 0; p < pcount; p += 1)
				db[p] = depth;
		}
		if(_rlcore->_back_db_type == RL_D32)
		{
			uint32_t* db = (uint32_t*) _rlcore->_back_depthbuffer;
			uint32_t depth = (_rlcore->_clear_depth > 0 && _rlcore->_clear_depth <= 0xFFFFFFFF) ? _rlcore->_clear_depth : 0xFFFFFFFF;
			for(uint64_t p = 0; p < pcount; p += 1)
				db[p] = depth;
		}
	}
}

/* sample currently active texture unit. (0,0) is bottom left and (1,1) is top right. Returns (0,0,0,1) if texture unit incomplete. */
rlVec4 rlSampleTexture(float x, float y)
{
	if(!_rlcore)
	{
		rlVec4 nil;
		return nil;
	}
	
	uint32_t format = _rlcore->_texture_formats[_rlcore->_texture_unit];
	if(!_rlcore->_textures[_rlcore->_texture_unit]
	|| _rlcore->_texture_widths[_rlcore->_texture_unit] == 0
	|| _rlcore->_texture_heights[_rlcore->_texture_unit] == 0
	|| (format != RL_RGB16 && 
		format != RL_RGBA16 &&
		format != RL_RGB32 &&
		format != RL_RGBA32) )		// incomplete texture unit
	{
		rlVec4 black;
		black.x = 0, black.y = 0;
		black.z = 0, black.w = 1;
		return black;
	}
	
	if(x < 0.0f) x = 0.0f;
	if(x > 1.0f) x = 1.0f;
	if(y < 0.0f) y = 0.0f;
	if(y > 1.0f) y = 1.0f;
	
	uint32_t tx = x * (_rlcore->_texture_widths[_rlcore->_texture_unit] - 1);
	uint32_t ty = (1.0f - y) * (_rlcore->_texture_heights[_rlcore->_texture_unit] - 1);
	
	rlVec4 color;
	_get_texel(x, y, &color, _rlcore->_textures[_rlcore->_texture_unit],
		_rlcore->_texture_formats[_rlcore->_texture_unit], _rlcore->_texture_widths[_rlcore->_texture_unit], 
		_rlcore->_texture_compressed_booleans[_rlcore->_texture_unit]);
	return color;
}

/* set active texture unit */
void rlActiveTexture(uint8_t unit)
{
	if(!_rlcore)
		return;
	_rlcore->_texture_unit = unit;
}

/* provide information for active texture unit. Pass 0 as data to reset all values */
void rlTexture(void* data, uint32_t format, uint32_t width, uint32_t height, bool compressed)
{
	if(!_rlcore)
		return;
	
	uint8_t unit = _rlcore->_texture_unit;
	
	if(!data)	// reset all values
	{
		_rlcore->_textures[unit] = 0;
		_rlcore->_texture_formats[unit] = 0;
		_rlcore->_texture_widths[unit] = 0;
		_rlcore->_texture_heights[unit] = 0;
		_rlcore->_texture_compressed_booleans[unit] = false;
		return;
	}
	if((format != RL_RGB16 && format != RL_RGBA16 && format != RL_RGB32 && format != RL_RGBA32)
	|| width == 0 || height == 0)
	{
		// unhandled error: incomplete texture data
		return;
	}
	_rlcore->_textures[unit] = data;
	_rlcore->_texture_formats[unit] = format;
	_rlcore->_texture_widths[unit] = width;
	_rlcore->_texture_heights[unit] = height;
	_rlcore->_texture_compressed_booleans[unit] = compressed;
}

/* bind a shader. */
void rlBindShader(uint32_t type, void* shader)
{
	if(!_rlcore)
		return;
	
	if(type == RL_VERTEX_SHADER)
	{
		rlVec4 (*ptr)(void*, uint32_t*, uint32_t) = (rlVec4 (*)(void*, uint32_t*, uint32_t)) shader;
		_rlcore->_vshader = ptr;
	}
	if(type == RL_FRAGMENT_SHADER)
	{
		rlVec4 (*ptr)(void*, uint32_t*, uint32_t, bool*) = (rlVec4 (*)(void*, uint32_t*, uint32_t, bool*)) shader;
		_rlcore->_fshader = ptr;
	}
}

/* compute a * b */
rlMat4 rlMat4Mat4(rlMat4 a, rlMat4 b)
{
	rlMat4 p;
	// first row
	p.m00 = a.m00*b.m00 + a.m01*b.m10 + a.m02*b.m20 + a.m03*b.m30;
	p.m01 = a.m00*b.m01 + a.m01*b.m11 + a.m02*b.m21 + a.m03*b.m31;
	p.m02 = a.m00*b.m02 + a.m01*b.m12 + a.m02*b.m22 + a.m03*b.m32;
	p.m03 = a.m00*b.m03 + a.m01*b.m13 + a.m02*b.m23 + a.m03*b.m33;
	// second row
	p.m10 = a.m10*b.m00 + a.m11*b.m10 + a.m12*b.m20 + a.m13*b.m30;
	p.m11 = a.m10*b.m01 + a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	p.m12 = a.m10*b.m02 + a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	p.m13 = a.m10*b.m03 + a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;
	// third row
	p.m20 = a.m20*b.m00 + a.m21*b.m10 + a.m22*b.m20 + a.m23*b.m30;
	p.m21 = a.m20*b.m01 + a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	p.m22 = a.m20*b.m02 + a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	p.m23 = a.m20*b.m03 + a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;
	// fourth row
	p.m30 = a.m30*b.m00 + a.m31*b.m10 + a.m32*b.m20 + a.m33*b.m30;
	p.m31 = a.m30*b.m01 + a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	p.m32 = a.m30*b.m02 + a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	p.m33 = a.m30*b.m03 + a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;	
	return p;
}

/* compute m * v */
rlVec4 rlMat4Vec4(rlMat4 m, rlVec4 v)
{
	rlVec4 prod;
	prod.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w;
	prod.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w;
	prod.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w;
	prod.w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w;
	return prod;
}

/* calculate a symmetrical-frustum projection matrix */
rlMat4 rlPerspective(float fovy, float aspect, float near, float far)
{
	fovy *= RL_RADIANS_PER_DEGREE;
	
	rlMat4 m = rlIdentity();
	
	float f = _safedivf(1.0f, tan(fovy/2.0f));

	m.m00 = _safedivf(f, aspect);
	m.m11 = f;
	m.m22 = -_safedivf(far+near, far-near);
	m.m23 = -_safedivf(2*far*near, far-near);
	m.m32 = -1;
	m.m33 = 0;
	return m;
}

/* calculate a projection matrix */
rlMat4 rlFrustum(float left, float right, float top, float bottom, float near, float far)
{
	rlMat4 m = rlIdentity();
	m.m00 = _safedivf(2.0f*near, right - left);
	m.m02 = _safedivf(right+left, right-left);
	m.m11 = _safedivf(2.0f*near, top-bottom);
	m.m12 = _safedivf(top+bottom, top-bottom);
	m.m22 = _safedivf(-(far+near), far-near);
	m.m23 = _safedivf(-2.0f*far*near, far-near);
	m.m32 = -1;
	m.m33 = 0;
	return m;
}

/* calculate a LookAt matrix */
rlMat4 rlLookAt(rlVec3 eye, rlVec3 center, rlVec3 up)
{	
	rlVec3 f;
	f.x = center.x - eye.x;
	f.y = center.y - eye.y;
	f.z = center.z - eye.z;
	f = _normalize_vec3(f);
	
	rlVec3 u = _normalize_vec3(up);
	rlVec3 s = _normalize_vec3(_cross_vec3(f,u));
	u = _cross_vec3(s,f);
	
	rlMat4 mat = rlIdentity();
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

/* calculate a scale matrix */
rlMat4 rlScale(rlVec3 scale)
{
	rlMat4 mscale = rlIdentity();
	mscale.m00 = scale.x;
	mscale.m11 = scale.y;
	mscale.m22 = scale.z;
	return mscale;
}

/* calculate a rotation matrix (rotation in degrees) */
rlMat4 rlRotate(float angle, rlVec3 axis)
{
	angle = fmod(angle, 360.0);
	angle *= RL_RADIANS_PER_DEGREE;

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
	
	rlMat4 mrotation = rlIdentity();
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

/* convert Euler angles (given in degrees) to quaternion */
rlVec4 rlEulerToQuat(rlVec3 angles)
{
	float c1, c2, c3;
	float s1, s2, s3;
	
	angles.x = fmod(angles.x, 360.0);
	angles.y = fmod(angles.y, 360.0);
	angles.z = fmod(angles.z, 360.0);
	
	angles.x *= RL_RADIANS_PER_DEGREE;
	angles.y *= RL_RADIANS_PER_DEGREE;
	angles.z *= RL_RADIANS_PER_DEGREE;
	
	c1 = cos(angles.y / 2.0);
	c2 = cos(angles.z / 2.0);
	c3 = cos(angles.x / 2.0);
	s1 = sin(angles.y / 2.0);
	s2 = sin(angles.z / 2.0);
	s3 = sin(angles.x / 2.0);
	
	rlVec4 quat;
	quat.w = c1*c2*c3 - s1*s2*s3;
	quat.x = s1*s2*c3 + c1*c2*s3;
	quat.y = s1*c2*c3 + c1*s2*s3;
	quat.z = c1*s2*c3 - s1*c2*s3;
	
	float n = sqrt(pow(quat.x, 2) + pow(quat.y, 2) + pow(quat.z, 2) + pow(quat.w, 2));
	float inv = _safedivf(1.0f, n);
	quat.x *= inv;
	quat.y *= inv;
	quat.z *= inv;
	quat.w *= inv;
	
	return quat;
}

/* convert quaternion to rotation matrix */
rlMat4 rlQuatToMat4(rlVec4 quat)
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
	
	rlMat4 rotation = rlIdentity();
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

/* calculate a translation matrix */
rlMat4 rlTranslate(rlVec3 translation)
{
	rlMat4 mtranslation = rlIdentity();
	mtranslation.m03 = translation.x;
	mtranslation.m13 = translation.y;
	mtranslation.m23 = translation.z;
	return mtranslation;
}

/* get an identity matrix */
rlMat4 rlIdentity()
{
	rlMat4 id;
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

#endif
