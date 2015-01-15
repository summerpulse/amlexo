package com.google.android.exoplayer;

import java.io.FileDescriptor;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.UUID;

import android.content.ContentResolver;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.MediaCodec.CryptoInfo;
import android.media.MediaFormat;
import android.net.Uri;


public class FFExtractor {

	public FFExtractor() {
		// TODO Auto-generated constructor stub
		native_setup();
	}

	public boolean advance() {
		// TODO Auto-generated method stub
		return advance_native();
	}

	public long getCachedDuration() {
		// TODO Auto-generated method stub
		return getCachedDuration_native();
	}

	public Map<UUID, byte[]> getPsshInfo() {
		// TODO Auto-generated method stub
		return null;
	}

	public boolean getSampleCryptoInfo(CryptoInfo arg0) {
		// TODO Auto-generated method stub
		return false;
	}

	public int getSampleFlags() {
		// TODO Auto-generated method stub
		return getSampleFlags_native();
	}

	public long getSampleTime() {
		// TODO Auto-generated method stub
		return getSampleTime_native();
	}

	public int getSampleTrackIndex() {
		// TODO Auto-generated method stub
		return getSampleTrackIndex_native();
	}

	public int getTrackCount() {
		// TODO Auto-generated method stub
		return getTrackCount_native();
	}

	public MediaFormat getTrackFormat(int index) {
		// TODO Auto-generated method stub
		return getTrackFormatNative_native(index);
	}

	public boolean hasCacheReachedEndOfStream() {
		// TODO Auto-generated method stub
		return hasCacheReachedEndOfStream_native();
	}

	public int readSampleData(ByteBuffer byteBuf, int offset) {
		// TODO Auto-generated method stub
		return readSampleData_native(byteBuf, offset);
	}

	public void release() {
		// TODO Auto-generated method stub
		release_native();

	}

	public void seekTo(long timeUs, int mode1) {
		// TODO Auto-generated method stub
		seekTo_native(timeUs, mode1);
	}

	public void selectTrack(int index) {
		// TODO Auto-generated method stub
		selectTrack_native(index);
	}

//	public final void setDataSource(DataSource source)
//	{
//		// TODO Auto-generated method stub
//		try {
//			setDataSource_native(source);
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//	}

	public void setDataSource(String path) {
		// TODO Auto-generated method stub
		try {
			setDataSource_native(path, null, null);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void setDataSource(FileDescriptor fd) {
		// TODO Auto-generated method stub
		setDataSource(fd, 0, 0x7ffffffffffffffL);
	}

	public void setDataSource(String path, Map<String, String> headers) {
		// TODO Auto-generated method stub
	        String[] keys = null;
	        String[] values = null;

	        if (headers != null) {
	            keys = new String[headers.size()];
	            values = new String[headers.size()];

	            int i = 0;
	            for (Map.Entry<String, String> entry: headers.entrySet()) {
	                keys[i] = entry.getKey();
	                values[i] = entry.getValue();
	                ++i;
	            }
	        }
	        try {
				setDataSource_native(path, keys, values);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	    }

	public void setDataSource(
            Context context, Uri uri, Map<String, String> headers) {
		// TODO Auto-generated method stub
        String scheme = uri.getScheme();
        if(scheme == null || scheme.equals("file")) {
            setDataSource(uri.getPath());
            return;
        }

        AssetFileDescriptor fd = null;
        try {
            ContentResolver resolver = context.getContentResolver();
            fd = resolver.openAssetFileDescriptor(uri, "r");
            if (fd == null) {
                return;
            }
            // Note: using getDeclaredLength so that our behavior is the same
            // as previous versions when the content provider is returning
            // a full file.
            if (fd.getDeclaredLength() < 0) {
                setDataSource(fd.getFileDescriptor());
            } else {
                setDataSource(
                        fd.getFileDescriptor(),
                        fd.getStartOffset(),
                        fd.getDeclaredLength());
            }
            return;
        } catch (SecurityException ex) {
        } catch (IOException ex) {
        } finally {
            if (fd != null) {
                try {
					fd.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
            }
        }

        setDataSource(uri.toString(), headers);
	}

	public void setDataSource(FileDescriptor fd, long offset, long length) {
		// TODO Auto-generated method stub
		try {
			setDataSource_native(fd, offset, length);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void unselectTrack(int index) {
		// TODO Auto-generated method stub
		unselectTrack_native(index);

	}

	/* Native methods. */
    /**
     * Sets the DataSource object to be used as the data source for this extractor
     * {@hide}
     */
	//private native final void setDataSource_native(DataSource source) throws IOException;

	
    private native final void setDataSource_native(
            String path, String[] keys, String[] values) throws IOException;
	
    /**
     * Sets the data source (FileDescriptor) to use.  The FileDescriptor must be
     * seekable (N.B. a LocalSocket is not seekable). It is the caller's responsibility
     * to close the file descriptor. It is safe to do so as soon as this call returns.
     *
     * @param fd the FileDescriptor for the file you want to extract from.
     * @param offset the offset into the file where the data to be extracted starts, in bytes
     * @param length the length in bytes of the data to be extracted
     */
    private native final void setDataSource_native(
            FileDescriptor fd, long offset, long length) throws IOException;

	
    /**
     * Make sure you call this when you're done to free up any resources
     * instead of relying on the garbage collector to do this for you at
     * some point in the future.
     */
    private native final void release_native();

    /**
     * Count the number of tracks found in the data source.
     */
    private native final int getTrackCount_native();
	
	private native Map<String, Object> getFileFormatNative_native();
	
    private native MediaFormat getTrackFormatNative_native(int index);

    /**
     * Subsequent calls to {@link #readSampleData}, {@link #getSampleTrackIndex} and
     * {@link #getSampleTime} only retrieve information for the subset of tracks
     * selected.
     * Selecting the same track multiple times has no effect, the track is
     * only selected once.
     */
    private native void selectTrack_native(int index);

    /**
     * Subsequent calls to {@link #readSampleData}, {@link #getSampleTrackIndex} and
     * {@link #getSampleTime} only retrieve information for the subset of tracks
     * selected.
     */
    private native void unselectTrack_native(int index);
	
    /**
     * All selected tracks seek near the requested time according to the
     * specified mode.
     */
    private native void seekTo_native(long timeUs, int mode);

    /**
     * Advance to the next sample. Returns false if no more sample data
     * is available (end of stream).
     */
    private native boolean advance_native();

    /**
     * Retrieve the current encoded sample and store it in the byte buffer
     * starting at the given offset. Returns the sample size (or -1 if
     * no more samples are available).
     */
    private native int readSampleData_native(ByteBuffer byteBuf, int offset);

    /**
     * Returns the track index the current sample originates from (or -1
     * if no more samples are available)
     */
    private native int getSampleTrackIndex_native();

    /**
     * Returns the current sample's presentation time in microseconds.
     * or -1 if no more samples are available.
     */
    private native long getSampleTime_native();

    /**
     * Returns the current sample's flags.
     */
    private native int getSampleFlags_native();
    
    /**
     * Returns an estimate of how much data is presently cached in memory
     * expressed in microseconds. Returns -1 if that information is unavailable
     * or not applicable (no cache).
     */
    private native long getCachedDuration_native();

    /**
     * Returns true iff we are caching data and the cache has reached the
     * end of the data stream (for now, a future seek may of course restart
     * the fetching of data).
     * This API only returns a meaningful result if {@link #getCachedDuration}
     * indicates the presence of a cache, i.e. does NOT return -1.
     */
    private native boolean hasCacheReachedEndOfStream_native();
    private static native void native_init();
    private native final void native_setup();
    private native final void native_finalize();

    static {
        System.loadLibrary("ExoPlayerLibJNI");
        native_init();
    }

    private int mNativeContext;
}
