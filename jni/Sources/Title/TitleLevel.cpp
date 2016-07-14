#include "TitleLevel.h"
#include "PuzzleCAM.h"

#include <libeng/Game/2D/Game2D.h>

#define MAX_LOAD_STEP           8

#define CAM_X0                  228
#define CAM_Y0                  121
#define CAM_X2                  503
#define CAM_Y2                  247

#define TITLE_TEXTURE_WIDTH     512.f
#define TITLE_TEXTURE_HEIGHT    256.f

#define PANEL_TEXTURE_WIDTH     1024
#define PANEL_TEXTURE_HEIGHT    256

#define ICON_START_Y0           249
#define ICON_START_SIZE         171
#define ICON_START_LAG          37

#define ICON_RESUME_X0          343
#define ICON_RESUME_Y0          250
#define ICON_RESUME_SIZE        168

#define ICON_SCORES_X0          171
#define ICON_SCORES_Y0          249
#define ICON_SCORES_SIZE        171

#define START_HEIGHT            82
#define START_WIDTH             84
#define START_NEW_WIDTH         295

#define RESUME_Y0               82
#define RESUME_HEIGHT           82
#define RESUME_WIDTH            262

#define SCORES_X0               262
#define SCORES_Y0               82
#define SCORES_X2               380
#define SCORES_Y2               164

#define TEXT_LEVEL_SIZE         95
#define TEXT_3X3_X0             392
#define TEXT_4X4_X0             (TEXT_3X3_X0 + TEXT_LEVEL_SIZE)
#define TEXT_5X5_X0             (TEXT_4X4_X0 + TEXT_LEVEL_SIZE)

#define SCORE_3X3               L"3\"3-"
#define SCORE_4X4               L"4\"4-"
#define SCORE_5X5               L"5\"5-"

#define SCORES_LAG_Y            -0.5f // Lag between 0 & bottom of the scores panel (in unit)

// Texture IDs
#define TEXTURE_ID_TITLE        2
#define TEXTURE_ID_PANEL        3

//////
TitleLevel::TitleLevel(Game* game) : Level(game), mCAM(false), mStep(STATUS_MENU), mScoreLagY(0.f), mGame(game) {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
    std::memset(&mResumeArea, 0, sizeof(TouchArea));
    std::memset(&mStartArea, 0, sizeof(TouchArea));
    std::memset(&mScoreArea, 0, sizeof(TouchArea));

    std::memset(&mLevelArea3, 0, sizeof(TouchArea));
    std::memset(&mLevelArea4, 0, sizeof(TouchArea));
    std::memset(&mLevelArea5, 0, sizeof(TouchArea));

    std::memset(&mExitArea, 0, sizeof(TouchArea));

    Inputs::InputUse = USE_INPUT_TOUCH;

    mCamera = Camera::getInstance();
    mPlayer = Player::getInstance();
#ifndef DEMO_VERSION
    mAdLoaded = false;
    mAdvertising = Advertising::getInstance();
#endif

    mHideCam = NULL;
    mResumePanel = NULL;
    mResumeIcon = NULL;
    mResume = NULL;
}
TitleLevel::~TitleLevel() {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (mHideCam) delete mHideCam;
    if (mResumePanel) delete mResumePanel;
    if (mResumeIcon) delete mResumeIcon;
    if (mResume) delete mResume;
}

void TitleLevel::pause() {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Level::pause();

    switch (mStep) {
        case STATUS_LEVEL: hideLevel(); break;
        case STATUS_SCORES: {

            hideScores();

            // Code below is... :(
            if (mScoreLagY < 0.f)
                hideScore();
            break;
        }
    }
    mStep = STATUS_MENU;

    if (mHideCam) mHideCam->pause();

    mBackground.pause();
    mTitle.pause();
    mCAM.pause();

    if (mResumePanel) mResumePanel->pause();
    if (mResumeIcon) mResumeIcon->pause();
    if (mResume) mResume->pause();

    mStartPanel.pause();
    mStartIcon.pause();
    mStart.pause();

    mScorePanel.pause();
    mScoreIcon.pause();
    mScore.pause();

    mScore3X3.pause();
    mScore4X4.pause();
    mScore5X5.pause();

    mLevel3.pause();
    mLevel4.pause();
    mLevel5.pause();

    mLevelText3.pause();
    mLevelText4.pause();
    mLevelText5.pause();

    mExit.pause();
}

bool TitleLevel::loaded(const Game* game) {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(" - l:%d"), __PRETTY_FUNCTION__, __LINE__, mLoadStep);
    static short sizeHeight = (static_cast<short>((game->getScreen()->width >> 2) * (80 / 100.f)));
    static short yLag = ((game->getScreen()->height >> 1) - (game->getScreen()->width >> 1)) / 3;
    static short yResumeLagA = (game->getScreen()->height / 6) - (game->getScreen()->width >> 2);
    static short yResumeLagB = (game->getScreen()->height << 4) / 600;
    static short resumeHeight = ((game->getScreen()->height >> 1) - ((game->getScreen()->height << 4) / 200)) / 3;
    static short resumeWidth = PANEL_TEXTURE_WIDTH * (resumeHeight >> 1) / PANEL_TEXTURE_HEIGHT; // Half
    static short yLevelLag = (game->getScreen()->height - game->getScreen()->width) / 6;

    short yResumeLag = 0;
    if (static_cast<const PuzzleCAM*>(game)->mCurGame)
        yResumeLag = (((game->getScreen()->height >> 1) / 3) > (game->getScreen()->width >> 2))? yResumeLagA:yResumeLagB;

    switch (mLoadStep) {
        case 1: {

            if (!mHideCam) {

                mHideCam = new Back2D;
                mHideCam->initialize(game2DVia(game));
                mHideCam->start(0, 0, 0);
            }
            else
                mHideCam->resume(0, 0, 0);
            break;
        }
        case 2: {

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
            if (!mCAM.isStarted()) {

                mCAM.initialize(game2DVia(game));
                if (!mCamera->isStarted())
                    mCamera->start(CAM_WIDTH, CAM_HEIGHT);
                mCAM.start(mCamera->getCamTexIdx());
#ifndef __ANDROID__
                mCAM.setBGRA(true);
#endif
                mCAM.setVertices(CAM_X0 * game->getScreen()->width / TITLE_TEXTURE_WIDTH, (game->getScreen()->height >> 1) +
                        (game->getScreen()->height >> 2) + (game->getScreen()->width >> 2) - (CAM_Y0 *
                        (game->getScreen()->width >> 1) / TITLE_TEXTURE_HEIGHT), CAM_X2 * game->getScreen()->width /
                        TITLE_TEXTURE_WIDTH, (game->getScreen()->height >> 1) + (game->getScreen()->height >> 2) +
                        (game->getScreen()->width >> 2) - (CAM_Y2 * (game->getScreen()->width >> 1) / TITLE_TEXTURE_HEIGHT));

                static const float texCoords[8] = { ((CAM_WIDTH >> 1) - ((((CAM_Y2 - CAM_Y0) * CAM_HEIGHT) /
                        (CAM_X2 - CAM_X0)) >> 1)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT / CAM_TEXTURE_HEIGHT, ((CAM_WIDTH >> 1) +
                        ((((CAM_Y2 - CAM_Y0) * CAM_HEIGHT) / (CAM_X2 - CAM_X0)) >> 1)) / CAM_TEXTURE_WIDTH, CAM_HEIGHT /
                        CAM_TEXTURE_HEIGHT, ((CAM_WIDTH >> 1) + ((((CAM_Y2 - CAM_Y0) * CAM_HEIGHT) / (CAM_X2 - CAM_X0)) >> 1)) /
                        CAM_TEXTURE_WIDTH, 0.f, ((CAM_WIDTH >> 1) - ((((CAM_Y2 - CAM_Y0) * CAM_HEIGHT) /
                        (CAM_X2 - CAM_X0)) >> 1)) / CAM_TEXTURE_WIDTH, 0.f };
                mCAM.setTexCoords(texCoords);
            }
            else
                mCAM.resume(mCamera->getCamTexIdx());

            mLevelArea3.left = game->getScreen()->width / 6;
            mLevelArea3.top = (game->getScreen()->height >> 1) + yLevelLag;
            mLevelArea3.right = mLevelArea3.left + (game->getScreen()->width >> 2);
            mLevelArea3.bottom = mLevelArea3.top + (game->getScreen()->width >> 2);

            mLevelArea4.left = (mLevelArea3.left << 1) + (game->getScreen()->width >> 2);
            mLevelArea4.top = mLevelArea3.top;
            mLevelArea4.right = mLevelArea4.left + (game->getScreen()->width >> 2);
            mLevelArea4.bottom = mLevelArea3.bottom;

            if (!mLevel3.isStarted()) {

                mLevel3.initialize(game2DVia(game));
                mLevel3.start(TEXTURE_ID_LEVEL);
                mLevel3.setTexCoords(FULL_TEXCOORD_BUFFER);
                mLevel3.setAlpha(0.f);

                mLevel3.setRed(GREEN_COLOR_R / MAX_COLOR);
                mLevel3.setGreen(GREEN_COLOR_G / MAX_COLOR);
                mLevel3.setBlue(GREEN_COLOR_B / MAX_COLOR);

                mLevel3.setVertices(mLevelArea3.left, game->getScreen()->height - mLevelArea3.top, mLevelArea3.right,
                        game->getScreen()->height - mLevelArea3.bottom);
            }
            else
                mLevel3.resume(TEXTURE_ID_LEVEL);
            break;
        }
        case 3: {

            if (!mScorePanel.isStarted()) {

                mScorePanel.initialize(game2DVia(game));
                mScorePanel.start(TEXTURE_ID_PANEL);
                mScorePanel.setTexCoords(FULL_TEXCOORD_BUFFER);

                short yScoreLag;
                if (!static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    yScoreLag = (game->getScreen()->height >> 1) - (yLag << 1) - (game->getScreen()->width >> 2);
                    mScorePanel.setVertices(0, yScoreLag, game->getScreen()->width, (game->getScreen()->height >> 1) - (yLag << 1) -
                            (game->getScreen()->width >> 1));
                }
                else if (yResumeLag == yResumeLagA) {

                    yScoreLag = (game->getScreen()->height >> 1) - (yResumeLag << 1) - (game->getScreen()->width >> 1);
                    mScorePanel.setVertices(0, yScoreLag, game->getScreen()->width, (game->getScreen()->height >> 1) -
                            (yResumeLag << 1) - ((game->getScreen()->width * 3) >> 2));
                }
                else {

                    yScoreLag = (game->getScreen()->height >> 1) - (resumeHeight << 1) - (yResumeLag << 1);
                    mScorePanel.setVertices((game->getScreen()->width >> 1) - resumeWidth, yScoreLag, (game->getScreen()->width >> 1) +
                                            resumeWidth, yScoreLag - resumeHeight);
                }
                mScoreLagY = (((game->getScreen()->height >> 1) - yScoreLag) << 1) * game->getScreen()->top /
                        game->getScreen()->height;
                if (yResumeLag == yResumeLagB)
                    mScoreLagY = -1.f;

                mScorePanel.setRed(BLUE_COLOR_R / MAX_COLOR);
                mScorePanel.setGreen(BLUE_COLOR_G / MAX_COLOR);
                mScorePanel.setBlue(BLUE_COLOR_B / MAX_COLOR);
            }
            else
                mScorePanel.resume(TEXTURE_ID_PANEL);

            if (!mLevel4.isStarted()) {

                mLevel4.initialize(game2DVia(game));
                mLevel4.start(TEXTURE_ID_LEVEL);
                mLevel4.setTexCoords(FULL_TEXCOORD_BUFFER);
                mLevel4.setAlpha(0.f);

                mLevel4.setRed(BLUE_COLOR_R / MAX_COLOR);
                mLevel4.setGreen(BLUE_COLOR_G / MAX_COLOR);
                mLevel4.setBlue(BLUE_COLOR_B / MAX_COLOR);

                mLevel4.setVertices(mLevelArea4.left, game->getScreen()->height - mLevelArea4.top, mLevelArea4.right,
                        game->getScreen()->height - mLevelArea4.bottom);
            }
            else
                mLevel4.resume(TEXTURE_ID_LEVEL);

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
        case 4: {

            if (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                if (!mResumePanel) {

                    mResumePanel = new Static2D;
                    mResumePanel->initialize(game2DVia(game));
                    mResumePanel->start(TEXTURE_ID_PANEL);
                    mResumePanel->setTexCoords(FULL_TEXCOORD_BUFFER);

                    mResumePanel->setRed(GREEN_COLOR_R / MAX_COLOR);
                    mResumePanel->setGreen(GREEN_COLOR_G / MAX_COLOR);
                    mResumePanel->setBlue(GREEN_COLOR_B / MAX_COLOR);

                    if (yResumeLag == yResumeLagA)
                        mResumePanel->setVertices(0, game->getScreen()->height >> 1, game->getScreen()->width,
                                (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2));
                    else
                        mResumePanel->setVertices((game->getScreen()->width >> 1) - resumeWidth, (game->getScreen()->height >> 1),
                                (game->getScreen()->width >> 1) + resumeWidth, (game->getScreen()->height >> 1) - resumeHeight);
                }
                else
                    mResumePanel->resume(TEXTURE_ID_PANEL);
            }
            if (!mStartPanel.isStarted()) {

                mStartPanel.initialize(game2DVia(game));
                mStartPanel.start(TEXTURE_ID_PANEL);
                mStartPanel.setTexCoords(FULL_TEXCOORD_BUFFER);

                if (!static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    mStartPanel.setRed(GREEN_COLOR_R / MAX_COLOR);
                    mStartPanel.setGreen(GREEN_COLOR_G / MAX_COLOR);
                    mStartPanel.setBlue(GREEN_COLOR_B / MAX_COLOR);

                    mStartPanel.setVertices(0, (game->getScreen()->height >> 1) - yLag, game->getScreen()->width,
                            (game->getScreen()->height >> 1) - yLag - (game->getScreen()->width >> 2));
                }
                else {

                    mStartPanel.setRed(RED_COLOR_R / MAX_COLOR);
                    mStartPanel.setGreen(RED_COLOR_G / MAX_COLOR);
                    mStartPanel.setBlue(RED_COLOR_B / MAX_COLOR);

                    if (yResumeLag == yResumeLagA)
                        mStartPanel.setVertices(0, (game->getScreen()->height >> 1) - yResumeLag - (game->getScreen()->width >> 2),
                                game->getScreen()->width, (game->getScreen()->height >> 1) - yResumeLag -
                                (game->getScreen()->width >> 1));
                    else
                        mStartPanel.setVertices((game->getScreen()->width >> 1) - resumeWidth, (game->getScreen()->height >> 1) -
                                resumeHeight - yResumeLag, (game->getScreen()->width >> 1) + resumeWidth,
                                (game->getScreen()->height >> 1) - resumeHeight - yResumeLag - resumeHeight);
                }
            }
            else
                mStartPanel.resume(TEXTURE_ID_PANEL);

            // Scores (start)
            if (!mScore3X3.isStarted()) {

                mScore3X3.initialize(game2DVia(game));
                std::wstring scores(SCORE_3X3);
                scores.append(*static_cast<const std::wstring*>(game->getData(DATA_STRING, DATA_KEY_3X3_SCORE)));

                mScore3X3.start(scores);
                mScore3X3.setAlpha(0.f);
            }
            else
                mScore3X3.resume();

            if (!mScore4X4.isStarted()) {

                mScore4X4.initialize(game2DVia(game));
                std::wstring scores(SCORE_4X4);
                scores.append(*static_cast<const std::wstring*>(game->getData(DATA_STRING, DATA_KEY_4X4_SCORE)));

                mScore4X4.start(scores);
                mScore4X4.setAlpha(0.f);
            }
            else
                mScore4X4.resume();

            if (!mScore5X5.isStarted()) {

                mScore5X5.initialize(game2DVia(game));
                std::wstring scores(SCORE_5X5);
                scores.append(*static_cast<const std::wstring*>(game->getData(DATA_STRING, DATA_KEY_5X5_SCORE)));

                mScore5X5.start(scores);
                mScore5X5.setAlpha(0.f);
            }
            else
                mScore5X5.resume();

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
        case 5: {

            if (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                if (!mResumeIcon) {

                    mResumeIcon = new Static2D;
                    mResumeIcon->initialize(game2DVia(game));
                    mResumeIcon->start(TEXTURE_ID_FONT);

                    mResumeIcon->setRed(YELLOW_COLOR_R / MAX_COLOR);
                    mResumeIcon->setGreen(YELLOW_COLOR_G / MAX_COLOR);
                    mResumeIcon->setBlue(YELLOW_COLOR_B / MAX_COLOR);

                    if (yResumeLag == yResumeLagA) {

                        static short width = RESUME_WIDTH * sizeHeight / RESUME_HEIGHT;
                        static short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) *
                                                                                                sizeHeight) + width)) >> 1);

                        mResumeIcon->setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 3) +
                                                 (sizeHeight >> 1), xLag + sizeHeight, (game->getScreen()->height >> 1) -
                                                 (game->getScreen()->width >> 3) - (sizeHeight >> 1));
                    }
                    else {

                        static short iconSize = (resumeHeight << 3) / 10;
                        static short yIconLag = (iconSize << 4) / 100;
                        static short xIconLag = (iconSize << 5) / 100;

                        mResumeIcon->setVertices((game->getScreen()->width >> 1) - resumeWidth + xIconLag,
                                                 (game->getScreen()->height >> 1) - yIconLag, (game->getScreen()->width >> 1) -
                                                 resumeWidth + xIconLag + iconSize, (game->getScreen()->height >> 1) - yIconLag -
                                                 iconSize);
                    }
                    static const float texCoords[8] = { ICON_RESUME_X0 / FONT_TEXTURE_WIDTH, ICON_RESUME_Y0 / FONT_TEXTURE_HEIGHT,
                            ICON_RESUME_X0 / FONT_TEXTURE_WIDTH, (ICON_RESUME_Y0 + ICON_RESUME_SIZE) / FONT_TEXTURE_HEIGHT,
                            (ICON_RESUME_X0 + ICON_RESUME_SIZE) / FONT_TEXTURE_WIDTH, (ICON_RESUME_Y0 + ICON_RESUME_SIZE) /
                            FONT_TEXTURE_HEIGHT, (ICON_RESUME_X0 + ICON_RESUME_SIZE) / FONT_TEXTURE_WIDTH, ICON_RESUME_Y0 /
                            FONT_TEXTURE_HEIGHT };
                    mResumeIcon->setTexCoords(texCoords);
                }
                else
                    mResumeIcon->resume(TEXTURE_ID_FONT);
            }
            if (!mStartIcon.isStarted()) {

                mStartIcon.initialize(game2DVia(game));
                mStartIcon.start(TEXTURE_ID_FONT);

                if (!static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    mStartIcon.setRed(YELLOW_COLOR_R / MAX_COLOR);
                    mStartIcon.setGreen(YELLOW_COLOR_G / MAX_COLOR);
                    mStartIcon.setBlue(YELLOW_COLOR_B / MAX_COLOR);

                    static short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                                                            (START_WIDTH * sizeHeight / RESUME_HEIGHT))) >> 1);
                    mStartIcon.setVertices(xLag, (game->getScreen()->height >> 1) - yLag - (game->getScreen()->width >> 3) +
                                        (sizeHeight >> 1), xLag + sizeHeight, (game->getScreen()->height >> 1) - yLag -
                                        (game->getScreen()->width >> 3) - (sizeHeight >> 1));
                }
                else {

                    mStartIcon.setRed(ORANGE_COLOR_R / MAX_COLOR);
                    mStartIcon.setGreen(ORANGE_COLOR_G / MAX_COLOR);
                    mStartIcon.setBlue(ORANGE_COLOR_B / MAX_COLOR);

                    if (yResumeLag == yResumeLagA) {
                    
                        static short xLag = ICON_START_LAG * game->getScreen()->width / static_cast<short>(FONT_TEXTURE_WIDTH);
                        mStartIcon.setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) - yResumeLag -
                                (game->getScreen()->width >> 3) + (sizeHeight >> 1), xLag + sizeHeight, (game->getScreen()->height >> 1) -
                                (game->getScreen()->width >> 2) - yResumeLag - (game->getScreen()->width >> 3) - (sizeHeight >> 1));
                    }
                    else {

                        static short iconSize = (resumeHeight << 3) / 10;
                        static short yIconLag = (iconSize * 12) / 100;
                        static short xIconLag = (iconSize << 4) / 100;

                        mStartIcon.setVertices((game->getScreen()->width >> 1) - resumeWidth + xIconLag,
                                            (game->getScreen()->height >> 1) - resumeHeight - yResumeLag - yIconLag,
                                            (game->getScreen()->width >> 1) - resumeWidth + xIconLag + iconSize,
                                            (game->getScreen()->height >> 1) - resumeHeight - yResumeLag - yIconLag - iconSize);
                    }
                }
                static const float texCoords[8] = { 0.f, ICON_START_Y0 / FONT_TEXTURE_HEIGHT, 0.f, (ICON_START_Y0 +
                        ICON_START_SIZE) / FONT_TEXTURE_HEIGHT, ICON_START_SIZE / FONT_TEXTURE_WIDTH, (ICON_START_Y0 +
                        ICON_START_SIZE) / FONT_TEXTURE_HEIGHT, ICON_START_SIZE / FONT_TEXTURE_WIDTH, ICON_START_Y0 /
                        FONT_TEXTURE_HEIGHT };
                mStartIcon.setTexCoords(texCoords);
            }
            else
                mStartIcon.resume(TEXTURE_ID_FONT);

            if (!mLevelText3.isStarted()) {

                mLevelText3.initialize(game2DVia(game));
                mLevelText3.start(TEXTURE_ID_FONT);
                mLevelText3.setAlpha(0.f);
                mLevelText3.copyVertices(mLevel3);

                static const float texCoords[8] = { TEXT_3X3_X0 / FONT_TEXTURE_WIDTH, 0.f, TEXT_3X3_X0 / FONT_TEXTURE_WIDTH,
                        TEXT_LEVEL_SIZE / FONT_TEXTURE_HEIGHT, (TEXT_3X3_X0 + TEXT_LEVEL_SIZE) / FONT_TEXTURE_WIDTH,
                        TEXT_LEVEL_SIZE / FONT_TEXTURE_HEIGHT, (TEXT_3X3_X0 + TEXT_LEVEL_SIZE) / FONT_TEXTURE_WIDTH, 0.f };
                mLevelText3.setTexCoords(texCoords);
            }
            else
                mLevelText3.resume(TEXTURE_ID_FONT);

            if (!mLevelText4.isStarted()) {

                mLevelText4.initialize(game2DVia(game));
                mLevelText4.start(TEXTURE_ID_FONT);
                mLevelText4.setAlpha(0.f);
                mLevelText4.copyVertices(mLevel4);

                static const float texCoords[8] = { TEXT_4X4_X0 / FONT_TEXTURE_WIDTH, 0.f, TEXT_4X4_X0 / FONT_TEXTURE_WIDTH,
                        TEXT_LEVEL_SIZE / FONT_TEXTURE_HEIGHT, (TEXT_4X4_X0 + TEXT_LEVEL_SIZE) / FONT_TEXTURE_WIDTH,
                        TEXT_LEVEL_SIZE / FONT_TEXTURE_HEIGHT, (TEXT_4X4_X0 + TEXT_LEVEL_SIZE) / FONT_TEXTURE_WIDTH, 0.f };
                mLevelText4.setTexCoords(texCoords);
            }
            else
                mLevelText4.resume(TEXTURE_ID_FONT);

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
        case 6: {

            if (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                if (!mResume) {

                    mResume = new Static2D;
                    mResume->initialize(game2DVia(game));
                    mResume->start(TEXTURE_ID_FONT);

                    if (yResumeLag == yResumeLagA) {

                        static short width = RESUME_WIDTH * sizeHeight / RESUME_HEIGHT;
                        static short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                                                                          width)) >> 1) + sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight);

                        mResume->setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 3) +
                                             (sizeHeight >> 1), xLag + width, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 3) -
                                             (sizeHeight >> 1));
                    }
                    else {

                        static short height = (resumeHeight << 3) / 10;
                        static short width = RESUME_WIDTH * height / RESUME_HEIGHT;
                        static short yIconLag = (((resumeHeight << 3) / 10) * 12) / 100;
                        static short xLag = (((((resumeHeight << 3) / 10) << 5) / 100) + ((resumeHeight << 3) / 10)) +
                                            (static_cast<short>((10 / 100.f) * resumeHeight));

                        mResume->setVertices((game->getScreen()->width >> 1) - resumeWidth + xLag, (game->getScreen()->height >> 1) -
                                        yIconLag, (game->getScreen()->width >> 1) - resumeWidth + xLag + width, (game->getScreen()->height >> 1) -
                                        yIconLag - height);
                    }
                    static const float texCoords[8] = { 0.f, RESUME_Y0 / FONT_TEXTURE_HEIGHT, 0.f, (RESUME_Y0 + RESUME_HEIGHT) /
                            FONT_TEXTURE_HEIGHT, RESUME_WIDTH / FONT_TEXTURE_WIDTH, (RESUME_Y0 + RESUME_HEIGHT) /
                            FONT_TEXTURE_HEIGHT, RESUME_WIDTH / FONT_TEXTURE_WIDTH, RESUME_Y0 / FONT_TEXTURE_HEIGHT };
                    mResume->setTexCoords(texCoords);
                }
                else
                    mResume->resume(TEXTURE_ID_FONT);
            }
            if (!mStart.isStarted()) {

                mStart.initialize(game2DVia(game));
                mStart.start(TEXTURE_ID_FONT);

                if (!static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    static short width = START_WIDTH * sizeHeight / START_HEIGHT;
                    static short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                            width)) >> 1) + sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight);

                    mStart.setVertices(xLag, (game->getScreen()->height >> 1) - yLag - (game->getScreen()->width >> 3) +
                            (sizeHeight >> 1), xLag + width, (game->getScreen()->height >> 1) - yLag -
                            (game->getScreen()->width >> 3) - (sizeHeight >> 1));

                    static const float texCoords[8] = { 0.f, 0.f, 0.f, START_HEIGHT / FONT_TEXTURE_HEIGHT, START_WIDTH /
                            FONT_TEXTURE_WIDTH, START_HEIGHT / FONT_TEXTURE_HEIGHT, START_WIDTH / FONT_TEXTURE_WIDTH, 0.f };
                    mStart.setTexCoords(texCoords);
                }
                else {

                    if (yResumeLag == yResumeLagA) {

                        short xLag = (ICON_START_LAG * game->getScreen()->width / static_cast<short>(FONT_TEXTURE_WIDTH)) + sizeHeight;
                        xLag += static_cast<short>((5 / 100.f) * (sizeHeight >> 1)); // +Lag between icon & label (5% of height size)

                        mStart.setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) - yResumeLag -
                                           (game->getScreen()->width >> 3) + (sizeHeight >> 1), xLag + (START_NEW_WIDTH * sizeHeight / START_HEIGHT),
                                           (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) - yResumeLag -
                                           (game->getScreen()->width >> 3) - (sizeHeight >> 1));
                    }
                    else {

                        static short height = (resumeHeight << 3) / 10;
                        static short width = START_NEW_WIDTH * height / START_HEIGHT;
                        static short yIconLag = (((resumeHeight << 3) / 10) * 12) / 100;
                        static short xLag = (((((resumeHeight << 3) / 10) << 3) / 100) + ((resumeHeight << 3) / 10)) +
                                            (static_cast<short>((10 / 100.f) * resumeHeight));

                        mStart.setVertices((game->getScreen()->width >> 1) - resumeWidth + xLag, (game->getScreen()->height >> 1) -
                                           resumeHeight - yResumeLag - yIconLag, (game->getScreen()->width >> 1) - resumeWidth + xLag +
                                           width, (game->getScreen()->height >> 1) - resumeHeight - yResumeLag - yIconLag - height);
                    }
                    static const float texCoords[8] = { 0.f, 0.f, 0.f, START_HEIGHT / FONT_TEXTURE_HEIGHT, START_NEW_WIDTH /
                            FONT_TEXTURE_WIDTH, START_HEIGHT / FONT_TEXTURE_HEIGHT, START_NEW_WIDTH / FONT_TEXTURE_WIDTH, 0.f };
                    mStart.setTexCoords(texCoords);
                }
            }
            else
                mStart.resume(TEXTURE_ID_FONT);

            // Area(s)
            if (static_cast<const PuzzleCAM*>(game)->mCurGame) {

                if (yResumeLag == yResumeLagA) {

                    mResumeArea.left = 0;
                    mResumeArea.top = game->getScreen()->height >> 1;
                    mResumeArea.right = game->getScreen()->width;
                    mResumeArea.bottom = mResumeArea.top + (game->getScreen()->width >> 2);
                    
                    mStartArea.left = 0;
                    mStartArea.top = (game->getScreen()->height >> 1) + yResumeLag + (game->getScreen()->width >> 2);
                    mStartArea.right = game->getScreen()->width;
                    mStartArea.bottom = mStartArea.top + (game->getScreen()->width >> 2);
                    
                    mScoreArea.left = 0;
                    mScoreArea.top = (game->getScreen()->height >> 1) + (yResumeLag << 1) + (game->getScreen()->width >> 1);
                    mScoreArea.right = game->getScreen()->width;
                    mScoreArea.bottom = mScoreArea.top + (game->getScreen()->width >> 2);
                }
                else {

                    mResumeArea.left = (game->getScreen()->width >> 1) - resumeWidth;
                    mResumeArea.top = game->getScreen()->height >> 1;
                    mResumeArea.right = (game->getScreen()->width >> 1) + resumeWidth;
                    mResumeArea.bottom = mResumeArea.top + resumeHeight;

                    mStartArea.left = mResumeArea.left;
                    mStartArea.top = (game->getScreen()->height >> 1) + resumeHeight + yResumeLag;
                    mStartArea.right = mResumeArea.right;
                    mStartArea.bottom = mStartArea.top + resumeHeight;
                    
                    mScoreArea.left = mResumeArea.left;
                    mScoreArea.top = (game->getScreen()->height >> 1) + (resumeHeight << 1) + (yResumeLag << 1);
                    mScoreArea.right = mResumeArea.right;
                    mScoreArea.bottom = mScoreArea.top + resumeHeight;
                }
            }
            else {

                mStartArea.left = 0;
                mStartArea.top = (game->getScreen()->height >> 1) + yLag;
                mStartArea.right = game->getScreen()->width;
                mStartArea.bottom = mStartArea.top + (game->getScreen()->width >> 2);

                mScoreArea.left = 0;
                mScoreArea.top = (game->getScreen()->height >> 1) + (yLag << 1) + (game->getScreen()->width >> 2);
                mScoreArea.right = game->getScreen()->width;
                mScoreArea.bottom = mScoreArea.top + (game->getScreen()->width >> 2);
            }
            mLevelArea5.left = (game->getScreen()->width >> 1) - (game->getScreen()->width >> 3);
            mLevelArea5.top = (game->getScreen()->height >> 1) + (yLevelLag << 1) + (game->getScreen()->width >> 2);
            mLevelArea5.right = mLevelArea5.left + (game->getScreen()->width >> 2);
            mLevelArea5.bottom = mLevelArea5.top + (game->getScreen()->width >> 2);

            mExitArea.left = (game->getScreen()->width * 7) >> 3;
            mExitArea.top = (game->getScreen()->height >> 1) + (yLevelLag << 1) + ((game->getScreen()->width * 3) >> 3);
            mExitArea.right = game->getScreen()->width;
            mExitArea.bottom = mExitArea.top + (game->getScreen()->width >> 3);

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
        case 7: {

            if (!mScoreIcon.isStarted()) {

                mScoreIcon.initialize(game2DVia(game));
                mScoreIcon.start(TEXTURE_ID_FONT);

                if (!static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    static short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                                                ((SCORES_X2 - SCORES_X0) * sizeHeight / RESUME_HEIGHT))) >> 1);
                        
                    mScoreIcon.setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) -
                                            (yLag << 1) - (game->getScreen()->width >> 3) + (sizeHeight >> 1), xLag + sizeHeight,
                                            (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) - (yLag << 1) -
                                            (game->getScreen()->width >> 3) - (sizeHeight >> 1));
                }
                else if (yResumeLag == yResumeLagA) {

                    static short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                            ((SCORES_X2 - SCORES_X0) * sizeHeight / RESUME_HEIGHT))) >> 1);

                    mScoreIcon.setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 1) -
                            (yResumeLag << 1) - (game->getScreen()->width >> 3) + (sizeHeight >> 1), xLag + sizeHeight,
                            (game->getScreen()->height >> 1) - (game->getScreen()->width >> 1) - (yResumeLag << 1) -
                            (game->getScreen()->width >> 3) - (sizeHeight >> 1));
                }
                else {

                    static short iconSize = (resumeHeight << 3) / 10;
                    static short yIconLag = (iconSize * 12) / 100;
                    static short xIconLag = iconSize + (iconSize >> 2);

                    mScoreIcon.setVertices((game->getScreen()->width >> 1) - resumeWidth + xIconLag,
                                           (game->getScreen()->height >> 1) - (resumeHeight << 1) - (yResumeLag << 1) - yIconLag,
                                           (game->getScreen()->width >> 1) - resumeWidth + xIconLag + iconSize,
                                           (game->getScreen()->height >> 1) - (resumeHeight << 1) - (yResumeLag << 1) - yIconLag -
                                           iconSize);
                }
                mScoreIcon.setRed(PURPLE_COLOR_R / MAX_COLOR);
                mScoreIcon.setGreen(PURPLE_COLOR_G / MAX_COLOR);
                mScoreIcon.setBlue(PURPLE_COLOR_B / MAX_COLOR);

                static const float texCoords[8] = { ICON_SCORES_X0 / FONT_TEXTURE_WIDTH, ICON_SCORES_Y0 / FONT_TEXTURE_HEIGHT,
                        ICON_SCORES_X0 / FONT_TEXTURE_WIDTH, (ICON_SCORES_Y0 + ICON_SCORES_SIZE) / FONT_TEXTURE_HEIGHT,
                        (ICON_SCORES_X0 + ICON_SCORES_SIZE) / FONT_TEXTURE_WIDTH, (ICON_SCORES_Y0 + ICON_SCORES_SIZE) /
                        FONT_TEXTURE_HEIGHT, (ICON_SCORES_X0 + ICON_SCORES_SIZE) / FONT_TEXTURE_WIDTH, ICON_SCORES_Y0 /
                        FONT_TEXTURE_HEIGHT };
                mScoreIcon.setTexCoords(texCoords);
            }
            else
                mScoreIcon.resume(TEXTURE_ID_FONT);

            if (!mTitle.isStarted()) {

                mTitle.initialize(game2DVia(game));
                mTitle.start(TEXTURE_ID_TITLE);
                mTitle.setVertices(0, (game->getScreen()->height >> 1) + (game->getScreen()->height >> 2) +
                        (game->getScreen()->width >> 2), game->getScreen()->width, (game->getScreen()->height >> 1) +
                        (game->getScreen()->height >> 2) - (game->getScreen()->width >> 2));

                mTitle.setTexCoords(FULL_TEXCOORD_BUFFER);
            }
            else
                mTitle.resume(TEXTURE_ID_TITLE);

            if (!mLevel5.isStarted()) {

                mLevel5.initialize(game2DVia(game));
                mLevel5.start(TEXTURE_ID_LEVEL);
                mLevel5.setTexCoords(FULL_TEXCOORD_BUFFER);
                mLevel5.setAlpha(0.f);

                mLevel5.setRed(ORANGE_COLOR_R / MAX_COLOR);
                mLevel5.setGreen(ORANGE_COLOR_G / MAX_COLOR);
                mLevel5.setBlue(ORANGE_COLOR_B / MAX_COLOR);

                mLevel5.setVertices(mLevelArea5.left, game->getScreen()->height - mLevelArea5.top, mLevelArea5.right,
                        game->getScreen()->height - mLevelArea5.bottom);
            }
            else
                mLevel5.resume(TEXTURE_ID_LEVEL);

            if (!mExit.isStarted()) {

                mExit.initialize(game2DVia(game));
                mExit.start(TEXTURE_ID_FONT);
                mExit.setAlpha(0.f);

                mExit.setVertices(mExitArea.left, game->getScreen()->height - mExitArea.top, game->getScreen()->width,
                        game->getScreen()->height - mExitArea.bottom);

                static const float texCoords[8] = { (FONT_TEXTURE_WIDTH - EXIT_SIZE) / FONT_TEXTURE_WIDTH, 0.f,
                        (FONT_TEXTURE_WIDTH - EXIT_SIZE) / FONT_TEXTURE_WIDTH, EXIT_SIZE / FONT_TEXTURE_HEIGHT,
                        1.f, EXIT_SIZE / FONT_TEXTURE_HEIGHT, 1.f, 0.f };
                mExit.setTexCoords(texCoords);

                // Scores (position)
                float scale = (mLevelArea5.bottom - (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2)) /
                        (SCORES_FONT_HEIGHT * 3.f);
                float width = (((SCORES_FONT_WIDTH * 9) << 1) / static_cast<float>(game->getScreen()->width)) * scale;
                if (width > (2.f - (mExit.getRight() - mExit.getLeft()))) {

                    float newScale = (scale * (mExit.getLeft() + 1.f)) / width;
                    mScore3X3.scale(newScale, newScale);
                    mScore4X4.scale(newScale, newScale);
                    mScore5X5.scale(newScale, newScale);

                    float height = (SCORES_FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width);
                    float yPos = SCORES_LAG_Y - (((height * scale) - (height * newScale)) * 3.f);
                    mScore3X3.position(-1.f, yPos);

                    height *= newScale;
                    yPos -= height;
                    mScore4X4.position(-1.f, yPos);
                    mScore5X5.position(-1.f, yPos - height);
                }
                else {

                    mScore3X3.scale(scale, scale);
                    mScore4X4.scale(scale, scale);
                    mScore5X5.scale(scale, scale);

                    mScore3X3.position(-1.f, SCORES_LAG_Y);

                    float height = ((SCORES_FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) * scale;
                    float yPos = SCORES_LAG_Y - height;
                    mScore4X4.position(-1.f, yPos);
                    mScore5X5.position(-1.f, yPos - height);
                }
            }
            else
                mExit.resume(TEXTURE_ID_FONT);

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
        case MAX_LOAD_STEP: {

            if (!mScore.isStarted()) {

                mScore.initialize(game2DVia(game));
                mScore.start(TEXTURE_ID_FONT);

                if (!static_cast<const PuzzleCAM*>(game)->mCurGame) {

                    static short width = (SCORES_X2 - SCORES_X0) * sizeHeight / (SCORES_Y2 - SCORES_Y0);
                    static short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                            width)) >> 1) + sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight);

                    mScore.setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) -
                            (yLag << 1) - (game->getScreen()->width >> 3) + (sizeHeight >> 1), xLag + width,
                            (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2) - (yLag << 1) -
                            (game->getScreen()->width >> 3) - (sizeHeight >> 1));
                }
                else if (yResumeLag == yResumeLagA) {

                    static short width = (SCORES_X2 - SCORES_X0) * sizeHeight / (SCORES_Y2 - SCORES_Y0);
                    static short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                            width)) >> 1) + sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight);

                    mScore.setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 1) -
                            (yResumeLag << 1) - (game->getScreen()->width >> 3) + (sizeHeight >> 1), xLag + width,
                            (game->getScreen()->height >> 1) - (game->getScreen()->width >> 1) - (yResumeLag << 1) -
                            (game->getScreen()->width >> 3) - (sizeHeight >> 1));
                }
                else {

                    static short height = (resumeHeight << 3) / 10;
                    static short width = (SCORES_X2 - SCORES_X0) * height / (SCORES_Y2 - SCORES_Y0);
                    static short yIconLag = (((resumeHeight << 3) / 10) * 12) / 100;
                    static short xLag = (((resumeHeight << 3) / 10) << 1) + (((resumeHeight << 3) / 10) >> 2) +
                            (static_cast<short>((10 / 100.f) * resumeHeight));

                    mScore.setVertices((game->getScreen()->width >> 1) - resumeWidth + xLag, (game->getScreen()->height >> 1) -
                                       (resumeHeight << 1) - (yResumeLag << 1) - yIconLag, (game->getScreen()->width >> 1) -
                                       resumeWidth + xLag + width, (game->getScreen()->height >> 1) - (resumeHeight << 1) -
                                       (yResumeLag << 1) - yIconLag - height);
                }
                static const float texCoords[8] = { SCORES_X0 / FONT_TEXTURE_WIDTH, SCORES_Y0 / FONT_TEXTURE_HEIGHT,
                        SCORES_X0 / FONT_TEXTURE_WIDTH, SCORES_Y2 / FONT_TEXTURE_HEIGHT, SCORES_X2 / FONT_TEXTURE_WIDTH,
                        SCORES_Y2 / FONT_TEXTURE_HEIGHT, SCORES_X2 / FONT_TEXTURE_WIDTH, SCORES_Y0 / FONT_TEXTURE_HEIGHT };
                mScore.setTexCoords(texCoords);
            }
            else
                mScore.resume(TEXTURE_ID_FONT);

            if (!mBackground.isStarted()) {

                mBackground.initialize(game2DVia(game));
                mBackground.start(TEXTURE_ID_TITLE);

                static const float texCoords[8] = { 0.f, 0.f, 0.f, 1 / TITLE_TEXTURE_HEIGHT, 1 / TITLE_TEXTURE_WIDTH,
                        1 / TITLE_TEXTURE_HEIGHT, 1 / TITLE_TEXTURE_WIDTH, 0.f };
                mBackground.setTexCoords(texCoords);
            }
            else
                mBackground.resume(TEXTURE_ID_TITLE);

            if (!mLevelText5.isStarted()) {

                mLevelText5.initialize(game2DVia(game));
                mLevelText5.start(TEXTURE_ID_FONT);
                mLevelText5.setAlpha(0.f);
                mLevelText5.copyVertices(mLevel5);

                static const float texCoords[8] = { TEXT_5X5_X0 / FONT_TEXTURE_WIDTH, 0.f, TEXT_5X5_X0 / FONT_TEXTURE_WIDTH,
                        TEXT_LEVEL_SIZE / FONT_TEXTURE_HEIGHT, (TEXT_5X5_X0 + TEXT_LEVEL_SIZE) / FONT_TEXTURE_WIDTH,
                        TEXT_LEVEL_SIZE / FONT_TEXTURE_HEIGHT, (TEXT_5X5_X0 + TEXT_LEVEL_SIZE) / FONT_TEXTURE_WIDTH, 0.f };
                mLevelText5.setTexCoords(texCoords);
            }
            else
                mLevelText5.resume(TEXTURE_ID_FONT);

            Player::loadSound(SOUND_ID_TOUCH);

#ifndef DEMO_VERSION
            // Advertising
            adDisplay(false);
#endif
            break;
        }
#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown load step: %d"), __PRETTY_FUNCTION__, __LINE__, mLoadStep);
            assert(NULL);
            break;
        }
#endif
    }
    if (mLoadStep != MAX_LOAD_STEP)
        return false;

    delete mHideCam;
    mHideCam = NULL;
    return true;
}
void TitleLevel::renderLoad() const {

    assert(mHideCam);
    mHideCam->render(false);
}

#ifndef DEMO_VERSION
void TitleLevel::adDisplay(bool delay) {

    if (!mAdLoaded) {

        static unsigned char counter = 0;
        if ((!delay) || (++counter == (DISPLAY_DELAY << 2))) { // Avoid to call 'Advertising::getStatus' continually

            LOGI(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(" - Check to load/display advertising"), __PRETTY_FUNCTION__, __LINE__);
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

void TitleLevel::hideScore() {

    short resumeHeight = ((mGame->getScreen()->height >> 1) - ((mGame->getScreen()->height << 4) / 200)) / 3;
    short resumeWidth = PANEL_TEXTURE_WIDTH * (resumeHeight >> 1) / PANEL_TEXTURE_HEIGHT; // Half
    short yResumeLag = (mGame->getScreen()->height << 4) / 600;

    short yScoreLag = (mGame->getScreen()->height >> 1) - (resumeHeight << 1) - (yResumeLag << 1);
    mScorePanel.setVertices((mGame->getScreen()->width >> 1) - resumeWidth, yScoreLag, (mGame->getScreen()->width >> 1) +
                            resumeWidth, yScoreLag - resumeHeight);

    short iconSize = (resumeHeight << 3) / 10;
    short yIconLag = (iconSize * 12) / 100;
    short xIconLag = iconSize + (iconSize >> 2);
    mScoreIcon.setVertices((mGame->getScreen()->width >> 1) - resumeWidth + xIconLag,
                           (mGame->getScreen()->height >> 1) - (resumeHeight << 1) - (yResumeLag << 1) - yIconLag,
                           (mGame->getScreen()->width >> 1) - resumeWidth + xIconLag + iconSize,
                           (mGame->getScreen()->height >> 1) - (resumeHeight << 1) - (yResumeLag << 1) - yIconLag -
                           iconSize);

    short height = (resumeHeight << 3) / 10;
    short width = (SCORES_X2 - SCORES_X0) * height / (SCORES_Y2 - SCORES_Y0);
    yIconLag = (((resumeHeight << 3) / 10) * 12) / 100;
    short xLag = (((resumeHeight << 3) / 10) << 1) + (((resumeHeight << 3) / 10) >> 2) + (static_cast<short>((10 / 100.f) *
            resumeHeight));

    mScore.setVertices((mGame->getScreen()->width >> 1) - resumeWidth + xLag, (mGame->getScreen()->height >> 1) -
                       (resumeHeight << 1) - (yResumeLag << 1) - yIconLag, (mGame->getScreen()->width >> 1) -
                       resumeWidth + xLag + width, (mGame->getScreen()->height >> 1) - (resumeHeight << 1) -
                       (yResumeLag << 1) - yIconLag - height);

    // Code above is... :(
}

bool TitleLevel::update(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_TITLELEVEL, (*game->getLog() % 100), LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
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

    // Touch...
    unsigned char touchCount = game->mTouchCount;
    while (touchCount--) {

        if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
            continue;

        switch (mStep) {
            case STATUS_MENU: {

                // Resume
                if ((static_cast<const PuzzleCAM*>(game)->mCurGame) && (game->mTouchData[touchCount].X > mResumeArea.left) &&
                        (game->mTouchData[touchCount].X < mResumeArea.right) && (game->mTouchData[touchCount].Y > mResumeArea.top) &&
                        (game->mTouchData[touchCount].Y < mResumeArea.bottom)) {

                    mPlayer->play(0);
                    mCamera->stop();
                    return false;
                }
                // Start
                else if ((game->mTouchData[touchCount].X > mStartArea.left) && (game->mTouchData[touchCount].X < mStartArea.right) &&
                        (game->mTouchData[touchCount].Y > mStartArea.top) && (game->mTouchData[touchCount].Y < mStartArea.bottom)) {

                    mPlayer->play(0);
                    displayLevel();
                    return true;
                }
                // Scores
                else if ((game->mTouchData[touchCount].X > mScoreArea.left) && (game->mTouchData[touchCount].X < mScoreArea.right) &&
                        (game->mTouchData[touchCount].Y > mScoreArea.top) && (game->mTouchData[touchCount].Y < mScoreArea.bottom)) {

                    mPlayer->play(0);
                    displayScores();
                    if (mScoreLagY  < 0.f) {

                        short sizeHeight = (static_cast<short>((game->getScreen()->width >> 2) * (80 / 100.f)));
                        short xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                                                            ((SCORES_X2 - SCORES_X0) * sizeHeight / RESUME_HEIGHT))) >> 1);
                        
                        mScoreIcon.setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 3) + (sizeHeight >> 1), xLag + sizeHeight,
                                               (game->getScreen()->height >> 1) -
                                               (game->getScreen()->width >> 3) - (sizeHeight >> 1));

                        short width = (SCORES_X2 - SCORES_X0) * sizeHeight / (SCORES_Y2 - SCORES_Y0);
                        xLag = ((game->getScreen()->width - (sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight) +
                                                        width)) >> 1) + sizeHeight + static_cast<short>((5 / 100.f) * sizeHeight);

                        mScore.setVertices(xLag, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 3) + (sizeHeight >> 1),
                                           xLag + width, (game->getScreen()->height >> 1) - (game->getScreen()->width >> 3) -
                                           (sizeHeight >> 1));
                        mScorePanel.setVertices(0, (game->getScreen()->height >> 1), game->getScreen()->width,
                                                (game->getScreen()->height >> 1) - (game->getScreen()->width >> 2));
                    }
                    // Code above is... :(
                    return true;
                }
                break;
            }
            case STATUS_LEVEL: {

                // Level 3x3
                if ((game->mTouchData[touchCount].X > mLevelArea3.left) && (game->mTouchData[touchCount].X < mLevelArea3.right) &&
                        (game->mTouchData[touchCount].Y > mLevelArea3.top) && (game->mTouchData[touchCount].Y < mLevelArea3.bottom)) {

                    mPlayer->play(0);
                    static_cast<const PuzzleCAM*>(game)->mCurGame = GAME_3X3;
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_LEVEL)) = GAME_NONE;
                    mCamera->stop();
                    return false;
                }
                // Level 4x4
                else if ((game->mTouchData[touchCount].X > mLevelArea4.left) && (game->mTouchData[touchCount].X < mLevelArea4.right) &&
                        (game->mTouchData[touchCount].Y > mLevelArea4.top) && (game->mTouchData[touchCount].Y < mLevelArea4.bottom)) {

                    mPlayer->play(0);
                    static_cast<const PuzzleCAM*>(game)->mCurGame = GAME_4X4;
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_LEVEL)) = GAME_NONE;
                    mCamera->stop();
                    return false;
                }
                // Level 5x5
                else if ((game->mTouchData[touchCount].X > mLevelArea5.left) && (game->mTouchData[touchCount].X < mLevelArea5.right) &&
                        (game->mTouchData[touchCount].Y > mLevelArea5.top) && (game->mTouchData[touchCount].Y < mLevelArea5.bottom)) {

                    mPlayer->play(0);
                    static_cast<const PuzzleCAM*>(game)->mCurGame = GAME_5X5;
                    *static_cast<unsigned char*>(game->getData(DATA_CHAR, DATA_KEY_LEVEL)) = GAME_NONE;
                    mCamera->stop();
                    return false;
                }
                //break;
            }
            case STATUS_SCORES: {

                // Exit
                if ((game->mTouchData[touchCount].X > mExitArea.left) && (game->mTouchData[touchCount].X < mExitArea.right) &&
                        (game->mTouchData[touchCount].Y > mExitArea.top) && (game->mTouchData[touchCount].Y < mExitArea.bottom)) {

                    mPlayer->play(0);
                    if (mStep == STATUS_LEVEL)
                        hideLevel();
                    else {

                        hideScores();

                        // Code below is... :(
                        if (mScoreLagY < 0.f)
                            hideScore();
                    }
                    return true;
                }
                break;
            }
        }
    }
    return true;
}
void TitleLevel::renderUpdate() const {

    mBackground.render(false);
    mCAM.render(false);
    mTitle.render(false);

    if (mResumePanel) {

        mResumePanel->render(false);
        mResumeIcon->render(false);
        mResume->render(false);
    }
    mStartPanel.render(false);
    mStartIcon.render(false);
    mStart.render(false);

    mLevel3.render(false);
    mLevel4.render(false);
    mLevel5.render(false);

    mLevelText3.render(false);
    mLevelText4.render(false);
    mLevelText5.render(false);

    mExit.render(false);

    mScore3X3.render(true);
    mScore4X4.render(true);
    mScore5X5.render(true);

    mScorePanel.render(true);
    mScoreIcon.render(true);
    mScore.render(true);
}
