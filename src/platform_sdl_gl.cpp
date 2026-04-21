#ifdef linux
#include "platform_sdl_gl.h"
// use a light version of stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern int wideScreen;

const char* BitMapRessourceName(const char* name) {
    static const char* resname[] = {
        "RoadYellowDark", "RoadYellowLight", "RoadRedDark", "RoadRedLight", "RoadBlack", "RoadWhite", 0};
    static const char* filename[] = {"data/Bitmap/RoadYellowDark.bmp",
                                     "data/Bitmap/RoadYellowLight.bmp",
                                     "data/Bitmap/RoadRedDark.bmp",
                                     "data/Bitmap/RoadRedLight.bmp",
                                     "data/Bitmap/RoadBlack.bmp",
                                     "data/Bitmap/RoadWhite.bmp",
                                     0};

    int i = 0;
    while (resname[i] && strcmp(resname[i], name))
        i++;
    if (filename[i] == 0)
        return name;
    return filename[i];
}

void GpuTexture::LoadTexture(const char* name) {
    if (texID)
        glDeleteTextures(1, &texID);
    glGenTextures(1, &texID);
    int x, y, n;
    unsigned char* img = stbi_load(BitMapRessourceName(name), &x, &y, &n, 0);
    if (!img) {
        printf("Warning, image \"%s\" => \"%s\" not loaded\n", name, BitMapRessourceName(name));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        return;
    }
    GLint intfmt = n;
    GLenum fmt = GL_RGBA;
    switch (intfmt) {
    case 1:
        fmt = GL_ALPHA;
        break;
    case 3: // no alpha channel
        fmt = GL_RGB;
        break;
    case 4: // contains an alpha channel
        fmt = GL_RGBA;
        break;
    }
    w2 = w = x;
    h2 = h = y;
    // will handle non-pot2 texture later? or resize the texture to POT?
    /*w2 = NP2(w);
    h2 = NP2(h);
    wf = (float)w2 / (float)w;
    hf = (float)h2 / (float)h;*/
    Bind();
    // ugly... Just blindly load the texture without much check!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Use fmt for internal format: WebGL only allows GL_RGB/GL_RGBA/GL_ALPHA/GL_LUMINANCE/GL_LUMINANCE_ALPHA
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w2, h2, 0, fmt, GL_UNSIGNED_BYTE, NULL);
    // simple and hugly way to make the texture upside down...
    int pitch = y * n;
    for (int i = 0; i < h; i++) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, (h - 1) - i, w, 1, fmt, GL_UNSIGNED_BYTE, img + (pitch * i));
    }
    UnBind();
    if (img)
        free(img);
}

sound_buffer_t* sound_load(void* data, int size, int bits, int sign, int channels, int freq);
sound_source_t* sound_source(sound_buffer_t* buffer);
void sound_play(sound_source_t* s);
void sound_play_looping(sound_source_t* s);
bool sound_is_playing(sound_source_t* s);
void sound_stop(sound_source_t* s);
void sound_release_source(sound_source_t* s);
void sound_release_buffer(sound_buffer_t* s);
void sound_set_frequency(sound_source_t* source, long frequency);
void sound_set_pitch(sound_source_t* s, float pitch);
void sound_volume(sound_source_t* s, long decibels);
void sound_pan(sound_source_t* s, long pan);
void sound_position(sound_source_t* s, float x, float y, float z, float min_distance, float max_distance);

void sound_set_position(sound_source_t* s, long newpos);
long sound_get_position(sound_source_t* s);

int npot(int n) {
    int i = 1;
    while (i < n)
        i <<= 1;
    return i;
}

IDirectSoundBuffer8::IDirectSoundBuffer8() {
    source = NULL;
    buffer = NULL;
    wav_bits = 8;
    wav_sign = 0;
    wav_channels = 1;
    wav_freq = 11025;
}

HRESULT IDirectSoundBuffer8::SetVolume(LONG lVolume) {
    if (!source)
        return DSERR_GENERIC;
    sound_volume(source, lVolume);
    return DS_OK;
}

HRESULT IDirectSoundBuffer8::Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) {
    if (!source)
        return DSERR_GENERIC;
    if (dwFlags & DSBPLAY_LOOPING)
        sound_play_looping(source);
    else
        sound_play(source);
    return DS_OK;
}

HRESULT IDirectSoundBuffer8::SetFrequency(DWORD dwFrequency) {
    if (!source)
        return DSERR_GENERIC;
    sound_set_frequency(source, dwFrequency);
    return DS_OK;
}

HRESULT IDirectSoundBuffer8::SetCurrentPosition(DWORD dwNewPosition) {
    if (!source)
        return DSERR_GENERIC;
    sound_set_position(source, dwNewPosition);
    return DS_OK;
}

HRESULT IDirectSoundBuffer8::GetCurrentPosition(LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor) {
    if (!source)
        return DSERR_GENERIC;
    if (pdwCurrentPlayCursor)
        *pdwCurrentPlayCursor = sound_get_position(source);
    return DS_OK;
}

bool IDirectSoundBuffer8::IsPlaying() const {
    return source ? sound_is_playing(source) : false;
}

HRESULT IDirectSoundBuffer8::Stop() {
    if (!source)
        return DSERR_GENERIC;
    sound_stop(source);
    return DS_OK;
}

HRESULT IDirectSoundBuffer8::SetPan(LONG lPan) {
    if (!source)
        return DSERR_GENERIC;
    sound_pan(source, lPan);
    return DS_OK;
}

IDirectSoundBuffer8::~IDirectSoundBuffer8() {
    if (buffer)
        Release();
}

HRESULT IDirectSoundBuffer8::Release() {
    if (source) {
        sound_release_source(source);
        source = NULL;
    }
    if (buffer) {
        sound_release_buffer(buffer);
        buffer = NULL;
    }
    return S_OK;
}

HRESULT IDirectSoundBuffer8::Lock(DWORD dwOffset, DWORD dwBytes, LPVOID* ppvAudioPtr1, LPDWORD pdwAudioBytes1,
                                  LPVOID* ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags) {
    if (dwOffset != 0)
        return E_FAIL;
    *ppvAudioPtr2 = NULL;
    *pdwAudioBytes2 = 0;
    *ppvAudioPtr1 = malloc(dwBytes);
    *pdwAudioBytes1 = dwBytes;
    return S_OK;
}
HRESULT IDirectSoundBuffer8::Unlock(LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2) {
    if (dwAudioBytes2 != 0)
        return E_FAIL;
    if (source || buffer)
        Release();
    buffer = sound_load(pvAudioPtr1, dwAudioBytes1, wav_bits, wav_sign, wav_channels, wav_freq);
    source = sound_source(buffer);
    free(pvAudioPtr1);
    return S_OK;
}

HRESULT IDirectSound8::CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER* ppDSBuffer,
                                         LPUNKNOWN pUnkOuter) {
    IDirectSoundBuffer8* tmp = new IDirectSoundBuffer8();
    if (pcDSBufferDesc && pcDSBufferDesc->lpwfxFormat) {
        LPWAVEFORMATEX wfx = pcDSBufferDesc->lpwfxFormat;
        tmp->wav_bits = (int)wfx->wBitsPerSample;
        tmp->wav_sign = (wfx->wBitsPerSample == 16) ? 1 : 0;
        tmp->wav_channels = (int)wfx->nChannels;
        tmp->wav_freq = (int)wfx->nSamplesPerSec;
    }
    *ppDSBuffer = tmp;
    return S_OK;
}

HRESULT DirectSoundCreate8(LPCGUID lpcGuidDevice, LPDIRECTSOUND8* ppDS8, LPUNKNOWN pUnkOuter) {
    *ppDS8 = new IDirectSound8();
    return DS_OK;
}

/*
 * Matrix
*/
// Try to keep everything column-major to make OpenGL happy...

glm::mat4* mat4Identity(glm::mat4* pOut) {
#ifdef USEGLM
    *pOut = glm::mat4(1.0f);
#else
    set_identity(pOut->m);
#endif
    return pOut;
}

glm::mat4* mat4RotationX(glm::mat4* pOut, float angle) {
#ifdef USEGLM
    *pOut = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
#else
    matrix_rot(angle, 1.0f, 0.0f, 0.0f, pOut->m);
#endif
    return pOut;
}
glm::mat4* mat4RotationY(glm::mat4* pOut, float angle) {
#ifdef USEGLM
    *pOut = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
#else
    matrix_rot(angle, 0.0f, 1.0f, 0.0f, pOut->m);
#endif
    return pOut;
}

glm::mat4* mat4RotationZ(glm::mat4* pOut, float angle) {
#ifdef USEGLM
    *pOut = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
#else
    matrix_rot(angle, 0.0f, 0.0f, 1.0f, pOut->m);
#endif
    return pOut;
}

glm::mat4* mat4Translation(glm::mat4* pOut, float x, float y, float z) {
#ifdef USEGLM
    *pOut = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
#else
    matrix_trans(x, y, z, pOut->m);
#endif
    return pOut;
}

glm::mat4* mat4Scaling(glm::mat4* pOut, float sx, float sy, float sz) {
#ifdef USEGLM
    *pOut = glm::translate(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
#else
    matrix_scale(sx, sy, sz, pOut->m);
#endif
    return pOut;
}

glm::mat4* mat4Multiply(glm::mat4* pOut, const glm::mat4* pM1, const glm::mat4* pM2) {
#ifdef USEGLM
    *pOut = (*pM2) * (*pM1); // reverse order for column-major OpenGL
#else
    matrix_mul(pM1->m, pM2->m, pOut->m);
#endif
    return pOut;
}

glm::mat4* mat4LookAt(glm::mat4* pOut, const glm::vec3* pEye, const glm::vec3* pAt, const glm::vec3* pUp) {
#ifdef USEGLM
    glm::vec3 eye = *pEye;
    glm::vec3 at = *pAt;
    glm::vec3 up = *pUp;
    *pOut = glm::lookAt(eye, at, up);
#else
    matrix_lookat((float*)pEye, (float*)pAt, (float*)pUp, pOut->m);
#endif
    return pOut;
}

glm::mat4* mat4PerspectiveFov(glm::mat4* pOut, float fovy, float Aspect, float zn, float zf) {
#ifdef USEGLM
    float fw, fh;
    fh = tanf(fovy / 2.0f) * zn;
    fw = fh * Aspect;
    *pOut = glm::frustum(-fw, +fw, +fh, -fh, zn, zf);
#else
    const float ymax = zn * tanf(fovy * 0.5f);
    const float xmax = ymax * Aspect;
    const float temp = 2.0f * zn;
    const float temp2 = 2.0f * xmax;
    const float temp3 = 2.0f * ymax;
    const float temp4 = zf - zn;
    pOut->m[0] = temp / temp2;
    pOut->m[1] = 0.0f;
    pOut->m[2] = 0.0f;
    pOut->m[3] = 0.0f;
    pOut->m[4] = 0.0f;
    pOut->m[5] = temp / temp3;
    pOut->m[6] = 0.0f;
    pOut->m[7] = 0.0f;
    pOut->m[8] = 0.0f;
    pOut->m[9] = 0.0f;
    pOut->m[10] = zf / temp4;
    pOut->m[11] = 1.0f;
    pOut->m[12] = 0.0f;
    pOut->m[13] = 0.0f;
    pOut->m[14] = (zn * zf) / (zn - zf);
    pOut->m[15] = 0.0f;
#endif
#endif
    return pOut;
}

extern DWORD SCRGB(long colour_index);

namespace {
constexpr GLuint ATTRIB_POSITION = 0;
constexpr GLuint ATTRIB_COLOR = 1;
constexpr GLuint ATTRIB_TEXCOORD = 2;
constexpr float SCREEN_VIRTUAL_HEIGHT = 480.0f;
constexpr float SCREEN_NEAR_Z = -1.0f;
constexpr float SCREEN_FAR_Z = 1.0f;
constexpr float FOG_DENSITY = 0.000008f;
constexpr float FOG_HEIGHT_SCALE = 8.0f;
constexpr float FOG_SKY_COLOR_R = 0.7f;
constexpr float FOG_SKY_COLOR_G = 0.6f;
constexpr float FOG_SKY_COLOR_B = 0.5f;
constexpr float INV_SQRT2 = 0.70710678118f;

#if !defined(HAVE_GLES) && !defined(__EMSCRIPTEN__)
struct GLFunctions {
    PFNGLACTIVETEXTUREPROC ActiveTexture;
    PFNGLATTACHSHADERPROC AttachShader;
    PFNGLBINDATTRIBLOCATIONPROC BindAttribLocation;
    PFNGLBINDBUFFERPROC BindBuffer;
    PFNGLBUFFERDATAPROC BufferData;
    PFNGLCOMPILESHADERPROC CompileShader;
    PFNGLCREATEPROGRAMPROC CreateProgram;
    PFNGLCREATESHADERPROC CreateShader;
    PFNGLDELETEBUFFERSPROC DeleteBuffers;
    PFNGLDELETEPROGRAMPROC DeleteProgram;
    PFNGLDELETESHADERPROC DeleteShader;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC DisableVertexAttribArray;
    PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
    PFNGLGENBUFFERSPROC GenBuffers;
    PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;
    PFNGLGETPROGRAMIVPROC GetProgramiv;
    PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog;
    PFNGLGETSHADERIVPROC GetShaderiv;
    PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation;
    PFNGLLINKPROGRAMPROC LinkProgram;
    PFNGLSHADERSOURCEPROC ShaderSource;
    PFNGLUNIFORM1FPROC Uniform1f;
    PFNGLUNIFORM1IPROC Uniform1i;
    PFNGLUNIFORM3FPROC Uniform3f;
    PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv;
    PFNGLUSEPROGRAMPROC UseProgram;
    PFNGLVERTEXATTRIB2FPROC VertexAttrib2f;
    PFNGLVERTEXATTRIB4FPROC VertexAttrib4f;
    PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer;
};

static GLFunctions gGl = {};
static bool gGlLoaded = false;

template <typename T> bool LoadGLProc(T* out, const char* name) {
    *out = reinterpret_cast<T>(SDL_GL_GetProcAddress(name));
    if (!(*out)) {
        printf("Missing OpenGL symbol: %s\n", name);
        return false;
    }
    return true;
}

static bool EnsureGLFunctions() {
    if (gGlLoaded)
        return true;
    bool ok = true;
    ok &= LoadGLProc(&gGl.ActiveTexture, "glActiveTexture");
    ok &= LoadGLProc(&gGl.AttachShader, "glAttachShader");
    ok &= LoadGLProc(&gGl.BindAttribLocation, "glBindAttribLocation");
    ok &= LoadGLProc(&gGl.BindBuffer, "glBindBuffer");
    ok &= LoadGLProc(&gGl.BufferData, "glBufferData");
    ok &= LoadGLProc(&gGl.CompileShader, "glCompileShader");
    ok &= LoadGLProc(&gGl.CreateProgram, "glCreateProgram");
    ok &= LoadGLProc(&gGl.CreateShader, "glCreateShader");
    ok &= LoadGLProc(&gGl.DeleteBuffers, "glDeleteBuffers");
    ok &= LoadGLProc(&gGl.DeleteProgram, "glDeleteProgram");
    ok &= LoadGLProc(&gGl.DeleteShader, "glDeleteShader");
    ok &= LoadGLProc(&gGl.DisableVertexAttribArray, "glDisableVertexAttribArray");
    ok &= LoadGLProc(&gGl.EnableVertexAttribArray, "glEnableVertexAttribArray");
    ok &= LoadGLProc(&gGl.GenBuffers, "glGenBuffers");
    ok &= LoadGLProc(&gGl.GetProgramInfoLog, "glGetProgramInfoLog");
    ok &= LoadGLProc(&gGl.GetProgramiv, "glGetProgramiv");
    ok &= LoadGLProc(&gGl.GetShaderInfoLog, "glGetShaderInfoLog");
    ok &= LoadGLProc(&gGl.GetShaderiv, "glGetShaderiv");
    ok &= LoadGLProc(&gGl.GetUniformLocation, "glGetUniformLocation");
    ok &= LoadGLProc(&gGl.LinkProgram, "glLinkProgram");
    ok &= LoadGLProc(&gGl.ShaderSource, "glShaderSource");
    ok &= LoadGLProc(&gGl.Uniform1f, "glUniform1f");
    ok &= LoadGLProc(&gGl.Uniform1i, "glUniform1i");
    ok &= LoadGLProc(&gGl.Uniform3f, "glUniform3f");
    ok &= LoadGLProc(&gGl.UniformMatrix4fv, "glUniformMatrix4fv");
    ok &= LoadGLProc(&gGl.UseProgram, "glUseProgram");
    ok &= LoadGLProc(&gGl.VertexAttrib2f, "glVertexAttrib2f");
    ok &= LoadGLProc(&gGl.VertexAttrib4f, "glVertexAttrib4f");
    ok &= LoadGLProc(&gGl.VertexAttribPointer, "glVertexAttribPointer");
    gGlLoaded = ok;
    return ok;
}

#define STGL(name) gGl.name
#else
static bool EnsureGLFunctions() { return true; }
#define STGL(name) gl##name
#endif

static void SetClearDepthValue(float depthValue) {
#if defined(HAVE_GLES) || defined(__EMSCRIPTEN__)
    glClearDepthf(depthValue);
#else
    glClearDepth(depthValue);
#endif
}

static GLenum MapBlendMode(int mode) {
    switch (mode) {
    case BLEND_ZERO:
        return GL_ZERO;
    case BLEND_ONE:
        return GL_ONE;
    case BLEND_SRCCOLOR:
        return GL_SRC_COLOR;
    case BLEND_INVSRCCOLOR:
        return GL_ONE_MINUS_SRC_COLOR;
    case BLEND_SRCALPHA:
        return GL_SRC_ALPHA;
    case BLEND_INVSRCALPHA:
        return GL_ONE_MINUS_SRC_ALPHA;
    case BLEND_DESTALPHA:
        return GL_DST_ALPHA;
    case BLEND_INVDESTALPHA:
        return GL_ONE_MINUS_DST_ALPHA;
    case BLEND_DESTCOLOR:
        return GL_DST_COLOR;
    case BLEND_INVDESTCOLOR:
        return GL_ONE_MINUS_DST_COLOR;
    case BLEND_SRCALPHASAT:
        return GL_SRC_ALPHA_SATURATE;
    default:
        return GL_ONE_MINUS_SRC_ALPHA;
    }
}

static GLuint CompileShader(GLenum shaderType, const char* source) {
    GLuint shader = STGL(CreateShader)(shaderType);
    if (!shader)
        return 0;

    STGL(ShaderSource)(shader, 1, &source, NULL);
    STGL(CompileShader)(shader);

    GLint compiled = GL_FALSE;
    STGL(GetShaderiv)(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE)
        return shader;

    char logBuf[2048] = {0};
    GLsizei logLen = 0;
    STGL(GetShaderInfoLog)(shader, (GLsizei)sizeof(logBuf), &logLen, logBuf);
    printf("Shader compile failed:\n%s\n", logBuf);
    STGL(DeleteShader)(shader);
    return 0;
}

static GLuint BuildShaderProgram() {
#if defined(HAVE_GLES) || defined(__EMSCRIPTEN__)
    const char* vertexSource =
        "attribute vec4 aPosition;\n"
        "attribute vec4 aColor;\n"
        "attribute vec2 aTexCoord;\n"
        "uniform mat4 uMvp;\n"
        "uniform mat4 uModelView;\n"
        "uniform mat4 uTexMatrix;\n"
        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"
        "varying vec3 vViewPos;\n"
        "void main() {\n"
        "  gl_Position = uMvp * vec4(aPosition.xyz, 1.0);\n"
        "  vViewPos = (uModelView * vec4(aPosition.xyz, 1.0)).xyz;\n"
        "  vColor = aColor;\n"
        "  vTexCoord = (uTexMatrix * vec4(aTexCoord, 0.0, 1.0)).xy;\n"
        "}\n";

    const char* fragmentSource =
        "precision mediump float;\n"
        "uniform sampler2D uTexture;\n"
        "uniform int uColorMode;\n"
        "uniform int uFogEnabled;\n"
        "uniform float uFogDensity;\n"
        "uniform float uFogHeightScale;\n"
        "uniform vec3 uFogSkyColor;\n"
        "uniform vec3 uSunDirView;\n"
        "uniform vec3 uCameraPos;\n"
        "uniform vec3 uWorldUpView;\n"
        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"
        "varying vec3 vViewPos;\n"
        "vec3 applyFog(in vec3 col, in float t, in vec3 rd, in vec3 lig) {\n"
        "  float a = uFogDensity;\n"
        "  float b = uFogDensity * uFogHeightScale;\n"
        "  vec3 ro = uCameraPos;\n"
        "  float rdY = dot(rd, normalize(uWorldUpView));\n"
        "  float safeRdY = (abs(rdY) < 0.0001) ? ((rdY < 0.0) ? -0.0001 : 0.0001) : rdY;\n"
        "  float fogAmount = (a / b) * exp(-ro.y * b) * (1.0 - exp(-t * safeRdY * b)) / safeRdY;\n"
        "  fogAmount = clamp(fogAmount, 0.0, 1.0);\n"
        "  float sunAmount = max(dot(rd, lig), 0.0);\n"
        "  vec3 fogColor = mix(uFogSkyColor, vec3(1.0, 0.9, 0.7), pow(sunAmount, 8.0));\n"
        "  return mix(col, fogColor, fogAmount);\n"
        "}\n"
        "void main() {\n"
        "  vec4 outColor = vec4(1.0);\n"
        "  if (uColorMode == 1) {\n"
        "    outColor = vColor;\n"
        "  } else if (uColorMode == 2) {\n"
        "    outColor = texture2D(uTexture, vTexCoord);\n"
        "  } else if (uColorMode == 3) {\n"
        "    outColor = texture2D(uTexture, vTexCoord) * vColor;\n"
        "  }\n"
        "  if (uFogEnabled == 1) {\n"
        "    float t = length(vViewPos);\n"
        "    vec3 rd = (t > 0.0001) ? (vViewPos / t) : vec3(0.0, 0.0, 1.0);\n"
        "    outColor.rgb = applyFog(outColor.rgb, t, rd, normalize(uSunDirView));\n"
        "  }\n"
        "  gl_FragColor = outColor;\n"
        "}\n";
#else
    const char* vertexSource =
        "#version 120\n"
        "attribute vec4 aPosition;\n"
        "attribute vec4 aColor;\n"
        "attribute vec2 aTexCoord;\n"
        "uniform mat4 uMvp;\n"
        "uniform mat4 uModelView;\n"
        "uniform mat4 uTexMatrix;\n"
        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"
        "varying vec3 vViewPos;\n"
        "void main() {\n"
        "  gl_Position = uMvp * vec4(aPosition.xyz, 1.0);\n"
        "  vViewPos = (uModelView * vec4(aPosition.xyz, 1.0)).xyz;\n"
        "  vColor = aColor;\n"
        "  vTexCoord = (uTexMatrix * vec4(aTexCoord, 0.0, 1.0)).xy;\n"
        "}\n";

    const char* fragmentSource =
        "#version 120\n"
        "uniform sampler2D uTexture;\n"
        "uniform int uColorMode;\n"
        "uniform int uFogEnabled;\n"
        "uniform float uFogDensity;\n"
        "uniform float uFogHeightScale;\n"
        "uniform vec3 uFogSkyColor;\n"
        "uniform vec3 uSunDirView;\n"
        "uniform vec3 uCameraPos;\n"
        "uniform vec3 uWorldUpView;\n"
        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"
        "varying vec3 vViewPos;\n"
        "vec3 applyFog(in vec3 col, in float t, in vec3 rd, in vec3 lig) {\n"
        "  float a = uFogDensity;\n"
        "  float b = uFogDensity * uFogHeightScale;\n"
        "  vec3 ro = uCameraPos;\n"
        "  float rdY = dot(rd, normalize(uWorldUpView));\n"
        "  float safeRdY = (abs(rdY) < 0.0001) ? ((rdY < 0.0) ? -0.0001 : 0.0001) : rdY;\n"
        "  float fogAmount = (a / b) * exp(-ro.y * b) * (1.0 - exp(-t * safeRdY * b)) / safeRdY;\n"
        "  fogAmount = clamp(fogAmount, 0.0, 1.0);\n"
        "  float sunAmount = max(dot(rd, lig), 0.0);\n"
        "  vec3 fogColor = mix(uFogSkyColor, vec3(1.0, 0.9, 0.7), pow(sunAmount, 8.0));\n"
        "  return mix(col, fogColor, fogAmount);\n"
        "}\n"
        "void main() {\n"
        "  vec4 outColor = vec4(1.0);\n"
        "  if (uColorMode == 1) {\n"
        "    outColor = vColor;\n"
        "  } else if (uColorMode == 2) {\n"
        "    outColor = texture2D(uTexture, vTexCoord);\n"
        "  } else if (uColorMode == 3) {\n"
        "    outColor = texture2D(uTexture, vTexCoord) * vColor;\n"
        "  }\n"
        "  if (uFogEnabled == 1) {\n"
        "    float t = length(vViewPos);\n"
        "    vec3 rd = (t > 0.0001) ? (vViewPos / t) : vec3(0.0, 0.0, 1.0);\n"
        "    outColor.rgb = applyFog(outColor.rgb, t, rd, normalize(uSunDirView));\n"
        "  }\n"
        "  gl_FragColor = outColor;\n"
        "}\n";
#endif

    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
    if (!vs)
        return 0;
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fs) {
        STGL(DeleteShader)(vs);
        return 0;
    }

    GLuint program = STGL(CreateProgram)();
    if (!program) {
        STGL(DeleteShader)(vs);
        STGL(DeleteShader)(fs);
        return 0;
    }

    STGL(AttachShader)(program, vs);
    STGL(AttachShader)(program, fs);
    STGL(BindAttribLocation)(program, ATTRIB_POSITION, "aPosition");
    STGL(BindAttribLocation)(program, ATTRIB_COLOR, "aColor");
    STGL(BindAttribLocation)(program, ATTRIB_TEXCOORD, "aTexCoord");
    STGL(LinkProgram)(program);

    GLint linked = GL_FALSE;
    STGL(GetProgramiv)(program, GL_LINK_STATUS, &linked);
    STGL(DeleteShader)(vs);
    STGL(DeleteShader)(fs);
    if (linked == GL_TRUE)
        return program;

    char logBuf[2048] = {0};
    GLsizei logLen = 0;
    STGL(GetProgramInfoLog)(program, (GLsizei)sizeof(logBuf), &logLen, logBuf);
    printf("Program link failed:\n%s\n", logBuf);
    STGL(DeleteProgram)(program);
    return 0;
}

struct VertexLayout {
    int positionComponents;
    size_t positionOffset;
    bool hasColor;
    size_t colorOffset;
    bool hasTexCoord;
    size_t texCoordOffset;
    size_t packedStride;
};

static bool BuildVertexLayout(DWORD fvf, VertexLayout* outLayout) {
    VertexLayout layout = {};
    size_t cursor = 0;

    if (fvf & FVF_XYZ) {
        layout.positionComponents = 3;
        layout.positionOffset = cursor;
        cursor += sizeof(float) * 3;
    }
    if (fvf & FVF_XYZW) {
        layout.positionComponents = 4;
        layout.positionOffset = cursor;
        cursor += sizeof(float) * 4;
    }
    if (fvf & FVF_XYZRHW) {
        // Match legacy fixed-function path: transformed vertices provided x/y in screen space.
        layout.positionComponents = 2;
        layout.positionOffset = cursor;
        cursor += sizeof(float) * 4;
    }
    if (fvf & FVF_DIFFUSE) {
        layout.hasColor = true;
        layout.colorOffset = cursor;
        cursor += sizeof(DWORD);
    }
    if (fvf & FVF_TEX0) {
        layout.hasTexCoord = true;
        layout.texCoordOffset = cursor;
        cursor += sizeof(float) * 2;
    }
    if (fvf & FVF_TEX1) {
        layout.hasTexCoord = true;
        layout.texCoordOffset = cursor;
        cursor += sizeof(float) * 2;
    }

    layout.packedStride = cursor;
    if (layout.positionComponents <= 0)
        return false;

    *outLayout = layout;
    return true;
}

struct TextVertex {
    float x, y, z, rhw;
    DWORD color;
    float u, v;
};
} // namespace

void RenderDevice::ActivateWorldMatrix() {}
void RenderDevice::DeactivateWorldMatrix() {}

uint32_t GetStrideFromFVF(DWORD fvf) {
    VertexLayout layout = {};
    if (BuildVertexLayout(fvf, &layout))
        return (uint32_t)layout.packedStride;
    return 0;
}

// RenderDevice
RenderDevice::RenderDevice()
    : fvf(0), mInitialized(false), mAlphaBlendEnabled(false), mSrcBlend(BLEND_SRCALPHA), mDstBlend(BLEND_INVSRCALPHA),
      mShaderProgram(0), mDynamicVbo(0), mUniformMvp(-1), mUniformTexture(-1), mUniformTexMatrix(-1),
      mUniformColorMode(-1), mUniformModelView(-1), mUniformFogEnabled(-1), mUniformFogDensity(-1),
      mUniformFogHeightScale(-1), mUniformFogSkyColor(-1), mUniformSunDirView(-1), mUniformCameraPos(-1),
      mUniformWorldUpView(-1) {
    for (int i = 0; i < 8; i++) {
        colorop[i] = 0;
        colorarg1[i] = 0;
        colorarg2[i] = 0;
        alphaop[i] = 0;
        buffer[i] = NULL;
        offset[i] = 0;
        stride[i] = 0;
    }
    mView = glm::mat4(1.0f);
    mWorld = glm::mat4(1.0f);
    mProj = glm::mat4(1.0f);
    mText = glm::mat4(1.0f);
    mInv = glm::mat4(-1, 0, 0, 0, 0, -1, 0, 0, 0, 0, +1, 0, 0, 0, 0, 1);
}

RenderDevice::~RenderDevice() {}

bool RenderDevice::EnsureInitialized() {
    if (mInitialized)
        return true;

    if (!EnsureGLFunctions())
        return false;

    mShaderProgram = BuildShaderProgram();
    if (!mShaderProgram)
        return false;

    mUniformMvp = STGL(GetUniformLocation)(mShaderProgram, "uMvp");
    mUniformTexture = STGL(GetUniformLocation)(mShaderProgram, "uTexture");
    mUniformTexMatrix = STGL(GetUniformLocation)(mShaderProgram, "uTexMatrix");
    mUniformColorMode = STGL(GetUniformLocation)(mShaderProgram, "uColorMode");
    mUniformModelView = STGL(GetUniformLocation)(mShaderProgram, "uModelView");
    mUniformFogEnabled = STGL(GetUniformLocation)(mShaderProgram, "uFogEnabled");
    mUniformFogDensity = STGL(GetUniformLocation)(mShaderProgram, "uFogDensity");
    mUniformFogHeightScale = STGL(GetUniformLocation)(mShaderProgram, "uFogHeightScale");
    mUniformFogSkyColor = STGL(GetUniformLocation)(mShaderProgram, "uFogSkyColor");
    mUniformSunDirView = STGL(GetUniformLocation)(mShaderProgram, "uSunDirView");
    mUniformCameraPos = STGL(GetUniformLocation)(mShaderProgram, "uCameraPos");
    mUniformWorldUpView = STGL(GetUniformLocation)(mShaderProgram, "uWorldUpView");

    STGL(GenBuffers)(1, &mDynamicVbo);
    if (!mDynamicVbo) {
        printf("Failed to create dynamic vertex buffer\n");
        return false;
    }

    STGL(UseProgram)(mShaderProgram);
    STGL(Uniform1i)(mUniformTexture, 0);
    if (mUniformFogDensity >= 0)
        STGL(Uniform1f)(mUniformFogDensity, FOG_DENSITY);
    if (mUniformFogHeightScale >= 0)
        STGL(Uniform1f)(mUniformFogHeightScale, FOG_HEIGHT_SCALE);
    if (mUniformFogEnabled >= 0)
        STGL(Uniform1i)(mUniformFogEnabled, 0);
    STGL(UseProgram)(0);

    mInitialized = true;
    return true;
}

glm::mat4 RenderDevice::BuildScreenProjection() const {
    GLint vp[4] = {0, 0, 640, 480};
    glGetIntegerv(GL_VIEWPORT, vp);
    const float aspect = (vp[3] > 0) ? (static_cast<float>(vp[2]) / static_cast<float>(vp[3])) : (4.0f / 3.0f);
    const float projWidth = SCREEN_VIRTUAL_HEIGHT * aspect;
    return glm::ortho(0.0f, projWidth, SCREEN_VIRTUAL_HEIGHT, 0.0f, SCREEN_NEAR_Z, SCREEN_FAR_Z);
}

void RenderDevice::ApplyBlendState(bool forceEnable) {
    const bool blendEnabled = forceEnable || mAlphaBlendEnabled;
    if (blendEnabled) {
        glEnable(GL_BLEND);
        glBlendFunc(MapBlendMode(mSrcBlend), MapBlendMode(mDstBlend));
    } else {
        glDisable(GL_BLEND);
    }
}

int RenderDevice::ResolveColorMode(bool hasTexture, bool hasColor) const {
    if (!hasTexture)
        return hasColor ? 1 : 0;

    const UINT op = colorop[0];
    if (op == TOP_MODULATE)
        return hasColor ? 3 : 2;
    if (op == TOP_SELECTARG1) {
        if ((colorarg1[0] == TA_DIFFUSE) && hasColor)
            return 1;
        return 2;
    }
    if (op == TOP_SELECTARG2) {
        if ((colorarg2[0] == TA_DIFFUSE) && hasColor)
            return 1;
        return 2;
    }

    return hasColor ? 3 : 2;
}

HRESULT RenderDevice::SetTransform(TransformState State, glm::mat4* pMatrix) {
    switch (State) {
    case TS_VIEW:
        mView = *pMatrix;
        break;
    case TS_WORLD:
        mWorld = *pMatrix;
        break;
    case TS_PROJECTION:
        mProj = *pMatrix;
        break;
    case TS_TEXTURE0:
    case TS_TEXTURE1:
    case TS_TEXTURE2:
    case TS_TEXTURE3:
    case TS_TEXTURE4:
        mText = *pMatrix;
        break;
    default:
        printf("Unhandled Matrix SetTransform(%X, %p)\n", State, pMatrix);
    }
    return S_OK;
}

HRESULT RenderDevice::GetTransform(TransformState State, glm::mat4* pMatrix) {
    switch (State) {
    case TS_VIEW:
        *pMatrix = mView;
        break;
    case TS_PROJECTION:
        *pMatrix = mProj;
        break;
    case TS_WORLD:
        *pMatrix = mWorld;
        break;
    case TS_TEXTURE0:
    case TS_TEXTURE1:
    case TS_TEXTURE2:
    case TS_TEXTURE3:
    case TS_TEXTURE4:
        *pMatrix = mText;
        break;
    default:
        printf("Unhandled GetTransform(%X, %p)\n", State, pMatrix);
    }
    return S_OK;
}

HRESULT RenderDevice::SetRenderState(RenderStateType State, int Value) {
    switch (State) {
    case RS_ZENABLE:
        if (Value) {
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
        } else {
            glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);
        }
        break;
    case RS_CULLMODE:
        switch (Value) {
        case CULL_NONE:
            glDisable(GL_CULL_FACE);
            break;
        case CULL_CW:
            glFrontFace(GL_CW);
            glCullFace(GL_FRONT);
            glEnable(GL_CULL_FACE);
            break;
        case CULL_CCW:
            glFrontFace(GL_CCW);
            glCullFace(GL_FRONT);
            glEnable(GL_CULL_FACE);
            break;
        }
        break;
    case RS_ALPHABLENDENABLE:
        mAlphaBlendEnabled = (Value != 0);
        break;
    case RS_SRCBLEND:
        mSrcBlend = Value;
        break;
    case RS_DESTBLEND:
        mDstBlend = Value;
        break;
    case RS_SRCBLENDALPHA:
    case RS_DESTBLENDALPHA:
        break;
    default:
        printf("Unhandled Render State %X=%d\n", State, Value);
    }
    return S_OK;
}

HRESULT RenderDevice::DrawPrimitive(PrimitiveType PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
    const GLenum primgl[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN};
    const UINT prim1[] = {1, 2, 1, 3, 1, 1};
    const UINT prim2[] = {0, 0, 1, 0, 2, 2};
    if (PrimitiveType < PT_POINTLIST || PrimitiveType > PT_TRIANGLEFAN) {
        printf("Unsupported Primitive %d\n", PrimitiveType);
        return E_FAIL;
    }
    if (PrimitiveCount == 0)
        return S_OK;

    if (!buffer[0] || !buffer[0]->buffer.buffer)
        return E_FAIL;
    if (!EnsureInitialized())
        return E_FAIL;

    VertexLayout layout = {};
    if (!BuildVertexLayout(fvf, &layout)) {
        printf("Unsupported FVF 0x%X\n", fvf);
        return E_FAIL;
    }

    const uint32_t streamStride = stride[0] ? stride[0] : GetStrideFromFVF(fvf);
    if (!streamStride)
        return E_FAIL;

    const UINT vertexCount = prim1[PrimitiveType - 1] * PrimitiveCount + prim2[PrimitiveType - 1];
    const size_t byteOffset = (size_t)offset[0] + ((size_t)streamStride * StartVertex);
    const size_t uploadBytes = (size_t)streamStride * vertexCount;
    const char* src = (const char*)buffer[0]->buffer.buffer + byteOffset;

    STGL(BindBuffer)(GL_ARRAY_BUFFER, mDynamicVbo);
    STGL(BufferData)(GL_ARRAY_BUFFER, (GLsizeiptr)uploadBytes, src, GL_DYNAMIC_DRAW);

    const bool hasTextureCoords = layout.hasTexCoord && (colorop[0] > TOP_DISABLE);
    bool hasVertexColor = layout.hasColor;
    if ((colorop[0] == TOP_SELECTARG1) && (colorarg1[0] != TA_DIFFUSE))
        hasVertexColor = false;
    if ((colorop[0] == TOP_SELECTARG2) && (colorarg2[0] != TA_DIFFUSE))
        hasVertexColor = false;

    const int colorMode = ResolveColorMode(hasTextureCoords, hasVertexColor);
    const bool worldSpaceVertices = ((fvf & FVF_XYZRHW) == 0) && ((fvf & FVF_XYZ) != 0);
    const glm::mat4 mvp = worldSpaceVertices ? (mInv * mProj * mView * mWorld) : BuildScreenProjection();
    const glm::mat4 modelView = mView * mWorld;

    const glm::vec3 fogSkyColor(FOG_SKY_COLOR_R, FOG_SKY_COLOR_G, FOG_SKY_COLOR_B);
    const glm::mat4 invView = glm::inverse(mView);
    const glm::vec3 cameraWorldPos = glm::vec3(invView * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    glm::vec3 worldUpView = glm::vec3(mView * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    const float worldUpLen = glm::length(worldUpView);
    if (worldUpLen > 0.0001f)
        worldUpView /= worldUpLen;
    else
        worldUpView = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 sunWorldDir(0.0f, INV_SQRT2, INV_SQRT2);
    glm::vec3 sunViewDir = glm::vec3(mView * glm::vec4(sunWorldDir, 0.0f));
    const float sunViewLen = glm::length(sunViewDir);
    if (sunViewLen > 0.0001f)
        sunViewDir /= sunViewLen;
    else
        sunViewDir = glm::vec3(0.0f, INV_SQRT2, INV_SQRT2);

    STGL(UseProgram)(mShaderProgram);
    STGL(ActiveTexture)(GL_TEXTURE0);
    STGL(UniformMatrix4fv)(mUniformMvp, 1, GL_FALSE, glm::value_ptr(mvp));
    STGL(UniformMatrix4fv)(mUniformModelView, 1, GL_FALSE, glm::value_ptr(modelView));
    STGL(UniformMatrix4fv)(mUniformTexMatrix, 1, GL_FALSE, glm::value_ptr(mText));
    STGL(Uniform1i)(mUniformColorMode, colorMode);
    STGL(Uniform1i)(mUniformFogEnabled, worldSpaceVertices ? 1 : 0);
    STGL(Uniform1f)(mUniformFogDensity, FOG_DENSITY);
    STGL(Uniform1f)(mUniformFogHeightScale, FOG_HEIGHT_SCALE);
    STGL(Uniform3f)(mUniformFogSkyColor, fogSkyColor.x, fogSkyColor.y, fogSkyColor.z);
    STGL(Uniform3f)(mUniformSunDirView, sunViewDir.x, sunViewDir.y, sunViewDir.z);
    STGL(Uniform3f)(mUniformCameraPos, cameraWorldPos.x, cameraWorldPos.y, cameraWorldPos.z);
    STGL(Uniform3f)(mUniformWorldUpView, worldUpView.x, worldUpView.y, worldUpView.z);

    STGL(EnableVertexAttribArray)(ATTRIB_POSITION);
    STGL(VertexAttribPointer)(ATTRIB_POSITION, layout.positionComponents, GL_FLOAT, GL_FALSE, (GLsizei)streamStride,
                              (const void*)(uintptr_t)layout.positionOffset);

    if (hasVertexColor) {
        STGL(EnableVertexAttribArray)(ATTRIB_COLOR);
        STGL(VertexAttribPointer)(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, (GLsizei)streamStride,
                                  (const void*)(uintptr_t)layout.colorOffset);
    } else {
        STGL(DisableVertexAttribArray)(ATTRIB_COLOR);
        STGL(VertexAttrib4f)(ATTRIB_COLOR, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    if (hasTextureCoords) {
        STGL(EnableVertexAttribArray)(ATTRIB_TEXCOORD);
        STGL(VertexAttribPointer)(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, (GLsizei)streamStride,
                                  (const void*)(uintptr_t)layout.texCoordOffset);
    } else {
        STGL(DisableVertexAttribArray)(ATTRIB_TEXCOORD);
        STGL(VertexAttrib2f)(ATTRIB_TEXCOORD, 0.0f, 0.0f);
    }

    ApplyBlendState(hasTextureCoords);
    glDrawArrays(primgl[PrimitiveType - 1], 0, (GLsizei)vertexCount);

    return S_OK;
}

HRESULT RenderDevice::SetTextureStageState(DWORD Stage, TextureStageStateType Type, DWORD Value) {
    if (Stage > 7) {
        printf("Unhandled SetTextureStageState(%d, 0x%X, 0x%X)\n", Stage, Type, Value);
        return S_OK;
    }

    switch (Type) {
    case TSS_COLOROP:
        colorop[Stage] = Value;
        break;
    case TSS_COLORARG1:
        colorarg1[Stage] = Value;
        break;
    case TSS_COLORARG2:
        colorarg2[Stage] = Value;
        break;
    case TSS_ALPHAOP:
        alphaop[Stage] = Value;
        break;
    case TSS_ALPHAARG1:
    case TSS_ALPHAARG2:
        break;
    default:
        printf("Unhandled SetTextureStageState(%d, 0x%X, 0x%X)\n", Stage, Type, Value);
    }

    return S_OK;
}

HRESULT RenderDevice::SetSamplerState(DWORD Sampler, SamplerStateType Type, DWORD Value) {
    (void)Sampler;
    GLint wrap = GL_REPEAT;
    switch (Value) {
    case TADDRESS_CLAMP:
        wrap = GL_CLAMP_TO_EDGE;
        break;
#ifdef GL_MIRRORED_REPEAT
    case TADDRESS_MIRROR:
        wrap = GL_MIRRORED_REPEAT;
        break;
#endif
    case TADDRESS_WRAP:
    default:
        wrap = GL_REPEAT;
        break;
    }

    switch (Type) {
    case SAMP_ADDRESSU:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        break;
    case SAMP_ADDRESSV:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        break;
    default:
        break;
    }

    return S_OK;
}

HRESULT RenderDevice::SetTexture(DWORD Sampler, GpuTexture* pTexture) {
    (void)Sampler;
    if (pTexture)
        pTexture->Bind();
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    return S_OK;
}

HRESULT RenderDevice::Clear(DWORD Count, const ClearRect* pRects, DWORD Flags, COLOR Color, float Z,
                                DWORD Stencil) {
    GLbitfield clearval = 0;
    if (Flags & CLEAR_STENCIL) {
        glClearStencil(Stencil);
        clearval |= GL_STENCIL_BUFFER_BIT;
    }
    if (Flags & CLEAR_ZBUFFER) {
        SetClearDepthValue(Z);
        clearval |= GL_DEPTH_BUFFER_BIT;
    }
    if (Flags & CLEAR_TARGET) {
        float r, g, b, a;
        r = ((Color >> 0) & 0xff) / 255.0f;
        g = ((Color >> 8) & 0xff) / 255.0f;
        b = ((Color >> 16) & 0xff) / 255.0f;
        a = ((Color >> 24) & 0xff) / 255.0f;
        glClearColor(r, g, b, a);
        clearval |= GL_COLOR_BUFFER_BIT;
    }
    if (clearval) {
        GLboolean scissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
        if (Count > 0 && pRects) {
            GLint vp[4];
            glGetIntegerv(GL_VIEWPORT, vp);
            /* pRects use top-left origin (y down); glScissor uses bottom-left (y up) */
            glEnable(GL_SCISSOR_TEST);
            glScissor((GLint)pRects[0].x1, (GLint)(vp[3] - pRects[0].y2),
                      (GLsizei)(pRects[0].x2 - pRects[0].x1), (GLsizei)(pRects[0].y2 - pRects[0].y1));
        }
        glClear(clearval);
        if (Count > 0 && pRects) {
            if (!scissorEnabled)
                glDisable(GL_SCISSOR_TEST);
        }
    }
    return S_OK;
}

HRESULT RenderDevice::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, PoolType Pool,
                                         VertexBuffer** ppVertexBuffer, HANDLE* pSharedHandle) {
    (void)Usage;
    (void)Pool;
    (void)pSharedHandle;
    *ppVertexBuffer = new VertexBuffer(Length, FVF);

    return S_OK;
}

HRESULT RenderDevice::SetStreamSource(UINT StreamNumber, VertexBuffer* pStreamData, UINT OffsetInBytes,
                                          UINT Stride) {
    buffer[StreamNumber] = pStreamData;
    offset[StreamNumber] = OffsetInBytes;
    stride[StreamNumber] = Stride;
    return S_OK;
}

HRESULT RenderDevice::SetFVF(DWORD FVF) {
    fvf = FVF;
    return S_OK;
}

VertexBuffer::VertexBuffer(uint32_t size, uint32_t fvf) {
    buffer.fvf = fvf;
    buffer.size = size;
    buffer.buffer = malloc(size);
}

VertexBuffer::~VertexBuffer() { Release(); }

HRESULT VertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags) {
    (void)SizeToLock;
    (void)Flags;
    if (!buffer.buffer)
        return E_FAIL;
    *ppbData = (void*)((char*)buffer.buffer + OffsetToLock);
    return S_OK;
}

HRESULT VertexBuffer::Unlock() {
    return S_OK;
}

HRESULT VertexBuffer::Release() {
    if (buffer.buffer) {
        free(buffer.buffer);
        buffer.buffer = NULL;
    }
    return S_OK;
}

TextHelper::TextHelper(TTF_Font* font, GLuint sprite, int size)
    : m_sprite(sprite), m_size(size), m_displayScale(1.0f), m_fontsize(0), m_posx(0), m_posy(0), m_inv(1.0f),
      m_texture(0), m_sizew(0), m_sizeh(0), m_vertexBuffer(NULL), m_vertexCapacity(0) {
    // set colors
    m_forecol[0] = m_forecol[1] = m_forecol[2] = m_forecol[3] = 1.0f;
    // setup texture (font is loaded at high pt size for sharp glyphs)
    m_fontsize = TTF_FontHeight(font);
    m_displayScale = (m_fontsize > 0) ? (static_cast<float>(size) / static_cast<float>(m_fontsize)) : 1.0f;
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int w = npot(16 * m_fontsize);
    void* tmp = malloc(w * w * 4);
    memset(tmp, 0, w * w * 4);
    m_sizew = w;
    m_sizeh = w;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_sizew, m_sizeh, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
    free(tmp);
    SDL_Color forecol = {255, 255, 255, 255};
    m_inv = 1.0f / (float)m_sizew;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            char text[2] = {(char)(i * 16 + j), 0};
            SDL_Surface* surf = TTF_RenderText_Blended(font, text, forecol);
            if (surf) {
                m_as[i * 16 + j] = surf->w;
                // Keep the full font cell height so descenders (e.g. 'g') are not clipped.
                int subh = (surf->h > m_fontsize) ? m_fontsize : surf->h;
#ifdef __EMSCRIPTEN__
                // WebGL 1 does not support GL_UNPACK_ROW_LENGTH; use tight buffer when pitch differs.
                int rowBytes = surf->w * surf->format->BytesPerPixel;
                if (surf->pitch != rowBytes && subh > 0) {
                    unsigned char* tight = (unsigned char*)malloc((size_t)rowBytes * subh);
                    for (int row = 0; row < subh; row++)
                        memcpy(tight + (size_t)row * rowBytes, (const char*)surf->pixels + (size_t)row * surf->pitch, (size_t)rowBytes);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, j * m_fontsize, i * m_fontsize, surf->w, subh, GL_RGBA, GL_UNSIGNED_BYTE, tight);
                    free(tight);
                } else
                    glTexSubImage2D(GL_TEXTURE_2D, 0, j * m_fontsize, i * m_fontsize, surf->w, subh, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
#else
                glPixelStorei(GL_UNPACK_ROW_LENGTH, surf->pitch / surf->format->BytesPerPixel);
                glTexSubImage2D(GL_TEXTURE_2D, 0, j * m_fontsize, i * m_fontsize, surf->w, subh, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
                SDL_FreeSurface(surf);
            } else {
                m_as[i * 16 + j] = m_fontsize / 2;
            }
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextHelper::~TextHelper() {
    if (m_texture)
        glDeleteTextures(1, &m_texture);
    if (m_vertexBuffer) {
        delete m_vertexBuffer;
        m_vertexBuffer = NULL;
    }
}

bool TextHelper::EnsureVertexCapacity(uint32_t requiredVertices) {
    if (m_vertexBuffer && (requiredVertices <= m_vertexCapacity))
        return true;

    uint32_t newCapacity = 64;
    while (newCapacity < requiredVertices)
        newCapacity <<= 1;

    RenderDevice* dev = GetRenderDevice();
    VertexBuffer* newBuffer = NULL;
    if (FAILED(dev->CreateVertexBuffer((UINT)(newCapacity * sizeof(TextVertex)), VB_USAGE_WRITEONLY,
                                       FVF_XYZRHW | FVF_DIFFUSE | FVF_TEX1, POOL_DEFAULT, &newBuffer, NULL)))
        return false;

    if (m_vertexBuffer)
        delete m_vertexBuffer;

    m_vertexBuffer = newBuffer;
    m_vertexCapacity = newCapacity;
    return true;
}

void TextHelper::SetInsertionPos(int x, int y) {
    m_posx = x;
    m_posy = y;
}

void TextHelper::SetDisplaySize(int size) {
    m_size = size;
    m_displayScale = (m_fontsize > 0) ? (static_cast<float>(size) / static_cast<float>(m_fontsize)) : 1.0f;
}

int TextHelper::MeasureTextWidth(const wchar_t* line) const {
    if (!line)
        return 0;

    float width = 0.0f;
    for (uint32_t i = 0; line[i] != 0; ++i) {
        const unsigned char ch = static_cast<unsigned char>(line[i] & 0xff);
        width += static_cast<float>(m_as[ch]) * m_displayScale;
    }

    return static_cast<int>(width + 0.5f);
}

void TextHelper::DrawTextLine(const wchar_t* line) {
    if (!line) {
        m_posy += m_size;
        return;
    }

    uint32_t glyphCount = 0;
    while (line[glyphCount] != 0)
        ++glyphCount;

    if (glyphCount == 0) {
        m_posy += m_size;
        return;
    }

    const uint32_t vertexCount = glyphCount * 6;
    if (!EnsureVertexCapacity(vertexCount)) {
        m_posy += m_size;
        return;
    }

    RenderDevice* dev = GetRenderDevice();
    dev->SetRenderState(RS_ZENABLE, FALSE);
    dev->SetRenderState(RS_CULLMODE, CULL_NONE);
    dev->SetRenderState(RS_ALPHABLENDENABLE, TRUE);
    dev->SetRenderState(RS_SRCBLEND, BLEND_SRCALPHA);
    dev->SetRenderState(RS_DESTBLEND, BLEND_INVSRCALPHA);
    dev->SetTextureStageState(0, TSS_COLOROP, TOP_MODULATE);
    dev->SetTextureStageState(0, TSS_COLORARG1, TA_TEXTURE);
    dev->SetTextureStageState(0, TSS_COLORARG2, TA_DIFFUSE);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    dev->SetStreamSource(0, m_vertexBuffer, 0, sizeof(TextVertex));
    dev->SetFVF(FVF_XYZRHW | FVF_DIFFUSE | FVF_TEX1);
    auto DrawPass = [&](float baseX, float baseY, DWORD passColor) {
        TextVertex* pVertices = NULL;
        if (FAILED(m_vertexBuffer->Lock(0, vertexCount * sizeof(TextVertex), (void**)&pVertices, 0)))
            return;

        float posx = baseX;
        const float posy = baseY;
        uint32_t out = 0;

        for (uint32_t i = 0; i < glyphCount; i++) {
            const unsigned char ch = (unsigned char)(line[i] & 0xff);
            const float col = (float)(ch % 16);
            const float lin = (float)(ch / 16);
            const float w = m_as[ch] * m_displayScale;
            const float h = m_fontsize * m_displayScale;

            // Sample inside texel centers to avoid bleeding from adjacent glyph cells.
            const float texelInset = 0.5f;
            const float uMinPx = col * m_fontsize + texelInset;
            float uMaxPx = col * m_fontsize + (float)m_as[ch] - texelInset;
            if (uMaxPx < uMinPx)
                uMaxPx = uMinPx;
            const float vMinPx = lin * m_fontsize + texelInset;
            const float vMaxPx = lin * m_fontsize + (float)m_fontsize - texelInset;

            const float u0 = uMinPx * m_inv;
            const float u1 = uMaxPx * m_inv;
            const float v0 = vMinPx * m_inv;
            const float v1 = vMaxPx * m_inv;

            pVertices[out++] = {posx, posy, 0.5f, 1.0f, passColor, u0, v0};
            pVertices[out++] = {posx + w, posy, 0.5f, 1.0f, passColor, u1, v0};
            pVertices[out++] = {posx + w, posy + h, 0.5f, 1.0f, passColor, u1, v1};

            pVertices[out++] = {posx, posy, 0.5f, 1.0f, passColor, u0, v0};
            pVertices[out++] = {posx + w, posy + h, 0.5f, 1.0f, passColor, u1, v1};
            pVertices[out++] = {posx, posy + h, 0.5f, 1.0f, passColor, u0, v1};

            posx += w;
        }

        m_vertexBuffer->Unlock();
        dev->DrawPrimitive(PT_TRIANGLELIST, 0, vertexCount / 3);
    };

    const BYTE alpha = (BYTE)(m_forecol[3] * 255.0f);
    const DWORD shadowColor = RGBA_MAKE(0, 0, 0, alpha);
    const DWORD color = RGBA_MAKE((BYTE)(m_forecol[0] * 255.0f), (BYTE)(m_forecol[1] * 255.0f),
                                  (BYTE)(m_forecol[2] * 255.0f), alpha);

    DrawPass(static_cast<float>(m_posx) + 1.0f, static_cast<float>(m_posy) + 1.0f, shadowColor);
    DrawPass(static_cast<float>(m_posx), static_cast<float>(m_posy), color);

    dev->SetTextureStageState(0, TSS_COLOROP, TOP_DISABLE);
    dev->SetRenderState(RS_ALPHABLENDENABLE, FALSE);
    dev->SetRenderState(RS_ZENABLE, TRUE);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_posy += m_size;
}

void TextHelper::DrawFormattedTextLine(const std::wstring& line) {
    DrawTextLine(line.c_str());
}

void TextHelper::SetForegroundColor(glm::vec4 clr) {
    m_forecol[0] = clr.r;
    m_forecol[1] = clr.g;
    m_forecol[2] = clr.b;
    m_forecol[3] = clr.a;
}

static RenderDevice* device = NULL;
RenderDevice* GetRenderDevice() {
    if (!device)
        device = new RenderDevice();
    return device;
}

static SurfaceDesc surface_desc = {0};
const SurfaceDesc* GetBackBufferSurfaceDesc() {
    surface_desc.Width = wideScreen ? 800 : 640;
    surface_desc.Height = 480;
    return &surface_desc;
}

DOUBLE GetTimeSeconds() { return ((DOUBLE)SDL_GetTicks()) / 1000.0; }

void ResetRenderEnvironment() {
    // NOTHING?
}
