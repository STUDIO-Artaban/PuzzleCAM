#include "PuzzleCAM.h"
#include "Global.h"

#include <libeng/Log/Log.h>
#include <libeng/Storage/Storage.h>
#include <libeng/Features/Internet/Internet.h>
#include <libeng/Advertising/Advertising.h>

#include <boost/thread.hpp>

#define DEFAULT_SCORE       L"99'99"

//////
PuzzleCAM::PuzzleCAM() : Game2D(0), mCurGame(GAME_NONE) {

    LOGV(LOG_LEVEL_PUZZLECAM, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (!mGameLevel)
        mGameIntro = new Intro(Intro::LANG_EN, true);

    mGameData = new StoreData;

    mFonts->addFont(0, SCORES_FONT_WIDTH, SCORES_FONT_HEIGHT, static_cast<short>(FONT_TEXTURE_WIDTH),
            static_cast<short>(FONT_TEXTURE_HEIGHT));
    Dynamic2D::initIncreaseBuffer();

    mTitle = NULL;
    mGame = NULL;
#ifndef DEMO_VERSION
    mWaitNet = NULL;
#endif
}
PuzzleCAM::~PuzzleCAM() {

    LOGV(LOG_LEVEL_PUZZLECAM, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Dynamic2D::freeIncreaseBuffer();
}

bool PuzzleCAM::start() {

    LOGV(LOG_LEVEL_PUZZLECAM, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (Game2D::start()) {

        if (Storage::loadFile(FILE_NAME)) {

            Storage* store = Storage::getInstance();
            if (!store->isEmpty(FILE_NAME)) {
                if (!Storage::File2Data(mGameData, (*store)[FILE_NAME])) {

                    LOGW(LOG_FORMAT(" - Failed to get data from '%s' file"), __PRETTY_FUNCTION__, __LINE__, FILE_NAME);
                    assert(NULL);
                }
            }
            else { // No data stored: Create data

                mGameData->add(DATA_STRING, DATA_KEY_3X3_SCORE, static_cast<void*>(new std::wstring(DEFAULT_SCORE)));
                mGameData->add(DATA_STRING, DATA_KEY_4X4_SCORE, static_cast<void*>(new std::wstring(DEFAULT_SCORE)));
                mGameData->add(DATA_STRING, DATA_KEY_5X5_SCORE, static_cast<void*>(new std::wstring(DEFAULT_SCORE)));

                mGameData->add(DATA_CHAR, DATA_KEY_LEVEL, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_SCORE_MIN, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_SCORE_SEC, static_cast<void*>(new unsigned char(0)));

                mGameData->add(DATA_CHAR, DATA_KEY_ROW1_COL1, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW1_COL2, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW1_COL3, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW1_COL4, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW1_COL5, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW2_COL1, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW2_COL2, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW2_COL3, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW2_COL4, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW2_COL5, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW3_COL1, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW3_COL2, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW3_COL3, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW3_COL4, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW3_COL5, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW4_COL1, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW4_COL2, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW4_COL3, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW4_COL4, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW4_COL5, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW5_COL1, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW5_COL2, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW5_COL3, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW5_COL4, static_cast<void*>(new unsigned char(0)));
                mGameData->add(DATA_CHAR, DATA_KEY_ROW5_COL5, static_cast<void*>(new unsigned char(0)));
            }
        }
#ifdef DEBUG
        else {

            LOGW(LOG_FORMAT(" - Failed to open existing '%s' file"), __PRETTY_FUNCTION__, __LINE__, FILE_NAME);
            assert(NULL);
        }
#endif
        return true;
    }
    else if (mGameLevel) {

        if (!(mGameLevel & 0x01))
            --mGameLevel;

#ifndef DEMO_VERSION
        // Check if still online
#ifdef __ANDROID__
        if (!Internet::getInstance()->isOnline(2000)) {
#else
        if (!Internet::getInstance()->isOnline()) {
#endif
            assert(!mWaitNet);
            mGameLevel = 0;

            mWaitNet = new WaitConn(true);
            mWaitNet->initialize(game2DVia(this));
            mWaitNet->start(mScreen);
        }
    }
    else if (mWaitNet) {

        // Check if still not online
#ifdef __ANDROID__
        if (!Internet::getInstance()->isOnline(2000))
#else
        if (!Internet::getInstance()->isOnline())
#endif
            mWaitNet->resume();

        else {

            delete mWaitNet;
            mWaitNet = NULL;

            ++mGameLevel;

            if (mTitle) delete mTitle;
            if (mGame) delete mGame;

            mTitle = NULL;
            mGame = NULL;
        }
#endif // DEMO_VERSION
    }
    return false;
}
void PuzzleCAM::pause() {

    LOGV(LOG_LEVEL_PUZZLECAM, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (mGameLevel == 4) {

        mGame->pause(this);
        delete mGame;
        mGame = NULL;

        // Back to title (pause)
        mGameLevel = 1;
    }
    Game2D::pause();

#ifndef DEMO_VERSION
    if (Advertising::getStatus() == Advertising::STATUS_DISPLAYED)
        Advertising::hide(0);
#endif

    if (mTitle) mTitle->pause();
    if (mGame) mGame->pause(this);
#ifndef DEMO_VERSION
    if (mWaitNet) mWaitNet->pause();
#endif
}
void PuzzleCAM::destroy() {

    LOGV(LOG_LEVEL_PUZZLECAM, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (mGame) mGame->save(this);

    Game2D::destroy();

    if (mTitle) delete mTitle;
    if (mGame) delete mGame;
#ifndef DEMO_VERSION
    if (mWaitNet) delete mWaitNet;
#endif
}

void PuzzleCAM::wait(float millis) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_PUZZLECAM, (mLog % 100), LOG_FORMAT(" - Delay: %f milliseconds"), __PRETTY_FUNCTION__, __LINE__, millis);
#endif
    boost::this_thread::sleep(boost::posix_time::microseconds(static_cast<unsigned long>(millis * 500)));
}
void PuzzleCAM::update() {

#ifdef DEBUG
    LOGV(LOG_LEVEL_PUZZLECAM, (mLog % 100), LOG_FORMAT(" - g:%d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel);
#endif
    switch (mGameLevel) {

        case 0: {

#ifndef DEMO_VERSION
            if (!mWaitNet)
                updateIntro(); // Introduction
            
            else { // Wait Internet
                
                static unsigned char wait = 0;
                if (wait < (LIBENG_WAITCONN_DELAY << 2)) {
                    
                    ++wait;
                    break;
                }
                mWaitNet->update();
            }
#else
            updateIntro(); // Introduction
#endif
            break;
        }

        ////// Title
        case 1: {

            if (!mTitle) {

                mCurGame = *static_cast<const unsigned char*>(mGameData->get(DATA_CHAR, DATA_KEY_LEVEL));

                mTitle = new TitleLevel(this);
                mTitle->initialize();
            }
            if (!mTitle->load(this))
                break;

            ++mGameLevel;
            //break;
        }
        case 2: {

            if (mTitle->update(this))
                break;

            delete mTitle;
            mTitle = NULL;

            ++mGameLevel;
            //break;
        }

        ////// Game
        case 3: {

            if (!mGame) {

                mGame = new GameLevel(this);
                mGame->initialize();
            }
            if (!mGame->load(this))
                break;

            ++mGameLevel;
            //break;
        }
        case 4: {

            if (!mGame->update(this)) {

                delete mGame;
                mGame = NULL;

                mGameLevel = 1;
            }
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown game level to update: %d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel);
            assert(NULL);
            break;
        }
#endif
    }
}

void PuzzleCAM::render() const {
    switch (mGameLevel) {

        case 0: {

#ifndef DEMO_VERSION
            if (!mWaitNet)
                renderIntro(); // Introduction
            else
                mWaitNet->render(); // Wait Internet
#else
            renderIntro(); // Introduction
#endif
            break;
        }
        case 1: if (mTitle) mTitle->renderLoad(); break;
        case 2: mTitle->renderUpdate(); break;
        case 3: if (mGame) mGame->renderLoad(); break;
        case 4: mGame->renderUpdate(); break;

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown game level to render: %d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel);
            assert(NULL);
            break;
        }
#endif
    }
}
