package com.meson.videoplayer2;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import android.app.Activity;
import android.app.ListFragment;
import android.content.res.AssetManager;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;

public class SimplePlayerFragmentActivity extends Activity
// OnBufferingUpdateListener, OnCompletionListener, OnPreparedListener,
// OnVideoSizeChangedListener,
{
    private static final String LOG_TAG = "MainActivity";
    @Override
    public void onCreate(Bundle icicle)
    {
        super.onCreate(icicle);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.multi_videos_layout);
    }
}
