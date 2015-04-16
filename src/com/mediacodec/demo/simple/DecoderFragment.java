package com.mediacodec.demo.simple;

import android.app.Fragment;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;

import com.meson.videoplayer2.R;

public class DecoderFragment extends Fragment implements SurfaceHolder.Callback
{
    private VideoPlayer mVideoPlayer = null;
//    private audioSPlayer mAudioPlayer = null;
    private Uri contentUri = null;
    private SurfaceView surfaceView = null;
    private SurfaceHolder mSurfaceHolder = null;
    
    private final static String LOG_TAG = "DecoderFragment";

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle saveInstanceState)
    {
        Log.d(LOG_TAG, "onCreateView");
        contentUri = Uri.parse(getArguments().getString("video_file_path"));
        Log.d(LOG_TAG, "file path is:" + contentUri.toString());
        
        View view = inflater.inflate(R.layout.decoder_fragment, container, false);
//        SurfaceView sv = new SurfaceView(this);
//        sv.getHolder().addCallback(this);
//        setContentView(sv);
//        mAudioPlayer = null;// new audioSPlayer("/data/tmp/test.mp4");
        surfaceView  =  (SurfaceView)view.findViewById(R.id.video_1_surfaceview);
        mSurfaceHolder = surfaceView.getHolder();
        mSurfaceHolder.addCallback(this);
        mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        return view;
    }

    public void onDestroy()
    {
        super.onDestroy();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
//        String url = "/data/media/0/test.mp4";
//        String ur2 = "/data/media/0/M4A-AACHE.m4a";
        mVideoPlayer = new VideoPlayer(holder.getSurface(), contentUri.toString());
//        mAudioPlayer = null;//new audioSPlayer(url);
        if (mVideoPlayer != null)
        {
            mVideoPlayer.start();
        }
//        if (mAudioPlayer != null)
//            mAudioPlayer.start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        if (mVideoPlayer != null)
        {
            mVideoPlayer.interrupt();
        }
//        if (mAudioPlayer != null)
//            mAudioPlayer.stop();
//        mAudioPlayer = null;
    }
}