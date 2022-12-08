#ifndef BATBOX_H
#define BATBOX_H

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdint.h>

#define ARRAY_LEN(x)  (sizeof(x) / sizeof((x)[0]))
#define HALF(x) ((float)(x) / 2.0)

typedef uint8_t byte;

Vector2 Smoovement(Vector2 pos, Vector2 vel);
int ModFloor(int a, int n);
void swapf(float* a, float* b);
float fclampf(float val, float min, float max);
double fclamp(double val, double min, double max);
Rectangle ScaleRectangle(Rectangle rec, float amount);
Rectangle ScaleRectangleEx(Rectangle rec, float left, float top, float right, float bottom);

void PlaySoundPitch(Sound sound, float pitch, bool multi);
void PlaySoundRandomPitch(Sound sound, float min, float max, bool multi);
void PlaySoundRandomPick(Sound* sounds, bool multi);
void PlaySoundRandom(Sound* sounds, float min, float max, bool multi);

float LerpDegrees(float a, float b, float lerpFactor);
Color LerpColor(Color a, Color b, float t);

Vector2 Vector2Int(Vector2 v);
Vector2 Vector2Floor(Vector2 v);
Vector2 Vector2MinMax(Vector2 v, float x, bool max);

void DrawTextCentered(const char *text, float x, float y, float fontSize, float spacing, Color tint);
void DrawTextCenteredEx(Font font, const char *text, Vector2 position, float rotation, float fontSize, float spacing, Color tint);

// Persistent storage functions
#define STORAGE_DATA_FILE "storage.data" // Storage file name
bool SaveStorageValue(unsigned int position, int value);
int LoadStorageValue(unsigned int position);

#ifdef BATBOX_IMPL

 // Returns a pixel-perfect diagonal position based on pos and vel
Vector2 Smoovement(Vector2 pos, Vector2 vel) {
    Vector2 smoothPos = pos;
    if (Vector2Length(vel) == 0) { return smoothPos; }
    if (fabsf(vel.x) > fabsf(vel.y)) {
        smoothPos.y = pos.y + (roundf(pos.x) - pos.x) * vel.y / vel.x;
    } else {
        smoothPos.x = pos.x + (roundf(pos.y) - pos.y) * vel.x / vel.y;
    }
    return (Vector2) {roundf(smoothPos.x), roundf(smoothPos.y)};
}
// Wraps int a positively and negatively within 0-n
int ModFloor(int a, int n) {
    return ((a % n) + n) % n;
}
// Swaps two floats
inline void swapf(float* a, float* b) {
    float c = *a;
    *a = *b; 
    *b = c;
}
// Returns float val between min and max
inline float fclampf(float val, float min, float max) {
    return fmaxf(min, fminf(max, val));
}
// Returns double val between min and max
inline double fclamp(double val, double min, double max) {
    return fmax(min, fmin(max, val));
}
// Scales position and size of rec by amount
Rectangle ScaleRectangle(Rectangle rec, float amount) {
    return (Rectangle) {rec.x * amount, rec.y * amount, rec.width * amount, rec.height * amount};
}
// Scales rec by amount in each specified direction
Rectangle ScaleRectangleEx(Rectangle rec, float left, float top, float right, float bottom) {
    return (Rectangle) {
        rec.x - left,
        rec.y - top,
        rec.width + left + right,
        rec.height + top + bottom
    };
}

void PlaySoundPitch(Sound sound, float pitch, bool multi) {
    if (GetSoundsPlaying() > 15) { return; } 
    SetSoundPitch(sound, pitch);
    multi ? PlaySoundMulti(sound) : PlaySound(sound);
}

void PlaySoundRandomPitch(Sound sound, float min, float max, bool multi) {
    if (GetSoundsPlaying() > 15) { return; } 
    float pitch = GetRandomValue(min * 100, max * 100) / 100.0;
    SetSoundPitch(sound, pitch);
    multi ? PlaySoundMulti(sound) : PlaySound(sound);
}

void PlaySoundRandomPick(Sound* sounds, bool multi) {
    if (GetSoundsPlaying() > 15) { return; }
    int soundID = GetRandomValue(0, ARRAY_LEN(&sounds));
    multi ? PlaySoundMulti(sounds[soundID]) : PlaySound(sounds[soundID]);
}

void PlaySoundRandom(Sound* sounds, float min, float max, bool multi) {
    if (GetSoundsPlaying() > 15) { return; } 
    int soundID = GetRandomValue(0, ARRAY_LEN(&sounds)); 
    float pitch = GetRandomValue(min * 100, max * 100) / 100.0;
    SetSoundPitch(sounds[soundID], pitch);
    multi ? PlaySoundMulti(sounds[soundID]) : PlaySound(sounds[soundID]);
}

#pragma region Lerps
/// Returns point t between angles a and b (both between 0.0f and 360.0f), taking the shortest path
float LerpDegrees(float a, float b, float t) {
    float result;
    float diff = b - a;
    if (diff < -180.0f) {
        // lerp upwards past 360
        b += 360.0f;
        result = Lerp(a, b, t);
        if (result >= 360.f) {
            result -= 360.f;
        }
    }
    else if (diff > 180.0f) {
        // lerp downwards past 0
        b -= 360.0f;
        result = Lerp(a, b, t);
        if (result < 0.f) {
            result += 360.0f;
        }
    }
    else {
        // straight lerp
        result = Lerp(a, b, t);
    }
    return result;
}

Color LerpColor(Color a, Color b, float t) {
    float outR = Lerp((float)a.r / 255.0f, (float)b.r / 255.0f, t);
    float outG = Lerp((float)a.g / 255.0f, (float)b.g / 255.0f, t);
    float outB = Lerp((float)a.b / 255.0f, (float)b.b / 255.0f, t);
    float outA = Lerp((float)a.a / 255.0f, (float)b.a / 255.0f, t);

    return (Color) { (char)(outR*255), (char)(outG*255), (char)(outB*255), (char)(outA*255) };
}

// Color LerpColor(Color a, Color b, float value) {
//     Vector3 A = ColorToHSV(a);
//     Vector3 B = ColorToHSV(b);
//     Vector3 blend = Vector3Lerp(A, B, value);
//     Color C = ColorFromHSV(blend.x, blend.y, blend.z);
//     return (Color) {C.r, C.g, C.b, Lerp(a.a, b.a, value)};
// }

// Color LerpColorEx(Color a, Color b, float value, int flags) {
//     Vector3 A = ColorToHSV(a);
//     Vector3 B = ColorToHSV(b);
//     float hue = (flags & LERP_HUE) ? Lerp(A.x, B.x, value) : A.x;
//     float sat = (flags & LERP_SAT) ? Lerp(A.y, B.y, value) : A.y;
//     float val = (flags & LERP_VAL) ? Lerp(A.z, B.z, value) : A.z;
//     return ColorFromHSV(hue, sat, val);
// }

#pragma endregion

#pragma region Vector2 Functions

Vector2 Vector2Int(Vector2 v) {
    Vector2 vi;
    vi.x = (int)(v.x);
    vi.y = (int)(v.y);
    return vi;
}

Vector2 Vector2Floor(Vector2 v) {
    Vector2 vf; 
    vf.x = floorf(v.x);
    vf.y = floorf(v.y);
    return vf;
}

Vector2 Vector2MinMax(Vector2 v, float x, bool max) {
    Vector2 vm;
    vm.x = max ? fmaxf(x, v.x) : fminf(x, v.x);
    vm.y = max ? fmaxf(x, v.y) : fminf(x, v.y);
    return vm;
}

#pragma endregion
// Draws centered text in the default font 
void DrawTextCentered(const char *text, float x, float y, float fontSize, float spacing, Color tint) {
    Vector2 measure = MeasureTextEx(GetFontDefault(), text, fontSize, spacing);
    DrawTextEx(GetFontDefault(), text, (Vector2) {x - measure.x / 2, y - measure.y / 2}, fontSize, spacing, tint);
}
// Draws centered text in the specified font with rotation 
void DrawTextCenteredEx(Font font, const char *text, Vector2 position, float rotation, float fontSize, float spacing, Color tint) {
    Vector2 measure = MeasureTextEx(font, text, fontSize, spacing);
    DrawTextPro(font, text, position, (Vector2) {measure.x / 2, measure.y / 2}, rotation, fontSize, spacing, tint);
}

// Save integer value to storage file (to defined position)
// NOTE: Storage positions is directly related to file memory layout (4 bytes each integer)
bool SaveStorageValue(unsigned int position, int value)
{
    bool success = false;
    unsigned int dataSize = 0;
    unsigned int newDataSize = 0;
    unsigned char *fileData = LoadFileData(STORAGE_DATA_FILE, &dataSize);
    unsigned char *newFileData = NULL;

    if (fileData != NULL) {
        if (dataSize <= (position*sizeof(int))) {
            // Increase data size up to position and store value
            newDataSize = (position + 1)*sizeof(int);
            newFileData = (unsigned char *)RL_REALLOC(fileData, newDataSize);

            if (newFileData != NULL) {
                // RL_REALLOC succeded
                int *dataPtr = (int *)newFileData;
                dataPtr[position] = value;
            }
            else {
                // RL_REALLOC failed
                TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to realloc data (%u), position in bytes (%u) bigger than actual file size", STORAGE_DATA_FILE, dataSize, position*sizeof(int));

                // We store the old size of the file
                newFileData = fileData;
                newDataSize = dataSize;
            }
        }
        else {
            // Store the old size of the file
            newFileData = fileData;
            newDataSize = dataSize;

            // Replace value on selected position
            int *dataPtr = (int *)newFileData;
            dataPtr[position] = value;
        }

        success = SaveFileData(STORAGE_DATA_FILE, newFileData, newDataSize);
        RL_FREE(newFileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Saved storage value: %i", STORAGE_DATA_FILE, value);
    }
    else {
        TraceLog(LOG_INFO, "FILEIO: [%s] File created successfully", STORAGE_DATA_FILE);

        dataSize = (position + 1)*sizeof(int);
        fileData = (unsigned char *)RL_MALLOC(dataSize);
        int *dataPtr = (int *)fileData;
        dataPtr[position] = value;

        success = SaveFileData(STORAGE_DATA_FILE, fileData, dataSize);
        UnloadFileData(fileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Saved storage value: %i", STORAGE_DATA_FILE, value);
    }

    return success;
}

// Load integer value from storage file (from defined position)
// NOTE: If requested position could not be found, value 0 is returned
int LoadStorageValue(unsigned int position)
{
    int value = 0;
    unsigned int dataSize = 0;
    unsigned char *fileData = LoadFileData(STORAGE_DATA_FILE, &dataSize);

    if (fileData != NULL) {
        if (dataSize < (position*4)) TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to find storage position: %i", STORAGE_DATA_FILE, position);
        else {
            int *dataPtr = (int *)fileData;
            value = dataPtr[position];
        }

        UnloadFileData(fileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Loaded storage value: %i", STORAGE_DATA_FILE, value);
    }

    return value;
}

#endif

#endif // BATBOX_H
