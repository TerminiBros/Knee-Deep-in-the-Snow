#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#define MAIN_IMPL
#include "main.h"
#undef MAIN_IMPL
#include "splash.h"


int main(void) {
#ifdef _BUILD_MAC_APP
    ChangeDirectory(GetApplicationDirectory());
#endif
    Init();
#if defined(_DEBUG)
    showSplash = 0;
    SetTraceLogLevel(LOG_ALL);
#else
#endif
    Setup();
#ifdef PLATFORM_WEB
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetExitKey(0);
    while (!WindowShouldClose()) {
        if (IsWindowResized()) { ResizeWindowSnapped(); }
        UpdateDrawFrame();
    }
#endif
    Unload();         
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
void Init(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_TOPMOST | FLAG_MSAA_4X_HINT);
    InitWindow(content.width * initScale, content.height * initScale, TextFormat("Knee Deep in the Snow v%s", VERSION));
    SetWindowMinSize(content.width, content.height);
    ResizeWindowSnapped();
    InitAudioDevice();
    LoadAssets();
    InitSplash(content.width, content.height, GetColor(0x8370d0ff), bgColor, false, MODE_GAME, &state.mode, &state.deltaTime);
}

void Setup(void) {
    state.mode = showSplash ? MODE_SPLASH : MODE_GAME;
}

void Update(void) {
#if defined(_DEBUG)
    BeginDebug();
    SetDebugFont(fntLilLabels);
    SetDebugFontSize(7);
#endif
    UpdateMusicStream(state.currentMusic);
    UpdateInput();

    state.deltaTime = GetFrameTime();
    state.totalTime += state.deltaTime;

    switch (state.mode) 
    {
    case MODE_SPLASH:
        state.UpdateFunc = &UpdateSplash;
        state.DrawFunc = &DrawSplash;
        break;
    case MODE_GAME:
        state.unpausedTime += state.deltaTime;
        state.UpdateFunc = &UpdateGame;
        state.DrawFunc = &DrawGame;
        break;
    case MODE_PAUSED:
        state.UpdateFunc = &UpdatePaused;
        break;
    }

    if (!IsWindowFocused() && !state.isPaused) {
        state.wasUnfocused = true;
        PauseGame();
    } else if (IsWindowFocused() && state.isPaused && state.wasUnfocused) {
        ResumeGame();
        state.wasUnfocused = false;
    }
    
    state.UpdateFunc();
}




void Draw(void) {
    ClearBackground(bgColor);

    RenderLightingTexture();

    BeginTextureMode(rtxContent);
        DrawContent();
        
    EndTextureMode();

BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    DrawTexturePro(rtxContent.texture,
        (Rectangle) {0, 0, content.width, -content.height},
        (Rectangle) {screenOrigin.x, screenOrigin.y, scaledWidth, scaledHeight},
        scaledOrigin, 0, WHITE
    );
EndBlendMode();
}

void Unload(void) {
    UnloadAssets();
}

void UpdateDrawFrame(void) {
    Update();
    BeginDrawing(); 
        Draw();
    EndDrawing();     
}
