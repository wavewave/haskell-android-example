package com.uphere.vchatter;

import java.io.UnsupportedEncodingException;
import java.io.IOException;
import java.util.List;
//
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

import com.uphere.vchatter.ObjectRegisterer; 
import com.uphere.vchatter.CameraFragment;
import com.uphere.vchatter.NicknameDialogFragment;
import com.uphere.vchatter.VideoFragment;

public class Chatter extends Activity
{
    NicknameDialogFragment dialog;
    VideoFragment vfrag;
    
    public String nickname;


    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.chatter);
	onCreateHS(0);
	ObjectRegisterer.registerJRef(0,this);
	
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
    
    public native void onCreateHS( int k ); 

    public void sendMsgToChatter( byte[] msg ) {  // ( String msg ) {
	try { 
	    final String msg1 = new String(msg, "UTF-8");
	    final Runnable myrun = new Runnable() {
		    public void run() {
			vfrag.tv.append(msg1);
			vfrag.sv.post( new Runnable() {
				public void run() {
				    vfrag.sv.fullScroll(View.FOCUS_DOWN);
				}
				
			    });
		    }
		};
	    runOnUiThread(myrun);
	    
	} catch(UnsupportedEncodingException e) {
	}

    }
    
   
    static {
	System.loadLibrary("haskell");
    }


}
