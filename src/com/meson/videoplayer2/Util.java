package com.meson.videoplayer2;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class Util
{
    public static String getSysProperty(String propName, String defaultValue)
    {
        String propValue = null;
        Class<?> clazz = null;
        try
        {
            clazz = Class.forName("android.os.SystemProperties");
        }
        catch (ClassNotFoundException e1)
        {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
        Method method = null;
        try
        {
            method = clazz.getDeclaredMethod("get", String.class);
        }
        catch (NoSuchMethodException e1)
        {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }

        try
        {
            propValue = (String) method.invoke(null, propName);
        }
        catch (IllegalArgumentException e1)
        {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
        catch (IllegalAccessException e1)
        {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
        catch (InvocationTargetException e1)
        {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
        
        if(propValue == null)
            return defaultValue;
        else
            return propValue;
    }
}
