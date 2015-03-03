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

public class MainActivity extends Activity
// OnBufferingUpdateListener, OnCompletionListener, OnPreparedListener,
// OnVideoSizeChangedListener,
{
    private static final String TAG = "AMLPlayer";
    private static final int[] FILELISTFRAGMENT_RES_IDS =
    { R.id.filelistfragment1, R.id.filelistfragment2 };
    ListFragment lf;;

    private FileListFragment[] mFileListFragments = new  FileListFragment[FILELISTFRAGMENT_RES_IDS.length];
    private boolean[] mSizeKnown = new boolean[FILELISTFRAGMENT_RES_IDS.length];
    private boolean[] mVideoReady = new boolean[FILELISTFRAGMENT_RES_IDS.length];

    private String[] mUries = new String[FILELISTFRAGMENT_RES_IDS.length];

    @Override
    public void onCreate(Bundle icicle)
    {
        super.onCreate(icicle);
        setContentView(R.layout.multi_videos_layout);

    }
}
