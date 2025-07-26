#pragma once
#include <stdint.h>
#include <SEED/Lib/Tensor/Vector2.h>

static int32_t kWindowSizeX = 1280;
static int32_t kWindowSizeY = 720;
static Vector2 kWindowSize = { float(kWindowSizeX),float(kWindowSizeY) };
static Vector2 kWindowCenter = kWindowSize * 0.5f;