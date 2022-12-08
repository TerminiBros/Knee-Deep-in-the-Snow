#include "splash.h"

static Texture2D texTBSplash;
static Sound sfxTBSplash;

static bool fadeToFG;
static int* mode;
static int nextMode;
static int screenWidth;
static int screenHeight;
static double* time;

static double splashTimer;
static double tbSplashEnd;
static double rlSplashEnd;
static Vector2 rlSplashLine0;
static Vector2 rlSplashLine1;
static Color splashTint0;
static Color splashTint1;
static Color fgColor;
static Color bgColor;

void InitSplash(int width, int height, Color fg, Color bg, bool ft, int nm, int* m, double* t) {
    texTBSplash = LoadTexture("assets/textures/tbsplash.png");
    sfxTBSplash = LoadSound("assets/audio/tbsplash.ogg");      // Load splash audio
    screenWidth = width;
    screenHeight = height;
    fgColor = fg;
    bgColor = bg;
    splashTint0 = fg;
    splashTint1 = bg;
    fadeToFG = ft;
    nextMode = nm;
    mode = m;
    time = t;
    splashTimer = 0;
    tbSplashEnd = 22;
    rlSplashEnd = 62;
    rlSplashLine0 = (Vector2) {screenWidth * 0.5f - 24, screenHeight * 0.5f - 24};
    rlSplashLine1 = (Vector2) {screenWidth * 0.5f - 24, screenHeight * 0.5f - 24};
}

void UpdateSplash(void) {
    if (splashTimer > 0 && splashTimer < 1.5f && !IsSoundPlaying(sfxTBSplash)) {
        PlaySound(sfxTBSplash);
    }
    if (splashTimer < rlSplashEnd) {
        splashTimer += *time * 8;
    } 
    if (splashTimer >= rlSplashEnd) {
        *mode = nextMode;
    }
    if (IsKeyPressed(KEY_ENTER)) {
         *mode = nextMode;
    }
}

void DrawSplash(void) {
    ClearBackground(bgColor);

    float left = screenWidth * 0.5f - 24;
    float top = screenHeight * 0.5f - 24;
    float right = screenWidth * 0.5f + 24;
    float bottom = screenHeight * 0.5f + 24;

    if (splashTimer < tbSplashEnd) {
        DrawTexturePro(
            texTBSplash,
            // Move rec along texture 24 pixels at a *time until animation is over (timer reaches 8) 
            (Rectangle) { (int)(splashTimer) < 8 ? (int)(splashTimer) * 24 : 8 * 24, 0, 24, 24},
            // Draw centered logo scaled by current screen width
            (Rectangle) {screenWidth * 0.5f, screenHeight * 0.5f, 72, 72},
            (Vector2) {36, 36},
            0,
            splashTint0
        ); 
        // Set the logo draw color white until timer reaches 14, then lerp to blank
        splashTint0 = (splashTimer > (tbSplashEnd - 4)) ? LerpColor(splashTint0, bgColor, *time * 10 ) : fgColor;
        // Make sure the logo is blank once we reach the end
    }
    else if (splashTimer > tbSplashEnd + 4 && splashTimer < tbSplashEnd + 12) {
        splashTint1 = LerpColor(splashTint1, fgColor, *time * 8);

        rlSplashLine0.x = Lerp(rlSplashLine0.x, right, *time * 8);
        rlSplashLine0.y = Lerp(rlSplashLine0.y, bottom, *time * 8);

        // Horizontal Left
        DrawLineEx(
            (Vector2) {left, top}, 
            (Vector2) {rlSplashLine0.x, top},
            2, fgColor
        );
        // Vertical Top
        DrawLineEx(
            (Vector2) {left, top}, 
            (Vector2) {left, rlSplashLine0.y}, 
            2, fgColor
        );

        DrawText("POWERED BY", screenWidth * 0.5f - 32, screenHeight * 0.5f - 38, 10, splashTint1);
    }
    if (splashTimer > tbSplashEnd + 12 && splashTimer < tbSplashEnd + 22) {
        
        rlSplashLine1.x = Lerp(rlSplashLine1.x, right, *time * 8);
        rlSplashLine1.y = Lerp(rlSplashLine1.y, bottom, *time * 8);

        DrawText("POWERED BY", screenWidth * 0.5f - 32, screenHeight * 0.5f - 38, 10, fgColor);

        DrawLineEx((Vector2) {left, top}, (Vector2) {right, top}, 2, fgColor);
        DrawLineEx((Vector2) {left, top}, (Vector2) {left, bottom}, 2, fgColor);
        // Horizontal Bottom
        DrawLineEx(
            (Vector2) {right, top}, 
            (Vector2) {right, rlSplashLine1.y}, 
            2, fgColor
        );
        // Vertical Right
        DrawLineEx(
            (Vector2) {left, bottom}, 
            (Vector2) {rlSplashLine1.x, bottom}, 
            2, fgColor
        );

        splashTint1 = bgColor;
    }
    if (splashTimer > tbSplashEnd + 22 && splashTimer < tbSplashEnd + 36) {
        
        splashTint0 = LerpColor(splashTint0, fgColor, *time * 10);

        DrawText("POWERED BY", screenWidth * 0.5f - 32, screenHeight * 0.5f - 38, 10, fgColor);
        DrawLineEx((Vector2) {left, top}, (Vector2) {right, top}, 2, fgColor);
        DrawLineEx((Vector2) {left, top}, (Vector2) {left, bottom}, 2, fgColor);
        DrawLineEx((Vector2) {right, top}, (Vector2) {right, bottom}, 2, fgColor);
        DrawLineEx((Vector2) {left, bottom}, (Vector2) {right, bottom}, 2, fgColor);
        DrawText("raylib", screenWidth * 0.5f - 8, screenHeight * 0.5f + 10, 10, splashTint0);
    }
    if (splashTimer > tbSplashEnd + 36) {
        Color* col;
        if (fadeToFG) {
            col = &fgColor;
            if (splashTimer < rlSplashEnd) {
                splashTint1 = LerpColor(splashTint1, fgColor, *time * 10);
            } else {
                splashTint1 = fgColor;
            }
            ClearBackground(splashTint1);
        } else {
            col = &splashTint0;
            if (splashTimer < rlSplashEnd - 1) {
                *col = LerpColor(*col, bgColor, *time * 10);
            } else {
                *col = bgColor;
            }
        }
        DrawText("POWERED BY", screenWidth * 0.5f - 32, screenHeight * 0.5f - 38, 10, *col);
        DrawLineEx((Vector2) {left, top}, (Vector2) {right, top}, 2, *col);
        DrawLineEx((Vector2) {left, top}, (Vector2) {left, bottom}, 2, *col);
        DrawLineEx((Vector2) {right, top}, (Vector2) {right, bottom}, 2, *col);
        DrawLineEx((Vector2) {left, bottom}, (Vector2) {right, bottom}, 2, *col);
        DrawText("raylib", screenWidth * 0.5f - 8, screenHeight * 0.5f + 10, 10, *col);
    }
}
