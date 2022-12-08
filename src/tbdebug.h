#pragma once
#include <stdint.h>
#include <raylib.h>

// modify the default properties of the library here
#define TBDB_MAX_COMMANDS (1024)
#define TBDB_DEFAULT_PEN_COLOR (LIME)
#define TBDB_DEFAULT_PEN_THICK (1.0)
#define TBDB_DEFAULT_FONT_SIZE (10)
#define TBDB_DEFAULT_FONT_SPACING (1.0)


// to use this library, define TBDB_DEBUG_IMPL before including (ONCE)

#define TBDB_NULL  0
#define TBDB_LINE  1
#define TBDB_RECT  2
#define TBDB_CIRC  3
#define TBDB_COLR  4
#define TBDB_TEXT  5
#define TBDB_NUMB  6
#define TBDB_REAL  7
#define TBDB_THCK  8
#define TBDB_FONT  9
#define TBDB_FTSZ  10
#define TBDB_FTSP  11

struct TBDB_DEBUG_command {
    int type;
    union
    {
        Color color;
        float thickness;
        Font font;
        int fontSize;
        float fontSpacing;
        Rectangle rec;

        struct {
            float x, y, rad;
        } circ;

        struct {
            float x1, y1, x2, y2;
        } line;

        struct {
            union {
                double  numbFlt;
                int64_t numbInt;
            } value;
            char text[11];
            float x; float y; 
        } ident;
    } data;
    
};
#define TBDB_TBDBC struct TBDB_DEBUG_command


void SetDebugColor(Color color);
void SetDebugThickness(float thickness);
void SetDebugFont(Font font);
void SetDebugFontSize(int fontSize);
void DebugDrawLine(float x1, float y1, float x2, float y2);
void DebugDrawRectangle(float x, float y, float width, float height);
void DebugDrawRectangleRec(Rectangle rec);
void DebugDrawCircle(float x, float y, float r);
void DebugDrawFloat(float x, float y, const char* label, double f);
void DebugDrawInt(float x, float y, const char* label, int64_t i);


#ifdef TBDB_DEBUG_IMPL

TBDB_TBDBC commands[TBDB_MAX_COMMANDS];
static Color penColor = TBDB_DEFAULT_PEN_COLOR;
static float penThickness = TBDB_DEFAULT_PEN_THICK;
static Font penFont;
static float penFontSize = TBDB_DEFAULT_FONT_SIZE;
static float penFontSpacing = TBDB_DEFAULT_FONT_SPACING;

static int debug_last_id = 0;

#define _INC \
if (debug_last_id >= TBDB_MAX_COMMANDS-1) {TraceLog(LOG_FATAL, "DEBUG COMMAND-BUFFER OVERRUN");}\
commands[debug_last_id+1] = (TBDB_TBDBC){.type = TBDB_NULL}; \
debug_last_id++;

static void BeginDebug() {
    penColor = TBDB_DEFAULT_PEN_COLOR;
    penThickness = TBDB_DEFAULT_PEN_THICK;
    penFont = GetFontDefault();
    penFontSize = TBDB_DEFAULT_FONT_SIZE;
    penFontSpacing = TBDB_DEFAULT_FONT_SPACING;
    debug_last_id = 0;
}

void SetDebugColor(Color color) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_COLR, .data.color = color};
    _INC
}

void SetDebugThickness(float thickness) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_THCK, .data.thickness = thickness};
    _INC
}

void SetDebugFont(Font font) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_FONT, .data.font = font};
    _INC
}

void SetDebugFontSize(int fontsize) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_FTSZ, .data.fontSize = fontsize};
    _INC
}

void DebugDrawLine(float x1, float y1, float x2, float y2) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_LINE, .data.line.x1 = x1, .data.line.y1 = y1, .data.line.x2 = x2, .data.line.y2 = y2};
    _INC
}

void DebugDrawRectangle(float x, float y, float width, float height) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_RECT, .data.rec = (Rectangle){.x = x, .y = y, .width = width, .height = height}};
    _INC
}

void DebugDrawRectangleRec(Rectangle rec) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_RECT, .data.rec = rec};
    _INC
}

void DebugDrawCircle(float x, float y, float r) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_CIRC, .data.circ.x = x, .data.circ.y = y, .data.circ.rad = r};
    _INC
}

void DebugDrawFloat(float x, float y, const char* label, double f) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_REAL, .data.ident = {.x = x, .y = y, .value.numbFlt = f} };
    bool copy = true;
    for (int i = 0; i < 10; i++) {
        if (label[i] == 0) {copy = false;}
        if (copy == false) {commands[debug_last_id].data.ident.text[i] = 0;}
        else {commands[debug_last_id].data.ident.text[i] = label[i];}
    }
    commands[debug_last_id].data.ident.text[10] = 0x00;
    _INC
}

void DebugDrawInt(float x, float y, const char* label, int64_t i) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_NUMB, .data.ident = {.x = x, .y = y, .value.numbInt = i} };
    bool copy = true;
    for (int i = 0; i < 10; i++) {
        if (label[i] == 0) {copy = false;}
        if (copy == false) {commands[debug_last_id].data.ident.text[i] = 0;}
        else {commands[debug_last_id].data.ident.text[i] = label[i];}
    }
    commands[debug_last_id].data.ident.text[10] = 0x00;
    _INC
}

void DebugDrawText(float x, float y, const char* text) {
    commands[debug_last_id] = (TBDB_TBDBC){.type = TBDB_TEXT, .data.ident = {.x = x, .y = y} };
    bool copy = true;
    for (int i = 0; i < 10; i++) {
        if (text[i] == 0) {copy = false;}
        if (copy == false) {commands[debug_last_id].data.ident.text[i] = 0;}
        else {commands[debug_last_id].data.ident.text[i] = text[i];}
    }
    commands[debug_last_id].data.ident.text[10] = 0x00;
    _INC
}



#undef _INC

void DrawDebug() {
    for (size_t i = 0; i < TBDB_MAX_COMMANDS; i++)
    {
        if (commands[i].type == TBDB_NULL) {break;}

        switch (commands[i].type)
        {
        case TBDB_LINE:
            DrawLineEx( 
                (Vector2){commands[i].data.line.x1, commands[i].data.line.y1}, 
                (Vector2){commands[i].data.line.x2, commands[i].data.line.y2,}, penThickness, penColor);
            break;

        case TBDB_RECT:
            DrawRectangleLinesEx(commands[i].data.rec, penThickness, penColor);
            break;

        case TBDB_CIRC:
            DrawCircleLines(commands[i].data.circ.x, commands[i].data.circ.y, commands[i].data.circ.rad, penColor);
            break;

        case TBDB_NUMB:
            DrawTextEx(penFont, TextFormat("%s: %lld", commands[i].data.ident.text, commands[i].data.ident.value.numbInt), (Vector2) {commands[i].data.ident.x, commands[i].data.ident.y}, penFontSize, penFontSpacing, penColor);
            break;

        case TBDB_REAL:
            DrawTextEx(penFont, TextFormat("%s: %3.2lf", commands[i].data.ident.text, commands[i].data.ident.value.numbFlt), (Vector2) {commands[i].data.ident.x, commands[i].data.ident.y}, penFontSize, penFontSpacing, penColor);
            break;

        case TBDB_TEXT:
            DrawTextEx(penFont, commands[i].data.ident.text, (Vector2) {commands[i].data.ident.x, commands[i].data.ident.y}, penFontSize, penFontSpacing, penColor);
            break;

        case TBDB_COLR:
            penColor = commands[i].data.color;
            break;
        
        case TBDB_THCK:
            penThickness = commands[i].data.thickness;
            break;

        case TBDB_FONT:
            penFont = commands[i].data.font;
            break;
        
        case TBDB_FTSZ:
            penFontSize = commands[i].data.fontSize;
            break;

        case TBDB_FTSP:
            penFontSpacing = commands[i].data.fontSpacing;
            break;

        default:
            TraceLog(LOG_FATAL, "CORRUPTED DEBUG DATA");
            break;
        }
    }
}


#endif


#undef TBDB_TBDBC
#undef TBDB_MAX_COMMANDS

#undef TBDB_NULL
#undef TBDB_LINE
#undef TBDB_RECT
#undef TBDB_CIRC
#undef TBDB_COLR
#undef TBDB_TEXT
#undef TBDB_NUMB
#undef TBDB_REAL
#undef TBDB_THCK
#undef TBDB_FONT
#undef TBDB_FTSZ
#undef TBDB_FTSP

