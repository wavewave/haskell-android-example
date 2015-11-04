package com.uphere.vchatter;

import java.io.UnsupportedEncodingException;
//
import android.app.Activity;
import android.app.AlertDialog;
//import android.app.Fragment;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toolbar;
import android.widget.VideoView;
//
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
//

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

import com.uphere.vchatter.Chatter;



public class MapFragment extends Fragment implements OnMapReadyCallback
{
    SupportMapFragment mMapFragment;
    Button button;
    Chatter parent;

    public MapFragment() {
    }
    
    public MapFragment( Chatter p ) {
	parent = p;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
			       Bundle savedInstanceState) {
	View rootView = inflater.inflate(R.layout.map, container, false); 
        FrameLayout childFragContainer = (FrameLayout) rootView.findViewById(R.id.map);

        button = (Button) rootView.findViewById(R.id.button);
	button.setOnClickListener( new View.OnClickListener() {
		@Override
		public void onClick(View view) {
		    Log.d("UPHERE", "TEST");
		    parent.vp.setCurrentItem( 1 );
		}
	    });
	mMapFragment = SupportMapFragment.newInstance();
	FragmentManager fm = getChildFragmentManager();
	FragmentTransaction fmt = fm.beginTransaction();
	fmt.replace(childFragContainer.getId(), mMapFragment, "map_fragment");
	fmt.commit();

	
	return rootView;
    }

    @Override
    public void onMapReady(GoogleMap map) {
	Log.d("UPHERE","MAPREADY");
	LatLng sydney = new LatLng(-34,151);
	map.moveCamera(CameraUpdateFactory.newLatLng(sydney));
	//viewPager.setAdapter(adapter);
	
    }

}
