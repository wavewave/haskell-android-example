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
package com.uphere.chatter;

import android.app.Activity;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.FragmentManager;
import android.content.Context;
import android.content.Intent;
import android.content.DialogInterface;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.MessageQueue;
import android.text.method.ScrollingMovementMethod;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toolbar;
import android.widget.VideoView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.util.Log;
import java.io.IOException;
import java.util.List;



    
public class Chatter extends Activity
{
    private class NicknameDialogFragment extends DialogFragment {
	// private String result;
	private EditText input; 
	
	@Override
	public Dialog onCreateDialog(Bundle savedInstanceState) {
	    AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
	    LayoutInflater i = getLayoutInflater();
	    View view = i.inflate(R.layout.dialog, null);
	    
	    builder.setMessage("Set your nickname")
		.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
			    Dialog f = (Dialog) dialog;
			    input = (EditText)f.findViewById(R.id.edit_nickname );
			    nickname = input.getText().toString();
			    // Log.d("HELLOJNI", result);
			    
			}
		    })
		.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
			    dialog.dismiss();
			}
		    });
	    builder.setView(view);
	    
	    return builder.create();
	}
	
    }

    private class IdleHandler implements MessageQueue.IdleHandler {
	private Looper _looper;
	private int count;
	protected IdleHandler(Looper looper) {
	    _looper = looper;
	    count = 0;
	}
	public boolean queueIdle() {
	    onIdleHS(tv);
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
    //FloatingActionButton fab;
    VideoView vv; 

    Toolbar toolbar;

    public String nickname;
     
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.sub);


	toolbar = (Toolbar)findViewById(R.id.toolbar2);
	toolbar.setTitle("Chat");
	toolbar.setSubtitle("This uses chatter haskell program.");


	tv = (TextView) findViewById(R.id.textview2);
	tv.setMovementMethod(ScrollingMovementMethod.getInstance());
	onCreateHS(tv);
	

	//fab = (FloatingActionButton)findViewById(R.id.favorite2);
	button = (Button)findViewById(R.id.button1);
        button.setOnClickListener( new View.OnClickListener() {
		@Override
		public void onClick(View view) {
                    Log.d("HELLOJNI" , "nickname = " + nickname);
		    if(nickname != null) { 
        		onClickHS(tv,nickname);
		    }
		}

	    });
	
	FragmentManager fm = getFragmentManager();
	NicknameDialogFragment n = new NicknameDialogFragment();
	n.show(fm,"fragment_nickname");

        // nickname = n.result ; 
	// Log.d("HELLOJNI" , "nickname = " + nickname);
	ProcessEvents();
	    
    }

    public native void onCreateHS(TextView tv);

    public native void onClickHS(TextView tv, String str);

    public native void onIdleHS(TextView tv);
    
    static {
        System.loadLibrary("haskell");
    }


}
