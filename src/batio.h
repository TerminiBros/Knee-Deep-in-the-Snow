#ifndef BATIO_H
#define BATIO_H
#include <raylib.h>
#include <stdint.h>

enum InputFlags {
    INPUT_A      = 1 << 0,
    INPUT_B      = 1 << 1,
    INPUT_UP     = 1 << 2,
    INPUT_LEFT   = 1 << 3,
    INPUT_DOWN   = 1 << 4,
    INPUT_RIGHT  = 1 << 5,
    INPUT_START  = 1 << 6,
    INPUT_SELECT = 1 << 7,
};

#define INPUT_ANY (255)

static inline bool IsInput(int flag);
static inline bool IsInputP(int flag);
static inline bool IsInputR(int flag);
static inline bool IsInputLocked(void);
static void LockInput(void);
static void UpdateInput(void);

#ifdef BATIO_IMPL

bool lockInput = false;
/*---Example-Implementation---*/
/*
static int mode = 0;
if (mode == 0) {
    DebugDrawText(10, 10, "Mode is 0");
    if (IsInputLocked()) {
        DebugDrawText(30, 20, "Locked");
    } else if (IsInput(INPUT_ANY)) {
        LockInput();
        mode = 1;
    }
}
else if (mode == 1) {
    DebugDrawText(10, 10, "Mode is 1");
    if (IsInputLocked()) {
        DebugDrawText(30, 20, "Locked");
    } else if (IsInput(INPUT_ANY)) {
        LockInput();
        mode = 0;
    }
}
*/

float inputDeadzone = 0.45f;
uint8_t input, inputP;
inline bool IsInput(int flag) { return input & flag; }
inline bool IsInputP(int flag) { return input & flag && !(inputP & flag); }
inline bool IsInputR(int flag) { return inputP & flag && !(input & flag); }
inline bool IsInputLocked(void) { return lockInput; }
inline void LockInput(void) { lockInput = true; }

void UpdateInput(void) {
    inputP = input;
    input = 0;
    if (IsGamepadAvailable(0)) {
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER)  > inputDeadzone || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) 
        { input |= INPUT_A; }
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > inputDeadzone || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
        { input |= INPUT_B; }
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -inputDeadzone || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
        { input |= INPUT_UP; }
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -inputDeadzone || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
        { input |= INPUT_LEFT; }
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) >  inputDeadzone || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) 
        { input |= INPUT_DOWN; }
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) >  inputDeadzone || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) 
        { input |= INPUT_RIGHT; }
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) 
        { input |= INPUT_START; }
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) 
        { input |= INPUT_SELECT; }
    }
    if (IsKeyDown(KEY_X))     { input |= INPUT_A; }
    if (IsKeyDown(KEY_Z))     { input |= INPUT_B; }
    if (IsKeyDown(KEY_UP))    { input |= INPUT_UP; }
    if (IsKeyDown(KEY_LEFT))  { input |= INPUT_LEFT; }
    if (IsKeyDown(KEY_DOWN))  { input |= INPUT_DOWN; }
    if (IsKeyDown(KEY_RIGHT)) { input |= INPUT_RIGHT; }
    if (IsKeyDown(KEY_ENTER)) { input |= INPUT_START; }
    if (IsKeyDown(KEY_TAB))   { input |= INPUT_SELECT; }

    if (lockInput && !input) { lockInput = false; }
}

#endif

#endif // BATIO_H
