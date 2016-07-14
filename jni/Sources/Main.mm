#include "Main.h"

////// Languages
typedef enum {

    LANG_EN = 0,
    LANG_FR,
    LANG_DE,
    LANG_ES,
    LANG_IT,
    LANG_PT

} Language;
static const Language g_MainLang = LANG_EN;

////// Textures
#define NO_TEXTURE_LOADED           0xFF

BOOL engGetFontGrayscale() { return YES; }

#define TEXTURE_ID_TITLE            2 // TEXTURE_ID_FONT + 1
#define TEXTURE_ID_PANEL            3
#define TEXTURE_ID_LEVEL            4
#define TEXTURE_ID_CHECK            5

unsigned char engLoadTexture(EngResources* resources, unsigned char Id) {
    switch (Id) {

        case TEXTURE_ID_TITLE: {

            unsigned char* data = [resources getBufferPNG:@"title" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_TITLE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_PANEL: {

            unsigned char* data = [resources getBufferPNG:@"panel" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_PANEL, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_LEVEL: {

            unsigned char* data = [resources getBufferPNG:@"level" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_LEVEL, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_CHECK: {

            unsigned char* data = [resources getBufferPNG:@"check" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_CHECK, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }

        default: {

            NSLog(@"ERROR: Failed to load PNG ID %d", Id);
            break;
        }
    }
    return NO_TEXTURE_LOADED;
}

////// Sounds
#define SOUND_ID_TOUCH           1 // SOUND_ID_LOGO + 1
#define SOUND_ID_SLIDE           2
#define SOUND_ID_CONGRATULATION  3
#define SOUND_ID_GAMEOVER        4

void engLoadSound(EngResources* resources, unsigned char Id) {
    switch (Id) {

        case SOUND_ID_TOUCH: {

            unsigned char* data = [resources getBufferOGG:@"touch"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_TOUCH, resources.oggLength, data, false);
            break;
        }
        case SOUND_ID_SLIDE: {

            unsigned char* data = [resources getBufferOGG:@"slide"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_SLIDE, resources.oggLength, data, false);
            break;
        }
        case SOUND_ID_CONGRATULATION: {

            unsigned char* data = [resources getBufferOGG:@"winner"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_CONGRATULATION, resources.oggLength, data, true);
            break;
        }
        case SOUND_ID_GAMEOVER: {

            unsigned char* data = [resources getBufferOGG:@"looser"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_GAMEOVER, resources.oggLength, data, true);
            break;
        }

        default: {

            NSLog(@"ERROR: Failed to load OGG ID %d", Id);
            break;
        }
    }
}

////// Advertising
#ifdef LIBENG_ENABLE_ADVERTISING

static NSString* ADV_UNIT_ID = @"ca-app-pub-1474300545363558/9999340626";
#ifdef DEBUG
static const NSString* IPAD_DEVICE_UID = @"655799b1c803de3417cbb36833b6c40c";
static const NSString* IPHONE_YACIN_UID = @"10053bb6983c6568b88812fbcfd7ab89";
#endif

BOOL engGetAdType() { return FALSE; } // TRUE: Interstitial; FALSE: Banner
void engLoadAd(EngAdvertising* ad, GADRequest* request) {

    static bool init = false;
    if (!init) {
        if ([[UIScreen mainScreen] bounds].size.width > 468)
            [ad getBanner].adSize = kGADAdSizeFullBanner;
        else
            [ad getBanner].adSize = kGADAdSizeBanner;
        [ad getBanner].adUnitID = ADV_UNIT_ID;
        init = true;
    }
#ifdef DEBUG
    request.testDevices = [NSArray arrayWithObjects: GAD_SIMULATOR_ID, IPAD_DEVICE_UID, IPHONE_YACIN_UID, nil];
#endif
    [[ad getBanner] loadRequest:request];
}
void engDisplayAd(EngAdvertising* ad, unsigned char Id) {

    CGFloat xPos = ([[UIScreen mainScreen] bounds].size.width - [ad getBanner].frame.size.width) / 2.0;
    [[ad getBanner] setHidden:NO];
    [ad getBanner].frame =  CGRectMake(xPos, -[ad getBanner].frame.size.height,
                                       [ad getBanner].frame.size.width, [ad getBanner].frame.size.height);
    [UIView animateWithDuration:1.5 animations:^{
        [ad getBanner].frame =  CGRectMake(xPos, 0, [ad getBanner].frame.size.width,
                                           [ad getBanner].frame.size.height);
    } completion:^(BOOL finished) {
        if (finished)
            ad.status = static_cast<unsigned char>(Advertising::STATUS_DISPLAYED);
    }];
}
void engHideAd(EngAdvertising* ad, unsigned char Id) { [[ad getBanner] setHidden:YES]; }
#endif

////// Social
#ifdef LIBENG_ENABLE_SOCIAL
BOOL engReqInfoField(SocialField field, unsigned char socialID) { return NO; }
#endif
