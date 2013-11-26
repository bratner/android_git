package com.bratner.cryptoid;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.regex.Pattern;

import org.apache.http.util.EncodingUtils;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputFilter;
import android.text.InputType;
import android.text.Spanned;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.widget.Toast;

public class ChangePass extends Activity implements OnClickListener, TextWatcher, 
			 OnEditorActionListener, OnCheckedChangeListener {
	private static final int MIN_PASSWD_LEN = 12; 
	private static final int MAX_PASSWD_LEN = 32; 
	static final int DIALOG_SHORT_PASSWD = 0;
	static final int DIALOG_NO_MATCH = 1;
	
	private EditText orig_et, new_et, ver_et;
	private LinearLayout ver_layout;
	
	private Button change_bt;
	private CheckBox show_passwd;
	private TextView[] input_views;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	
    	
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        orig_et = (EditText)findViewById(R.id.orig_passwd);
        new_et = (EditText)findViewById(R.id.new_passwd);
        ver_et = (EditText)findViewById(R.id.ver_passwd);
        change_bt = (Button)findViewById(R.id.btn_change);
        ver_layout = (LinearLayout)findViewById(R.id.verLayout);
        show_passwd = (CheckBox)findViewById(R.id.showPasswd);
        
        change_bt.setOnClickListener(this);
        new_et.addTextChangedListener(this);
        
        new_et.setOnEditorActionListener(this);
        ver_et.setOnEditorActionListener(this);
        show_passwd.setOnCheckedChangeListener(this);
        
        input_views = new TextView[]{show_passwd,orig_et,new_et,ver_et,change_bt};
        
        InputFilter filter = new InputFilter(){
        	public CharSequence filter(CharSequence source, int start, int end,
        			Spanned dest, int dstart, int dend) {
        		byte[] good_syms = new byte[]
        				{'@','#','%','&','*','/','-','+',')','(','!',':',';',',','.'};
        		if(dest.length() >= MAX_PASSWD_LEN)
        			return "";
        		
        		Pattern p = Pattern.compile("[A-Za-z0-9@#%&*/\\-+()?!:;,.]");
        		
        		
        		
        		for (int i = start; i< end; i++){
        			if (!p.matcher(Character.toString(source.charAt(i))).matches())
        				return "";
        		}
        			
        	
        			
        		
        		return null;
        	}
        };
        InputFilter[] filters = new InputFilter[]{filter};
        orig_et.setFilters(filters);
        new_et.setFilters(filters);
        ver_et.setFilters(filters);
        
        
    }

	public void onClick(View sender) {
		if (validInput()){
			inputEnabled(false);
			doChangePasswd();
			
		}
		
		
	}

	public void afterTextChanged(Editable s) {
		// TODO Auto-generated method stub
		
	}

	public void beforeTextChanged(CharSequence s, int start, int count,
			int after) {
		// TODO Auto-generated method stub
		
	}

	public void onTextChanged(CharSequence s, int start, int before, int count) {
		if( s.toString().contains("talf") ){
			new_et.setText("");
			new_et.invalidate();
			showTalf();
			//change_bt.setEnabled(false); 
			
		}
	
	}

	
	private void showTalf(){
	
		Intent intent = new Intent(this,TalfDialog.class);
		startActivity(intent);
	}

	protected void inputEnabled(boolean enabled){		
		for(TextView t : input_views){
			t.setEnabled(enabled);

		}
	}

	public boolean onKey(View v, int keyCode, KeyEvent event) {
				

		return false;
	}

	@Override
	protected Dialog onCreateDialog(int id) {
		AlertDialog dialog;
		switch(id){
		case DIALOG_SHORT_PASSWD:
			dialog = new AlertDialog.Builder(this).create();
			dialog.setTitle("Error");
			dialog.setMessage("Your password should be at least "+Integer.toString(MIN_PASSWD_LEN)+
					" charachters long.");
			dialog.setButton("OK", new DialogInterface.OnClickListener() {
				
				public void onClick(DialogInterface dialog, int which) {
				   dialog.dismiss();
				}
			});
			return dialog;
			
		case DIALOG_NO_MATCH:
			dialog = new AlertDialog.Builder(this).create();
			dialog.setTitle("Error");
			dialog.setMessage("Verification password doesn't match!");
			dialog.setButton("OK", new DialogInterface.OnClickListener() {
			
				public void onClick(DialogInterface dialog, int which) {
					dialog.dismiss();
				}
			});
			return dialog;
		}
	
		return super.onCreateDialog(id);
	}

	public boolean onEditorAction(TextView v, int actionId, KeyEvent event){
		if (v==(TextView)new_et && actionId == EditorInfo.IME_ACTION_NEXT){
			if(new_et.getText().length() < MIN_PASSWD_LEN ){
				showDialog(DIALOG_SHORT_PASSWD);
				return true;
			}
		}
		if ( actionId == EditorInfo.IME_ACTION_DONE  ){	
			inputEnabled(false);
			if (!validInput()){
				inputEnabled(true);
				return true;
			}else{				
				doChangePasswd();
			}
		}  		
		return false;
	}
    
    private boolean validInput(){
		if(new_et.getText().length() < MIN_PASSWD_LEN ){
			showDialog(DIALOG_SHORT_PASSWD);
			return false;
		} else if ( new_et.getText().toString().compareTo(ver_et.getText().toString()) != 0 ){
			showDialog(DIALOG_NO_MATCH);
			return false;
		}
		return true ;
    }
    
	private int doChangePasswd(){
		
			Toast.makeText(this, "Changing password. Please wait...", Toast.LENGTH_SHORT ).show();
		 	
			new DoChangePassword().execute(orig_et.getText().toString(),new_et.getText().toString());
			return 0;
	}

	public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
		if((CheckBox)buttonView == show_passwd ){
			if(isChecked){
				//ver_layout.setVisibility(View.GONE);
				orig_et.setInputType(InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD);
				new_et.setInputType(InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD);
			}else{
				//ver_layout.setVisibility(View.VISIBLE);
				orig_et.setInputType(InputType.TYPE_TEXT_VARIATION_PASSWORD | InputType.TYPE_CLASS_TEXT );
				new_et.setInputType(InputType.TYPE_TEXT_VARIATION_PASSWORD | InputType.TYPE_CLASS_TEXT);

			}
		}
		
	}
	
 @Override
	protected void onStop() {
	 	super.onStop();
		new_et.setText("");
		orig_et.setText("");
		ver_et.setText("");
	}

class DoChangePassword extends AsyncTask<String, Integer, Integer>  {

		@Override
		protected Integer doInBackground(String... params)  {		    
		 	Process process;		 	
			try {
				process = Runtime.getRuntime().exec("/system/bin/csrun "+params[0]+" "+params[1]+"");
				Log.e("cryptoid", "Running: "+"/system/bin/csrun "+params[0]+" "+params[1]+"");
				BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(process.getInputStream()));
				String line = bufferedReader.readLine();
				while( line != null ){
					//Toast.makeText(ChangePass.this, "Got: "+line, Toast.LENGTH_SHORT).show();
					Log.e("cryptoid", line );
					line = bufferedReader.readLine();
				}
				
				try
				{
				   return process.waitFor();
				   
				} catch (InterruptedException e) {
						//Toast.makeText(ChangePass.this, "IO error: "+e.toString(), Toast.LENGTH_LONG ).show();
						Log.e("cryptoid", e.toString());
						return 126; 
						//this.cancel(true);						
			}
			 	 
			} catch (IOException e) {
			
				Log.e("cryptoid", e.toString());
				return 125;
				//Toast.makeText(ChangePass.this, "IO error: "+e.toString(), Toast.LENGTH_LONG ).show();			 
			} 
			
			//return 100;
		}

		
		@Override
		protected void onCancelled() {
			//Toast.makeText(ChangePass.this, "Error: You caught a bug! Contact your support representative. ", Toast.LENGTH_SHORT ).show();
			
		}


		@Override
		protected void onPostExecute(Integer result) {
			inputEnabled(true);
			String msg;
			
			new_et.setText("");
			orig_et.setText("");
			ver_et.setText("");
			
			if(result == 0){
				Toast.makeText(ChangePass.this, "Successfully changed your password.", Toast.LENGTH_SHORT ).show();
			}else if( result == 3){
				Toast.makeText(ChangePass.this, "Error changing your password. Please check your original password.", Toast.LENGTH_SHORT ).show();
			} else {
				Toast.makeText(ChangePass.this, "ERROR: Problem with changing password. Error code "+result, Toast.LENGTH_SHORT ).show();
			}
			
			
			
		}
		
		
	
		
	}
	
	
}