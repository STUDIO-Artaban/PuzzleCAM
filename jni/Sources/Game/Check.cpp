#include "Check.h"
#include "PuzzleCAM.h"

#define PUZZLE_X0           ((CAM_WIDTH - CAM_HEIGHT) >> 1)
#define CAM_SCALE           0.98f

// Texture IDs
#define TEXTURE_ID_CHECK    5

static const float g_InitialCheckPos[3][8] = {
        { RATIO_3X3 - 1.f, 1.f - RATIO_3X3, RATIO_3X3 - 1.f, RATIO_3X3 - 1.f, 1.f - RATIO_3X3, RATIO_3X3 - 1.f, 1.f - RATIO_3X3, 1.f - RATIO_3X3 },
        { -0.25f, 0.25f, -0.25f, -0.25f, 0.25f, -0.25f, 0.25f, 0.25f },
        { -0.2f, 0.2f, -0.2f, -0.2f, 0.2f, -0.2f, 0.2f, 0.2f } };

static const float g_CameraCoords3X3[(3 * 3) - 1][8] = {
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT, // x, y
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, 0.f,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, 0.f,
          (PUZZLE_X0 + (CAM_HEIGHT / 3)) / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 3)) / CAM_TEXTURE_HEIGHT } };
static const float g_CameraCoords4X4[(4 * 4) - 1][8] = {
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT, // x, y
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, 0.f,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, 0.f,
          (PUZZLE_X0 + (CAM_HEIGHT / 4)) / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, 0.f,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 4)) / CAM_TEXTURE_HEIGHT } };
static const float g_CameraCoords5X5[(5 * 5) - 1][8] = {
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT, // x, y
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT },
        { PUZZLE_X0 / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, 0.f,
          PUZZLE_X0 / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT, // x, y
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, 0.f,
          (PUZZLE_X0 + (CAM_HEIGHT / 5)) / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT, // x, y
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, 0.f,
          (PUZZLE_X0 + (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT, // x, y
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, 0.f,
          (PUZZLE_X0 + (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_WIDTH, 0.f },

        { (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT, // x, y
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 2 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT },
        { (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 3 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + CAM_HEIGHT) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT,
          (PUZZLE_X0 + (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_WIDTH, (CAM_HEIGHT - (CAM_HEIGHT * 4 / 5)) / CAM_TEXTURE_HEIGHT } };

//////
Check::Check() : mCAM(false), mSlideStep(0) { LOGV(LOG_LEVEL_CHECK, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__); }
Check::~Check() { LOGV(LOG_LEVEL_CHECK, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__); }

void Check::start(const Game* game, unsigned char position, unsigned char level, unsigned char check) {

    LOGV(LOG_LEVEL_CHECK, 0, LOG_FORMAT(" - g:%x; p:%d; l:%d; c:%d"), __PRETTY_FUNCTION__, __LINE__, game, position, level, check);
    assert((level > GAME_NONE) && (level <= GAME_5X5));
    assert(((position < 9) && (level == GAME_3X3)) || ((position < 16) && (level == GAME_4X4)) ||
            ((position < 25) && (level == GAME_5X5)));
    assert((check) && (((check < 9) && (level == GAME_3X3)) || ((check < 16) && (level == GAME_4X4)) ||
            ((check < 25) && (level == GAME_5X5))));
    assert(Camera::getInstance()->getCamTexIdx() != TEXTURE_IDX_INVALID);

    mCheck.initialize(game2DVia(game));
    mCAM.initialize(game2DVia(game));

    mCheck.start(TEXTURE_ID_CHECK);
    mCAM.start(Camera::getInstance()->getCamTexIdx());
#ifndef __ANDROID__
    mCAM.setBGRA(true);
#endif

    mCheck.setVertices(g_InitialCheckPos[level - 1]);
    mCAM.copyVertices(mCheck);

    mCheck.setTexCoords(FULL_TEXCOORD_BUFFER);
    switch (level) {
        case GAME_3X3: {

            mCheck.setRed(GREEN_COLOR_R / MAX_COLOR);
            mCheck.setGreen(GREEN_COLOR_G / MAX_COLOR);
            mCheck.setBlue(GREEN_COLOR_B / MAX_COLOR);

            mCAM.setTexCoords(g_CameraCoords3X3[check - 1]);

            mCheck.translate(g_CheckPosition3X3[position][0], g_CheckPosition3X3[position][1]);
            mCAM.translate(g_CheckPosition3X3[position][0], g_CheckPosition3X3[position][1]);
            break;
        }
        case GAME_4X4: {

            mCheck.setRed(BLUE_COLOR_R / MAX_COLOR);
            mCheck.setGreen(BLUE_COLOR_G / MAX_COLOR);
            mCheck.setBlue(BLUE_COLOR_B / MAX_COLOR);

            mCAM.setTexCoords(g_CameraCoords4X4[check - 1]);

            mCheck.translate(g_CheckPosition4X4[position][0], g_CheckPosition4X4[position][1]);
            mCAM.translate(g_CheckPosition4X4[position][0], g_CheckPosition4X4[position][1]);
            break;
        }
        case GAME_5X5: {

            mCheck.setRed(ORANGE_COLOR_R / MAX_COLOR);
            mCheck.setGreen(ORANGE_COLOR_G / MAX_COLOR);
            mCheck.setBlue(ORANGE_COLOR_B / MAX_COLOR);

            mCAM.setTexCoords(g_CameraCoords5X5[check - 1]);

            mCheck.translate(g_CheckPosition5X5[position][0], g_CheckPosition5X5[position][1]);
            mCAM.translate(g_CheckPosition5X5[position][0], g_CheckPosition5X5[position][1]);
            break;
        }
    }
    mCAM.scale(CAM_SCALE, CAM_SCALE);
}

void Check::render() const {

    mCAM.render(true);
    mCheck.render(true);
}
