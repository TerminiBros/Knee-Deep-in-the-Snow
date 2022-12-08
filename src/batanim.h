#ifndef BATANIM_H
#define BATANIM_H
#include <stdint.h>
#include <assert.h>

#define DEFAULT_FRAME_DURATION (0.1f) // Tenth of a second

// { index, duration }
typedef struct Frame {
    uint8_t index;    // 0, 1, 2, ...
    float duration;   // (Seconds)
} Frame;

// Animation of frames of specified length
typedef struct Anim {
    uint8_t length; // Frame count (0-255)
    uint8_t frame;  // Current frame
    float timer;    // Next frame when 0
    float speed;    // Playback rate
    bool isOver;    // Set to true when animation ends
    bool isUsed;    // Keep track of anims in use
    Frame* frames;  // [ {index, duration}, {...} ]
} Anim;

// Configures an animation
uint16_t SetupAnim(int length, float speed, int startFrame, int u, int v, int framesPerRow);
// Returns true if the animID is used; Check this before calling SetupAnim if in a loop
bool IsAnimSetup(uint16_t id);
// Standard linear animation
void UpdateAnim(Anim* anim, double* deltaTime);
// Update all anims in array
void UpdateAnims(double* deltaTime);
// Frees the memory allocated for frames array
void UnloadAnim(Anim* anim);
// Unload all anims in array
void UnloadAnims(void);

// Set the duration of a single frame
void SetFrameDuration(uint16_t animID, uint8_t frame, float duration);
// Set the duration for every frame of animation
void SetFrameDurations(uint16_t animID, float* durations);

void SetAnimSpeed(uint16_t animID, float speed);

void SetAnimFrame(uint16_t animID, uint8_t index);

uint8_t GetAnimFrame(uint16_t animID);

uint8_t GetAnimLength(uint16_t animID);

#ifdef BATANIM_IMPL

//  struct Example { 
//      int x;      // example
//      int y;      // example
//      int animID; // Store index into anims[]
//  }

#define MAX_ANIMS (1024) // Change this if you run out
Anim anims[MAX_ANIMS];
uint16_t highestAnimID = 0;

uint16_t SetupAnim(int length, float speed, int startFrame, int u, int v, int framesPerRow) {
    uint16_t id = 0;
    for (uint16_t i = (uint16_t)(highestAnimID + 1); i < MAX_ANIMS; i++)
    {
        assert(i < MAX_ANIMS); // Reached maximum allocated anims
        if (anims[i].length < 0) { continue; }
        assert(length > 0);    // Don't create anim if length <= 0
        if (anims[i].isUsed) { continue; }
        Anim* anim = &anims[i];
        anim->length = length;
        anim->speed = speed;
        anim->frame = startFrame % anim->length;
        anim->timer = 0;
        anim->frames = (Frame*)MemAlloc(anim->length * sizeof(Frame));

        for (uint8_t f = 0; f < anim->length; f++) {
            anim->frames[f].index = u + f + v * framesPerRow;
            SetFrameDuration(i, f, DEFAULT_FRAME_DURATION);
        }
        
        anim->isUsed = true;
        id = i;
        if (id > highestAnimID) { highestAnimID = id; }
        break;
    }
    return id;
}

bool IsAnimSetup(uint16_t id) {
    return anims[id].isUsed;
}

void UpdateAnim(Anim* anim, double* deltaTime) {
    if (anim->timer >= anim->frames[anim->frame].duration) {
        anim->frame++;
        anim->frame %= anim->length;
        anim->timer = 0;
        anim->isOver = (anim->frame == anim->length - 1);
    }
    anim->timer += *deltaTime * anim->speed;
}

void UpdateAnims(double* deltaTime) {
    for (uint16_t i = 0; i < MAX_ANIMS; i++) {
        Anim* anim = &anims[i];
        if (anim->isUsed == false) continue;
        UpdateAnim(anim, deltaTime);
    }
}

void UnloadAnim(Anim* anim) {
    if (!anim->isUsed) { return; }
    anim->isUsed = false;
    MemFree(anim->frames);
}

void UnloadAnims(void) {
    for (uint16_t i = 0; i < highestAnimID; i++) {
        Anim* anim = &anims[i];
        if (anim->isUsed == false) continue;
        UnloadAnim(anim);
    }
}

void SetFrameDuration(uint16_t animID, uint8_t frame, float duration) {
    anims[animID].frames[frame].duration = duration;
}

void SetFrameDurations(uint16_t animID, float* durations) {
    for (uint8_t i = 0; i < anims[animID].length; i++) {
        SetFrameDuration(animID, i, durations[i]);
    }
}

void SetAnimSpeed(uint16_t animID, float speed) {
    assert(anims[animID].isUsed);
    anims[animID].speed = speed;
}

void SetAnimFrame(uint16_t animID, uint8_t index) {
    assert(anims[animID].isUsed);
    anims[animID].timer = 0;
    anims[animID].frame = index % anims[animID].length;
}

uint8_t GetAnimFrame(uint16_t animID) {
    assert(anims[animID].isUsed);
    return anims[animID].frame;
}

uint8_t GetAnimLength(uint16_t animID) {
    assert(anims[animID].isUsed);
    return anims[animID].length;
}

#endif

#endif
