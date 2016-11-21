package org.wooyd.android.JNIExample;
    import android.app.Activity;
    import android.view.View;
    import android.widget.Button;
    import android.widget.TextView;
    import android.os.Bundle;
    import android.os.Handler;
    import android.os.Message;
    import java.util.zip.*;
    import java.io.InputStream;
    import java.io.OutputStream;
    import java.io.FileOutputStream;
    import java.io.File;
    import org.wooyd.android.JNIExample.JNIExampleInterface;
    import org.wooyd.android.JNIExample.Data;
    import android.util.Log;
public class JNIExample extends Activity
{
    TextView callVoidText, getNewDataText, getDataStringText;
    Button callVoidButton, getNewDataButton, getDataStringButton;
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
    public void onCreate(Bundle savedInstanceState) 
    {
         super.onCreate(savedInstanceState);
         setContentView(R.layout.main);

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
    }
}
