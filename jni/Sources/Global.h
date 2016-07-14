#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifdef __ANDROID__
////// DEBUG | RELEASE

// Debug
//#ifndef DEBUG
//#define DEBUG
//#endif
//#undef NDEBUG

// Relase
#ifndef NDEBUG
#define NDEBUG
#endif
#undef DEBUG

#endif // __ANDROID__

//#ifdef DEBUG
#define DEMO_VERSION
//#endif

#define DISPLAY_DELAY               (50 >> 2)
#define MAX_COLOR                   255.f
#define EXIT_SIZE                   256 // Font texture

#define FONT_TEXTURE_WIDTH          1024.f
#define FONT_TEXTURE_HEIGHT         512.f

// Colors
#define GREEN_COLOR_R               0xa2
#define GREEN_COLOR_G               0xcd
#define GREEN_COLOR_B               0x5a

#define BLUE_COLOR_R                0x49
#define BLUE_COLOR_G                0xc2
#define BLUE_COLOR_B                0xc3

#define ORANGE_COLOR_R              0xfd
#define ORANGE_COLOR_G              0xb9
#define ORANGE_COLOR_B              0x4e

#define RED_COLOR_R                 0xf0
#define RED_COLOR_G                 0x5e
#define RED_COLOR_B                 0x61

#define YELLOW_COLOR_R              0xfd
#define YELLOW_COLOR_G              0xe2
#define YELLOW_COLOR_B              0x91

#define PURPLE_COLOR_R              0xd8
#define PURPLE_COLOR_G              0xb7
#define PURPLE_COLOR_B              0xea

enum {

    GAME_NONE = 0, // No started game

    GAME_3X3,
    GAME_4X4,
    GAME_5X5
};

// Data key(s)
#define DATA_KEY_3X3_SCORE          "Best3X3"
#define DATA_KEY_4X4_SCORE          "Best4X4"
#define DATA_KEY_5X5_SCORE          "Best5X5"

#define DATA_KEY_LEVEL              "Level"
#define DATA_KEY_SCORE_MIN          "Minutes"
#define DATA_KEY_SCORE_SEC          "Seconds"

#define DATA_KEY_ROW1_COL1          "R1C1"
#define DATA_KEY_ROW1_COL2          "R1C2"
#define DATA_KEY_ROW1_COL3          "R1C3"
#define DATA_KEY_ROW1_COL4          "R1C4"
#define DATA_KEY_ROW1_COL5          "R1C5"
#define DATA_KEY_ROW2_COL1          "R2C1"
#define DATA_KEY_ROW2_COL2          "R2C2"
#define DATA_KEY_ROW2_COL3          "R2C3"
#define DATA_KEY_ROW2_COL4          "R2C4"
#define DATA_KEY_ROW2_COL5          "R2C5"
#define DATA_KEY_ROW3_COL1          "R3C1"
#define DATA_KEY_ROW3_COL2          "R3C2"
#define DATA_KEY_ROW3_COL3          "R3C3"
#define DATA_KEY_ROW3_COL4          "R3C4"
#define DATA_KEY_ROW3_COL5          "R3C5"
#define DATA_KEY_ROW4_COL1          "R4C1"
#define DATA_KEY_ROW4_COL2          "R4C2"
#define DATA_KEY_ROW4_COL3          "R4C3"
#define DATA_KEY_ROW4_COL4          "R4C4"
#define DATA_KEY_ROW4_COL5          "R4C5"
#define DATA_KEY_ROW5_COL1          "R5C1"
#define DATA_KEY_ROW5_COL2          "R5C2"
#define DATA_KEY_ROW5_COL3          "R5C3"
#define DATA_KEY_ROW5_COL4          "R5C4"
#define DATA_KEY_ROW5_COL5          "R5C5"

// Log levels (< 5 to log)
#define LOG_LEVEL_PUZZLECAM         4
#define LOG_LEVEL_TITLELEVEL        4
#define LOG_LEVEL_GAMELEVEL         4
#define LOG_LEVEL_CHECK             4

typedef struct {

    short left;
    short right;
    short top;
    short bottom;

} TouchArea;

#endif // GLOBAL_H_
