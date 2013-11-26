package com.bratner.cryptoid;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.Window;
import android.view.WindowManager;

public class TalfDialog extends Activity {

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		Handler handler;
		final TalfDialog talf = this;
	    super.onCreate(savedInstanceState);
	    
	    requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
	    setContentView(R.layout.talf);
	    
	   
		
				
		handler = new Handler();
		handler.postDelayed(new Runnable()
		{
		    public void run(){
		    		talf.finish();
		    	}
		      // 3000 milliseconds
		},3000);
	}

}
