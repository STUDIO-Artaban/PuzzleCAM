#ifndef CHECK_H_
#define CHECK_H_

#include "Global.h"

#include <libeng/Game/Game.h>
#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Tools/Tools.h>

#define MAX_SLIDE_STEP      8

#define RATIO_3X3           (2.f / 3.f)
#define RATIO_4X4           (2.f / 4.f)
#define RATIO_5X5           (2.f / 5.f)

#define ROTATE_VELOCITY     (eng::PI_F / 200.f)

static const float g_CheckPosition3X3[3 * 3][2] = {
        { -RATIO_3X3, RATIO_3X3 },  { 0.f, RATIO_3X3 },  { RATIO_3X3, RATIO_3X3 },
        { -RATIO_3X3, 0.f },        { 0.f, 0.f },        { RATIO_3X3, 0.f },
        { -RATIO_3X3, -RATIO_3X3 }, { 0.f, -RATIO_3X3 }, { RATIO_3X3, -RATIO_3X3 } };
static const float g_CheckPosition4X4[4 * 4][2] = {
        { -(RATIO_4X4 / 2.f) - RATIO_4X4, (RATIO_4X4 / 2.f) + RATIO_4X4 },
            { -(RATIO_4X4 / 2.f), (RATIO_4X4 / 2.f) + RATIO_4X4 },
                { (RATIO_4X4 / 2.f), (RATIO_4X4 / 2.f) + RATIO_4X4 },
                    { (RATIO_4X4 / 2.f) + RATIO_4X4, (RATIO_4X4 / 2.f) + RATIO_4X4 },
        { -(RATIO_4X4 / 2.f) - RATIO_4X4, (RATIO_4X4 / 2.f) },
            { -(RATIO_4X4 / 2.f), (RATIO_4X4 / 2.f) },
                { (RATIO_4X4 / 2.f), (RATIO_4X4 / 2.f) },
                    { (RATIO_4X4 / 2.f) + RATIO_4X4, (RATIO_4X4 / 2.f) },
        { -(RATIO_4X4 / 2.f) - RATIO_4X4, -(RATIO_4X4 / 2.f) },
            { -(RATIO_4X4 / 2.f), -(RATIO_4X4 / 2.f) },
                { (RATIO_4X4 / 2.f), -(RATIO_4X4 / 2.f) },
                    { (RATIO_4X4 / 2.f) + RATIO_4X4, -(RATIO_4X4 / 2.f) },
        { -(RATIO_4X4 / 2.f) - RATIO_4X4, -(RATIO_4X4 / 2.f) - RATIO_4X4 },
            { -(RATIO_4X4 / 2.f), -(RATIO_4X4 / 2.f) - RATIO_4X4 },
                { (RATIO_4X4 / 2.f), -(RATIO_4X4 / 2.f) - RATIO_4X4 },
                    { (RATIO_4X4 / 2.f) + RATIO_4X4, -(RATIO_4X4 / 2.f) - RATIO_4X4 } };
static const float g_CheckPosition5X5[5 * 5][2] = {
        { -(RATIO_5X5 * 2.f), (RATIO_5X5 * 2.f) },
            { -RATIO_5X5, (RATIO_5X5 * 2.f) },
                { 0.f, (RATIO_5X5 * 2.f) },
                    { RATIO_5X5, (RATIO_5X5 * 2.f) },
                        { (RATIO_5X5 * 2.f), (RATIO_5X5 * 2.f) },
        { -(RATIO_5X5 * 2.f), RATIO_5X5 },
            { -RATIO_5X5, RATIO_5X5 },
                { 0.f, RATIO_5X5 },
                    { RATIO_5X5, RATIO_5X5 },
                        { (RATIO_5X5 * 2.f), RATIO_5X5 },
        { -(RATIO_5X5 * 2.f), 0.f },
            { -RATIO_5X5, 0.f },
                { 0.f, 0.f },
                    { RATIO_5X5, 0.f },
                        { (RATIO_5X5 * 2.f), 0.f },
        { -(RATIO_5X5 * 2.f), -RATIO_5X5 },
            { -RATIO_5X5, -RATIO_5X5 },
                { 0.f, -RATIO_5X5 },
                    { RATIO_5X5, -RATIO_5X5 },
                        { (RATIO_5X5 * 2.f), -RATIO_5X5 },
        { -(RATIO_5X5 * 2.f), -(RATIO_5X5 * 2.f) },
            { -RATIO_5X5, -(RATIO_5X5 * 2.f) },
                { 0.f, -(RATIO_5X5 * 2.f) },
                    { RATIO_5X5, -(RATIO_5X5 * 2.f) },
                        { (RATIO_5X5 * 2.f), -(RATIO_5X5 * 2.f) } };

using namespace eng;

//////
class Check {

private:
    Element2D mCheck;
    Element2D mCAM;

    unsigned char mSlideStep;

public:
    Check();
    virtual ~Check();

    inline unsigned char getSlideStep() const { return mSlideStep; }
    inline void setSlideStep() { mSlideStep = 0; }

    inline bool slide(bool slide, unsigned char level, unsigned char curpos, unsigned char newpos) {

        // Check already slided/unslided
        if ((!slide) && (!mSlideStep))
            return true;
        if ((slide) && (mSlideStep == MAX_SLIDE_STEP))
            return true;

        float transX, transY;
        switch (level) {
            case GAME_3X3: {

                transX = (g_CheckPosition3X3[newpos][0] - g_CheckPosition3X3[curpos][0]) / MAX_SLIDE_STEP;
                transY = (g_CheckPosition3X3[newpos][1] - g_CheckPosition3X3[curpos][1]) / MAX_SLIDE_STEP;
                break;
            }
            case GAME_4X4: {

                transX = (g_CheckPosition4X4[newpos][0] - g_CheckPosition4X4[curpos][0]) / MAX_SLIDE_STEP;
                transY = (g_CheckPosition4X4[newpos][1] - g_CheckPosition4X4[curpos][1]) / MAX_SLIDE_STEP;
                break;
            }
            case GAME_5X5: {

                transX = (g_CheckPosition5X5[newpos][0] - g_CheckPosition5X5[curpos][0]) / MAX_SLIDE_STEP;
                transY = (g_CheckPosition5X5[newpos][1] - g_CheckPosition5X5[curpos][1]) / MAX_SLIDE_STEP;
                break;
            }
        }
        mCheck.translate(transX, transY);
        mCAM.translate(transX, transY);

        return (slide)? (++mSlideStep == MAX_SLIDE_STEP):!static_cast<bool>(--mSlideStep);
    };

    inline void rotate(bool clockwise) {

        LOGV(LOG_LEVEL_CHECK, 0, LOG_FORMAT(" - r:%s"), __PRETTY_FUNCTION__, __LINE__, (clockwise)? "true":"false");
        float angle = mCheck.getTransform()[Dynamic2D::ROTATE_C] + ((clockwise)? ROTATE_VELOCITY:-ROTATE_VELOCITY);

        mCheck.rotate(angle);
        mCAM.rotate(angle);
    };

    //////
    void start(const Game* game, unsigned char position, unsigned char level, unsigned char check);
    void render() const;

};

#endif // CHECK_H_
