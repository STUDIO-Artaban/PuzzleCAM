package com.studio.artaban.puzzlecam.data;

import com.studio.artaban.puzzlecam.EngActivity;
import com.studio.artaban.puzzlecam.EngAdvertising;
import com.studio.artaban.puzzlecam.EngLibrary;
import com.studio.artaban.puzzlecam.EngResources;

import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.AnimationUtils;
import android.widget.RelativeLayout;

import com.studio.artaban.puzzlecam.R;

public class EngData {

    ////// JNI
    static public final String PROJECT_NAME_LIB = "PuzzleCAM";

    ////// Languages
    static private final short LANG_EN = 0;
    @SuppressWarnings("unused") static private final short LANG_FR = 1;
    @SuppressWarnings("unused") static private final short LANG_DE = 2;
    @SuppressWarnings("unused") static private final short LANG_ES = 3;
    @SuppressWarnings("unused") static private final short LANG_IT = 4;
    @SuppressWarnings("unused") static private final short LANG_PT = 5;

    @SuppressWarnings("unused") static private final short mLanguage = LANG_EN;

    ////// Permissions
    static public final boolean USES_PERMISSION_CAMERA = true; // android.permission.CAMERA
    static public final boolean USES_PERMISSION_MIC = false; // android.permission.RECORD_AUDIO
    static public final boolean USES_PERMISSION_INTERNET = true; // android.permission.INTERNET & ACCESS_NETWORK_STATE
    static public final boolean USES_PERMISSION_STORAGE = false; // android.permission.WRITE_INTERNAL_STORAGE & WRITE_EXTERNAL_STORAGE
    static public final boolean USES_PERMISSION_BLUETOOTH = false; // android.permission.BLUETOOTH & BLUETOOTH_ADMIN

    ////// Font
    static public final boolean FONT_TEXTURE_GRAYSCALE = true;

    ////// Textures
    static private final short TEXTURE_ID_TITLE = 2; // EngActivity.TEXTURE_ID_FONT + 1
    static private final short TEXTURE_ID_PANEL = 3;
    static private final short TEXTURE_ID_LEVEL = 4;
    static private final short TEXTURE_ID_CHECK = 5;

    static public short loadTexture(EngResources resources, short id) {

        switch (id) {
            case TEXTURE_ID_TITLE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("title.png");
                return EngLibrary.loadTexture(TEXTURE_ID_TITLE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_PANEL: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("panel.png");
                return EngLibrary.loadTexture(TEXTURE_ID_PANEL, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_LEVEL: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("level.png");
                return EngLibrary.loadTexture(TEXTURE_ID_LEVEL, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_CHECK: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("check.png");
                return EngLibrary.loadTexture(TEXTURE_ID_CHECK, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }

            default: {
                Log.e("EngData", "Failed to load PNG ID: " + Integer.toString(id));
                return EngActivity.NO_TEXTURE_LOADED;
            }
        }
    }

    ////// Sounds
    static private final short SOUND_ID_TOUCH = 1; // EngActivity.SOUND_ID_LOGO + 1
    static private final short SOUND_ID_SLIDE = 2;
    static private final short SOUND_ID_CONGRATULATION = 3;
    static private final short SOUND_ID_GAMEOVER = 4;

    static public void loadOgg(EngResources resources, short id) {

        switch (id) {
            case SOUND_ID_TOUCH: {
                EngLibrary.loadOgg(SOUND_ID_TOUCH, resources.getBufferOGG("touch.ogg"), false);
                break;
            }
            case SOUND_ID_SLIDE: {
                EngLibrary.loadOgg(SOUND_ID_SLIDE, resources.getBufferOGG("slide.ogg"), false);
                break;
            }
            case SOUND_ID_CONGRATULATION: {
                EngLibrary.loadOgg(SOUND_ID_CONGRATULATION, resources.getBufferOGG("winner.ogg"), true);
                break;
            }
            case SOUND_ID_GAMEOVER: {
                EngLibrary.loadOgg(SOUND_ID_GAMEOVER, resources.getBufferOGG("looser.ogg"), true);
                break;
            }

            default: {
                Log.e("EngData", "Failed to load OGG ID: " + Integer.toString(id));
                break;
            }
        }
    }

    ////// Advertising
    static public final boolean INTERSTITIAL_AD = false; // TRUE: InterstitialAd; FALSE: AdView
    static private final boolean TEST_ADVERTISING = false; // Set to 'false' in release mode
    static private final String ADV_UNIT_ID = "ca-app-pub-1474300545363558/1164987428";

    static private class AdAnimListener implements AnimationListener {

		public void onAnimationEnd(Animation animation) { EngAdvertising.mStatus = EngAdvertising.AD_STATUS_DISPLAYED; }
		public void onAnimationRepeat(Animation animation) { }
		public void onAnimationStart(Animation animation) { }
    };

    static public void loadAd(EngActivity activity) {

    	if (!USES_PERMISSION_INTERNET)
    		Log.e("EngData", "Missing INTERNET & ACCESS_NETWORK_STATE permissions");

    	//
    	DisplayMetrics metrics = new DisplayMetrics();
    	activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
        if (((activity.getWindowManager().getDefaultDisplay().getWidth() * 160) / metrics.xdpi) > 468)
        	EngAdvertising.setBanner(EngAdvertising.BANNER_ID_FULL_BANNER);
        else
    		EngAdvertising.setBanner(EngAdvertising.BANNER_ID_BANNER);

		EngAdvertising.setUnitID(ADV_UNIT_ID);
		if (TEST_ADVERTISING)
			EngAdvertising.load(activity.getContentResolver());
		else
			EngAdvertising.load(null);
    }
    static public void displayAd(short id, EngActivity activity) {

		if (activity.mSurfaceLayout.getChildCount() == 2)
			activity.mSurfaceLayout.removeView(EngAdvertising.getView());
		EngAdvertising.getView().setVisibility(View.VISIBLE);

        RelativeLayout.LayoutParams adParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT,
        																	RelativeLayout.LayoutParams.WRAP_CONTENT);
        adParams.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        adParams.addRule(RelativeLayout.CENTER_HORIZONTAL);

        activity.mSurfaceLayout.addView(EngAdvertising.getView(), adParams);

        Animation animAds = (Animation)AnimationUtils.loadAnimation(activity, R.anim.disp_ad);
        animAds.setAnimationListener(new AdAnimListener());
        EngAdvertising.getView().startAnimation(animAds);

        EngAdvertising.mStatus = EngAdvertising.AD_STATUS_DISPLAYING;
    }
    static public void hideAd(short id, EngActivity activity) {

		EngAdvertising.getView().clearAnimation();

        EngAdvertising.mStatus = EngAdvertising.AD_STATUS_LOADED;
		EngAdvertising.getView().setVisibility(View.INVISIBLE);
		// -> Always set invisible when hidden (see 'onAdLoaded' method in 'EngAdvertising')
    }

    ////// Social
    static public final boolean INFO_FIELD_FACEBOOK_BIRTHDAY = true;
    static public final boolean INFO_FIELD_FACEBOOK_LOCATION = true;
    // NB: INFO_FIELD_FACEBOOK_NAME & INFO_FIELD_FACEBOOK_GENDER are always 'true'

}
