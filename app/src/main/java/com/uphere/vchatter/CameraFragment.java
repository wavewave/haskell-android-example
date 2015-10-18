package com.uphere.vchatter;

import java.io.IOException;
import java.util.List;
//
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.Camera;
import android.os.Bundle;
//import android.os.Handler;
//import android.os.Looper;
//import android.os.MessageQueue;
import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
//
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v4.app.Fragment;
//
import com.uphere.vchatter.NicknameDialogFragment;
import com.uphere.vchatter.VideoFragment;

public class CameraFragment extends Fragment
{

    Camera mCamera;
    CameraPreview mPreview;
    private View mCameraView;

    public CameraFragment() {
	
    }
    
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
			       Bundle savedInstanceState) {
	View rootView = inflater.inflate(R.layout.camera, container, false); 

	/* toolbar = (Toolbar)rootView.findViewById(R.id.toolbar);
	toolbar.setTitle("Chat");
	toolbar.setSubtitle("This uses chatter haskell program."); */

        mCameraView = (View) rootView.findViewById(R.id.camera_preview);
	boolean opened = safeCameraOpenInView( mCameraView );

	if(opened == false ) {
	    Log.d("UPHERE", "Error, Camera failed to open");
	}

	return rootView; 
    }

    public boolean safeCameraOpenInView(View view) {
	boolean qOpened = false;
	releaseCameraAndPreview();
	mCamera = getCameraInstance();
	mCameraView = view;
	qOpened = (mCamera != null);

	if( qOpened == true ) {
	    mPreview = new CameraPreview(view.getContext(), mCamera, view);
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


}
