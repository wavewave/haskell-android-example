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
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.MessageQueue;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toolbar;
import android.widget.VideoView;

import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.util.Log;
import java.io.IOException;
import java.util.List;



    
public class Sub extends Activity
{
    private class IdleHandler implements MessageQueue.IdleHandler {
	private Looper _looper;
	private int count;
	protected IdleHandler(Looper looper) {
	    _looper = looper;
	    count = 0;
	}
	public boolean queueIdle() {
	    Log.d("HELLOJNI", "queueIdle called : " + count);
	    count++;
	    return(true);
	}
       
    }

    public void ProcessEvents()
    {
	Log.d("HELLOJNI", "ProcessEvents");
        Looper looper = Looper.myLooper();
        looper.myQueue().addIdleHandler(new IdleHandler(looper));	
    }
    
    Button button;
    TextView  tv;
    ListView lv;
    FloatingActionButton fab;
    VideoView vv; 

    Toolbar toolbar;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.sub);
	//toolbar = (Toolbar)findViewById(R.id.toolbar);
	//toolbar.setTitle("My toolbar");
	//toolbar.setSubtitle("Subtitle");
	////setActionBar(toolbar);
	//toolbar.inflateMenu(R.menu.toolbar); 

        //vv = (VideoView) findViewById(R.id.myvideo);
	//String vaddr = "http://ianwookim.org/video/test.mp4";
	//Uri vuri= Uri.parse(vaddr);
	//vv.setVideoURI(vuri);
	//vv.start();


	
	fab = (FloatingActionButton)findViewById(R.id.favorite2);
        fab.setOnClickListener( new View.OnClickListener() {
		@Override
		public void onClick(View view) {
		    onClickHS(tv);
		    //		    Intent act1 = new Intent(view.getContext(),HelloJni.class);
		    // startActivity(act1);
		}

	    });
	ProcessEvents();
	    
    }

    
    /*    public void addListenerOnButton() {
        //fab = (FloatingActionButton) findViewById(R.id.favorite);
	fab.setOnClickListener(new OnClickListener() {
            @Override
	    public void onClick(View view) {
		onClickHS(tv);
		//Snackbar.make(tv, "hello", Snackbar.LENGTH_LONG).show();
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

}
