#pragma once
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include "batbox.h"

void InitSplash(int width, int height, Color fg, Color bg, bool ft, int nm, int* m, double* t);
void UpdateSplash(void);
void DrawSplash(void);
