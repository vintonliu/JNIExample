package org.wooyd.android.JNIExample;

import android.app.Activity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import java.util.ArrayList;
import java.util.zip.*;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.File;

import org.wooyd.android.JNIExample.JNIExampleInterface;
import org.wooyd.android.JNIExample.Data;

import android.util.Log;

public class JNIExample extends Activity {
    TextView callVoidText, getNewDataText, getDataStringText;
    Button callVoidButton, getNewDataButton, getDataStringButton, getPresetDataButton;
    Button setPresetDataButton;
    Button getDataList, setDataList;
    Handler callbackHandler;
    JNIExampleInterface jniInterface;

    static {
        try {
            System.loadLibrary("jniexample");
        } catch (Exception ex) {
            Log.e("JNIExample", "failed to install native library: " + ex);
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        VideoPresetAdapter preset = new VideoPresetAdapter();
        preset.low = preset.new VideoPresetLowCpu();
        preset.medium = preset.new VideoPresetMediumCpu();
        preset.high = preset.new VideoPresetHighCpu();

        Log.i("JNIExample", "low complexity");
        Log.i("JNIExample", preset.low.complexity_w240
                + " " + preset.low.complexity_w360
                + " " + preset.low.complexity_w480);
        Log.i("JNIExample", "medium complexity");
        Log.i("JNIExample", preset.medium.complexity_w240
                + " " + preset.medium.complexity_w360
                + " " + preset.medium.complexity_w480);
        Log.i("JNIExample", "high complexity");
        Log.i("JNIExample", preset.high.complexity_w240
                + " " + preset.high.complexity_w360
                + " " + preset.high.complexity_w480);
        Log.i("JNIExample", "set value");
        preset.low.complexity_w240 = 0;
        preset.low.complexity_w360 = 0;
        preset.low.complexity_w480 = -1;
        preset.medium.complexity_w240 = 1;
        preset.medium.complexity_w360 = 1;
        preset.medium.complexity_w480 = -1;
        preset.high.complexity_w240 = 5;
        preset.high.complexity_w360 = 5;
        preset.high.complexity_w480 = -1;
        Log.i("JNIExample", "low complexity");
        Log.i("JNIExample", preset.low.complexity_w240
                + " " + preset.low.complexity_w360
                + " " + preset.low.complexity_w480);
        Log.i("JNIExample", "medium complexity");
        Log.i("JNIExample", preset.medium.complexity_w240
                + " " + preset.medium.complexity_w360
                + " " + preset.medium.complexity_w480);
        Log.i("JNIExample", "high complexity");
        Log.i("JNIExample", preset.high.complexity_w240
                + " " + preset.high.complexity_w360
                + " " + preset.high.complexity_w480);

        callVoidText = (TextView) findViewById(R.id.callVoid_text);

        callbackHandler = new Handler() {
            public void handleMessage(Message msg) {
                Bundle b = msg.getData();
                callVoidText.setText(b.getString("callback_string"));
            }
        };

        jniInterface = new JNIExampleInterface(callbackHandler);
        callVoidButton = (Button) findViewById(R.id.callVoid_button);
        callVoidButton.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                jniInterface.callVoid();
            }
        });

        getNewDataText = (TextView) findViewById(R.id.getNewData_text);
        getNewDataButton = (Button) findViewById(R.id.getNewData_button);
        getNewDataButton.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Data d = jniInterface.getNewData(42, "foo");
                getNewDataText.setText(
                        "getNewData(42, \"foo\") == Data(" + d.i + ", \"" + d.s + "\")");
            }
        });

        getDataStringText = (TextView) findViewById(R.id.getDataString_text);
        getDataStringButton = (Button) findViewById(R.id.getDataString_button);
        getDataStringButton.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Data d = new Data(43, "bar");
                String s = jniInterface.getDataString(d);
                getDataStringText.setText(
                        "getDataString(Data(43, \"bar\")) == \"" + s + "\"");
            }
        });

        getPresetDataButton = (Button)findViewById(R.id.getPresetData_button);
        getPresetDataButton.setOnClickListener(new Button.OnClickListener(){
            @Override
            public void onClick(View v) {
                VideoPresetAdapter presetData = new VideoPresetAdapter();
                presetData.low = presetData.new VideoPresetLowCpu();
                presetData.medium = presetData.new VideoPresetMediumCpu();
                presetData.high = presetData.new VideoPresetHighCpu();
                if ( jniInterface.getVideoPresetData(presetData) == 0) {
                    Log.i("JNIExample", "Jni getVideoPresetData()");
                    Log.i("JNIExample", "low complexity");
                    Log.i("JNIExample", presetData.low.complexity_w240
                            + " " + presetData.low.complexity_w360
                            + " " + presetData.low.complexity_w480);
                    Log.i("JNIExample", "medium complexity");
                    Log.i("JNIExample", presetData.medium.complexity_w240
                            + " " + presetData.medium.complexity_w360
                            + " " + presetData.medium.complexity_w480);
                    Log.i("JNIExample", "high complexity");
                    Log.i("JNIExample", presetData.high.complexity_w240
                            + " " + presetData.high.complexity_w360
                            + " " + presetData.high.complexity_w480);
                }
            }
        });

        setPresetDataButton = (Button) findViewById(R.id.setPresetData_button);
        setPresetDataButton.setOnClickListener(new Button.OnClickListener(){
            @Override
            public void onClick(View v) {
                VideoPresetAdapter preset = new VideoPresetAdapter();
                preset.low = preset.new VideoPresetLowCpu();
                preset.medium = preset.new VideoPresetMediumCpu();
                preset.high = preset.new VideoPresetHighCpu();

                Log.i("JNIExample", "set value");
                preset.low.complexity_w240 = 1;
                preset.low.complexity_w360 = 1;
                preset.low.complexity_w480 = -2;

                Log.i("JNIExample", "low complexity");
                Log.i("JNIExample", preset.low.complexity_w240
                        + " " + preset.low.complexity_w360
                        + " " + preset.low.complexity_w480);

                if (jniInterface.setPresetData(preset) > 0) {
                    Log.i("JNIExample", "setPresetData success");
                    VideoPresetAdapter presetData = jniInterface.getPresetData();
                    Log.i("JNIExample", "Jni getPresetData()");
                    Log.i("JNIExample", "low complexity");
                    Log.i("JNIExample", presetData.low.complexity_w240
                            + " " + presetData.low.complexity_w360
                            + " " + presetData.low.complexity_w480);
                }
            }
        });

        getDataList = (Button) findViewById(R.id.getDataList_button);
        getDataList.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ArrayList<Data> dataList = new ArrayList();
                        jniInterface.getDataList(dataList);
                for (int i = 0; i < dataList.size(); i++) {
                    Data data = dataList.get(i);
                    Log.i("JNIExample", "idx: " + i + " Data: " + data.i + " " + data.s);
                }
            }
        });

        setDataList = (Button) findViewById(R.id.setDataList_button);
        setDataList.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ArrayList dataList = new ArrayList();
                for (int i = 0; i < 3; i++) {
                    Data data = new Data(10 + i, "foo " + i * 10);
                    dataList.add(data);
                }

                jniInterface.setDataList(dataList);
            }
        });
    }
}
