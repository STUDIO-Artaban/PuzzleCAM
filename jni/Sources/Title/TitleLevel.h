#ifndef TITLELEVEL_H_
#define TITLELEVEL_H_

#include "Global.h"

#include <libeng/Game/Level.h>
#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Graphic/Text/2D/Text2D.h>
#include <libeng/Features/Camera/Camera.h>
#include <libeng/Advertising/Advertising.h>

using namespace eng;

//////
class TitleLevel : public Level {

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
    Static2D mTitle;
    Static2D mCAM;

    Static2D* mResumePanel;
    Static2D* mResumeIcon;
    Static2D* mResume;
    TouchArea mResumeArea;

    Static2D mStartPanel;
    Static2D mStartIcon;
    Static2D mStart;
    TouchArea mStartArea;

    enum {

        STATUS_MENU = 0,
        STATUS_LEVEL,
        STATUS_SCORES
    };
    unsigned char mStep;

    Static2D mLevel3;
    Static2D mLevel4;
    Static2D mLevel5;

    Static2D mLevelText3;
    Static2D mLevelText4;
    Static2D mLevelText5;

    TouchArea mLevelArea3;
    TouchArea mLevelArea4;
    TouchArea mLevelArea5;

    inline void displayLevel() {

        LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mLevel3.setAlpha(1.f);
        mLevel4.setAlpha(1.f);
        mLevel5.setAlpha(1.f);

        mLevelText3.setAlpha(1.f);
        mLevelText4.setAlpha(1.f);
        mLevelText5.setAlpha(1.f);

        mExit.setAlpha(1.f);

        if (mResumePanel) {

            mResumePanel->setAlpha(0.f);
            mResumeIcon->setAlpha(0.f);
            mResume->setAlpha(0.f);
        }
        mStartPanel.setAlpha(0.f);
        mStartIcon.setAlpha(0.f);
        mStart.setAlpha(0.f);

        mScorePanel.setAlpha(0.f);
        mScoreIcon.setAlpha(0.f);
        mScore.setAlpha(0.f);

        mStep = STATUS_LEVEL;
    };
    inline void hideLevel() {

        LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mLevel3.setAlpha(0.f);
        mLevel4.setAlpha(0.f);
        mLevel5.setAlpha(0.f);

        mLevelText3.setAlpha(0.f);
        mLevelText4.setAlpha(0.f);
        mLevelText5.setAlpha(0.f);

        mExit.setAlpha(0.f);

        if (mResumePanel) {

            mResumePanel->setAlpha(1.f);
            mResumeIcon->setAlpha(1.f);
            mResume->setAlpha(1.f);
        }
        mStartPanel.setAlpha(1.f);
        mStartIcon.setAlpha(1.f);
        mStart.setAlpha(1.f);

        mScorePanel.setAlpha(1.f);
        mScoreIcon.setAlpha(1.f);
        mScore.setAlpha(1.f);

        mStep = STATUS_MENU;
    };


    Element2D mScorePanel;
    Element2D mScoreIcon;
    Element2D mScore;
    TouchArea mScoreArea;

    float mScoreLagY;

    Text2D mScore3X3;
    Text2D mScore4X4;
    Text2D mScore5X5;

    inline void displayScores() {

        LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        if (mResumePanel) {

            mResumePanel->setAlpha(0.f);
            mResumeIcon->setAlpha(0.f);
            mResume->setAlpha(0.f);
        }
        mStartPanel.setAlpha(0.f);
        mStartIcon.setAlpha(0.f);
        mStart.setAlpha(0.f);

        if (mScoreLagY > 0.f) {

            mScorePanel.translate(0.f, mScoreLagY);
            mScoreIcon.translate(0.f, mScoreLagY);
            mScore.translate(0.f, mScoreLagY);
        }
        mScore3X3.setAlpha(1.f);
        mScore4X4.setAlpha(1.f);
        mScore5X5.setAlpha(1.f);

        mExit.setAlpha(1.f);

        mStep = STATUS_SCORES;
    };
    inline void hideScores() {

        LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        if (mResumePanel) {

            mResumePanel->setAlpha(1.f);
            mResumeIcon->setAlpha(1.f);
            mResume->setAlpha(1.f);
        }
        mStartPanel.setAlpha(1.f);
        mStartIcon.setAlpha(1.f);
        mStart.setAlpha(1.f);

        if (mScoreLagY > 0.f) {

            mScorePanel.translate(0.f, -mScoreLagY);
            mScoreIcon.translate(0.f, -mScoreLagY);
            mScore.translate(0.f, -mScoreLagY);
        }
        mScore3X3.setAlpha(0.f);
        mScore4X4.setAlpha(0.f);
        mScore5X5.setAlpha(0.f);

        mExit.setAlpha(0.f);

        mStep = STATUS_MENU;
    };
    
    void hideScore();
    const Game* mGame;
    // Code above is... :(

    Static2D mExit;
    TouchArea mExitArea;

public:
    TitleLevel(Game* game);
    virtual ~TitleLevel();

    //////
    void init() { }
    void pause();

    void wait(float millis) { }

protected:
    bool loaded(const Game* game);
public:
    bool update(const Game* game);

    void renderLoad() const;
    void renderUpdate() const;

};

#endif // TITLELEVEL_H_
