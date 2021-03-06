package com.meson.videoplayer2;

import java.io.File;
import java.io.FileFilter;
import java.util.StringTokenizer;

public class MyFilter implements FileFilter
{
    private String extensions;

    public MyFilter(String extensions)
    {
        this.extensions = extensions;
    }

    @Override
    public boolean accept(File file)
    {
        StringTokenizer st = new StringTokenizer(this.extensions, ",");
        if (file.isDirectory())
        {
            return true;
        }

        String name = file.getName();
        int index = name.lastIndexOf(".");
        if (index == -1)
        {
            return false;
        }
        else
            if (index == (name.length() - 1))
            {
                return false;
            }
            else
            {
                // for(int i = 0; i<st.countTokens(); i++)
                while (st.hasMoreElements())
                {
                    String extension = st.nextToken();
                    if (extension.equals(name.substring(index + 1).toLowerCase()))
                        return true;
                }
                return false;
            }
    }
}
