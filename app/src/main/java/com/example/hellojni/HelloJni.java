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
import android.hardware.Camera;
import android.os.Bundle;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toolbar;
import android.widget.VideoView;
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


public class HelloJni extends Activity
{
    
    Button button;
    TextView  tv;
    ListView lv;
    FloatingActionButton fab;
    VideoView vv; 


    Camera mCamera;
    CameraPreview mPreview;
    private View mCameraView;    

    
    Toolbar toolbar;

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
	
        fab = (FloatingActionButton) findViewById(R.id.favorite);

	tv = (TextView) findViewById(R.id.textview1);
	onCreateHS(tv);
	addListenerOnButton();


        //vv = (VideoView) findViewById(R.id.myvideo);
	//String vaddr = "http://ianwookim.org/video/test.mp4";
	//Uri vuri= Uri.parse(vaddr);
	//vv.setVideoURI(vuri);
	//vv.start();

        mCameraView = (View) findViewById(R.id.camera_preview);
  
	
	//boolean opened = safeCameraOpenInView( mCameraView );

	//if(opened == false ) {
	//    Log.d("CameraGuide", "Error, Camera failed to open");
	//}



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

    
    public void addListenerOnButton() {
        //fab = (FloatingActionButton) findViewById(R.id.favorite);
	fab.setOnClickListener(new OnClickListener() {
            @Override
	    public void onClick(View view) {
		//onClickHS(tv);
                Intent act2 = new Intent(view.getContext(),Sub.class);
		startActivity(act2);
 	    }
	});
							

    } 
       
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
}
