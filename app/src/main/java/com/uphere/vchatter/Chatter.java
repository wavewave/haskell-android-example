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
package com.uphere.vchatter;

import android.app.Activity;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.FragmentManager;
import android.content.Context;
import android.content.Intent;
import android.content.DialogInterface;
import android.graphics.Color;
import android.hardware.Camera;
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
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
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

    Button button;
    TextView  tv;

    Toolbar toolbar;
    private EditText msginput;
    public String nickname;


    Camera mCamera;
    CameraPreview mPreview;
    private View mCameraView;
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.chatter);
	onCreateHS();

	toolbar = (Toolbar)findViewById(R.id.toolbar);
	toolbar.setTitle("Chat");
	toolbar.setSubtitle("This uses chatter haskell program.");


	tv = (TextView) findViewById(R.id.textview);
	tv.setMovementMethod(ScrollingMovementMethod.getInstance());
        tv.setTextColor(Color.WHITE);
	
	msginput = (EditText) findViewById(R.id.edit_msg);
        msginput.setTextColor(Color.WHITE);
	msginput.setHintTextColor(Color.GRAY);
	
	button = (Button)findViewById(R.id.button);
        button.setOnClickListener( new View.OnClickListener() {
		@Override
		public void onClick(View view) {
		    String msg = msginput.getText().toString();
		    if(nickname != null && msg != null) { 
        		onClickHS(tv,nickname,msg);
			msginput.setText("");
		    }
		}

	    });
	

        mCameraView = (View) findViewById(R.id.camera_preview);
	boolean opened = safeCameraOpenInView( mCameraView );

	if(opened == false ) {
	    Log.d("UPHERE", "Error, Camera failed to open");
	}

	FragmentManager fm = getFragmentManager();
	NicknameDialogFragment n = new NicknameDialogFragment();
	n.show(fm,"fragment_nickname");
	    
    }

    public boolean safeCameraOpenInView(View view) {
	boolean qOpened = false;
	releaseCameraAndPreview();
	mCamera = getCameraInstance();
	mCameraView = view;
	qOpened = (mCamera != null);

	if( qOpened == true ) {
	    mPreview = new CameraPreview(this.getBaseContext(), mCamera, view);
	    FrameLayout preview = (FrameLayout)view;
	    preview.addView(mPreview);
	    mPreview.startCameraPreview();
	}

	return qOpened;
    }

    public static Camera getCameraInstance() {
	Camera c = null;
	try {
	    c = Camera.open();
	}
	catch (Exception e) {
	    e.printStackTrace();
	}
	return c;
    }

    private void releaseCameraAndPreview() {
	if (mCamera != null) {
	    mCamera.stopPreview();
	    mCamera.release();
	    mCamera = null;
	}
	if (mPreview != null) {
	    mPreview.destroyDrawingCache();
	    mPreview.mCamera = null;
	}
    }


    class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {
	private SurfaceHolder mHolder;
	private Camera mCamera;
	private Context mContext;
	private Camera.Size mPreviewSize;
	private View mCameraView;
	private List<Camera.Size> mSupportedPreviewSizes;
	private List<String> mSupportedFlashModes;

	public CameraPreview( Context context, Camera camera, View cameraView ) {
	    super(context);
	    mCameraView = cameraView;
	    mContext = context;
	    setCamera(camera);
	    mHolder = getHolder();
	    mHolder.addCallback(this);
	    mHolder.setKeepScreenOn(true);
	}

	public void startCameraPreview()
	{
	    try {
		mCamera.setPreviewDisplay(mHolder);
		mCamera.startPreview();
	    }
	    catch(Exception e) {
		e.printStackTrace();
	    }
	}

	private void setCamera( Camera camera )
	{
	    mCamera = camera;
	    mSupportedPreviewSizes = mCamera.getParameters().getSupportedPreviewSizes();
	    mSupportedFlashModes = mCamera.getParameters().getSupportedFlashModes();

	    if(mSupportedFlashModes != null && mSupportedFlashModes.contains(Camera.Parameters.FLASH_MODE_AUTO)) {
		Camera.Parameters parameters = mCamera.getParameters();
		parameters.setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);
		mCamera.setParameters(parameters);
		mCamera.setDisplayOrientation(90);

	    }
	    requestLayout();
	}


	public void surfaceCreated( SurfaceHolder holder ) {
	    try {
		mCamera.setPreviewDisplay(holder);
	    } catch (IOException e) {
		e.printStackTrace();
	    }
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
	    if(mCamera != null) {
	    }
	}

	public void surfaceChanged( SurfaceHolder holder, int format, int w, int h ) {
	}

    } 

    
    public native void onClickHS(TextView tv, String nick, String msg);

    public native void onCreateHS(); 
    
    public void sendMsgToChatter( String msg ) {
	final String msg1 = msg;
	runOnUiThread( new Runnable() {
		public void run() { 
		    tv.append(msg1);
		}
	    });
    }
    
   
    static {
	System.loadLibrary("haskell");
    }


}
