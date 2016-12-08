package org.wooyd.android.JNIExample;

import android.os.Handler;
import android.os.Bundle;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;

import org.wooyd.android.JNIExample.Data;

public class JNIExampleInterface {
    static Handler h;
    public JNIExampleInterface() {}
    public JNIExampleInterface(Handler h) {
        this.h = h;
    }
    public static native void callVoid();
    public static native Data getNewData(int i, String s);
    public static native String getDataString(Data d);
    public static void callBack(String s) {
        Log.i("JNIExampleInterface", "callBack() s:　" + s);

        Bundle b = new Bundle();
        b.putString("callback_string", s);
        Message m = Message.obtain();
        m.setData(b);
        m.setTarget(h);
        m.sendToTarget();
    }

    public static native VideoPresetAdapter getPresetData();
    public static native int getVideoPresetData(VideoPresetAdapter data);
    public static native int setPresetData(VideoPresetAdapter data);
}
