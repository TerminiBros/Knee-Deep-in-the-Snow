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

#define NUM_SPRITES (512)

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
static RenderTexture2D rtxLightingTexture;
static RenderTexture2D rtxCombinedTexture;
static Texture2D texGrid;
static Texture2D texPlane;
static Texture2D texSnowman;
static Texture2D texSky;
static Texture2D texLight0;
static Texture2D texWeapons;
static Shader shdWarp;
static Sound sfxPause;
static Font fntLilLabels;
#define LIL_LABELS_FONT_SIZE (7)

//-----Definitions-----//
typedef struct State { // Global state, keeps track of time and mode
    int mode;
    int prevMode;
    double unpausedTime;
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
    rtxLightingTexture = LoadRenderTexture(256*4, 256*4);
    rtxCombinedTexture = LoadRenderTexture(256*4, 256*4);

    texGrid = LoadTexture("assets/textures/grid.png");                              // Load a texture
    texPlane = LoadTexture("assets/textures/snow.png");
    texSnowman = LoadTexture("assets/textures/snowman.png");
    GenTextureMipmaps(&texPlane);
    texSky  = LoadTexture("assets/textures/skybox.png");
    texLight0 = LoadTexture("assets/textures/light0.png");
    SetTextureFilter(texLight0, TEXTURE_FILTER_BILINEAR);

    texWeapons = LoadTexture("assets/textures/weapons.png");

    shdWarp = LoadShader(0, TextFormat("assets/shaders/warp%d.fs", GLSL_VERSION));  // Load a shader based on GLSL version
    sfxPause = LoadSound("assets/audio/pause.ogg");                                 // Load a sound
    fntLilLabels = LoadFontEx("assets/fonts/lil_labels.ttf", 7, 0, 0);              // Load a font
    SetTextureFilter(GetFontDefault().texture, TEXTURE_FILTER_POINT);               // Ensure default font is pixelated
}

void UnloadAssets(void) {
    UnloadRenderTexture(rtxContent);
}

typedef struct Light {
  float radius;
  Color color;  
} Light;

typedef struct GameSprite {
    bool enabled;
    float x, y;
    float angle;
    uint16_t animID;
    bool isAnimated;
    Rectangle rect;
    Color c;
    bool hasLight;
    Light light;
} GameSprite;

GameSprite sprites[NUM_SPRITES] = {
    {.x = 0, .y = 10, .c = RED, .enabled = true, .rect = {0,0,16,16}, .hasLight = true, .light = {.radius = 1.2, .color = RED}},
    {.x = 50, .y = 32, .c = PURPLE, .enabled = true, .rect = {0,0,16,16}, .hasLight = true, .light = {.radius = 1, .color = PINK}},
};

static Vector2 playerPos = {0,0};
static Vector2 playerVel = {0,0};
static float rotationY = 0;
static float rotationX = 0;
static float skyScroll = 0;
bool isMouseLocked = false;
Vector2 mouseDelta = {0,0};
Vector2 mouseSensitivity = {40.1,20.05};
Vector2 moveSpeed = {10,10};

void UpdateGame(void) {
    if (IsInputP(INPUT_START)) { PauseGame(); }

    if (!isMouseLocked) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            isMouseLocked = true;
            DisableCursor();
        }
    } else {
        mouseDelta = GetMouseDelta();
        // SetMousePosition(content.x,content.y);

        if (IsKeyPressed(KEY_ESCAPE)) {
            isMouseLocked = false;
            mouseDelta = (Vector2) {0,0};
            EnableCursor();
        }
    }



    rotationY -= mouseDelta.x * mouseSensitivity.x * state.deltaTime;
    if (rotationY > 360) rotationY -= 360;
    if (rotationY < 0) rotationY += 360;
    rotationX += mouseDelta.y * mouseSensitivity.y * state.deltaTime;
    rotationX = Clamp(rotationX,-80,70);

    skyScroll -= mouseDelta.x * mouseSensitivity.x * state.deltaTime;
    if (skyScroll > 360*2) skyScroll -= 360*2;
    if (skyScroll < 0) skyScroll += 360*2;

    int horizontal = (( (input & INPUT_RIGHT) > 0) - ((input & INPUT_LEFT) > 0 ));
    int vertical   = (( (input & INPUT_DOWN) > 0) - ((input & INPUT_UP) > 0 ));
    
    Vector2 playerInput = Vector2Normalize((Vector2){-horizontal, -vertical});

    playerVel =  Vector2Lerp( playerVel, (Vector2){playerInput.x * moveSpeed.x, playerInput.y * moveSpeed.y}, state.deltaTime * 15);

    playerPos = Vector2Add(Vector2Rotate((Vector2){playerVel.x * state.deltaTime, playerVel.y * state.deltaTime }, (-rotationY) * DEG2RAD),playerPos); 
    
    

}


void DrawSnowman(Camera cam, int spriteIndex) {
    int i = spriteIndex;

    Rectangle r0 = (Rectangle){0,48,64,48};
    Rectangle r1 = (Rectangle){48,0,64,48};
    Rectangle r2 = (Rectangle){0,0,48,48};
    
    float bob0 = 0.03 * sin(state.unpausedTime * 4);
    float bob1 = 0.02 * sin(state.unpausedTime * 3.8);
    float bob2 = 0.01 * sin(state.unpausedTime * 3.5);


    DrawBillboardRec(cam, texSnowman, r0, (Vector3){ sprites[i].x, 0.5 + bob0, sprites[i].y }, (Vector2){1,1}, WHITE);

    Vector2 pos = Vector2MoveTowards((Vector2){sprites[i].x, sprites[i].y}, playerPos, 0.1f);

    DrawBillboardRec(cam, texSnowman, r1, (Vector3){ pos.x, 1.1 + bob0 + bob1, pos.y }, (Vector2){1,1}, WHITE);

    pos = Vector2MoveTowards(pos, playerPos, 0.1f);
    
    DrawBillboardRec(cam, texSnowman, r2, (Vector3){ pos.x, 1.7 + bob0 + bob1 + bob2, pos.y }, (Vector2){1,1}, WHITE);

}



Vector2 MapCoordToLightCoord(float x, float y) {
    return (Vector2){(x+128) * 4, (y+128) * 4};
}


void RenderLightingTexture(void) {
    BeginTextureMode(rtxLightingTexture); {

        DrawRectangle(0,0,256*4,256*4, GetColor(0x01021aFF));
        Vector2 pl = MapCoordToLightCoord(playerPos.x,playerPos.y);

        DrawTextureEx(texLight0, Vector2Subtract((Vector2){pl.x, pl.y} , (Vector2){16*20,16*20}), 0, 20, GetColor(0x22223222));

        float scale = 2.3;
        float colorscale = scale * 1.2;
        DrawTextureEx(texLight0, Vector2Subtract((Vector2){pl.x, pl.y} , (Vector2){16*scale,16*scale}), 0, scale, GetColor(0x999999AA));
        
        for (size_t i = 0; i < NUM_SPRITES; i++)
        {
            if (!sprites[i].enabled || !sprites[i].hasLight) continue;

            Vector2 p = MapCoordToLightCoord(sprites[i].x, sprites[i].y);
            float s   = sprites[i].light.radius;

            DrawTextureEx(texLight0, Vector2Subtract((Vector2){p.x, p.y} , (Vector2){16*s,16*s}), 0, s, GetColor(0xAAAAAA77));
        }
        

        BeginBlendMode(BLEND_ADDITIVE);
        for (size_t i = 0; i < NUM_SPRITES; i++)
        {
            if (!sprites[i].enabled || !sprites[i].hasLight) continue;

            Vector2 p = MapCoordToLightCoord(sprites[i].x, sprites[i].y);
            float s   = sprites[i].light.radius * 1.2;

            DrawTextureEx(texLight0, Vector2Subtract((Vector2){p.x, p.y} , (Vector2){16*s,16*s}), 0, s, sprites[i].light.color);
        }
        DrawTextureEx(texLight0, Vector2Subtract((Vector2){pl.x, pl.y} , (Vector2){16*colorscale,16*colorscale}), 0, colorscale, GetColor(0xAAAAAAAA) );
        EndBlendMode();
        
        

        EndTextureMode();
    }

    BeginTextureMode(rtxCombinedTexture); {

        DrawTextureTiled(texPlane, (Rectangle) {0, 0, 512, 512}, (Rectangle) {0, 0, 1024, 1024}, (Vector2) {0, 0}, 0, 1, WHITE);

        BeginBlendMode(BLEND_MULTIPLIED);
        DrawTextureQuad(rtxLightingTexture.texture, (Vector2){1,-1}, (Vector2){0,0}, (Rectangle){0,0,256*4,256*4}, WHITE);
        EndBlendMode();

        EndTextureMode();
    } 

}

void RenderScene(void) {

    DrawTextureRec(texSky, (Rectangle) {0, 0, 360, 256},    (Vector2) {skyScroll, 0}, WHITE);
    DrawTextureRec(texSky, (Rectangle) {0, 0, 360, 256},    (Vector2) {skyScroll - 360, 0}, WHITE);
    DrawTextureRec(texSky, (Rectangle) {0, 256, 360, 256},  (Vector2) {skyScroll - 360 * 2, 0}, WHITE);


    Camera cam;
    cam.projection = CAMERA_PERSPECTIVE;
    cam.fovy = 90;

    cam.up = (Vector3){0,1,0};

    float bobbing = sin(state.unpausedTime * 12) * Vector2Length( (Vector2) {playerVel.x / moveSpeed.x, playerVel.y / moveSpeed.y} ) * 0.06;

    cam.position = (Vector3){playerPos.x, 1 +  bobbing,playerPos.y};


    Quaternion Q = QuaternionMultiply(
        QuaternionFromAxisAngle((Vector3){0,1,0}, rotationY * DEG2RAD),
        QuaternionFromAxisAngle((Vector3){1,0,0}, rotationX * DEG2RAD)
    );

    cam.target = Vector3Add(
        cam.position,
        Vector3RotateByQuaternion((Vector3){0,0,1}, Q)
    );

    BeginMode3D(cam);


    
    DrawCubeTexture(rtxCombinedTexture.texture, (Vector3){0,0,0}, 256, 0.1, 256, WHITE);

    for (size_t i = 0; i < NUM_SPRITES; i++)
    {
        if (sprites[i].enabled == false) {continue;}
        
        DrawSnowman(cam, i);
        //DrawBillboardRec(cam, texGrid, sprites[i].rect, (Vector3){ sprites[i].x, 1, sprites[i].y }, (Vector2){1,1}, sprites[i].c);
        
    }

    EndMode3D();

    

}

void RenderMapOverlay(void) {
    float ox = 128;
    float oy = 128;


    //DrawTextureEx(rtxLightingTexture.texture, (Vector2){0,0}, 0, 0.25, WHITE);
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        DrawTextureQuad(rtxLightingTexture.texture, (Vector2){1,-1}, (Vector2){0,0}, (Rectangle){0,0,256,256}, WHITE);
    }

    DrawPixel(ox + playerPos.x,oy + playerPos.y, GREEN);
    Vector2 lookLine = Vector2Add(Vector2Rotate((Vector2){0, 8}, (-rotationY) * DEG2RAD), playerPos);
    DrawLine( ox + playerPos.x, oy + playerPos.y, ox + lookLine.x, oy + lookLine.y, GREEN);

    Vector2 lookLine1 = Vector2Add(Vector2Rotate((Vector2){0, 13}, (-rotationY-45) * DEG2RAD), playerPos);
    DrawLine( ox + playerPos.x, oy + playerPos.y, ox + lookLine1.x, oy + lookLine1.y, BLUE);
    Vector2 lookLine2 = Vector2Add(Vector2Rotate((Vector2){0, 13}, (-rotationY+45) * DEG2RAD), playerPos);
    DrawLine( ox + playerPos.x, oy + playerPos.y, ox + lookLine2.x, oy + lookLine2.y, BLUE);

    for (size_t i = 0; i < NUM_SPRITES; i++)
    {
        if (!sprites[i].enabled) continue;
        DrawPixel(ox + sprites[i].x, oy + sprites[i].y, sprites[i].c);
    }


}


void DrawGame(void) {
    ClearBackground(RAYWHITE);

    //for (int j = 0; j < content.height / 16; j++) { for (int i = 0; i < content.width / 16; i++) DrawTexture(texGrid, i * 16, j * 16, Fade(LIGHTGRAY, 0.4)); }
    
    RenderScene();

    DrawTexture(texWeapons, 128 - texWeapons.width/2, 256 - texWeapons.height, WHITE);

    if (IsKeyDown(KEY_TAB)) {
        RenderMapOverlay();
    }

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

