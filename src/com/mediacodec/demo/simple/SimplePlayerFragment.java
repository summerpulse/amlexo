/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.mediacodec.demo.simple;

import com.google.android.exoplayer.ExoPlaybackException;
import com.google.android.exoplayer.ExoPlayer;
import com.google.android.exoplayer.MediaCodecAudioTrackRenderer;
import com.google.android.exoplayer.MediaCodecTrackRenderer.DecoderInitializationException;
import com.google.android.exoplayer.MediaCodecVideoTrackRenderer;
import com.google.android.exoplayer.VideoSurfaceView;
import com.mediacodec.demo.DemoUtil;
import com.meson.videoplayer2.R;
import com.google.android.exoplayer.util.PlayerControl;

import android.app.Activity;
import android.app.Fragment;
import android.content.Intent;
import android.media.MediaCodec.CryptoException;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnTouchListener;
import android.widget.MediaController;
import android.widget.Toast;

/**
 * An activity that plays media using {@link ExoPlayer}.
 */
public class SimplePlayerFragment extends Fragment implements SurfaceHolder.Callback, ExoPlayer.Listener, MediaCodecVideoTrackRenderer.EventListener
{
    /**
     * Builds renderers for the player.
     */
    public interface RendererBuilderFragment
    {
        void buildRenderers(RendererBuilderCallbackFragment callback);
    }

    public static final int RENDERER_COUNT = 2;
    public static final int TYPE_VIDEO = 0;
    public static final int TYPE_AUDIO = 1;

    private static final String TAG = "PlayerActivity";

    public static final int TYPE_DASH_VOD = 0;
    public static final int TYPE_SS_VOD = 1;
    public static final int TYPE_OTHER = 2;

    private MediaController mediaController;
    private Handler mainHandler;
    private View shutterView;
    private VideoSurfaceView surfaceView;

    private ExoPlayer player;
    private RendererBuilderFragment builder;
    private RendererBuilderCallbackFragment callback;
    private MediaCodecVideoTrackRenderer videoRenderer;

    private boolean autoPlay = true;
    private int playerPosition;

    private Uri contentUri;
    private int contentType;
    private String contentId;
    private final String LOG_TAG = "SimplePlayerFragment";

    // Activity lifecycle

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle saveInstanceState)
    {
        // Intent intent = this.getActivity().getIntent();
        Log.d(LOG_TAG, "onCreateView");
        contentUri = Uri.parse(getArguments().getString("video_file_path"));

        Log.d(LOG_TAG, "file path is:" + contentUri.toString());

        // contentType = intent.getIntExtra(DemoUtil.CONTENT_TYPE_EXTRA,
        // TYPE_OTHER);
        contentType = getArguments().getInt(DemoUtil.CONTENT_TYPE_EXTRA);
        // contentId = intent.getStringExtra(DemoUtil.CONTENT_ID_EXTRA);
        contentId = getArguments().getString(DemoUtil.CONTENT_ID_EXTRA);
        mainHandler = new Handler(this.getActivity().getMainLooper());
        builder = getRendererBuilder();

        // setContentView(R.layout.player_activity_simple);
        View view = inflater.inflate(R.layout.player_activity_simple, container, false);
        View root = this.getActivity().findViewById(R.id.root);
        view.setOnTouchListener(new OnTouchListener()
        {
            @Override
            public boolean onTouch(View arg0, MotionEvent arg1)
            {
                if (arg1.getAction() == MotionEvent.ACTION_DOWN)
                {
                    toggleControlsVisibility();
                }
                return true;
            }
        });

        mediaController = new MediaController(this.getActivity());
        mediaController.setAnchorView(view);
        shutterView = view.findViewById(R.id.shutter);
        surfaceView = (VideoSurfaceView)view.findViewById(R.id.surface_view);
        surfaceView.getHolder().addCallback(this);
        Log.d(LOG_TAG, this.toString());
        return view;
    }

    @Override
    public void onResume()
    {
        super.onResume();
        // Setup the player
        player = ExoPlayer.Factory.newInstance(RENDERER_COUNT, 1000, 5000);
        player.addListener(this);
        player.seekTo(playerPosition);
        // Build the player controls
        mediaController.setMediaPlayer(new PlayerControl(player));
        mediaController.setEnabled(true);
        // Request the renderers
        callback = new RendererBuilderCallbackFragment();
        builder.buildRenderers(callback);
    }

    @Override
    public void onPause()
    {
        super.onPause();
        // Release the player
        if (player != null)
        {
            playerPosition = player.getCurrentPosition();
            player.release();
            player = null;
        }
        callback = null;
        videoRenderer = null;
        shutterView.setVisibility(View.VISIBLE);
    }

    // Public methods

    public Handler getMainHandler()
    {
        return mainHandler;
    }

    // Internal methods

    private void toggleControlsVisibility()
    {
        Log.d(LOG_TAG, "toggleControlsVisibility");
        if (mediaController.isShowing())
        {
            mediaController.hide();
        }
        else
        {
            mediaController.show(0);
        }
    }

    private RendererBuilderFragment getRendererBuilder()
    {
        Log.d(LOG_TAG, "RendererBuilderFragment::getRendererBuilder");
//        String userAgent = DemoUtil.getUserAgent(this.getActivity());
//        switch (contentType)
//        {
//        case TYPE_SS_VOD:
//            return new SmoothStreamingRendererBuilder(this, userAgent, contentUri.toString(), contentId);
//        case TYPE_DASH_VOD:
//            return new DashVodRendererBuilder(this, userAgent, contentUri.toString(), contentId);
//        default:
//            return new DefaultRendererBuilder(this.getActivity(), contentUri);
//        }
        
        return new DefaultRendererBuilderFragment(this, contentUri);
    }

    private void onRenderers(RendererBuilderCallbackFragment callback, MediaCodecVideoTrackRenderer videoRenderer, MediaCodecAudioTrackRenderer audioRenderer)
    {
        Log.d(LOG_TAG," onRenderers");
        if (this.callback != callback)
        {
            return;
        }
        this.callback = null;
        this.videoRenderer = videoRenderer;
        player.prepare(videoRenderer, audioRenderer);
        maybeStartPlayback();
    }

    private void maybeStartPlayback()
    {
        Log.d(LOG_TAG," maybeStartPlayback");
        Surface surface = surfaceView.getHolder().getSurface();
        if (videoRenderer == null || surface == null || !surface.isValid())
        {
            Log.d(LOG_TAG," We're not ready yet.");
            if (videoRenderer == null)
                Log.d(LOG_TAG,"videoRenderer == null");
            if (surface == null)
                Log.d(LOG_TAG, "surface == null");
            if(!surface.isValid())
                Log.d(LOG_TAG, "surface.isValid()");
            return;
        }
        player.sendMessage(videoRenderer, MediaCodecVideoTrackRenderer.MSG_SET_SURFACE, surface);
        if (autoPlay)
        {
            player.setPlayWhenReady(true);
            autoPlay = false;
        }
    }

    private void onRenderersError(RendererBuilderCallbackFragment callback, Exception e)
    {
        if (this.callback != callback)
        {
            return;
        }
        this.callback = null;
        onError(e);
    }

    private void onError(Exception e)
    {
        Log.e(TAG, "Playback failed", e);
        Toast.makeText(this.getActivity(), R.string.failed, Toast.LENGTH_SHORT).show();
        this.getActivity().finish();
    }

    // ExoPlayer.Listener implementation

    @Override
    public void onPlayerStateChanged(boolean playWhenReady, int playbackState)
    {
        // Do nothing.
    }

    @Override
    public void onPlayWhenReadyCommitted()
    {
        // Do nothing.
    }

    @Override
    public void onPlayerError(ExoPlaybackException e)
    {
        onError(e);
    }

    // MediaCodecVideoTrackRenderer.Listener

    @Override
    public void onVideoSizeChanged(int width, int height)
    {
        surfaceView.setVideoWidthHeightRatio(height == 0 ? 1 : (float) width / height);
    }

    @Override
    public void onDrawnToSurface(Surface surface)
    {
        shutterView.setVisibility(View.GONE);
    }

    @Override
    public void onDroppedFrames(int count, long elapsed)
    {
        Log.d(TAG, "Dropped frames: " + count);
    }

    @Override
    public void onDecoderInitializationError(DecoderInitializationException e)
    {
        // This is for informational purposes only. Do nothing.
    }

    @Override
    public void onCryptoError(CryptoException e)
    {
        // This is for informational purposes only. Do nothing.
    }

    // SurfaceHolder.Callback implementation

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        Log.d(LOG_TAG, "surfaceCreated");
        maybeStartPlayback();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        // Do nothing.
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        Log.d(LOG_TAG, "surfaceDestroyed");
        if (videoRenderer != null)
        {
            player.blockingSendMessage(videoRenderer, MediaCodecVideoTrackRenderer.MSG_SET_SURFACE, null);
        }
    }

    /* package */
    final class RendererBuilderCallbackFragment
    {
        
        public void onRenderers(MediaCodecVideoTrackRenderer videoRenderer, MediaCodecAudioTrackRenderer audioRenderer)
        {
            SimplePlayerFragment.this.onRenderers(this, videoRenderer, audioRenderer);
        }

        public void onRenderersError(Exception e)
        {
            SimplePlayerFragment.this.onRenderersError(this, e);
        }

    }

}
