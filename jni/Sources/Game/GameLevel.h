#ifndef GAMELEVEL_H_
#define GAMELEVEL_H_

#include "Global.h"

#include <libeng/Game/Level.h>
#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Graphic/Text/2D/Text2D.h>
#include <libeng/Features/Camera/Camera.h>
#include <libeng/Advertising/Advertising.h>

#ifdef __ANDROID__
#include "Game/Check.h"
#else
#include "Check.h"
#endif

#include <vector>
#include <time.h>

#define MIX_COUNT       37 // Prime number...

using namespace eng;

//////
class GameLevel : public Level {

private:
    Player* mPlayer;
    Camera* mCamera;

#ifndef DEMO_VERSION
    Advertising* mAdvertising;
    bool mAdLoaded;
    void adDisplay(bool delay);
#endif

    //
    Back2D* mHideCam;
    Back2D mBackground;

    unsigned char* mPuzzleIdx;
    std::vector<Check*> mPuzzle2D;
    inline void mix(unsigned char dim, unsigned char len) {

        LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - d:%d; l:%d"), __PRETTY_FUNCTION__, __LINE__, dim, len);
        unsigned char nocheck = 0;
        for ( ; nocheck < len; ++nocheck)
            mPuzzleIdx[nocheck] = (nocheck + 1);
        mPuzzleIdx[--nocheck] = 0;

        unsigned char max = dim * MIX_COUNT;
        for (unsigned char move = 0; move < max; ++move) {
            switch (std::rand() % 4) {

                case 0: { // Up
                    if ((nocheck - dim) > -1) {
                        std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck - dim]);
                        nocheck -= dim;
                    }
                    else { // Down
                        std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck + dim]);
                        nocheck += dim;
                    }
                    break;
                }
                case 1: { // Right
                    if ((nocheck + 1) % dim) {
                        std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck + 1]);
                        ++nocheck;
                    }
                    else { // Left
                        std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck - 1]);
                        --nocheck;
                    }
                    break;
                }
                case 2: { // Down
                    if ((nocheck + dim) < len) {
                        std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck + dim]);
                        nocheck += dim;
                    }
                    else { // Up
                        std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck - dim]);
                        nocheck -= dim;
                    }
                    break;
                }
                case 3: { // Left
                    if ((nocheck) && (nocheck % dim)) {
                        std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck - 1]);
                        --nocheck;
                    }
                    else { // Right
                        std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck + 1]);
                        ++nocheck;
                    }
                    break;
                }
            }
        }
    };
    inline void replace(unsigned char dim, unsigned char len) { // Replace empty tile on bottom right position

        LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - d:%d; l:%d"), __PRETTY_FUNCTION__, __LINE__, dim, len);
        unsigned char nocheck = 0;
        while (mPuzzleIdx[nocheck++]);
        --nocheck;

        while ((nocheck + 1) % dim) {
            std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck + 1]);
            ++nocheck;
        }
        --len;
        while (nocheck != len) {
            std::swap<unsigned char>(mPuzzleIdx[nocheck], mPuzzleIdx[nocheck + dim]);
            nocheck += dim;
        }
        mNoCheckIdx = len;
    };

    enum {

        GAME_NEW = 0,

        GAME_PLAYING,
        GAME_WIN,
        GAME_OVER
    };
    unsigned char mStatus;
    inline bool win(unsigned char level) {

        LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - l:%d"), __PRETTY_FUNCTION__, __LINE__, level);
        unsigned char idx = 0;
        while (!(mPuzzleIdx[idx] - (idx + 1))) ++idx;
        switch (level) {

            case GAME_3X3: return (idx == 8);
            case GAME_4X4: return (idx == 15);
            case GAME_5X5: return (idx == 24);
        }
        return false;
    };

    Text2D mScore;

    time_t mTimer;
    unsigned char mMinutes;
    unsigned char mSeconds;
    void updateScore();

    TouchArea mSlideArea[4];
    unsigned char mSlideCheck[4];
    inline void fillSlideInfo(unsigned char level, short width, short height) {

        LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - l:%d; w:%d; h:%d"), __PRETTY_FUNCTION__, __LINE__, level, width, height);
        switch (level) {
            case GAME_3X3: {

                short size = width / 3;
                switch (mNoCheckIdx) {
                    case 0: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = size;
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 1;

                        mSlideArea[2].left = 0;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 3;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 1: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = width - size;
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 2;

                        mSlideArea[2].left = mSlideArea[1].left - size;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 4;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 0;
                        break;
                    }
                    case 2: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;
                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = width - size;
                        mSlideArea[2].top = (height >> 1) - (size >> 1);
                        mSlideArea[2].right = width;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 5;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[2].top - size;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 1;
                        break;
                    }
                    case 3: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1) - (size >> 1);

                        mSlideCheck[0] = 0;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 4;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 6;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 4: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width - size;
                        mSlideArea[0].bottom = (height >> 1) - (size >> 1);

                        mSlideCheck[0] = 1;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 5;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 7;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 3;
                        break;
                    }
                    case 5: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1) - (size >> 1);

                        mSlideCheck[0] = 2;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[0].bottom + size;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 8;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 4;
                        break;
                    }
                    case 6: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1) + (size >> 1);

                        mSlideCheck[0] = 3;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 7;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;
                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 7: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = mSlideArea[0].left + size;
                        mSlideArea[0].bottom = (height >> 1) + (size >> 1);

                        mSlideCheck[0] = 4;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 8;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 6;
                        break;
                    }
                    case 8: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1) + (size >> 1);

                        mSlideCheck[0] = 5;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;
                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[3] = 7;
                        break;
                    }
                }
                break;
            }
            case GAME_4X4: {

                short size = width >> 2;
                switch (mNoCheckIdx) {
                    case 0: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = size;
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 1;

                        mSlideArea[2].left = 0;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 4;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 1: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = width >> 1;
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 2;

                        mSlideArea[2].left = size;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 5;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 0;
                        break;
                    }
                    case 2: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = width - size;
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 3;

                        mSlideArea[2].left = width >> 1;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 6;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 1;
                        break;
                    }
                    case 3: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;
                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = width - size;
                        mSlideArea[2].top = (height >> 1) - size;
                        mSlideArea[2].right = width;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 7;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[2].top - size;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 2;
                        break;
                    }
                    case 4: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1) - size;

                        mSlideCheck[0] = 0;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 5;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 8;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 5: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width >> 1;
                        mSlideArea[0].bottom = (height >> 1) - size;

                        mSlideCheck[0] = 1;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 6;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 9;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 4;
                        break;
                    }
                    case 6: {

                        mSlideArea[0].left = width >> 1;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = mSlideArea[0].left + size;
                        mSlideArea[0].bottom = (height >> 1) - size;

                        mSlideCheck[0] = 2;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 7;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 10;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 5;
                        break;
                    }
                    case 7: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1) - size;

                        mSlideCheck[0] = 3;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = (height >> 1);
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 11;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 6;
                        break;
                    }
                    case 8: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1);

                        mSlideCheck[0] = 4;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 9;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 12;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 9: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width >> 1;
                        mSlideArea[0].bottom = (height >> 1);

                        mSlideCheck[0] = 5;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 10;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 13;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 8;
                        break;
                    }
                    case 10: {

                        mSlideArea[0].left = width >> 1;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = mSlideArea[0].left + size;
                        mSlideArea[0].bottom = (height >> 1);

                        mSlideCheck[0] = 6;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 11;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 14;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 9;
                        break;
                    }
                    case 11: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1);

                        mSlideCheck[0] = 7;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[0].bottom + size;
                        mSlideArea[2].right = width;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 15;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 10;
                        break;
                    }
                    case 12: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1) + size;

                        mSlideCheck[0] = 8;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 13;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;
                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 13: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width >> 1;
                        mSlideArea[0].bottom = (height >> 1) + size;

                        mSlideCheck[0] = 9;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 14;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 12;
                        break;
                    }
                    case 14: {

                        mSlideArea[0].left = width >> 1;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = mSlideArea[0].left + size;
                        mSlideArea[0].bottom = (height >> 1) + size;

                        mSlideCheck[0] = 10;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 15;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 13;
                        break;
                    }
                    case 15: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1) + size;

                        mSlideCheck[0] = 11;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;
                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[3].top + size;

                        mSlideCheck[3] = 14;
                        break;
                    }
                }
                break;
            }
            case GAME_5X5: {

                short size = width / 5;
                switch (mNoCheckIdx) {
                    case 0: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = size;
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 1;

                        mSlideArea[2].left = mSlideArea[1].left - size;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 5;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 1: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = (size << 1);
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 2;

                        mSlideArea[2].left = mSlideArea[1].left - size;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 6;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 0;
                        break;
                    }
                    case 2: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = width - (size << 1);
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 3;

                        mSlideArea[2].left = mSlideArea[1].left - size;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 7;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 1;
                        break;
                    }
                    case 3: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;

                        mSlideArea[1].left = width - size;
                        mSlideArea[1].top = (height >> 1) - (width >> 1);
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 4;

                        mSlideArea[2].left = mSlideArea[1].left - size;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[1].left;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 8;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 2;
                        break;
                    }
                    case 4: {

                        mSlideArea[0].left = mSlideArea[0].top = mSlideArea[0].right = mSlideArea[0].bottom = 0;
                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = width - size;
                        mSlideArea[2].top = (height >> 1) - (width >> 1) + size;
                        mSlideArea[2].right = width;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 9;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[2].top - size;
                        mSlideArea[3].right = mSlideArea[2].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 3;
                        break;
                    }
                    case 5: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1) - (width >> 1) + size;

                        mSlideCheck[0] = 0;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 6;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 10;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 6: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size << 1;
                        mSlideArea[0].bottom = (height >> 1) - (width >> 1) + size;

                        mSlideCheck[0] = 1;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 7;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 11;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 5;
                        break;
                    }
                    case 7: {

                        mSlideArea[0].left = size << 1;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = mSlideArea[0].left +  size;
                        mSlideArea[0].bottom = (height >> 1) - (width >> 1) + size;

                        mSlideCheck[0] = 2;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 8;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 12;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 6;
                        break;
                    }
                    case 8: {

                        mSlideArea[0].left = width - (size << 1);
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width - size;
                        mSlideArea[0].bottom = (height >> 1) - (width >> 1) + size;

                        mSlideCheck[0] = 3;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 9;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 13;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 7;
                        break;
                    }
                    case 9: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1) - (width >> 1) + size;

                        mSlideCheck[0] = 4;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[0].bottom + size;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 14;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 8;
                        break;
                    }
                    case 10: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1) - (size >> 1);

                        mSlideCheck[0] = 5;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 11;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 15;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 11: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size << 1;
                        mSlideArea[0].bottom = (height >> 1) - (size >> 1);

                        mSlideCheck[0] = 6;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 12;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 16;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 10;
                        break;
                    }
                    case 12: {

                        mSlideArea[0].left = size << 1;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width - (size << 1);
                        mSlideArea[0].bottom = (height >> 1) - (size >> 1);

                        mSlideCheck[0] = 7;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 13;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 17;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 11;
                        break;
                    }
                    case 13: {

                        mSlideArea[0].left = width - (size << 1);
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width - size;
                        mSlideArea[0].bottom = (height >> 1) - (size >> 1);

                        mSlideCheck[0] = 8;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 14;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 18;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 12;
                        break;
                    }
                    case 14: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1) - (size >> 1);

                        mSlideCheck[0] = 9;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[0].bottom + size;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 19;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 13;
                        break;
                    }
                    case 15: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1) + (size >> 1);

                        mSlideCheck[0] = 10;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 16;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 20;

                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 16: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size << 1;
                        mSlideArea[0].bottom = (height >> 1) + (size >> 1);

                        mSlideCheck[0] = 11;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 17;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 21;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 15;
                        break;
                    }
                    case 17: {

                        mSlideArea[0].left = size << 1;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width - (size << 1);
                        mSlideArea[0].bottom = (height >> 1) + (size >> 1);

                        mSlideCheck[0] = 12;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 18;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 22;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 16;
                        break;
                    }
                    case 18: {

                        mSlideArea[0].left = width - (size << 1);
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width - size;
                        mSlideArea[0].bottom = (height >> 1) + (size >> 1);

                        mSlideCheck[0] = 13;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[1].top + size;

                        mSlideCheck[1] = 19;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[1].bottom;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 23;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 17;
                        break;
                    }
                    case 19: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1) + (size >> 1);

                        mSlideCheck[0] = 14;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;

                        mSlideArea[2].left = mSlideArea[0].left;
                        mSlideArea[2].top = mSlideArea[0].bottom + size;
                        mSlideArea[2].right = mSlideArea[0].right;
                        mSlideArea[2].bottom = height;

                        mSlideCheck[2] = 24;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[2].top;

                        mSlideCheck[3] = 18;
                        break;
                    }
                    case 20: {

                        mSlideArea[0].left = 0;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size;
                        mSlideArea[0].bottom = (height >> 1) + (width >> 1) - size;

                        mSlideCheck[0] = 15;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 21;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;
                        mSlideArea[3].left = mSlideArea[3].top = mSlideArea[3].right = mSlideArea[3].bottom = 0;
                        break;
                    }
                    case 21: {

                        mSlideArea[0].left = size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = size << 1;
                        mSlideArea[0].bottom = (height >> 1) + (width >> 1) - size;

                        mSlideCheck[0] = 16;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 22;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 20;
                        break;
                    }
                    case 22: {

                        mSlideArea[0].left = size << 1;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width - (size << 1);
                        mSlideArea[0].bottom = (height >> 1) + (width >> 1) - size;

                        mSlideCheck[0] = 17;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 23;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 21;
                        break;
                    }
                    case 23: {

                        mSlideArea[0].left = width - (size << 1);
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width - size;
                        mSlideArea[0].bottom = (height >> 1) + (width >> 1) - size;

                        mSlideCheck[0] = 18;

                        mSlideArea[1].left = mSlideArea[0].right;
                        mSlideArea[1].top = mSlideArea[0].bottom;
                        mSlideArea[1].right = width;
                        mSlideArea[1].bottom = mSlideArea[0].bottom + size;

                        mSlideCheck[1] = 24;

                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[1].top;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[1].bottom;

                        mSlideCheck[3] = 22;
                        break;
                    }
                    case 24: {

                        mSlideArea[0].left = width - size;
                        mSlideArea[0].top = 0;
                        mSlideArea[0].right = width;
                        mSlideArea[0].bottom = (height >> 1) + (width >> 1) - size;

                        mSlideCheck[0] = 19;

                        mSlideArea[1].left = mSlideArea[1].top = mSlideArea[1].right = mSlideArea[1].bottom = 0;
                        mSlideArea[2].left = mSlideArea[2].top = mSlideArea[2].right = mSlideArea[2].bottom = 0;

                        mSlideArea[3].left = 0;
                        mSlideArea[3].top = mSlideArea[0].bottom;
                        mSlideArea[3].right = mSlideArea[0].left;
                        mSlideArea[3].bottom = mSlideArea[3].top + size;

                        mSlideCheck[3] = 23;
                        break;
                    }
                }
                break;
            }
        }
    };

    unsigned char mSlideAreaIdx;
    unsigned int mTouchID;
    bool mSlideFlag; // TRUE: Slide; FALSE: Unslide
    bool mWaitSlide;

    unsigned char mNoCheckIdx;

    Element2D* mGameOver;
    Element2D* mCongratulation;
    Static2D* mBestPanel;
    Static2D* mBestText;

    Static2D mExit;
    TouchArea mExitArea;

public:
    GameLevel(Game* game);
    virtual ~GameLevel();

    void save(const Game* game); // Save data

    //////
    void init() { }
    inline void pause() { }
    void pause(const Game* game);

    void wait(float millis) { }

protected:
    bool loaded(const Game* game);
public:
    bool update(const Game* game);

    void renderLoad() const;
    void renderUpdate() const;

};

#endif // GAMELEVEL_H_
