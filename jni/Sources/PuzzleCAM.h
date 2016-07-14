#ifndef PUZZLECAM_H_
#define PUZZLECAM_H_

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Intro/Intro.h>

#ifdef __ANDROID__
#include "Title/TitleLevel.h"
#include "Game/GameLevel.h"
#else
#include "TitleLevel.h"
#include "GameLevel.h"
#endif

#define FILE_NAME           "PuzzleCAM.backup"

#define CAM_WIDTH                   640
#define CAM_HEIGHT                  480

#define CAM_TEXTURE_WIDTH           1024.f // OpenGL camera texture width (64 texels)
#define CAM_TEXTURE_HEIGHT          512.f // OpenGL camera texture height (64 texels)

// Font size
#define SCORES_FONT_WIDTH           47
#define SCORES_FONT_HEIGHT          83

// Sound IDs
#define SOUND_ID_TOUCH              1

// Texture IDs
#define TEXTURE_ID_LEVEL        4

using namespace eng;

//////
class PuzzleCAM : public Game2D {

private:
    PuzzleCAM();
    virtual ~PuzzleCAM();

    TitleLevel* mTitle;
    GameLevel* mGame;
#ifndef DEMO_VERSION
    WaitConn* mWaitNet;
#endif

public:
    static PuzzleCAM* getInstance() {
        if (!mThis)
            mThis = new PuzzleCAM;
        return static_cast<PuzzleCAM*>(mThis);
    }
    static void freeInstance() {
        if (mThis) {
            delete mThis;
            mThis = NULL;
        }
    }

    mutable unsigned char mCurGame; // Current level

    //////
    void init() { }

    bool start();
    void pause();
    void destroy();

    void wait(float millis);
    void update();

    void render() const;

};

#endif // PUZZLECAM_H_
