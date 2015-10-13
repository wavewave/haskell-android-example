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

import com.uphere.vchatter.CameraFragment;
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

    //Camera mCamera;
    //CameraPreview mPreview;
    //private View mCameraView;

    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.chatter);
	onCreateHS();
	
	FragmentManager fm = getFragmentManager();
        
        FragmentTransaction ft = fm.beginTransaction();
	vfrag = new VideoFragment(this);
        ft.add(R.id.fragment_container,vfrag);
        // CameraFragment cfrag = new CameraFragment();
	// ft.add(R.id.fragment_container,cfrag);
	ft.commit();
	
	
	dialog = new NicknameDialogFragment(this);
	dialog.show(fm,"fragment_nickname");

    }
    
    public native void onCreateHS(); 
    
    public void sendMsgToChatter( String msg ) {
	final String msg1 = msg;
	final Runnable myrun = new Runnable() {
		public void run() {
		    //Log.d("UPHERE", "sendMsgToChatter : " + msg1); 
		    vfrag.tv.append(msg1);
		    vfrag.sv.post( new Runnable() {
			    public void run() {
				vfrag.sv.fullScroll(View.FOCUS_DOWN);
			    }
				
			});
		    //synchronized(this) {
		    //	this.notify();
		    //}
		}
	    };

	//synchronized( myrun ) { 
	    runOnUiThread(myrun);
	    //   try {     
	    //	myrun.wait();
	    //  }
	    // catch (InterruptedException e) {
	    //e.printStackTrace();
	    // }
	    //}
	
    }
    
   
    static {
	System.loadLibrary("haskell");
    }


}
