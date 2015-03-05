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

import com.google.android.exoplayer.FrameworkSampleSource;
import com.google.android.exoplayer.FFSampleSource;
import com.google.android.exoplayer.MediaCodecAudioTrackRenderer;
import com.google.android.exoplayer.MediaCodecVideoTrackRenderer;
import com.mediacodec.demo.simple.SimplePlayerActivity.RendererBuilder;
import com.mediacodec.demo.simple.SimplePlayerActivity.RendererBuilderCallback;
import com.mediacodec.demo.simple.SimplePlayerFragment.RendererBuilderCallbackFragment;
import com.mediacodec.demo.simple.SimplePlayerFragment.RendererBuilderFragment;

import android.media.MediaCodec;
import android.net.Uri;

/**
 * A {@link RendererBuilder} for streams that can be read using
 * {@link android.media.MediaExtractor}.
 */
/* package */
class DefaultRendererBuilderFragment implements RendererBuilderFragment
{

//    private final SimplePlayerActivity playerActivity;
    private final SimplePlayerFragment playerFragment;
    private final Uri uri;

//    public DefaultRendererBuilderFragment(SimplePlayerActivity playerActivity, Uri uri)
//    {
//        this.playerActivity = playerActivity;
//        this.uri = uri;
//    }
    public DefaultRendererBuilderFragment(SimplePlayerFragment playerFragment, Uri uri)
    {
        this.playerFragment = playerFragment;
        this.uri = uri;
    }

    @Override
    public void buildRenderers(RendererBuilderCallbackFragment callback)
    {
        // Build the video and audio renderers.
        //FFSampleSource sampleSource2 = new FFSampleSource(playerFragment, uri, null, 2);
        FrameworkSampleSource sampleSource = new FrameworkSampleSource(playerFragment.getActivity(), uri, null, 2);
        MediaCodecVideoTrackRenderer videoRenderer = new MediaCodecVideoTrackRenderer(sampleSource, MediaCodec.VIDEO_SCALING_MODE_SCALE_TO_FIT, 0, playerFragment.getMainHandler(), playerFragment, 50);
        MediaCodecAudioTrackRenderer audioRenderer = new MediaCodecAudioTrackRenderer(sampleSource);

        // Invoke the callback.
        callback.onRenderers(videoRenderer, audioRenderer);
    }

}
