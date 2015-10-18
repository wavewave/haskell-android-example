package com.uphere.vchatter;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
//
import android.support.v4.app.DialogFragment;
//
import com.uphere.vchatter.Chatter;
    
public class NicknameDialogFragment extends DialogFragment {
    //public String nickname;
    private EditText input;
    private Chatter parent;

    
    public NicknameDialogFragment( Chatter p ) {
	parent = p;
    }
    
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
	AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
	LayoutInflater i = parent.getLayoutInflater();
	View view = i.inflate(R.layout.dialog, null);
	
	builder.setMessage("Set your nickname")
	    .setPositiveButton("OK", new DialogInterface.OnClickListener() {
		    public void onClick(DialogInterface dialog, int id) {
			Dialog f = (Dialog) dialog;
			input = (EditText)f.findViewById(R.id.edit_nickname );
			parent.nickname = input.getText().toString();
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
