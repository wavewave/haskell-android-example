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
//import android.os.Handler;
//import android.os.Looper;
//import android.os.MessageQueue;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;
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

import com.uphere.vchatter.ObjectRegisterer; 
import com.uphere.vchatter.CameraFragment;
import com.uphere.vchatter.NicknameDialogFragment;
import com.uphere.vchatter.VideoFragment;

public class Chatter extends FragmentActivity
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
        FragmentManager fm = getSupportFragmentManager();

        ViewPager viewPager = (ViewPager) findViewById(R.id.viewpager);
	setupViewPager(fm,viewPager);
	
	/* FragmentManager fm = getFragmentManager();
        
        FragmentTransaction ft = fm.beginTransaction();
        ft.add(R.id.fragment_container,vfrag);
	ft.commit();
	
	
        */

	dialog = new NicknameDialogFragment(this);
	dialog.show(fm,"fragment_nickname"); 
    }

    private void setupViewPager(FragmentManager fm, ViewPager viewPager) {
	ViewPagerAdapter adapter = new ViewPagerAdapter(fm);
	adapter.addFrag(new DummyFragment(Color.RED), "CAT" );
	//adapter.addFrag(new DummyFragment(Color.BLUE), "DOG");
	vfrag = new VideoFragment(this);
	adapter.addFrag(vfrag, "VIDEO");
	adapter.addFrag(new DummyFragment(Color.GREEN), "MOUSE");
	viewPager.setAdapter(adapter);
    }
    
    public native void onCreateHS( int k ); 

    public void sendMsgToChatter( byte[] msg ) {
	try { 
	    final String msg1 = new String(msg, "UTF-8");
	    final Runnable myrun = new Runnable() {
		    public void run() {
			if(vfrag.tv != null && vfrag.sv != null) {
			    vfrag.tv.append(msg1);
			    vfrag.sv.post( new Runnable() {
				    public void run() {
					vfrag.sv.fullScroll(View.FOCUS_DOWN);
				    }
				    
				});
			}
		    }
		};
	    runOnUiThread(myrun);
	    
	} catch(UnsupportedEncodingException e) {
	}

    }
    
   
    static {
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
