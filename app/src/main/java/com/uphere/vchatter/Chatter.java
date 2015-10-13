package com.uphere.vchatter;

import android.app.Activity;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.Intent;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.MessageQueue;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.util.Log;
import java.io.IOException;
import java.util.List;


import com.uphere.vchatter.NicknameDialogFragment;
import com.uphere.vchatter.VideoFragment;

public class Chatter extends Activity
{
    NicknameDialogFragment dialog;
    VideoFragment vfrag;
    
    public String nickname;

    /*    Button button;
    TextView  tv;

    Toolbar toolbar;
    private EditText msginput;
    */

    Camera mCamera;
    CameraPreview mPreview;
    private View mCameraView;

    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.chatter);
	onCreateHS();
	
	FragmentManager fm = getFragmentManager();
        
        FragmentTransaction ft = fm.beginTransaction();
	vfrag = new VideoFragment(this);
        ft.add(R.id.fragment_container,vfrag).commit();

	dialog = new NicknameDialogFragment(this);
	dialog.show(fm,"fragment_nickname");

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

    

    public native void onCreateHS(); 
    
    public void sendMsgToChatter( String msg ) {
	final String msg1 = msg;
	runOnUiThread( new Runnable() {
		public void run() { 
		    vfrag.tv.append(msg1);
		}
	    }); 
    }
    
   
    static {
	System.loadLibrary("haskell");
    }


}
