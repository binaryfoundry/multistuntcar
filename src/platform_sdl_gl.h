#ifndef _PLATFORM_SDL_GL_H_
#define _PLATFORM_SDL_GL_H_

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#if defined(__EMSCRIPTEN__) && !defined(HAVE_GLES)
#define HAVE_GLES
#endif

#ifdef USE_SDL2
#if __has_include(<SDL.h>)
#include <SDL.h>
#include <SDL_ttf.h>
#elif __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#else
#error "SDL2 headers not found"
#endif

#ifdef HAVE_GLES
#include <SDL_opengles2.h>
#else
#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <SDL_opengl.h>
#if __has_include(<SDL_opengl_glext.h>)
#include <SDL_opengl_glext.h>
#elif __has_include(<SDL2/SDL_opengl_glext.h>)
#include <SDL2/SDL_opengl_glext.h>
#endif
#endif
#else
#ifdef HAVE_GLES
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#endif
#include <wchar.h>
#include <string>

#if defined(_WIN32)
#define wcscasecmp _wcsicmp
#endif
#define USEGLM
#ifdef USEGLM
#define GLM_FORCE_RADIANS
//#define GLM_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#else
#include "matvec.h"
#endif
#define STBI_NO_PSD
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "stb_image.h"

// DX -> OpenGL inspired by forsaken project
typedef uint32_t DWORD;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t BOOL;

typedef const wchar_t* LPCWSTR;

typedef void* HMODULE;

typedef int32_t LONG;
typedef float FLOAT;
typedef double DOUBLE;
typedef int32_t INT;

typedef BYTE* LPBYTE;

#ifndef PI
#define PI 3.14159265358979323846
#endif
#define CALLBACK

#define TRUE true
#define FALSE false

#define S_OK 0x00000000
#define E_ABORT 0x80004004
#define E_FAIL 0x80004005

#define ERROR_SUCCESS 0

#define _FACDS 0x878
#define MAKE_DSHRESULT(code) MAKE_HRESULT(1, _FACDS, code)

#define DS_OK 0
#define DSERR_ALLOCATED MAKE_DSHRESULT(10)
#define DSERR_CONTROLUNAVAIL MAKE_DSHRESULT(30)
#define DSERR_INVALIDPARAM E_INVALIDARG
#define DSERR_INVALIDCALL M AKE_DSHRESULT(50)
#define DSERR_GENERIC E_FAIL
#define DSERR_PRIOLEVELNEEDED MAKE_DSHRESULT(70)
#define DSERR_OUTOFMEMORY E_OUTOFMEMORY
#define DSERR_BADFORMAT MAKE_DSHRESULT(100)
#define DSERR_UNSUPPORTED E_NOTIMPL
#define DSERR_NODRIVER MAKE_DSHRESULT(120)
#define DSERR_ALREADYINITIALIZED MAKE_DSHRESULT(130)
#define DSERR_NOAGGREGATION CLASS_E_NOAGGREGATION
#define DSERR_BUFFERLOST MAKE_DSHRESULT(150)
#define DSERR_OTHERAPPHASPRIO MAKE_DSHRESULT(160)
#define DSERR_UNINITIALIZED MAKE_DSHRESULT(170)

#define DSCAPS_PRIMARYMONO 0x00000001
#define DSCAPS_PRIMARYSTEREO 0x00000002
#define DSCAPS_PRIMARY8BIT 0x00000004
#define DSCAPS_PRIMARY16BIT 0x00000008
#define DSCAPS_CONTINUOUSRATE 0x00000010
#define DSCAPS_EMULDRIVER 0x00000020
#define DSCAPS_CERTIFIED 0x00000040
#define DSCAPS_SECONDARYMONO 0x00000100
#define DSCAPS_SECONDARYSTEREO 0x00000200
#define DSCAPS_SECONDARY8BIT 0x00000400
#define DSCAPS_SECONDARY16BIT 0x00000800

#define DSSCL_NORMAL 1
#define DSSCL_PRIORITY 2
#define DSSCL_EXCLUSIVE 3
#define DSSCL_WRITEPRIMARY 4

#define DSBPLAY_LOOPING 0x00000001
#define DSBSTATUS_PLAYING 0x00000001
#define DSBSTATUS_BUFFERLOST 0x00000002
#define DSBSTATUS_LOOPING 0x00000004

#define DSBLOCK_FROMWRITECURSOR 0x00000001

#define DSBCAPS_PRIMARYBUFFER 0x00000001
#define DSBCAPS_STATIC 0x00000002
#define DSBCAPS_LOCHARDWARE 0x00000004
#define DSBCAPS_LOCSOFTWARE 0x00000008
#define DSBCAPS_CTRLFREQUENCY 0x00000020
#define DSBCAPS_CTRLPAN 0x00000040
#define DSBCAPS_CTRLVOLUME 0x00000080
#define DSBCAPS_CTRLDEFAULT 0x000000E0 /* Pan + volume + frequency. */
#define DSBCAPS_CTRLALL 0x000000E0     /* All control capabilities */
#define DSBCAPS_STICKYFOCUS 0x00004000
#define DSBCAPS_GETCURRENTPOSITION2 0x00010000 /* More accurate play cursor under emulation*/

#define DSBPAN_RIGHT 10000
#define DSBPAN_LEFT -10000
#define DSBPAN_CENTER 0

// taken from d3d9.h
typedef DWORD COLOR; // bgra originaly, rgba for OpenGL

// bjd - taken from d3dtypes.h
//#define RGBA_MAKE(r, g, b, a)     ((COLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))
//#define    RGB_MAKE(r, g, b)     ((COLOR) (((r) << 16) | ((g) << 8) | (b)))
#define RGBA_MAKE(r, g, b, a) ((COLOR)(((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))
#define RGB_MAKE(r, g, b) ((COLOR)(((b) << 16) | ((g) << 8) | (r)))
// COLOR is packed bgra, but converted to rgba for OpenGL
#define RGBA_GETALPHA(rgb) ((rgb) >> 24)
//#define RGBA_GETRED(rgb)     (((rgb) >> 16) & 0xff)
#define RGBA_GETRED(rgb) ((rgb) & 0xff)
#define RGBA_GETGREEN(rgb) (((rgb) >> 8) & 0xff)
//#define RGBA_GETBLUE(rgb)     ((rgb) & 0xff)
#define RGBA_GETBLUE(rgb) (((rgb) >> 16) & 0xff)

#define RENDERVAL(val) ((float)val)

#define COLOR_RGB(r, g, b) RGBA_MAKE(r, g, b, 255)

typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} PALETTEENTRY;

/*
Pre-DX8 vertex formats
taken from http://www.mvps.org/directx/articles/definitions_for_dx7_vertex_types.htm
*/
typedef struct {
    union {
        float x;
        float dvX;
    };
    union {
        float y;
        float dvY;
    };
    union {
        float z;
        float dvZ;
    };
    union {
        COLOR color;
        COLOR dcColor;
    };
    union {
        float tu;
        float dvTU;
    };
    union {
        float tv;
        float dvTV;
    };
} LVERTEX, *LPLVERTEX;

typedef struct {
    union {
        float x;
        float sx;
        float dvSX;
    };
    union {
        float y;
        float sy;
        float dvSY;
    };
    union {
        float z;
        float sz;
        float dvSZ;
    };
    union {
        float w;
        float rhw;
        float dvRHW;
    };
    union {
        COLOR color;
        COLOR dcColor;
    };
    union {
        float tu;
        float dvTU;
    };
    union {
        float tv;
        float dvTV;
    };
} TLVERTEX, *LPTLVERTEX;

typedef struct {
    union {
        WORD v1;
        WORD wV1;
    };
    union {
        WORD v2;
        WORD wV2;
    };
    union {
        WORD v3;
        WORD wV3;
    };
    // WORD wFlags;
} TRIANGLE, *LPTRIANGLE;

/*===================================================================
2D Vertices
===================================================================*/

typedef struct VERT2D {
    float x;
    float y;
} VERT2D;

typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT;
/*===================================================================
3D Vertices
===================================================================*/

typedef struct VERT {
    float x;
    float y;
    float z;
    VERT(float nx, float ny, float nz) {
        x = nx;
        y = ny;
        z = nz;
    }
    VERT() { x = y = z = 0.0f; }
} VERT;

/*===================================================================
3D Normal
===================================================================*/
typedef struct NORMAL {
    union {
        float nx;
        float x;
    };
    union {
        float ny;
        float y;
    };
    union {
        float nz;
        float z;
    };
} NORMAL;

/*===================================================================
4 X 4 Matrix
===================================================================*/

typedef struct MATRIX {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
} MATRIX;

/*===================================================================
3 X 3 Matrix
===================================================================*/

typedef struct MATRIX3X3 {
    float _11, _12, _13;
    float _21, _22, _23;
    float _31, _32, _33;
} MATRIX3X3;

/*===================================================================
Vector
===================================================================*/

typedef struct VECTOR {
    float x;
    float y;
    float z;
} VECTOR;

/*===================================================================
Short Vector
===================================================================*/

typedef struct SHORTVECTOR {
    int16_t x;
    int16_t y;
    int16_t z;
} SHORTVECTOR;

/*===================================================================
Plane
===================================================================*/
typedef struct PLANE {
    VECTOR Normal;
    float Offset;
} PLANE;

static int NP2(int a) {
    int j = 1;
    while (j < a)
        j = j * 2;
    return j;
}

// Textures (OpenGL)
class GpuTexture {
  protected:
    GLuint texID;
    int w, h;   // real size
    int w2, h2; // pow2 size
  public:
    float wf, hf; // ratio...
    GpuTexture() {
        texID = 0;
        w = h = w2 = h2 = 0;
        wf = hf = 1.0f;
    }
    ~GpuTexture() {
        if (texID)
            glDeleteTextures(1, &texID);
    }
    void LoadTexture(const char* name);
    void Bind() { glBindTexture(GL_TEXTURE_2D, texID); }
    void UnBind() { glBindTexture(GL_TEXTURE_2D, 0); }
};
/*============================================================


  SOUND related functions....

===============================================================*/

//
// Generic Functions
//
bool sound_init(void);
void sound_destroy(void);
//
// Listener
//
bool sound_listener_position(float x, float y, float z);
bool sound_listener_velocity(float x, float y, float z);
bool sound_listener_orientation(float fx, float fy, float fz, // forward vector
                                float ux, float uy, float uz  // up vector
);

typedef struct sound_source_t sound_source_t;
typedef struct sound_buffer_t sound_buffer_t;

typedef wchar_t WCHAR;
typedef unsigned int HRESULT;
typedef DWORD* LPDWORD;
typedef void* LPVOID;

typedef sound_source_t sound_source_t;

class IDirectSoundBuffer8 {
  public:
    sound_source_t* source;
    sound_buffer_t* buffer;
    /* WAV format for Unlock(); set by CreateSoundBuffer from DSBUFFERDESC.lpwfxFormat */
    int wav_bits;
    int wav_sign;
    int wav_channels;
    int wav_freq;
    IDirectSoundBuffer8();
    ~IDirectSoundBuffer8();

    HRESULT SetVolume(LONG lVolume);
    HRESULT Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
    HRESULT SetFrequency(DWORD dwFrequency);
    HRESULT SetCurrentPosition(DWORD dwNewPosition);
    HRESULT GetCurrentPosition(LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor);
    bool IsPlaying() const;
    HRESULT Stop();
    HRESULT SetPan(LONG lPan);
    HRESULT Release();
    HRESULT Lock(DWORD dwOffset, DWORD dwBytes, LPVOID* ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID* ppvAudioPtr2,
                 LPDWORD pdwAudioBytes2, DWORD dwFlags);
    HRESULT Unlock(LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2);
};

typedef IDirectSoundBuffer8* LPDIRECTSOUNDBUFFER8;
#define LPDIRECTSOUNDBUFFER LPDIRECTSOUNDBUFFER8

typedef struct {
    WORD wFormatTag;       /* format type */
    WORD nChannels;        /* number of channels */
    DWORD nSamplesPerSec;  /* sample rate */
    DWORD nAvgBytesPerSec; /* for buffer estimation */
    WORD nBlockAlign;      /* block size of data */
} WAVEFORMAT, *LPWAVEFORMAT;

#define WAVE_FORMAT_PCM 1

typedef struct {
    WAVEFORMAT wf;
    WORD wBitsPerSample;
} PCMWAVEFORMAT, *LPPCMWAVEFORMAT;

typedef struct {
    WORD wFormatTag;       /* format type */
    WORD nChannels;        /* number of channels (i.e. mono, stereo...) */
    DWORD nSamplesPerSec;  /* sample rate */
    DWORD nAvgBytesPerSec; /* for buffer estimation */
    WORD nBlockAlign;      /* block size of data */
    WORD wBitsPerSample;   /* number of bits per sample of mono data */
    WORD cbSize;           /* the count in bytes of the size of */
                           /* extra information (after cbSize) */
} WAVEFORMATEX, *LPWAVEFORMATEX;

typedef struct {
    uint32_t f1;
    uint16_t f2;
    uint16_t f3;
    uint8_t f4[8];
} GUID;
typedef const GUID* LPCGUID;

typedef void* LPUNKNOWN;

typedef uint32_t HWND;

typedef struct DSBUFFERDESC {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    LPWAVEFORMATEX lpwfxFormat;
    GUID guid3DAlgorithm;
} DSBUFFERDESC, *LPDSBUFFERDESC, * const LPCDSBUFFERDESC;

class IDirectSound8 {
  public:
    IDirectSound8() {};
    ~IDirectSound8() {};
    HRESULT SetCooperativeLevel(HWND hwnd, DWORD dwFlags) { return DS_OK; };
    HRESULT Release() { return DS_OK; };
    HRESULT CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER* ppDSBuffer, LPUNKNOWN pUnkOuter);
};
typedef IDirectSound8* LPDIRECTSOUND8;

HRESULT DirectSoundCreate8(LPCGUID lpcGuidDevice, LPDIRECTSOUND8* ppDS8, LPUNKNOWN pUnkOuter);

/*============================================================


  UTILITY functions....

===============================================================*/

#define MB_OK 1

#define MessageBox(a, text, type, button) printf("%ls: %ls\n", text, type)

#define UINT uint32_t

#define GetWindowHandle() 0

/*=============================================================
 * Matrix functions (GLM)
 *=============================================================*/
#ifdef USEGLM
typedef glm::mat4 mat4_t;
#else
typedef struct { float m[16]; } mat4_t;
#endif

glm::mat4* mat4PerspectiveFov(glm::mat4* pOut, float fovy, float aspect, float zn, float zf);
glm::mat4* mat4Identity(glm::mat4* pOut);
glm::mat4* mat4RotationX(glm::mat4* pOut, float angle);
glm::mat4* mat4RotationY(glm::mat4* pOut, float angle);
glm::mat4* mat4RotationZ(glm::mat4* pOut, float angle);
glm::mat4* mat4Translation(glm::mat4* pOut, float x, float y, float z);
glm::mat4* mat4Scaling(glm::mat4* pOut, float sx, float sy, float sz);
glm::mat4* mat4Multiply(glm::mat4* pOut, const glm::mat4* pM1, const glm::mat4* pM2);
glm::mat4* mat4LookAt(glm::mat4* pOut, const glm::vec3* pEye, const glm::vec3* pAt, const glm::vec3* pUp);

/*=============================================================
 * Render device (OpenGL)
 *=============================================================*/
typedef enum TransformState {
    TS_VIEW = 2,
    TS_PROJECTION = 3,
    TS_WORLD = 4,
    TS_TEXTURE0 = 16,
    TS_TEXTURE1 = 17,
    TS_TEXTURE2 = 18,
    TS_TEXTURE3 = 19,
    TS_TEXTURE4 = 20,
    TS_TEXTURE5 = 21,
    TS_TEXTURE6 = 22,
    TS_TEXTURE7 = 23,
    TS_FORCE_DWORD = 0x7fffffff
} TransformState;

typedef enum RenderStateType {
    RS_ZENABLE = 7,
    RS_FILLMODE = 8,
    RS_SHADEMODE = 9,
    RS_ZWRITEENABLE = 14,
    RS_ALPHATESTENABLE = 15,
    RS_LASTPIXEL = 16,
    RS_SRCBLEND = 19,
    RS_DESTBLEND = 20,
    RS_CULLMODE = 22,
    RS_ZFUNC = 23,
    RS_ALPHAREF = 24,
    RS_ALPHAFUNC = 25,
    RS_DITHERENABLE = 26,
    RS_ALPHABLENDENABLE = 27,
    RS_FOGENABLE = 28,
    RS_SPECULARENABLE = 29,
    RS_FOGCOLOR = 34,
    RS_FOGTABLEMODE = 35,
    RS_FOGSTART = 36,
    RS_FOGEND = 37,
    RS_FOGDENSITY = 38,
    RS_RANGEFOGENABLE = 48,
    RS_STENCILENABLE = 52,
    RS_STENCILFAIL = 53,
    RS_STENCILZFAIL = 54,
    RS_STENCILPASS = 55,
    RS_STENCILFUNC = 56,
    RS_STENCILREF = 57,
    RS_STENCILMASK = 58,
    RS_STENCILWRITEMASK = 59,
    RS_TEXTUREFACTOR = 60,
    RS_WRAP0 = 128,
    RS_WRAP1 = 129,
    RS_WRAP2 = 130,
    RS_WRAP3 = 131,
    RS_WRAP4 = 132,
    RS_WRAP5 = 133,
    RS_WRAP6 = 134,
    RS_WRAP7 = 135,
    RS_CLIPPING = 136,
    RS_LIGHTING = 137,
    RS_AMBIENT = 139,
    RS_FOGVERTEXMODE = 140,
    RS_COLORVERTEX = 141,
    RS_LOCALVIEWER = 142,
    RS_NORMALIZENORMALS = 143,
    RS_DIFFUSEMATERIALSOURCE = 145,
    RS_SPECULARMATERIALSOURCE = 146,
    RS_AMBIENTMATERIALSOURCE = 147,
    RS_EMISSIVEMATERIALSOURCE = 148,
    RS_VERTEXBLEND = 151,
    RS_CLIPPLANEENABLE = 152,
    RS_POINTSIZE = 154,
    RS_POINTSIZE_MIN = 155,
    RS_POINTSPRITEENABLE = 156,
    RS_POINTSCALEENABLE = 157,
    RS_POINTSCALE_A = 158,
    RS_POINTSCALE_B = 159,
    RS_POINTSCALE_C = 160,
    RS_MULTISAMPLEANTIALIAS = 161,
    RS_MULTISAMPLEMASK = 162,
    RS_PATCHEDGESTYLE = 163,
    RS_DEBUGMONITORTOKEN = 165,
    RS_POINTSIZE_MAX = 166,
    RS_INDEXEDVERTEXBLENDENABLE = 167,
    RS_COLORWRITEENABLE = 168,
    RS_TWEENFACTOR = 170,
    RS_BLENDOP = 171,
    RS_POSITIONDEGREE = 172,
    RS_NORMALDEGREE = 173,
    RS_SCISSORTESTENABLE = 174,
    RS_SLOPESCALEDEPTHBIAS = 175,
    RS_ANTIALIASEDLINEENABLE = 176,
    RS_MINTESSELLATIONLEVEL = 178,
    RS_MAXTESSELLATIONLEVEL = 179,
    RS_ADAPTIVETESS_X = 180,
    RS_ADAPTIVETESS_Y = 181,
    RS_ADAPTIVETESS_Z = 182,
    RS_ADAPTIVETESS_W = 183,
    RS_ENABLEADAPTIVETESSELLATION = 184,
    RS_TWOSIDEDSTENCILMODE = 185,
    RS_CCW_STENCILFAIL = 186,
    RS_CCW_STENCILZFAIL = 187,
    RS_CCW_STENCILPASS = 188,
    RS_CCW_STENCILFUNC = 189,
    RS_COLORWRITEENABLE1 = 190,
    RS_COLORWRITEENABLE2 = 191,
    RS_COLORWRITEENABLE3 = 192,
    RS_BLENDFACTOR = 193,
    RS_SRGBWRITEENABLE = 194,
    RS_DEPTHBIAS = 195,
    RS_WRAP8 = 198,
    RS_WRAP9 = 199,
    RS_WRAP10 = 200,
    RS_WRAP11 = 201,
    RS_WRAP12 = 202,
    RS_WRAP13 = 203,
    RS_WRAP14 = 204,
    RS_WRAP15 = 205,
    RS_SEPARATEALPHABLENDENABLE = 206,
    RS_SRCBLENDALPHA = 207,
    RS_DESTBLENDALPHA = 208,
    RS_BLENDOPALPHA = 209,
    RS_FORCE_DWORD = 0x7fffffff
} RenderStateType;

typedef enum CullMode {
    CULL_NONE = 1,
    CULL_CW = 2,
    CULL_CCW = 3,
    CULL_FORCE_DWORD = 0x7fffffff
} CullMode;

typedef enum PrimitiveType {
    PT_POINTLIST = 1,
    PT_LINELIST = 2,
    PT_LINESTRIP = 3,
    PT_TRIANGLELIST = 4,
    PT_TRIANGLESTRIP = 5,
    PT_TRIANGLEFAN = 6,
    PT_FORCE_DWORD = 0x7fffffff
} PrimitiveType;

typedef enum TextureStageStateType {
    TSS_COLOROP = 1,
    TSS_COLORARG1 = 2,
    TSS_COLORARG2 = 3,
    TSS_ALPHAOP = 4,
    TSS_ALPHAARG1 = 5,
    TSS_ALPHAARG2 = 6,
    TSS_BUMPENVMAT00 = 7,
    TSS_BUMPENVMAT01 = 8,
    TSS_BUMPENVMAT10 = 9,
    TSS_BUMPENVMAT11 = 10,
    TSS_TEXCOORDINDEX = 11,
    TSS_BUMPENVLSCALE = 22,
    TSS_BUMPENVLOFFSET = 23,
    TSS_TEXTURETRANSFORMFLAGS = 24,
    TSS_COLORARG0 = 26,
    TSS_ALPHAARG0 = 27,
    TSS_RESULTARG = 28,
    TSS_CONSTANT = 32,
    TSS_FORCE_DWORD = 0x7fffffff
} TextureStageStateType;

typedef enum SamplerStateType {
    SAMP_ADDRESSU = 13,
    SAMP_ADDRESSV = 14,
    SAMP_ADDRESSW = 15,
    SAMP_FORCE_DWORD = 0x7fffffff
} SamplerStateType;

typedef enum TextureAddress {
    TADDRESS_WRAP = 1,
    TADDRESS_MIRROR = 2,
    TADDRESS_CLAMP = 3,
    TADDRESS_BORDER = 4,
    TADDRESS_MIRRORONCE = 5,
    TADDRESS_FORCE_DWORD = 0x7fffffff
} TextureAddress;

typedef enum TextureOp {
    TOP_DISABLE = 1,
    TOP_SELECTARG1 = 2,
    TOP_SELECTARG2 = 3,
    TOP_MODULATE = 4,
    TOP_MODULATE2X = 5,
    TOP_MODULATE4X = 6,
    TOP_ADD = 7,
    TOP_ADDSIGNED = 8,
    TOP_ADDSIGNED2X = 9,
    TOP_SUBTRACT = 10,
    TOP_ADDSMOOTH = 11,
    TOP_BLENDDIFFUSEALPHA = 12,
    TOP_BLENDTEXTUREALPHA = 13,
    TOP_BLENDFACTORALPHA = 14,
    TOP_BLENDTEXTUREALPHAPM = 15,
    TOP_BLENDCURRENTALPHA = 16,
    TOP_PREMODULATE = 17,
    TOP_MODULATEALPHA_ADDCOLOR = 18,
    TOP_MODULATECOLOR_ADDALPHA = 19,
    TOP_MODULATEINVALPHA_ADDCOLOR = 20,
    TOP_MODULATEINVCOLOR_ADDALPHA = 21,
    TOP_BUMPENVMAP = 22,
    TOP_BUMPENVMAPLUMINANCE = 23,
    TOP_DOTPRODUCT3 = 24,
    TOP_MULTIPLYADD = 25,
    TOP_LERP = 26,
    TOP_FORCE_DWORD = 0x7fffffff
} TextureOp;

typedef enum BlendMode {
    BLEND_ZERO = 1,
    BLEND_ONE = 2,
    BLEND_SRCCOLOR = 3,
    BLEND_INVSRCCOLOR = 4,
    BLEND_SRCALPHA = 5,
    BLEND_INVSRCALPHA = 6,
    BLEND_DESTALPHA = 7,
    BLEND_INVDESTALPHA = 8,
    BLEND_DESTCOLOR = 9,
    BLEND_INVDESTCOLOR = 10,
    BLEND_SRCALPHASAT = 11,
    BLEND_BOTHSRCALPHA = 12,
    BLEND_BOTHINVSRCALPHA = 13,
    BLEND_BLENDFACTOR = 14,
    BLEND_INVBLENDFACTOR = 15,
    BLEND_SRCCOLOR2 = 16,
    BLEND_INVSRCCOLOR2 = 17,
    BLEND_FORCE_DWORD = 0x7fffffff
} BlendMode;

typedef struct SurfaceDesc {
    //  D3DFORMAT           Format;
    //  D3DRESOURCETYPE     Type;
    //  DWORD               Usage;
    //  D3DPOOL             Pool;
    //  D3DMULTISAMPLE_TYPE MultiSampleType;
    //  DWORD               MultiSampleQuality;
    UINT Width;
    UINT Height;
} SurfaceDesc;

typedef struct ClearRect {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
} ClearRect;

#define TA_TEXTURE 1
#define TA_CURRENT 2
#define TA_DIFFUSE 3

#define CLEAR_STENCIL 1
#define CLEAR_TARGET 2
#define CLEAR_ZBUFFER 4

#define VB_USAGE_WRITEONLY 1

#define FVF_DIFFUSE (1)
#define FVF_NORMAL (1 << 1)
#define FVF_XYZ (1 << 2)
#define FVF_XYZRHW (1 << 3)
#define FVF_XYZW (1 << 4)
#define FVF_TEX0 (1 << 5)
#define FVF_TEX1 (1 << 6)
#define FVF_UTVERTEX (FVF_XYZ | FVF_DIFFUSE | FVF_TEX1)

typedef enum PoolType {
    POOL_DEFAULT = 0,
    POOL_MANAGED = 1,
    POOL_SYSTEMMEM = 2,
    POOL_SCRATCH = 3,
    POOL_FORCE_DWORD = 0x7fffffff
} PoolType;

struct UTVERTEX {
    glm::vec3 pos; // position for the vertex
    DWORD color;     // The vertex diffuse color value
    FLOAT tu, tv;    // The texture co-ordinates
};
// UTBuffer, used for IDirect3DVertexBuffer9 limited simulation (there are better way of courss to do that)
struct UTBuffer {
    void* buffer;
    uint32_t fvf;
    uint32_t size; // estimated size of the array
};

typedef void* HANDLE;

class VertexBuffer {
  public:
    VertexBuffer(uint32_t capacity, uint32_t fvf);
    ~VertexBuffer();
    HRESULT Release();
    HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    HRESULT Unlock();

    UTBuffer buffer;
};

class RenderDevice {
  public:
    RenderDevice();
    ~RenderDevice();
    HRESULT SetTransform(TransformState State, glm::mat4* pMatrix);
    HRESULT GetTransform(TransformState State, glm::mat4* pMatrix);
    HRESULT SetRenderState(RenderStateType State, int Value);
    HRESULT DrawPrimitive(PrimitiveType PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    HRESULT SetTextureStageState(DWORD Stage, TextureStageStateType Type, DWORD Value);
    HRESULT SetSamplerState(DWORD Sampler, SamplerStateType Type, DWORD Value);
    HRESULT SetTexture(DWORD Sampler, GpuTexture* pTexture);
    HRESULT Clear(DWORD Count, const ClearRect* pRects, DWORD Flags, COLOR Color, float Z, DWORD Stencil);
    HRESULT BeginScene() { return S_OK; };
    HRESULT EndScene() { return S_OK; };
    HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, PoolType Pool,
                               VertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle);
    HRESULT SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride);
    HRESULT SetFVF(DWORD FVF);

    void ActivateWorldMatrix();
    void DeactivateWorldMatrix();

  private:
    bool EnsureInitialized();
    void ApplyBlendState(bool forceEnable);
    glm::mat4 BuildScreenProjection() const;
    int ResolveColorMode(bool hasTexture, bool hasColor) const;

    UINT colorop[8];
    UINT alphaop[8];
    UINT colorarg1[8];
    UINT colorarg2[8];
    glm::mat4 mWorld, mView, mProj, mText, mInv;
    VertexBuffer* buffer[8];
    uint32_t offset[8];
    uint32_t stride[8];
    DWORD fvf;
    bool mInitialized;
    bool mAlphaBlendEnabled;
    int mSrcBlend;
    int mDstBlend;
    GLuint mShaderProgram;
    GLuint mDynamicVbo;
    GLint mUniformMvp;
    GLint mUniformTexture;
    GLint mUniformTexMatrix;
    GLint mUniformColorMode;
    GLint mUniformModelView;
    GLint mUniformFogEnabled;
    GLint mUniformFogDensity;
    GLint mUniformFogHeightScale;
    GLint mUniformFogSkyColor;
    GLint mUniformSunDirView;
    GLint mUniformCameraPos;
    GLint mUniformWorldUpView;
};

class TextHelper {
  public:
    TextHelper(TTF_Font* font, GLuint sprite, int size);
    ~TextHelper();
    void SetInsertionPos(int x, int y);
    void SetDisplaySize(int size);  // update display size (e.g. when window scale changes)
    int MeasureTextWidth(const wchar_t* line) const;
    void DrawTextLine(const wchar_t* line);
    void DrawFormattedTextLine(const std::wstring& line);
    void Begin() {};
    void End() {};
    void SetForegroundColor(glm::vec4 clr);

  private:
    bool EnsureVertexCapacity(uint32_t requiredVertices);

    GLuint m_sprite;
    int m_size;           // display line height (for layout)
    float m_displayScale; // display size / font texture size, for sharp scaling
    int m_fontsize;
    int m_posx, m_posy;
    float m_inv;
    int m_as[256];
    float m_forecol[4];
    GLuint m_texture;
    int m_sizew, m_sizeh;
    VertexBuffer* m_vertexBuffer;
    uint32_t m_vertexCapacity;
};

RenderDevice* GetRenderDevice();

const SurfaceDesc* GetBackBufferSurfaceDesc();

DOUBLE GetTimeSeconds();

#define StringCchPrintf swprintf
// V macro should test the result...
#define V(a) a
#define SUCCEEDED(a) a == S_OK
#define FAILED(a) a != S_OK

void ResetRenderEnvironment();

#define mmioFOURCC(ch0, ch1, ch2, ch3) MAKEFOURCC(ch0, ch1, ch2, ch3)
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                                                                                 \
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))

#define ZeroMemory(a, b) memset(a, 0, b)
#define CopyMemory(a, b, c) memcpy(a, b, c)

#define OutputDebugStringW(A) printf("%S", A)

#endif //_PLATFORM_SDL_GL_H_
