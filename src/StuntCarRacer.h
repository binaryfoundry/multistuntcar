
#ifndef _STUNT_CAR_RACER
#define _STUNT_CAR_RACER

/*    ========= */
/*    Constants */
/*    ========= */
#define SCR_BASE_COLOUR 26

// Screen resolution constants
#define BASE_WIDTH_STANDARD 640   // Standard 4:3 base width
#define BASE_WIDTH_WIDESCREEN 800 // Widescreen 16:10 base width
#define BASE_HEIGHT 480           // Base height for both modes

typedef enum { TRACK_MENU = 0, TRACK_PREVIEW, GAME_IN_PROGRESS, GAME_OVER } GameModeType;

// Untransformed coloured textured vertex: FVF_UTVERTEX in platform_sdl_gl.h

/*    ===================== */
/*    Structure definitions */
/*    ===================== */
/*
// Untransformed coloured vertex
struct UTVERTEX
{
    glm::vec3 pos, normal;
    DWORD color;
};
*/
#ifndef linux
// Untransformed coloured textured vertex
struct UTVERTEX {
    glm::vec3 pos; // The untransformed position for the vertex
    DWORD color;     // The vertex diffuse color value
    FLOAT tu, tv;    // The texture co-ordinates
};
#endif

/*    ============================== */
/*    External function declarations */
/*    ============================== */
extern void GetScreenDimensions(long* screen_width, long* screen_height);

extern DWORD SCRGB(long colour_index);
extern DWORD SCColour(long colour_index);

extern void SetSolidColour(long colour_index);
extern void SetLineColour(long colour_index);
extern void SetTextureColour(long colour_index);

// Debug
extern long VALUE1, VALUE2, VALUE3;

/** Call when the car is placed back on the track so engine audio restarts on next input (keyboard or gamepad). */
extern void RequestRestartEngineAudioOnFirstInput(void);

#endif /* _STUNT_CAR_RACER */
