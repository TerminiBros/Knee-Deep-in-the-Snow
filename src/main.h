#pragma once
#include <raylib.h>
#ifdef MAIN_IMPL
#include <reasings.h>
#define BATBOX_IMPL
#include "batbox.h"
#define BATANIM_IMPL
#include "batanim.h"
#define BATIO_IMPL
#include "batio.h"
#if defined(_DEBUG)
    #define TBDB_DEBUG_IMPL
    #include "tbdebug.h"
#endif
#endif
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
    #define GLSL_VERSION (100)
#else
    #define GLSL_VERSION (330)
#endif

//-----Globals-----//
#ifdef MVERSION
#define TITLE MTITLE        // Makefile definitions
#define VERSION MVERSION
#else
#define TITLE ("Template")  // Game title
#define VERSION ("2.2.2")   // Generated by makefile
#endif

static bool showSplash = 1;
static int monitor = 0;
static int targetRefresh = 60;

static int initScale = 3;
static Rectangle content = {128, 128, 256, 256};
static float scaledWidth, scaledHeight;
static float oldScreenWidth, oldScreenHeight;
static Vector2 scaledOrigin, screenOrigin;

//-----Assets-----//
static RenderTexture2D rtxContent; // Pixel canvas
static Texture2D texGrid;
static Texture2D texTestPlane;
static Texture2D texSky;
static Shader shdWarp;
static Sound sfxPause;
static Font fntLilLabels;
#define LIL_LABELS_FONT_SIZE (7)

//-----Definitions-----//
typedef struct State { // Global state, keeps track of time and mode
    int mode;
    int prevMode;
    double totalTime;
    double deltaTime;
    void (*UpdateFunc)(void);
    void (*DrawFunc)(void);
    bool isPaused;
    bool wasUnfocused;
} State;

enum Modes {
    MODE_SPLASH,
    MODE_GAME,
    MODE_PAUSED,
};

#ifdef MAIN_IMPL
State state;
#endif

//-----Functions-----//

 // Initializes window, target framerate, audio device, assets, splash screen
static void Init(void);
 // Runs once before update
static void Setup(void);
static void Update(void);
static void Draw(void);
static void Unload(void);
static void UpdateDrawFrame(void);

static void LoadAssets(void); 
static void UnloadAssets(void);

static void UpdateGame(void);
static void DrawGame(void);

static void PauseGame(void);
static void ResumeGame(void);
static void UpdatePaused(void);

static void ScaleWindowToContent(void);
static void DrawDebugInfo(void);

#ifdef MAIN_IMPL
#include <raymath.h>

//-----Implementation-----//
void LoadAssets(void) { // Loads textures, shaders, audio, fonts, etc.
    rtxContent = LoadRenderTexture(content.width, content.height);
    texGrid = LoadTexture("assets/textures/grid.png");                              // Load a texture
    texTestPlane = LoadTexture("assets/textures/snow.png");
    texSky  = LoadTexture("assets/textures/skybox.png");
    shdWarp = LoadShader(0, TextFormat("assets/shaders/warp%d.fs", GLSL_VERSION));  // Load a shader based on GLSL version
    sfxPause = LoadSound("assets/audio/pause.ogg");                                 // Load a sound
    fntLilLabels = LoadFontEx("assets/fonts/lil_labels.ttf", 7, 0, 0);              // Load a font
    SetTextureFilter(GetFontDefault().texture, TEXTURE_FILTER_POINT);               // Ensure default font is pixelated
}

void UnloadAssets(void) {
    UnloadRenderTexture(rtxContent);
}


static Vector2 playerPos = {0,0};
static float rotationY = 0;
static float rotationX = 0;
static float skyScroll = 0;
bool isMouseLocked = false;
Vector2 mouseDelta = {0,0};
Vector2 mouseSensitivity = {0.1,0.05};

void UpdateGame(void) {
    if (IsInputP(INPUT_START)) { PauseGame(); }

    if (!isMouseLocked) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            isMouseLocked = true;
            DisableCursor();
        }
    } else {
        mouseDelta = GetMouseDelta();
        SetMousePosition(content.x,content.y);

        if (IsKeyPressed(KEY_ESCAPE)) {
            isMouseLocked = false;
            EnableCursor();
        }
    }



    rotationY -= mouseDelta.x * mouseSensitivity.x;
    if (rotationY > 360) rotationY -= 360;
    if (rotationY < 0) rotationY += 360;
    rotationX += mouseDelta.y * mouseSensitivity.y;
    rotationX = Clamp(rotationX,-80,70);

    skyScroll -= mouseDelta.x * mouseSensitivity.x;
    if (skyScroll > 512) skyScroll -= 512;
    if (skyScroll < 0) skyScroll += 512;

    int horizontal = (( (input & INPUT_RIGHT) > 0) - ((input & INPUT_LEFT) > 0 ));
    int vertical   = (( (input & INPUT_DOWN) > 0) - ((input & INPUT_UP) > 0 ));

    playerPos = Vector2Add(Vector2Rotate((Vector2){-horizontal * state.deltaTime, -vertical * state.deltaTime}, rotationY),playerPos); 
    


}

void RenderScene(void) {

    DrawTexture(texSky, skyScroll, 64, WHITE);
    DrawTexture(texSky, skyScroll-512, 64, WHITE);


    Camera cam;
    cam.projection = CAMERA_PERSPECTIVE;
    cam.fovy = 90;

    cam.up = (Vector3){0,1,0};
    cam.position = (Vector3){playerPos.x,1,playerPos.y};


    Quaternion Q = QuaternionMultiply(
        QuaternionFromAxisAngle((Vector3){0,1,0}, rotationY * DEG2RAD),
        QuaternionFromAxisAngle((Vector3){1,0,0}, rotationX * DEG2RAD)
    );

    cam.target = Vector3Add(
        cam.position,
        Vector3RotateByQuaternion((Vector3){0,0,1}, Q)
    );

    BeginMode3D(cam);

    DrawCubeTexture(texTestPlane, (Vector3){0,0,0}, 256, 0.1, 256, WHITE);

    EndMode3D();
}

void DrawGame(void) {
    ClearBackground(RAYWHITE);

    for (int j = 0; j < content.height / 16; j++) { for (int i = 0; i < content.width / 16; i++) DrawTexture(texGrid, i * 16, j * 16, Fade(LIGHTGRAY, 0.4)); }
    
    RenderScene();

    DrawText(TextFormat("%s v%s", TITLE, VERSION), 3, 2, 10, DARKGRAY);

    static double fadeTimer = 0.4;
    if (fadeTimer > 0) { DrawRectangle(0, 0, content.width, content.height, Fade(BLACK, EaseQuadOut(fadeTimer, 0, 1, 0.4))); fadeTimer -= state.deltaTime; }
}

void PauseGame(void) {
    state.isPaused = true;
    state.prevMode = state.mode;
    state.mode = MODE_PAUSED;
    if (state.wasUnfocused) { return; }
    PlaySoundPitch(sfxPause, 1.0, false);
}

void ResumeGame(void) {
    state.isPaused = false;
    state.mode = state.prevMode;
    if (state.wasUnfocused) { return; }
    PlaySoundPitch(sfxPause, 1.1, false);
}

void UpdatePaused(void) {
    if (IsInputP(INPUT_START)) { ResumeGame(); }
}

void DrawPausedOverlay(void) {
    if (!state.isPaused) { return; }
    DrawRectangle(0, 0, content.width, content.height, Fade(BLACK, 0.5));
    if (state.wasUnfocused) { return; }
    DrawTextCentered("PAUSED", content.x, content.y, 10, 1, WHITE);
}

void DrawContent(void) {
    DrawTexturePro(rtxContent.texture,
    (Rectangle) {0, 0, content.width, -content.height},
    (Rectangle) {0, 0, content.width, content.height},
    (Vector2) {0, 0}, 0, WHITE);

    state.DrawFunc();

    DrawDebugInfo();
    
    DrawPausedOverlay();
}

void DrawDebugInfo(void) {
#if defined(_DEBUG)
    #define MAX_FRAME_SAMPLES (100)
    static float frameSamples[MAX_FRAME_SAMPLES] = {0};
    static float frameSampleTimer = 0;

    frameSampleTimer -= state.deltaTime;
    if (frameSampleTimer <= 0) {
        for (int i = MAX_FRAME_SAMPLES - 1; i > 0; i--) { frameSamples[i] = frameSamples[i-1]; }
        frameSamples[0] = GetFrameTime();
        frameSampleTimer = (1 / 60) * 10; 
    }
    for (int i = 0; i < MAX_FRAME_SAMPLES; i++) {
        float delayTime = frameSamples[i];
        for (int j = 0; j < delayTime * 50; j++) {
            DrawPixel(content.width - MAX_FRAME_SAMPLES + i, content.height - 1 - j, LerpColor(GetColor(0x00BB0011), GetColor(0xEE0000FF), (float)j / content.height));
        }
    }
    const char* fps = TextFormat("%d FPS", GetFPS());
    DebugDrawText(content.width - MeasureTextEx(fntLilLabels, fps, LIL_LABELS_FONT_SIZE, 1).x - 3, 2, fps);
    #undef MAX_FRAME_SAMPLES

    DrawDebug();
#endif
}

void ResizeWindowSnapped(void) {
    monitor = GetCurrentMonitor();
    targetRefresh = GetMonitorRefreshRate(monitor);
    SetTargetFPS(targetRefresh);
    float monitorWidth = GetMonitorWidth(monitor);
    float monitorHeight = GetMonitorHeight(monitor);

    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float widthRatio = screenWidth / content.width;
    float heightRatio = screenHeight / content.height;
    
    float contentScale = fminf(widthRatio, heightRatio); // pick smallest between width ratio and height ratio
    float snappedScale = oldScreenWidth * oldScreenHeight < screenWidth * screenHeight ? ceilf(contentScale) : floorf(contentScale);

    if (screenWidth < monitorWidth && screenHeight < monitorHeight ) { SetWindowSize(content.width * snappedScale, content.height * snappedScale); }

    screenWidth  = (float)GetScreenWidth();
    screenHeight = (float)GetScreenHeight();
    widthRatio = screenWidth / content.width;
    heightRatio  = screenHeight / content.height;
    contentScale = fminf(widthRatio, heightRatio); // pick smallest between width ratio and height ratio

    content.x = HALF(content.width);
    content.y = HALF(content.height);

    scaledWidth  = content.width * contentScale;
    scaledHeight = content.height * contentScale;
    scaledOrigin = (Vector2) { HALF(scaledWidth), HALF(scaledHeight) };
    screenOrigin = (Vector2) { HALF(screenWidth), HALF(screenHeight) };

    oldScreenWidth  = GetScreenWidth();
    oldScreenHeight = GetScreenHeight();
}

#endif

