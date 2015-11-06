package com.uphere.vchatter;

import java.io.UnsupportedEncodingException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
//
import android.app.Activity;
//import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
//import android.hardware.Camera;
import android.os.Bundle;
import android.os.Handler;
//import android.os.Looper;
//import android.os.MessageQueue;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;

import android.view.Choreographer;
import android.view.LayoutInflater;
//import android.view.Surface;
//import android.view.SurfaceHolder;
//import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
//
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.util.Log;



import com.uphere.vchatter.Bridge; 
import com.uphere.vchatter.CameraFragment;
import com.uphere.vchatter.MapFragment;
import com.uphere.vchatter.NicknameDialogFragment;
import com.uphere.vchatter.VideoFragment;


public class Chatter extends FragmentActivity
{
    NicknameDialogFragment dialog;
    VideoFragment vfrag;
    CameraFragment cfrag;
    MapFragment mapFragment;
    ViewPagerAdapter adapter;
    ViewPager vp; 
    
    public String nickname;

    int n = 0;
    Choreographer mChoreographer;
    Choreographer.FrameCallback mFrameCallback;

    ArrayList<String> mMsgbox;
    
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
	
        super.onCreate(savedInstanceState);
	mMsgbox = new ArrayList<String>();
	setContentView(R.layout.chatter);
	onCreateHS(0);
	Bridge.registerJRef(0,this);
        FragmentManager fm = getSupportFragmentManager();

        vp = (ViewPager) findViewById(R.id.viewpager);
	setupViewPager(fm,vp);
        mChoreographer = Choreographer.getInstance();
        mFrameCallback = new Choreographer.FrameCallback() {
		@Override public void doFrame( long frameTimeNanos ) {
		    Bridge.onFrameHS();
		    n = n+1;
                    mChoreographer.postFrameCallback(mFrameCallback);
		}
	    };
        mChoreographer.postFrameCallback(mFrameCallback);

	
	// final OnMapReadyCallback listener = this;
	//new Handler().postDelayed(new Runnable() {
	//	@Override
	//		public void run() {
	//	    
	//          mapFragment.getMapAsync(listener);
	//	}
	//   }, 1000);
	
	/* FragmentManager fm = getFragmentManager();
        
        FragmentTransaction ft = fm.beginTransaction();
        ft.add(R.id.fragment_container,vfrag);
	ft.commit();
	
	
        */

	dialog = new NicknameDialogFragment(this);
	dialog.show(fm,"fragment_nickname"); 
    }

    private void setupViewPager(FragmentManager fm, ViewPager viewPager) {
	adapter = new ViewPagerAdapter(fm);
	// cfrag = new CameraFragment();
	// adapter.addFrag(cfrag, "CAMERA" );
	adapter.addFrag(new DummyFragment(Color.BLUE), "DOG");
	vfrag = new VideoFragment(this);
	adapter.addFrag(vfrag, "VIDEO");
        //mapFragment = new MapFragment(this); // SupportMapFragment.newInstance();
	//adapter.addFrag(mapFragment, "MAP");
	
	adapter.addFrag(new DummyFragment(Color.GREEN), "MOUSE");
	viewPager.setAdapter(adapter);
    }

    //@Override
    //public void onMapReady(GoogleMap map) {
    //	Log.d("UPHERE","MAPREADY");
    //	LatLng sydney = new LatLng(-34,151);
    //	map.moveCamera(CameraUpdateFactory.newLatLng(sydney));
    //	//viewPager.setAdapter(adapter);
    //	
    //}
    
    public native void onCreateHS( int k );

    public void sendMsgToChatter( byte[] dat ) {
	try { 
	    String msg = new String(dat, "UTF-8");
	    mMsgbox.add(msg);
	} catch(UnsupportedEncodingException e) {
	}

    }

    public void flushMsg() {
	Log.d("UPHERE", "FLUSH MSG");
	final Runnable myrun = new Runnable() {
		public void run() {
		    if(vfrag.tv != null && vfrag.sv != null) {
			for(String tmp: mMsgbox ) { 
			    vfrag.tv.append(tmp);
			}
			mMsgbox.clear();
			vfrag.sv.post( new Runnable() {
				public void run() {
				    vfrag.sv.fullScroll(View.FOCUS_DOWN);
				}
				
			    });
		    }
		}
	    };
	runOnUiThread(myrun);
	
    }

   
    static {
	System.loadLibrary("c++_shared");
	System.loadLibrary("haskell");
    }

    private class ViewPagerAdapter extends FragmentPagerAdapter {
	private final List<Fragment> mFragmentList = new ArrayList<>();
	private final List<String> mFragmentTitleList = new ArrayList<>();

	public ViewPagerAdapter(FragmentManager manager) {
	    super(manager);
	}

	@Override
	public Fragment getItem(int position) {
	    return mFragmentList.get(position);
	}

	@Override
	public int getCount() {
	    return mFragmentList.size();
	}

	public void addFrag(Fragment fragment, String title) {
	    mFragmentList.add(fragment);
	    mFragmentTitleList.add(title);
	}

	@Override
	public CharSequence getPageTitle(int position) {
	    return mFragmentTitleList.get(position);
	}
	    
	
    }
    public class DummyFragment extends Fragment {
	int color;
	//SimpleRecyclerAdapter adapter;

	public DummyFragment(int color) {
	    this.color = color;
	}

	@Override
	public View onCreateView( LayoutInflater inflater, ViewGroup container, Bundle SavedInstanceState) {
            View view = inflater.inflate(R.layout.dummy, container, false);
            final FrameLayout fr = (FrameLayout)view.findViewById(R.id.dummy);
	    fr.setBackgroundColor(color);
	    return view;
	}
	
    }
}
