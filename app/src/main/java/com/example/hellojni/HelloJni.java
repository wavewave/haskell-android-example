/*
 * Copyright (C) 2009 The Android Open Source Project
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
package com.example.hellojni;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toolbar;
import android.view.View;
import android.view.View.OnClickListener;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.CoordinatorLayout;
import android.util.Log;

public class HelloJni extends Activity
{
    
    Button button;
    TextView  tv;
    ListView lv;
    FloatingActionButton fab;

    
    Toolbar toolbar;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.main);
	toolbar = (Toolbar)findViewById(R.id.toolbar);
	toolbar.setTitle("My toolbar");
	toolbar.setSubtitle("Subtitle");
	//setActionBar(toolbar);
	toolbar.inflateMenu(R.menu.toolbar); 
	
        // onCreateHS(savedInstanceState);
        /*


	
        //tv.setText( stringFromJNI() );
	tv = (TextView) findViewById(R.id.textview1);
	onCreateHS(tv);

	

	addListenerOnButton(); 
	//stringFromJNI();
        //setContentView(tv); */
    }

    /*
    public void addListenerOnButton() {
        button = (Button) findViewById(R.id.button1);
	button.setOnClickListener(new OnClickListener() {
            @Override
	    public void onClick(View arg0) {

                // Log.d("HELLOJNI", "button clicked");
		onClickHS(tv);

		
	    }
	});
							

	} */
       
    /* A native method that is implemented by the
     * 'hello-jni' native library, which is packaged
     * with this application.
     */
    //public native String stringFromJNI();
    public native void onCreateHS(TextView tv); // (Bundle savedInstanceState);

    public native void onClickHS(TextView tv); // (Bundle savedInstanceState);

    /* This is another native method declaration that is *not*
     * implemented by 'hello-jni'. This is simply to show that
     * you can declare as many native methods in your Java code
     * as you want, their implementation is searched in the
     * currently loaded native libraries only the first time
     * you call them.
     *
     * Trying to call this function will result in a
     * java.lang.UnsatisfiedLinkError exception !
     */
    public native String  stringFromJNI(); // (Bundle savedInstanceState);

    public native String  unimplementedStringFromJNI();

    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.hellojni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("haskell");
    }
}
