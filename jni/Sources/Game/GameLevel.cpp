#include "GameLevel.h"
#include "PuzzleCAM.h"

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Tools/Tools.h>

#define MAX_LOAD_STEP               5
#ifdef __ANDROID__
#define NO_TOUCH_ID                 0x12345678
#else
#define NO_TOUCH_ID                 0
#endif

#define CONGRATULATION_X0           512
#define CONGRATULATION_Y0           257
#define CONGRATULATION_X2           885
#define CONGRATULATION_Y2           385

#define BEST_X0                     885
#define BEST_Y0                     257
#define BEST_Y2                     396

#define GAME_OVER_X0                CONGRATULATION_X0
#define GAME_OVER_Y0                CONGRATULATION_Y2
#define GAME_OVER_X2                CONGRATULATION_X2
#define GAME_OVER_Y2                (static_cast<short>(FONT_TEXTURE_HEIGHT) - 1)

#define SCALE_VELOCITY              0.02f

// Sound IDs
#define SOUND_ID_SLIDE              2
#define SOUND_ID_CONGRATULATION     3
#define SOUND_ID_GAMEOVER           4

//////
GameLevel::GameLevel(Game* game) : Level(game), mPuzzleIdx(NULL), mSlideAreaIdx(0), mNoCheckIdx(0),
    mSlideFlag(false), mTouchID(NO_TOUCH_ID), mWaitSlide(false), mStatus(GAME_NEW), mTimer(0), mMinutes(0), mSeconds(0) {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
    std::memset(&mExitArea, 0, sizeof(TouchArea));

    std::memset(&mSlideArea[0], 0, sizeof(TouchArea));
    std::memset(&mSlideArea[1], 0, sizeof(TouchArea));
    std::memset(&mSlideArea[2], 0, sizeof(TouchArea));
    std::memset(&mSlideArea[3], 0, sizeof(TouchArea));

    std::memset(&mSlideCheck, 0, 4);

    Inputs::InputUse = USE_INPUT_TOUCH;

    mCamera = Camera::getInstance();
    mPlayer = Player::getInstance();
#ifndef DEMO_VERSION
    mAdLoaded = false;
    mAdvertising = Advertising::getInstance();
#endif

    mHideCam = NULL;
    mGameOver = NULL;
    mCongratulation = NULL;
    mBestPanel = NULL;
    mBestText = NULL;
}
GameLevel::~GameLevel() {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (mPuzzleIdx) delete mPuzzleIdx;
    if (mHideCam) delete mHideCam;
    if (mGameOver) delete mGameOver;
    if (mCongratulation) delete mCongratulation;
    if (mBestPanel) delete mBestPanel;
    if (mBestText) delete mBestText;

    for (std::vector<Check*>::iterator iter = mPuzzle2D.begin(); iter != mPuzzle2D.end(); ++iter)
        if (*iter) delete (*iter);
    mPuzzle2D.clear();
}

void GameLevel::pause(const Game* game) {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
    if (mLoadStep == MAX_LOAD_STEP)
        save(game);

    Level::pause();

    if (mHideCam) mHideCam->pause();
    if (mGameOver) mGameOver->pause();
    if (mCongratulation) mCongratulation->pause();
    if (mBestPanel) mBestPanel->pause();
    if (mBestText) mBestText->pause();

    mBackground.pause();
    mScore.pause();
    mExit.pause();

    for (std::vector<Check*>::iterator iter = mPuzzle2D.begin(); iter != mPuzzle2D.end(); ++iter)
        if (*iter) delete (*iter);
    mPuzzle2D.clear();

    if (mPuzzleIdx) {

        delete mPuzzleIdx;
        mPuzzleIdx = NULL;
    }
}

bool GameLevel::loaded(const Game* game) {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - l:%d"), __PRETTY_FUNCTION__, __LINE__, mLoadStep);
    switch (mLoadStep) {
        case 1: {

            if (!mHideCam) {

                mHideCam = new Back2D;
                mHideCam->initialize(game2DVia(game));
                mHideCam->start(0, 0, 0);
            }
            else
                mHideCam->resume(0, 0, 0);

            // Camera
            if (!mCamera->isStarted())
                mCamera->start(CAM_WIDTH, CAM_HEIGHT);

            break;
        }
        case 2: {

            if (static_cast<bool>(*static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_LEVEL))))
                mStatus = GAME_PLAYING;

#ifndef DEMO_VERSION
            // Advertising
            unsigned char adStatus = Advertising::getStatus();
            assert(adStatus != Advertising::STATUS_NONE);

            mAdLoaded = false;
            if ((adStatus == Advertising::STATUS_READY) || (adStatus == Advertising::STATUS_FAILED))
                mAdvertising->load();
            else
                mAdLoaded = (adStatus > Advertising::STATUS_LOADED); // STATUS_DISPLAYING | STATUS_DISPLAYED
#endif
            //
            if (!mBackground.isStarted()) {

                mBackground.initialize(game2DVia(game));
                switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    case GAME_3X3: mBackground.start(GREEN_COLOR_R, GREEN_COLOR_G, GREEN_COLOR_B); break;
                    case GAME_4X4: mBackground.start(BLUE_COLOR_R, BLUE_COLOR_G, BLUE_COLOR_B); break;
                    case GAME_5X5: mBackground.start(ORANGE_COLOR_R, ORANGE_COLOR_G, ORANGE_COLOR_B); break;
                }
            }
            else switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                case GAME_3X3: mBackground.resume(GREEN_COLOR_R, GREEN_COLOR_G, GREEN_COLOR_B); break;
                case GAME_4X4: mBackground.resume(BLUE_COLOR_R, BLUE_COLOR_G, BLUE_COLOR_B); break;
                case GAME_5X5: mBackground.resume(ORANGE_COLOR_R, ORANGE_COLOR_G, ORANGE_COLOR_B); break;
            }
            mExitArea.left = (game->getScreen()->width * 7) >> 3;
            mExitArea.top = (game->getScreen()->height >> 1) + (game->getScreen()->width >> 1) +
                    (((game->getScreen()->height >> 1) - (game->getScreen()->width >> 1)) >> 1) -
                    (game->getScreen()->width >> 4);
            mExitArea.right = game->getScreen()->width;
            mExitArea.bottom = mExitArea.top + (game->getScreen()->width >> 3);

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
        case 3: {

            if (!mScore.isStarted()) {

                mScore.initialize(game2DVia(game));
                std::wstring score;
                if (mStatus == GAME_PLAYING) {

                    mMinutes = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_SCORE_MIN));
                    if (mMinutes < 10)
                        score += L'0';
                    score.append(numToWStr<short>(static_cast<short>(mMinutes)));
                    score += L'\''; // ' == :
                    mSeconds = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_SCORE_SEC));
                    if (mSeconds < 10)
                        score += L'0';
                    score.append(numToWStr<short>(static_cast<short>(mSeconds)));
                }
                else
                    score = L"00'00";

                mScore.start(score);

                float scale = ((((game->getScreen()->height >> 1) - (game->getScreen()->width >> 1)) << 3) / 10.f) /
                                SCORES_FONT_HEIGHT; // 80% of the place below the checkerboard
                float yPos = ((1.f - game->getScreen()->top) / 10.f) - 1.f;
                float xPos = (((5 * SCORES_FONT_WIDTH) << 3) / 10.f) * (1.f - game->getScreen()->top) /
                                                                                (SCORES_FONT_HEIGHT << 1);
                if (xPos < -0.5f) {

                    scale = (game->getScreen()->width >> 1) / (5.f * SCORES_FONT_WIDTH);
                    yPos = (((game->getScreen()->top - 1.f) - ((SCORES_FONT_HEIGHT >> 1) / (2.5f *
                                                                                SCORES_FONT_WIDTH))) / -2.f) - 1.f;
                    xPos = -0.5f;
                }
                mScore.scale(scale, scale);
                mScore.position(xPos, yPos);
            }
            else
                mScore.resume();

            if (!mExit.isStarted()) {

                mExit.initialize(game2DVia(game));
                mExit.start(TEXTURE_ID_FONT);
                mExit.setVertices(mExitArea.left, game->getScreen()->height - mExitArea.top, game->getScreen()->width,
                        game->getScreen()->height - mExitArea.bottom);

                static const float texCoords[8] = { (FONT_TEXTURE_WIDTH - EXIT_SIZE) / FONT_TEXTURE_WIDTH, 0.f,
                        (FONT_TEXTURE_WIDTH - EXIT_SIZE) / FONT_TEXTURE_WIDTH, EXIT_SIZE / FONT_TEXTURE_HEIGHT,
                        1.f, EXIT_SIZE / FONT_TEXTURE_HEIGHT, 1.f, 0.f };
                mExit.setTexCoords(texCoords);
            }
            else
                mExit.resume(TEXTURE_ID_FONT);

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
        case 4: {

            Player::loadSound(SOUND_ID_TOUCH);
            Player::loadSound(SOUND_ID_SLIDE);
            Player::loadSound(SOUND_ID_CONGRATULATION);
            Player::loadSound(SOUND_ID_GAMEOVER);

            mPlayer->setVolume(1, 0.25f);
            mPlayer->setVolume(2, 0.3f);

            assert(!mPuzzleIdx);
            switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                case GAME_3X3: mPuzzleIdx = new unsigned char[3 * 3]; break;
                case GAME_4X4: mPuzzleIdx = new unsigned char[4 * 4]; break;
                case GAME_5X5: mPuzzleIdx = new unsigned char[5 * 5]; break;
            }

            // Check resume game...
            if (mStatus == GAME_PLAYING) {

                switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {
                    case GAME_3X3: {

                        mPuzzleIdx[0] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL1));
                        mPuzzleIdx[1] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL2));
                        mPuzzleIdx[2] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL3));

                        mPuzzleIdx[3] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL1));
                        mPuzzleIdx[4] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL2));
                        mPuzzleIdx[5] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL3));

                        mPuzzleIdx[6] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL1));
                        mPuzzleIdx[7] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL2));
                        mPuzzleIdx[8] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL3));
                        break;
                    }
                    case GAME_4X4: {

                        mPuzzleIdx[0] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL1));
                        mPuzzleIdx[1] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL2));
                        mPuzzleIdx[2] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL3));
                        mPuzzleIdx[3] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL4));

                        mPuzzleIdx[4] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL1));
                        mPuzzleIdx[5] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL2));
                        mPuzzleIdx[6] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL3));
                        mPuzzleIdx[7] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL4));

                        mPuzzleIdx[8] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL1));
                        mPuzzleIdx[9] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL2));
                        mPuzzleIdx[10] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL3));
                        mPuzzleIdx[11] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL4));

                        mPuzzleIdx[12] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL1));
                        mPuzzleIdx[13] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL2));
                        mPuzzleIdx[14] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL3));
                        mPuzzleIdx[15] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL4));
                        break;
                    }
                    case GAME_5X5: {

                        mPuzzleIdx[0] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL1));
                        mPuzzleIdx[1] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL2));
                        mPuzzleIdx[2] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL3));
                        mPuzzleIdx[3] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL4));
                        mPuzzleIdx[4] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL5));

                        mPuzzleIdx[5] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL1));
                        mPuzzleIdx[6] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL2));
                        mPuzzleIdx[7] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL3));
                        mPuzzleIdx[8] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL4));
                        mPuzzleIdx[9] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL5));

                        mPuzzleIdx[10] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL1));
                        mPuzzleIdx[11] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL2));
                        mPuzzleIdx[12] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL3));
                        mPuzzleIdx[13] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL4));
                        mPuzzleIdx[14] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL5));

                        mPuzzleIdx[15] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL1));
                        mPuzzleIdx[16] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL2));
                        mPuzzleIdx[17] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL3));
                        mPuzzleIdx[18] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL4));
                        mPuzzleIdx[19] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL5));

                        mPuzzleIdx[20] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL1));
                        mPuzzleIdx[21] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL2));
                        mPuzzleIdx[22] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL3));
                        mPuzzleIdx[23] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL4));
                        mPuzzleIdx[24] = *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL5));
                        break;
                    }
                }
            }
            else switch (static_cast<const PuzzleCAM*>(game)->mCurGame) { // New Game

                case GAME_3X3: mix(3, 3 * 3); break;
                case GAME_4X4: mix(4, 4 * 4); break;
                case GAME_5X5: mix(5, 5 * 5); break;
            }

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
        case MAX_LOAD_STEP: {

            // Check resume game...
            if (mStatus == GAME_PLAYING) {

                unsigned char len = 0;
                switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    case GAME_3X3: len = 3 * 3; break;
                    case GAME_4X4: len = 4 * 4; break;
                    case GAME_5X5: len = 5 * 5; break;
                }
                while (static_cast<unsigned char>(mPuzzle2D.size()) != len) {

                    if (mPuzzleIdx[static_cast<unsigned char>(mPuzzle2D.size())]) {

                        Check* check = new Check;
                        check->start(game, static_cast<unsigned char>(mPuzzle2D.size()),
                                static_cast<const PuzzleCAM*>(game)->mCurGame,
                                mPuzzleIdx[static_cast<unsigned char>(mPuzzle2D.size())]);

                        mPuzzle2D.push_back(check);
                    }
                    else
                        mPuzzle2D.push_back(NULL);
                }
                mNoCheckIdx = 0;
                while (mPuzzleIdx[mNoCheckIdx++]);
                --mNoCheckIdx;
            }
            else switch (static_cast<const PuzzleCAM*>(game)->mCurGame) { // New game

                // Replace empty tile on bottom right position
                case GAME_3X3: replace(3, 3 * 3); break;
                case GAME_4X4: replace(4, 4 * 4); break;
                case GAME_5X5: replace(5, 5 * 5); break;
            }
            fillSlideInfo(static_cast<const PuzzleCAM*>(game)->mCurGame, game->getScreen()->width,
                    game->getScreen()->height);

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
    }
    if (mLoadStep != MAX_LOAD_STEP)
        return false;

    delete mHideCam;
    mHideCam = NULL;

    if (mStatus == GAME_PLAYING)
        mTimer = time(NULL) - (mMinutes * 60) - mSeconds;

    return true;
}
void GameLevel::renderLoad() const {

    assert(mHideCam);
    mHideCam->render(false);
}

#ifndef DEMO_VERSION
void GameLevel::adDisplay(bool delay) {

    if (!mAdLoaded) {

        static unsigned char counter = 0;
        if ((!delay) || (++counter == (DISPLAY_DELAY << 2))) { // Avoid to call 'Advertising::getStatus' continually

            LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Check to load/display advertising"), __PRETTY_FUNCTION__, __LINE__);
            switch (Advertising::getStatus()) {
                case Advertising::STATUS_FAILED: {

                    mAdvertising->load();
                    break;
                }
                case Advertising::STATUS_LOADED: {

                    mAdLoaded = true;
                    Advertising::display(0);
                    break;
                }
            }
            counter = 0;
        }
    }
}
#endif

void GameLevel::save(const Game* game) {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - g:%x (s:%d)"), __PRETTY_FUNCTION__, __LINE__, game, mStatus);
    switch (mStatus) {

        case GAME_PLAYING: {
            switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {
                case GAME_3X3: {

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL1)) = mPuzzleIdx[0];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL2)) = mPuzzleIdx[1];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL3)) = mPuzzleIdx[2];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL1)) = mPuzzleIdx[3];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL2)) = mPuzzleIdx[4];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL3)) = mPuzzleIdx[5];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL1)) = mPuzzleIdx[6];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL2)) = mPuzzleIdx[7];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL3)) = mPuzzleIdx[8];
                    break;
                }
                case GAME_4X4: {

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL1)) = mPuzzleIdx[0];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL2)) = mPuzzleIdx[1];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL3)) = mPuzzleIdx[2];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL4)) = mPuzzleIdx[3];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL1)) = mPuzzleIdx[4];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL2)) = mPuzzleIdx[5];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL3)) = mPuzzleIdx[6];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL4)) = mPuzzleIdx[7];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL1)) = mPuzzleIdx[8];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL2)) = mPuzzleIdx[9];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL3)) = mPuzzleIdx[10];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL4)) = mPuzzleIdx[11];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL1)) = mPuzzleIdx[12];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL2)) = mPuzzleIdx[13];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL3)) = mPuzzleIdx[14];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL4)) = mPuzzleIdx[15];
                    break;
                }
                case GAME_5X5: {

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL1)) = mPuzzleIdx[0];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL2)) = mPuzzleIdx[1];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL3)) = mPuzzleIdx[2];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL4)) = mPuzzleIdx[3];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW1_COL5)) = mPuzzleIdx[4];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL1)) = mPuzzleIdx[5];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL2)) = mPuzzleIdx[6];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL3)) = mPuzzleIdx[7];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL4)) = mPuzzleIdx[8];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW2_COL5)) = mPuzzleIdx[9];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL1)) = mPuzzleIdx[10];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL2)) = mPuzzleIdx[11];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL3)) = mPuzzleIdx[12];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL4)) = mPuzzleIdx[13];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW3_COL5)) = mPuzzleIdx[14];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL1)) = mPuzzleIdx[15];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL2)) = mPuzzleIdx[16];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL3)) = mPuzzleIdx[17];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL4)) = mPuzzleIdx[18];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW4_COL5)) = mPuzzleIdx[19];

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL1)) = mPuzzleIdx[20];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL2)) = mPuzzleIdx[21];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL3)) = mPuzzleIdx[22];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL4)) = mPuzzleIdx[23];
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_ROW5_COL5)) = mPuzzleIdx[24];
                    break;
                }
            }
            *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_SCORE_MIN)) = mMinutes;
            *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_SCORE_SEC)) = mSeconds;
            break;
        }
        case GAME_WIN: {

            switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {
                case GAME_3X3: {

                    if (static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_3X3_SCORE))->compare(mScore.getText()) > 0)
                        static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_3X3_SCORE))->assign(mScore.getText());
                    break;
                }
                case GAME_4X4: {

                    if (static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_4X4_SCORE))->compare(mScore.getText()) > 0)
                        static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_4X4_SCORE))->assign(mScore.getText());
                    break;
                }
                case GAME_5X5: {

                    if (static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_5X5_SCORE))->compare(mScore.getText()) > 0)
                        static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_5X5_SCORE))->assign(mScore.getText());
                    break;
                }
            }
            //break;
        }
        case GAME_OVER: {

            *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_LEVEL)) = GAME_NONE;
            break;
        }
    }
}

void GameLevel::updateScore() {

    double secElapsed = difftime(time(NULL), mTimer);

    unsigned char minutes = static_cast<unsigned char>(static_cast<short>(secElapsed) / 60);
    unsigned char seconds = static_cast<unsigned char>(static_cast<short>(secElapsed) % 60);
    if ((minutes != mMinutes) || (seconds != mSeconds)) {

        mMinutes = minutes;
        mSeconds = seconds;

        std::wstring score;
        if (mMinutes < 10)
            score += L'0';
        score.append(numToWStr<short>(static_cast<short>(mMinutes)));
        score += L'\''; // ' == :
        if (mSeconds < 10)
            score += L'0';
        score.append(numToWStr<short>(static_cast<short>(mSeconds)));
        mScore.update(score);

        if ((mMinutes == 99) && (mSeconds == 59)) {

            mTouchID = NO_TOUCH_ID;
            mStatus = GAME_OVER;
        }
    }
}

bool GameLevel::update(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_GAMELEVEL, (*game->getLog() % 100), LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
#endif
#ifdef __ANDROID__
    // Restart player (needed for lock/unlock operation)
    mPlayer->resume();
#endif
    mCamera->updateTexture();

#ifndef DEMO_VERSION
    // Advertising
    adDisplay(true);
#endif

    // Display new game...
    static bool displaying = false;
    if (mPuzzle2D.empty())
        displaying = true;

    if (displaying) {
        if (mPlayer->getStatus(0) == AL_PLAYING)
            return true;

        if (mPuzzleIdx[static_cast<unsigned char>(mPuzzle2D.size())]) {

            Check* check = new Check;
            check->start(game, static_cast<unsigned char>(mPuzzle2D.size()), static_cast<const PuzzleCAM*>(game)->mCurGame,
                    mPuzzleIdx[static_cast<unsigned char>(mPuzzle2D.size())]);

            mPuzzle2D.push_back(check);
        }
        unsigned char len = 0;
        switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {

            case GAME_3X3: len = 8; break;
            case GAME_4X4: len = 15; break;
            case GAME_5X5: len = 24; break;
        }
        if (mPuzzle2D.size() == len) {

            mPuzzle2D.push_back(NULL);
            displaying = false;
        }
        mPlayer->play(0);
        return true;
    }

    // Score, win or game over...
    static unsigned char step = 0;
    switch (mStatus) {

        case GAME_PLAYING: updateScore(); break;
        case GAME_WIN: {

            static unsigned char sound = 0;
            if (!mCongratulation) {

                step = 0;
                switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    case GAME_3X3: sound = 3; break;
                    case GAME_4X4: sound = 4; break;
                    case GAME_5X5: sound = 5; break;
                }
                mCongratulation = new Element2D;
                mCongratulation->initialize(game2DVia(game));
                mCongratulation->start(TEXTURE_ID_FONT);

                short height = ((((game->getScreen()->height >> 1) - (game->getScreen()->width >> 1)) << 3) / 10) >> 1;
                // 80% of part above checkerboard (half)

                short width = (CONGRATULATION_X2 - CONGRATULATION_X0) * height / (CONGRATULATION_Y2 - CONGRATULATION_Y0);
                mCongratulation->setVertices((game->getScreen()->width >> 1) - width, (game->getScreen()->height >> 1) + height,
                        (game->getScreen()->width >> 1) + width, (game->getScreen()->height >> 1) - height);

                static const float texCoords[8] = { CONGRATULATION_X0 / FONT_TEXTURE_WIDTH,
                        CONGRATULATION_Y0 / FONT_TEXTURE_HEIGHT, CONGRATULATION_X0 / FONT_TEXTURE_WIDTH,
                        CONGRATULATION_Y2 / FONT_TEXTURE_HEIGHT, CONGRATULATION_X2 / FONT_TEXTURE_WIDTH,
                        CONGRATULATION_Y2 / FONT_TEXTURE_HEIGHT, CONGRATULATION_X2 / FONT_TEXTURE_WIDTH,
                        CONGRATULATION_Y0 / FONT_TEXTURE_HEIGHT };
                mCongratulation->setTexCoords(texCoords);
                mCongratulation->translate(0.f, ((game->getScreen()->top - 1.f) / 2.f) + 1.f);

                bool best = false;
                switch (static_cast<const PuzzleCAM*>(game)->mCurGame) {
                    case GAME_3X3: {
                        if (static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_3X3_SCORE))->compare(mScore.getText()) > 0)
                            best = true;
                        break;
                    }
                    case GAME_4X4: {
                        if (static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_4X4_SCORE))->compare(mScore.getText()) > 0)
                            best = true;
                        break;
                    }
                    case GAME_5X5: {
                        if (static_cast<std::wstring*>(game->getData(DATA_STRING, DATA_KEY_5X5_SCORE))->compare(mScore.getText()) > 0)
                            best = true;
                        break;
                    }
                }
                if (best) {

                    assert(!mBestPanel);
                    assert(!mBestText);
                    mBestPanel = new Static2D;
                    mBestPanel->initialize(game2DVia(game));
                    mBestPanel->start(TEXTURE_ID_LEVEL);

                    float verCoords[8];
                    verCoords[0] = (mScore.getLeft() - 1.f - (mScore.getTop() - mScore.getBottom())) / 2.f;
                    verCoords[1] = mScore.getTop();
                    verCoords[2] = verCoords[0];
                    verCoords[3] = mScore.getBottom();
                    verCoords[4] = verCoords[0] + (mScore.getTop() - mScore.getBottom());
                    verCoords[5] = mScore.getBottom();
                    verCoords[6] = verCoords[4];
                    verCoords[7] = mScore.getTop();
                    mBestPanel->setVertices(verCoords);
                    mBestPanel->setTexCoords(FULL_TEXCOORD_BUFFER);

                    mBestPanel->setRed(1.f);
                    mBestPanel->setGreen(0.f);
                    mBestPanel->setBlue(0.f);

                    mBestText = new Static2D;
                    mBestText->initialize(game2DVia(game));
                    mBestText->start(TEXTURE_ID_FONT);
                    mBestText->setVertices(verCoords);

                    static const float texCoords[8] = { BEST_X0 / FONT_TEXTURE_WIDTH, BEST_Y0 / FONT_TEXTURE_HEIGHT,
                            BEST_X0 / FONT_TEXTURE_WIDTH, BEST_Y2 / FONT_TEXTURE_HEIGHT, 1.f, BEST_Y2 / FONT_TEXTURE_HEIGHT,
                            1.f, BEST_Y0 / FONT_TEXTURE_HEIGHT };
                    mBestText->setTexCoords(texCoords);
                }
            }

            if (++step < DISPLAY_DELAY) {
                float scale = mCongratulation->getTransform()[Dynamic2D::SCALE_X] + SCALE_VELOCITY;
                mCongratulation->scale(scale, scale);
            }
            else if (step < ((DISPLAY_DELAY * 2) - 1)) {
                float scale = mCongratulation->getTransform()[Dynamic2D::SCALE_X] - SCALE_VELOCITY;
                mCongratulation->scale(scale, scale);
            }
            else if (step < (DISPLAY_DELAY * 3)) {
                for (std::vector<Check*>::iterator iter = mPuzzle2D.begin(); iter != mPuzzle2D.end(); ++iter)
                    if (*iter) (*iter)->rotate(true);
            }
            else if (step < ((DISPLAY_DELAY * 5) + 1)) {
                for (std::vector<Check*>::iterator iter = mPuzzle2D.begin(); iter != mPuzzle2D.end(); ++iter)
                    if (*iter) (*iter)->rotate(false);
            }
            else if (step < ((DISPLAY_DELAY * 6) + 1)) { // Still < 255!
                for (std::vector<Check*>::iterator iter = mPuzzle2D.begin(); iter != mPuzzle2D.end(); ++iter)
                    if (*iter) (*iter)->rotate(true);
            }
            else
                step = 0;

            if ((sound) && (mPlayer->getStatus(2) != AL_PLAYING)) {

                mPlayer->play(2);
                --sound;
            }
            break;
        }
        case GAME_OVER: {

            static unsigned char sound = 2;
            if (!mGameOver) {

                mGameOver = new Element2D;
                mGameOver->initialize(game2DVia(game));
                mGameOver->start(TEXTURE_ID_FONT);

                short height = ((((game->getScreen()->height >> 1) - (game->getScreen()->width >> 1)) << 3) / 10) >> 1;
                // 80% of part above checkerboard (half)

                short width = (CONGRATULATION_X2 - CONGRATULATION_X0) * height / (CONGRATULATION_Y2 - CONGRATULATION_Y0);
                mGameOver->setVertices((game->getScreen()->width >> 1) - width, (game->getScreen()->height >> 1) + height,
                        (game->getScreen()->width >> 1) + width, (game->getScreen()->height >> 1) - height);

                static const float texCoords[8] = { GAME_OVER_X0 / FONT_TEXTURE_WIDTH, GAME_OVER_Y0 / FONT_TEXTURE_HEIGHT,
                        GAME_OVER_X0 / FONT_TEXTURE_WIDTH, GAME_OVER_Y2 / FONT_TEXTURE_HEIGHT,
                        GAME_OVER_X2 / FONT_TEXTURE_WIDTH, GAME_OVER_Y2 / FONT_TEXTURE_HEIGHT,
                        GAME_OVER_X2 / FONT_TEXTURE_WIDTH, GAME_OVER_Y0 / FONT_TEXTURE_HEIGHT };
                mGameOver->setTexCoords(texCoords);
                mGameOver->translate(0.f, ((game->getScreen()->top - 1.f) / 2.f) + 1.f);

                mScore.setColor(1.f, 0.f, 0.f);

                step = 0;
                sound = 2;
            }
            if (++step < (DISPLAY_DELAY * 2)) {
                float scale = mGameOver->getTransform()[Dynamic2D::SCALE_X] + (SCALE_VELOCITY / 2.f);
                mGameOver->scale(scale, scale);
            }
            else if (step < ((DISPLAY_DELAY * 4) - 1)) {
                float scale = mGameOver->getTransform()[Dynamic2D::SCALE_X] - (SCALE_VELOCITY / 2.f);
                mGameOver->scale(scale, scale);
            }
            else
                step = 0;

            if ((sound) && (mPlayer->getStatus(3) != AL_PLAYING)) {

                mPlayer->play(3);
                --sound;
            }
            break;
        }
    }

    // Slide...
    if ((mSlideAreaIdx) && (mStatus != GAME_OVER)) {

        unsigned char checkIdx = mSlideCheck[mSlideAreaIdx - 1];
        assert(mPuzzle2D[checkIdx]);
        assert(!mPuzzle2D[mNoCheckIdx]);

        bool done;
        if (mSlideFlag) { // Slide

            done = (mPuzzle2D[checkIdx]->getSlideStep() == MAX_SLIDE_STEP);
            if (mPuzzle2D[checkIdx]->slide(true, static_cast<const PuzzleCAM*>(game)->mCurGame, checkIdx, mNoCheckIdx) != done)
                mPlayer->play(1);
        }
        else { // Unslide

            done = !static_cast<bool>(mPuzzle2D[checkIdx]->getSlideStep());
            if (mPuzzle2D[checkIdx]->slide(false, static_cast<const PuzzleCAM*>(game)->mCurGame, mNoCheckIdx, checkIdx) != done)
                mPlayer->play(1);
        }
        if (mWaitSlide) { // Wait slide finished

            if (!done)
                return true;

            if (mSlideFlag) { // Slide confirmed

                mPuzzle2D[checkIdx]->setSlideStep();

                std::swap<Check*>(mPuzzle2D[mNoCheckIdx], mPuzzle2D[checkIdx]);
                std::swap<unsigned char>(mPuzzleIdx[mNoCheckIdx], mPuzzleIdx[checkIdx]);

                mNoCheckIdx = checkIdx;
                fillSlideInfo(static_cast<const PuzzleCAM*>(game)->mCurGame, game->getScreen()->width, game->getScreen()->height);

                if (mStatus == GAME_NEW) {

                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_LEVEL)) = static_cast<const PuzzleCAM*>(game)->mCurGame;
                    mStatus = GAME_PLAYING;

                    mMinutes = 0;
                    mSeconds = 0;
                    mTimer = time(NULL);
                }
                else if (win(static_cast<const PuzzleCAM*>(game)->mCurGame))
                    mStatus = GAME_WIN; // Congratulation!

                mSlideFlag = false;
            }
            //else // Slide cancelled

            mWaitSlide = false;
            mTouchID = NO_TOUCH_ID;
            mSlideAreaIdx = 0;
        }
    }

    // Touch...
    unsigned char touchCount = game->mTouchCount;
    while (touchCount--) {

        if ((mTouchID != NO_TOUCH_ID) && (mTouchID != game->mTouchData[touchCount].Id))
            continue; // Avoid several touch when slide a check

        switch (game->mTouchData[touchCount].Type) {
            case TouchInput::TOUCH_DOWN: {
                if ((mStatus != GAME_NEW) && (mStatus != GAME_PLAYING))
                    break;
                if ((game->mTouchData[touchCount].Y < ((game->getScreen()->height >> 1) - (game->getScreen()->width >> 1))) ||
                        (game->mTouchData[touchCount].Y > ((game->getScreen()->height >> 1) + (game->getScreen()->width >> 1))))
                    break;

                if ((game->mTouchData[touchCount].X > mSlideArea[0].left) && (game->mTouchData[touchCount].X < mSlideArea[0].right) &&
                        (game->mTouchData[touchCount].Y > mSlideArea[0].top) && (game->mTouchData[touchCount].Y < mSlideArea[0].bottom)) {

                    mTouchID = game->mTouchData[touchCount].Id;
                    mSlideAreaIdx = 1;
                }
                else if ((game->mTouchData[touchCount].X > mSlideArea[1].left) && (game->mTouchData[touchCount].X < mSlideArea[1].right) &&
                        (game->mTouchData[touchCount].Y > mSlideArea[1].top) && (game->mTouchData[touchCount].Y < mSlideArea[1].bottom)) {

                    mTouchID = game->mTouchData[touchCount].Id;
                    mSlideAreaIdx = 2;
                }
                else if ((game->mTouchData[touchCount].X > mSlideArea[2].left) && (game->mTouchData[touchCount].X < mSlideArea[2].right) &&
                        (game->mTouchData[touchCount].Y > mSlideArea[2].top) && (game->mTouchData[touchCount].Y < mSlideArea[2].bottom)) {

                    mTouchID = game->mTouchData[touchCount].Id;
                    mSlideAreaIdx = 3;
                }
                else if ((game->mTouchData[touchCount].X > mSlideArea[3].left) && (game->mTouchData[touchCount].X < mSlideArea[3].right) &&
                        (game->mTouchData[touchCount].Y > mSlideArea[3].top) && (game->mTouchData[touchCount].Y < mSlideArea[3].bottom)) {

                    mTouchID = game->mTouchData[touchCount].Id;
                    mSlideAreaIdx = 4;
                }
                break;
            }
            case TouchInput::TOUCH_MOVE: {
                if ((mStatus != GAME_NEW) && (mStatus != GAME_PLAYING))
                    break;

                switch (mSlideAreaIdx) {
                    case 1: { // Slide/Unslide the check on the top

                        if ((game->mTouchData[touchCount].X > mSlideArea[0].left) && (game->mTouchData[touchCount].X < mSlideArea[0].right))
                            mSlideFlag = (game->mTouchData[touchCount].Y > mSlideArea[0].bottom);
                        break;
                    }
                    case 2: { // Slide/Unslide the check on the right

                        if ((game->mTouchData[touchCount].Y > mSlideArea[1].top) && (game->mTouchData[touchCount].Y < mSlideArea[1].bottom))
                            mSlideFlag = (game->mTouchData[touchCount].X < mSlideArea[1].left);
                        break;
                    }
                    case 3: { // Slide/Unslide the check on the bottom

                        if ((game->mTouchData[touchCount].X > mSlideArea[2].left) && (game->mTouchData[touchCount].X < mSlideArea[2].right))
                            mSlideFlag = (game->mTouchData[touchCount].Y < mSlideArea[2].top);
                        break;
                    }
                    case 4: { // Slide/Unslide the check on the left

                        if ((game->mTouchData[touchCount].Y > mSlideArea[3].top) && (game->mTouchData[touchCount].Y < mSlideArea[3].bottom))
                            mSlideFlag = (game->mTouchData[touchCount].X > mSlideArea[3].right);
                        break;
                    }
                }
                break;
            }
            default: { // TOUCH_UP & TOUCH_CANCEL

                if (mTouchID != NO_TOUCH_ID) {

                    LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - TOUCH_UP (Slide)"), __PRETTY_FUNCTION__, __LINE__);
                    assert(mSlideAreaIdx);
                    mWaitSlide = true; // Wait finish to slide
                    break;
                }

                // Exit
                if ((game->mTouchData[touchCount].X > mExitArea.left) && (game->mTouchData[touchCount].X < mExitArea.right) &&
                        (game->mTouchData[touchCount].Y > mExitArea.top) && (game->mTouchData[touchCount].Y < mExitArea.bottom)) {

                    mPlayer->stop(ALL_TRACK);
                    mPlayer->play(0);
                    mCamera->stop();
                    save(game);
                    return false; // Pause
                }
                break;
            }
        }
    }
    return true;
}
void GameLevel::renderUpdate() const {

    mBackground.render(false);

    mExit.render(false);
    mScore.render(true);
    for (std::vector<Check*>::const_iterator iter = mPuzzle2D.begin(); iter != mPuzzle2D.end(); ++iter)
        if (*iter) (*iter)->render();

    if (mGameOver) mGameOver->render(true);
    if (mCongratulation) mCongratulation->render(true);
    if (mBestPanel) mBestPanel->render(true);
    if (mBestText) mBestText->render(false);
}
