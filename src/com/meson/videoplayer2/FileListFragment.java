/*@file FileListFragment.java
 * Based on FileList2.java, change from activity to fragment for multiple presences
 * */
package com.meson.videoplayer2;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.app.ListFragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager.NameNotFoundException;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.BaseColumns;
import android.provider.MediaStore;
import android.provider.MediaStore.MediaColumns;
import android.provider.MediaStore.Video.VideoColumns;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.mediacodec.demo.DemoUtil;
import com.mediacodec.demo.simple.DecoderFragment;
import com.mediacodec.demo.simple.SimplePlayerActivity;
import com.mediacodec.demo.simple.SimplePlayerFragment;

public class FileListFragment extends ListFragment
{
    private static final String ROOT_PATH = "/storage";
    private static final String SHEILD_EXT_STOR = "/storage/sdcard0/external_storage";
    private static final String NAND_PATH = Environment.getDataDirectory().getPath();
    private static final String SD_PATH = Environment.getExternalStorageDirectory().getPath();
    private static final String USB_PATH = "/storage/external_storage";
//    private static final String ROOT = "/";

    private boolean listAllFiles = false;
    private List<File> listFiles = null;
    private List<File> listVideos = null;
    private List<String> items = null;
    private List<String> paths = null;
    private List<String> currentlist = null;
    private String currenturl = null;
    private String root_path = ROOT_PATH;
    private String extensions;
    private static String ISOpath = null;

    private TextView tileText;
    private TextView nofileText;
    private TextView searchText;
    private ProgressBar sp;
    private boolean isScanning = false;
    private boolean isQuerying = false;
    private int scanCnt = 0;
    private File file;
    private static String TAG = "player_FileList";
    Timer timer = new Timer();
    Timer timerScan = new Timer();

    private int item_position_selected, item_position_first, fromtop_piexl;
    private ArrayList<Integer> fileDirectory_position_selected = new ArrayList<Integer>();
    private ArrayList<Integer> fileDirectory_position_piexl = new ArrayList<Integer>();
    private int pathLevel = 0;
    private final String iso_mount_dir = "/storage/external_storage/VIRTUAL_CDROM";
    private Uri uri;
    private final String LOG_TAG = "FileListFragment";

    private void waitForRescan()
    {
        final Handler handler = new Handler()
        {
            @Override
            public void handleMessage(Message msg)
            {
                switch (msg.what)
                {
                case 0x5c:
                    isScanning = false;
                    prepareFileForList();
                    timerScan.cancel();
                    break;
                }
                super.handleMessage(msg);
            }
        };
        TimerTask task = new TimerTask()
        {
            @Override
            public void run()
            {
                Message message = Message.obtain();
                message.what = 0x5c;
                handler.sendMessage(message);
            }
        };

        timer.cancel();
        timer = new Timer();
        timer.schedule(task, 500);
    }

    private void waitForScanFinish()
    {
        final Handler handler = new Handler()
        {
            @Override
            public void handleMessage(Message msg)
            {
                switch (msg.what)
                {
                case 0x6c:
                    scanCnt--;
                    isScanning = false;
                    prepareFileForList();
                    break;
                }
                super.handleMessage(msg);
            }
        };
        TimerTask task = new TimerTask()
        {
            @Override
            public void run()
            {
                Message message = Message.obtain();
                message.what = 0x6c;
                handler.sendMessage(message);
            }
        };

        timerScan.cancel();
        timerScan = new Timer();
        timerScan.schedule(task, 20000);
    }

    private BroadcastReceiver mScanListener = new BroadcastReceiver()
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {

            String action = intent.getAction();
            if (Intent.ACTION_MEDIA_UNMOUNTED.equals(action))
            {
                prepareFileForList();
            }
            else
                if (Intent.ACTION_MEDIA_SCANNER_STARTED.equals(action))
                {
                    if (!isScanning)
                    {
                        isScanning = true;
                        setListAdapter(null);
                        showSpinner();
                        scanCnt++;
                        waitForScanFinish();
                    }
                }
                else
                    if (Intent.ACTION_MEDIA_SCANNER_FINISHED.equals(action))
                    {
                        if ((isScanning) && (scanCnt == 1))
                        {
                            scanCnt--;
                            waitForRescan();
                        }
                    }
            /*
             * else if(Intent.ACTION_MEDIA_MOUNTED.equals(action)) { } else
             * if(Intent.ACTION_MEDIA_EJECT.equals(action)) { }
             */
        }
    };

    @Override
    public void onResume()
    {
        super.onResume();
        if (!listAllFiles)
        {
            File file = null;
            if (PlayList.getinstance().rootPath != null)
                file = new File(PlayList.getinstance().rootPath);

            if ((file != null) && file.exists())
            {
                File[] the_Files;
                the_Files = file.listFiles(new MyFilter(extensions));
                if (the_Files == null)
                {
                    PlayList.getinstance().rootPath = root_path;
                }
                BrowserFile(PlayList.getinstance().rootPath);

            }
            else
            {
                PlayList.getinstance().rootPath = root_path;
                BrowserFile(PlayList.getinstance().rootPath);
            }
            getListView().setSelectionFromTop(item_position_selected, fromtop_piexl);
        }
        else
        {
            IntentFilter f = new IntentFilter();
            f.addAction(Intent.ACTION_MEDIA_EJECT);
            f.addAction(Intent.ACTION_MEDIA_MOUNTED);
            f.addAction(Intent.ACTION_MEDIA_UNMOUNTED);
            f.addAction(Intent.ACTION_MEDIA_SCANNER_STARTED);
            f.addAction(Intent.ACTION_MEDIA_SCANNER_FINISHED);
            f.addDataScheme("file");
            //registerReceiver(mScanListener, f);
        }
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
    }

    @Override
    public void onPause()
    {
        super.onPause();
        if (!listAllFiles)
        {
        }
        else
        {
            isScanning = false;
            isQuerying = false;
            scanCnt = 0;
            timer.cancel();
            timerScan.cancel();
            //unregisterReceiver(mScanListener);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle saveInstanceState)
    {
        //super.onCreate(icicle);
        Log.d(LOG_TAG, "onCreateView");
        Log.d(LOG_TAG, "NAND_PATH="+NAND_PATH);
        Log.d(LOG_TAG, "SD_PATH="+SD_PATH);
        extensions = getResources().getString(R.string.support_video_extensions);
        View view = inflater.inflate(R.layout.listfragment, container, false);
        //requestWindowFeature(Window.FEATURE_NO_TITLE);
        //setContentView(R.layout.file_list);
        //PlayList.setContext(this);

        listAllFiles = false;

        currentlist = new ArrayList<String>();

        if (!listAllFiles)
        {
            try
            {
                Bundle bundle = new Bundle();
                bundle = this.getActivity().getIntent().getExtras();
                if (bundle != null)
                {
                    item_position_selected = bundle.getInt("item_position_selected");
                    item_position_first = bundle.getInt("item_position_first");
                    fromtop_piexl = bundle.getInt("fromtop_piexl");
                    fileDirectory_position_selected = bundle.getIntegerArrayList("fileDirectory_position_selected");
                    fileDirectory_position_piexl = bundle.getIntegerArrayList("fileDirectory_position_piexl");
                    pathLevel = fileDirectory_position_selected.size();
                }
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }

            if (PlayList.getinstance().rootPath == null)
                PlayList.getinstance().rootPath = root_path;

            BrowserFile(PlayList.getinstance().rootPath);
        }

        Button home = (Button) view.findViewById(R.id.Button_home);
        home.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if (listAllFiles)
                {
                    if (!isScanning)
                    {
                        reScanVideoFiles();
                    }
                }
                else
                {
                    FileListFragment.this.getActivity().finish();
                    PlayList.getinstance().rootPath = null;
                }
            }

        });
        Button exit = (Button) view.findViewById(R.id.Button_exit);
        exit.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if (listAllFiles)
                {
                    FileListFragment.this.getActivity().finish();
                    return;
                }

                if (paths == null)
                {
                    FileListFragment.this.getActivity().finish();
                    PlayList.getinstance().rootPath = null;
                }
                else
                {
                    if (paths.isEmpty())
                    {
                        FileListFragment.this.getActivity().finish();
                        PlayList.getinstance().rootPath = null;
                    }
                    file = new File(paths.get(0).toString());
                    if (file.getParent().compareTo(iso_mount_dir) == 0 && ISOpath != null)
                    {
                        file = new File(ISOpath + "/VIRTUAL_CDROM");
                        Log.i(TAG, "[exit button]file:" + file);
                        ISOpath = null;
                    }
                    currenturl = file.getParentFile().getParent();
                    if ((file.getParent().compareToIgnoreCase(root_path) != 0) && (pathLevel > 0))
                    {
                        String path = file.getParent();
                        String parent_path = file.getParentFile().getParent();
                        if ((path.equals(NAND_PATH) || path.equals(SD_PATH) || parent_path.equals(USB_PATH)) && (pathLevel > 0))
                        {
                            pathLevel = 0;
                            BrowserFile(ROOT_PATH);
                        }
                        else
                        {
                            BrowserFile(currenturl);
                            pathLevel--;
                            getListView().setSelectionFromTop(fileDirectory_position_selected.get(pathLevel), fileDirectory_position_piexl.get(pathLevel));
                            fileDirectory_position_selected.remove(pathLevel);
                            fileDirectory_position_piexl.remove(pathLevel);
                        }
                    }
                    else
                    {
                        FileListFragment.this.getActivity().finish();
                        PlayList.getinstance().rootPath = null;
                    }
                }

            }

        });

        nofileText = (TextView) view.findViewById(R.id.TextView_nofile);
        searchText = (TextView) view.findViewById(R.id.TextView_searching);
        sp = (ProgressBar) view.findViewById(R.id.spinner);
        if (listAllFiles)
        {
            prepareFileForList();
        }
        return view;
    }

    private void showSpinner()
    {
        if (listAllFiles)
        {
            if ((isScanning) || (isQuerying))
            {
                sp.setVisibility(View.VISIBLE);
                searchText.setVisibility(View.VISIBLE);
                nofileText.setVisibility(View.INVISIBLE);
            }
            else
            {
                sp.setVisibility(View.INVISIBLE);
                searchText.setVisibility(View.INVISIBLE);
                int total = paths.size();
                if (total == 0)
                {
                    nofileText.setVisibility(View.VISIBLE);
                }
                else
                    if (total > 0)
                    {
                        nofileText.setVisibility(View.INVISIBLE);
                    }
            }
        }
        else
        {
            sp.setVisibility(View.GONE);
            nofileText.setVisibility(View.GONE);
            searchText.setVisibility(View.GONE);
        }
    }

    private void prepareFileForList()
    {
        if (listAllFiles)
        {
            // Intent intent = getIntent();
            // uri = intent.getData();
            String[] mCursorCols = new String[]
            { BaseColumns._ID, MediaColumns.DATA, MediaColumns.TITLE, MediaColumns.SIZE, VideoColumns.DURATION,
            // MediaStore.Video.Media.BOOKMARK,
            // MediaStore.Video.Media.PLAY_TIMES
            };
            String patht = null;
            String namet = null;
            paths = new ArrayList<String>();
            items = new ArrayList<String>();
            paths.clear();
            items.clear();

            setListAdapter(null);
            isQuerying = true;
            showSpinner();

            uri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
            Cursor cursor = this.getActivity().getContentResolver().query(uri, mCursorCols, null, null, null);
            cursor.moveToFirst();
            int colidx = cursor.getColumnIndexOrThrow(MediaColumns.DATA);
            for (int i = 0; i < cursor.getCount(); i++)
            {
                patht = cursor.getString(colidx);
                // Log.e("wxl", "cursor["+colidx+"]:"+patht);
                if (patht != null)
                {
                    int index = patht.lastIndexOf("/");
                    if (index >= 0)
                    {
                        namet = patht.substring(index);
                    }
                    items.add(namet);
                    paths.add(patht);
                }
                cursor.moveToNext();
            }

//            tileText = (TextView) .findViewById(R.id.TextView_path);
//            tileText.setText(R.string.all_file);
            if (paths.size() > 0)
            {
                setListAdapter(new MyAdapter(this.getActivity(), items, paths));
            }

            isQuerying = false;
            showSpinner();

            if (cursor != null)
            {
                cursor.close();
            }
        }
    }

    private void BrowserFile(String filePath)
    {
        int i = 0;
        int dev_usb_count = 0;
        int dev_cd_count = 0;
        file = new File(filePath);
        listFiles = new ArrayList<File>();
        items = new ArrayList<String>();
        paths = new ArrayList<String>();
        String[] files = file.list();
        if (files != null)
        {
            for (i = 0; i < files.length; i++)
            {
                if (files[i].equals("VIRTUAL_CDROM"))
                {
                    execCmd("vdc loop unmount");
                    break;
                }
            }
        }
        searchFile(file);
        if (listFiles.isEmpty())
        {
            Toast.makeText(FileListFragment.this.getActivity(), R.string.str_no_file, Toast.LENGTH_SHORT).show();
            // paths =currentlist;
            paths.clear();
            paths.addAll(currentlist);
            return;
        }
        Log.d(TAG, "BrowserFile():" + filePath);
        PlayList.getinstance().rootPath = filePath;

        File[] fs = new File[listFiles.size()];
        for (i = 0; i < listFiles.size(); i++)
        {
            fs[i] = listFiles.get(i);
        }
        if (!filePath.equals(ROOT_PATH))
        {
            // System.setProperty("java.util.Arrays.useLegacyMergeSort",
            // "true");
            try
            {
                Arrays.sort(fs, new MyComparator(MyComparator.NAME_ASCEND));
            }
            catch (IllegalArgumentException ex)
            {
            }
        }

        for (i = 0; i < fs.length; i++)
        {
            File tempF = fs[i];
            String tmppath = tempF.getName();

            // change device name;
            if (filePath.equals(ROOT_PATH))
            {
                String tpath = tempF.getAbsolutePath();

                if (tpath.equals(NAND_PATH))
                    tmppath = "sdcard";
                else
                    if (tpath.equals(SD_PATH))
                        tmppath = "external_sdcard";
                    else
                        if ((!tpath.equals(SD_PATH)) && tpath.startsWith(USB_PATH + "/sd"))
                        {
                            dev_usb_count++;
                            char data = (char) ('A' + dev_usb_count - 1);
                            tmppath = getText(R.string.usb_device_str) + "(" + data + ":)";
                            // tmppath = "usb"+" "+tpath.substring(5);//5 is the
                            // len of
                            // "/mnt/"
                        }
                        else
                            if ((!tpath.equals(SD_PATH)) && tpath.startsWith(USB_PATH + "/sr"))
                            {
                                dev_cd_count++;
                                char data = (char) ('A' + dev_cd_count - 1);
                                tmppath = getText(R.string.cdrom_device_str) + "(" + data + ":)";
                            }

                // delete used folder
                if ((!tpath.equals("/mnt/asec")) && (!tpath.equals("/mnt/secure")) && (!tpath.equals("/mnt/obb")) && (!tpath.equals("/mnt/usbdrive")) && (!tpath.equals("/mnt/shell")))
                {
                    String path = changeDevName(tmppath);
                    items.add(path);
                    paths.add(tempF.getPath());
                }
            }
            else
            {
                items.add(tmppath);
                paths.add(tempF.getPath());
            }
        }

//        tileText = (TextView) findViewById(R.id.TextView_path);
//        tileText.setText(catShowFilePath(filePath));
        setListAdapter(new MyAdapter(this.getActivity(), items, paths));
    }

    private String changeDevName(String tmppath)
    {
        String path = "";
        String internal = getString(R.string.memory_device_str);
        String sdcard = getString(R.string.sdcard_device_str);
        String usb = getString(R.string.usb_device_str);
        String cdrom = getString(R.string.cdrom_device_str);
        String sdcardExt = getString(R.string.ext_sdcard_device_str);

        // Log.i("wxl","[changeDevName]tmppath:"+tmppath);

        if (tmppath.equals("flash"))
        {
            path = internal;
        }
        else
            if (tmppath.equals("sdcard"))
            {
                path = sdcard;
            }
            else
                if (tmppath.equals("usb"))
                {
                    path = usb;
                }
                else
                    if (tmppath.equals("cd-rom"))
                    {
                        path = cdrom;
                    }
                    else
                        if (tmppath.equals("external_sdcard"))
                        {
                            path = sdcardExt;
                        }
                        else
                        {
                            path = tmppath;
                        }

        // Log.i("wxl","[changeDevName]path:"+path);
        return path;
    }

    private String catShowFilePath(String path)
    {
        String text = null;

        if (path.startsWith("/mnt/flash"))
            text = path.replaceFirst("/mnt/flash", "/mnt/nand");
        else
            if (path.startsWith("/mnt/sda"))
                text = path.replaceFirst("/mnt/sda", "/mnt/usb sda");
            else
                if (path.startsWith("/mnt/sdb"))
                    text = path.replaceFirst("/mnt/sdb", "/mnt/usb sdb");
        // else if(path.startsWith("/mnt/sdcard"))
        // text=path.replaceFirst("/mnt/sdcard","sdcard");
        return text;
    }

    public void searchFile(File file)
    {
        File[] the_Files;
        the_Files = file.listFiles(new MyFilter(extensions));

        if (the_Files == null)
        {
            Toast.makeText(FileListFragment.this.getActivity(), R.string.str_no_file, Toast.LENGTH_SHORT).show();
            return;
        }

        String curPath = file.getPath();
        if (curPath.equals(root_path))
        {
            File dir = new File(NAND_PATH);
            if (dir.exists() && dir.isDirectory())
            {
                listFiles.add(dir);
            }

            dir = new File(SD_PATH);
            if (dir.exists() && dir.isDirectory())
            {
                listFiles.add(dir);
            }

            dir = new File(USB_PATH);
            if (dir.exists() && dir.isDirectory())
            {
                if (dir.listFiles() != null)
                {
                    for (File pfile : dir.listFiles())
                    {
                        if (pfile.isDirectory())
                        {
                            String path = pfile.getAbsolutePath();
                            if ((path.startsWith(USB_PATH + "/sd") || path.startsWith(USB_PATH + "/sr")) && !path.equals(SD_PATH))
                            {
                                listFiles.add(pfile);
                            }
                        }
                    }
                }
            }

            return;
        }

        for (int i = 0; i < the_Files.length; i++)
        {
            File tempF = the_Files[i];

            if (tempF.isDirectory())
            {
                if (!tempF.isHidden())
                    listFiles.add(tempF);

                // shield some path
                if ((tempF.toString()).equals(SHEILD_EXT_STOR))
                {
                    listFiles.remove(tempF);
                    continue;
                }
            }
            else
            {
                try
                {
                    listFiles.add(tempF);
                }
                catch (Exception e)
                {
                    return;
                }
            }
        }
    }

    public boolean isISOFile(File file)
    {
        String fname = file.getName();
        String sname = ".iso";

        if (fname == "")
        {
            Log.e(TAG, "NULL file");
            return false;
        }
        if (fname.toLowerCase().endsWith(sname))
        {
            return true;
        }
        return false;
    }

    public void execCmd(String cmd)
    {
        int ch;
        Process p = null;
        Log.d(TAG, "exec command: " + cmd);
        try
        {
            p = Runtime.getRuntime().exec(cmd);
            InputStream in = p.getInputStream();
            InputStream err = p.getErrorStream();
            StringBuffer sb = new StringBuffer(512);
            while ((ch = in.read()) != -1)
            {
                sb.append((char) ch);
            }
            if (sb.toString() != "")
                Log.d(TAG, "exec out:" + sb.toString());
            while ((ch = err.read()) != -1)
            {
                sb.append((char) ch);
            }
            if (sb.toString() != "")
                Log.d(TAG, "exec error:" + sb.toString());
        }
        catch (IOException e)
        {
            Log.d(TAG, "IOException: " + e.toString());
        }
    }

    @Override
    public void onListItemClick(ListView l, View v, int position, long id)
    {
        File file = new File(paths.get(position));
        currentlist.clear();
        currentlist.addAll(paths);
        // currentlist =paths;

        if (file.isDirectory())
        {
            item_position_selected = getListView().getSelectedItemPosition();
            item_position_first = getListView().getFirstVisiblePosition();
            View cv = getListView().getChildAt(item_position_selected - item_position_first);
            if (cv != null)
            {
                fromtop_piexl = cv.getTop();
            }
            BrowserFile(paths.get(position));
            if (!listFiles.isEmpty())
            {
                fileDirectory_position_selected.add(item_position_selected);
                fileDirectory_position_piexl.add(fromtop_piexl);
                pathLevel++;
            }
        }
        else
            if (isISOFile(file))
            {
                execCmd("vdc loop unmount");
                ISOpath = file.getPath();
                String cm = "vdc loop mount " + "\"" + ISOpath + "\"";
                Log.d(TAG, "ISO path:" + ISOpath);
                execCmd(cm);
                BrowserFile(iso_mount_dir);
                fileDirectory_position_selected.add(item_position_selected);
                fileDirectory_position_piexl.add(fromtop_piexl);
                pathLevel++;
            }
            else
            {
                // if (!listAllFiles) {
                // int pos = filterDir(file);
                // if (pos < 0)
                // return;
                // PlayList.getinstance().rootPath = file.getParent();
                // PlayList.getinstance().setlist(paths, pos);
                // item_position_selected = getListView()
                // .getSelectedItemPosition();
                // item_position_first =
                // getListView().getFirstVisiblePosition();
                //
                // if (!listAllFiles) {
                // View cv = getListView().getChildAt(
                // item_position_selected - item_position_first);
                // if (cv != null) {
                // fromtop_piexl = cv.getTop();
                // }
                // }
                // } else {
                // PlayList.getinstance().setlist(paths, position);
                // }
                // showvideobar();

                Intent intent = new Intent(this.getActivity(), SimplePlayerActivity.class);
                intent.setData(Uri.parse(file.getAbsolutePath()));
                intent.putExtra(DemoUtil.CONTENT_ID_EXTRA, "");
                intent.putExtra(DemoUtil.CONTENT_TYPE_EXTRA, 2);
                Bundle bundle = new Bundle();
                bundle.putString("video_file_path", file.getAbsolutePath());
                bundle.putString(DemoUtil.CONTENT_ID_EXTRA, "");
                bundle.putInt(DemoUtil.CONTENT_TYPE_EXTRA, 2);
                
//                startActivity(intent);
                FragmentManager fm = getFragmentManager();
                FragmentTransaction transaction = fm.beginTransaction();
                
                Fragment video_fragment = null;
                
                String prop = Util.getSysProperty("media.omx.player", "exo");
                Log.d(LOG_TAG, "media.omx.player = "+prop);
                if(prop.equals("exo"))
                {
                    Log.d(LOG_TAG, "PLAY video using exoplayer");
                    video_fragment = new SimplePlayerFragment();    
                }
                else
                {
                    Log.d(LOG_TAG, "PLAY video using naked MediaCodec");
                    video_fragment = new DecoderFragment();
                }
                video_fragment.setArguments(bundle);
                Log.d(LOG_TAG, "listview.id="+l.getId());
                Log.d(LOG_TAG, "view.id="+v.getId());
                Log.d(LOG_TAG, "this.getId()="+this.getId());
                switch(this.getId())
                {
                case R.id.filelist1:
                    transaction.replace(R.id.video1_container, video_fragment);
                    break;
                case R.id.filelist2:
                    transaction.replace(R.id.video2_container, video_fragment);
                    break;
                default:
                    Log.d(LOG_TAG, "why we are here?");
                }
                transaction.commit();
            }
    }

//    @Override
//    public boolean onKeyDown(int keyCode, KeyEvent event)
//    {
//        if (keyCode == KeyEvent.KEYCODE_BACK)
//        {
//            if (listAllFiles)
//            {
//                FileListFragment.this.finish();
//                return true;
//            }
//
//            if (paths == null)
//            {
//                FileListFragment.this.finish();
//                PlayList.getinstance().rootPath = null;
//            }
//            else
//            {
//                if (paths.isEmpty())
//                {
//                    FileListFragment.this.finish();
//                    PlayList.getinstance().rootPath = null;
//                }
//                file = new File(paths.get(0).toString());
//                if (file.getParent().compareTo(iso_mount_dir) == 0 && ISOpath != null)
//                {
//                    file = new File(ISOpath + "/VIRTUAL_CDROM");
//                    Log.i(TAG, "[onKeyDown]file:" + file);
//                    ISOpath = null;
//                }
//                currenturl = file.getParentFile().getParent();
//                if ((file.getParent().compareToIgnoreCase(root_path) != 0) && (pathLevel > 0))
//                {
//                    String path = file.getParent();
//                    String parent_path = file.getParentFile().getParent();
//                    if ((path.equals(NAND_PATH) || path.equals(SD_PATH) || parent_path.equals(USB_PATH)) && (pathLevel > 0))
//                    {
//                        pathLevel = 0;
//                        BrowserFile(ROOT_PATH);
//                    }
//                    else
//                    {
//                        BrowserFile(currenturl);
//                        pathLevel--;
//                        getListView().setSelectionFromTop(fileDirectory_position_selected.get(pathLevel), fileDirectory_position_piexl.get(pathLevel));
//                        fileDirectory_position_selected.remove(pathLevel);
//                        fileDirectory_position_piexl.remove(pathLevel);
//                    }
//                }
//                else
//                {
//                    FileListFragment.this.finish();
//                    PlayList.getinstance().rootPath = null;
//                }
//            }
//            return true;
//        }
//        return super.onKeyDown(keyCode, event);
//    }

//    private void showvideobar()
//    {
//        // * new an Intent object and ponit a class to start
//        Intent intent = new Intent();
//        Bundle bundle = new Bundle();
//        if (!listAllFiles)
//        {
//            bundle.putInt("item_position_selected", item_position_selected);
//            bundle.putInt("item_position_first", item_position_first);
//            bundle.putInt("fromtop_piexl", fromtop_piexl);
//            bundle.putIntegerArrayList("fileDirectory_position_selected", fileDirectory_position_selected);
//            bundle.putIntegerArrayList("fileDirectory_position_piexl", fileDirectory_position_piexl);
//        }
//        bundle.putBoolean("backToOtherAPK", false);
//        intent.setClass(FileListFragment.this, FileListFragment.class);
//        intent.putExtras(bundle);
//
//        // /wxl delete
//        /*
//         * SettingsVP.setSystemWrite(sw); if(SettingsVP.chkEnableOSD2XScale() ==
//         * true) this.setVisible(false);
//         */
//
//        startActivity(intent);
//        FileListFragment.this.finish();
//    }

    public int filterDir(File file)
    {
        int pos = -1;
        File[] the_Files;
        File parent = new File(file.getParent());
        the_Files = parent.listFiles(new MyFilter(extensions));

        if (the_Files == null)
            return pos;

        pos = 0;
        listVideos = new ArrayList<File>();
        for (int i = 0; i < the_Files.length; i++)
        {
            File tempF = the_Files[i];

            if (tempF.isFile())
            {
                listVideos.add(tempF);
            }
        }

        paths = new ArrayList<String>();
        File[] fs = new File[listVideos.size()];
        for (int i = 0; i < listVideos.size(); i++)
        {
            fs[i] = listVideos.get(i);
        }

        try
        {
            Arrays.sort(fs, new MyComparator(MyComparator.NAME_ASCEND));
        }
        catch (IllegalArgumentException ex)
        {
        }

        for (int i = 0; i < fs.length; i++)
        {
            File tempF = fs[i];
            if (tempF.getPath().equals(file.getPath()))
            {
                pos = i;
            }
            paths.add(tempF.getPath());
        }
        return pos;
    }

    // option menu
    private final int MENU_ABOUT = 0;

//    @Override
//    public boolean onCreateOptionsMenu(Menu menu)
//    {
//        menu.add(0, MENU_ABOUT, 0, R.string.str_about);
//        return true;
//    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId())
        {
        case MENU_ABOUT:
            try
            {
                Toast.makeText(FileListFragment.this.getActivity(), " VideoPlayer \n Version: " + FileListFragment.this.getActivity().getPackageManager().getPackageInfo("com.meson.videoplayer", 0).versionName, Toast.LENGTH_SHORT).show();
            }
            catch (NameNotFoundException e)
            {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            return true;
        }
        return false;
    }

    public void reScanVideoFiles()
    {
        Intent intent = new Intent(Intent.ACTION_MEDIA_MOUNTED, Uri.parse("file://" + ROOT_PATH));
        this.getActivity().sendBroadcast(intent);
    }

    public void stopMediaPlayer()// stop the backgroun music player
    {
        Intent intent = new Intent();
        intent.setAction("com.android.music.musicservicecommand.pause");
        intent.putExtra("command", "stop");
        this.getActivity().sendBroadcast(intent);
    }
}
