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
	Log.d("UPHERE", "ProcessEvents");
        Looper looper = Looper.myLooper();
        looper.myQueue().addIdleHandler(new IdleHandler(looper));	
    }
    
    Button button;
    TextView  tv;

    Toolbar toolbar;
    private EditText msginput;
    public String nickname;
     
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.chatter);
        startHS();

	toolbar = (Toolbar)findViewById(R.id.toolbar);
	toolbar.setTitle("Chat");
	toolbar.setSubtitle("This uses chatter haskell program.");


	tv = (TextView) findViewById(R.id.textview);
	tv.setMovementMethod(ScrollingMovementMethod.getInstance());
	// onCreateHS(tv);

	msginput = (EditText) findViewById(R.id.edit_msg);

	button = (Button)findViewById(R.id.button);
        button.setOnClickListener( new View.OnClickListener() {
		@Override
		public void onClick(View view) {
		    String msg = msginput.getText().toString();
		    if(nickname != null && msg != null) { 
        		//onClickHS(tv,nickname,msg);
			msginput.setText("");
		    }
		}

	    });
	
	FragmentManager fm = getFragmentManager();
	NicknameDialogFragment n = new NicknameDialogFragment();
	n.show(fm,"fragment_nickname");

	//ProcessEvents();
	    
    }

    public native void startHS();
     
    public native void onCreateHS(TextView tv);

    public native void onClickHS(TextView tv, String nick, String msg);

    public native void onIdleHS(TextView tv);
    
    static {
	System.loadLibrary("haskell");
    }


}
