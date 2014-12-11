package com.example.finalproject462;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;

import java.net.Socket;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.net.UnknownHostException;
import java.lang.Object;

import org.json.JSONException;
import org.json.JSONObject;



public class MainActivity extends Activity {
	
	private Socket s;
	
	private static final int default_port = 5000;
	private static final String default_ip = "192.168.240.1";
	private int conn_port = default_port;
	String status = "Waiting for connection";
	boolean listen = true;
	TextView leftIR, leftUltra, frontIR, frontUltra, rightIR, rightUltra, heading;
	Thread listenThread;
	
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        System.out.println("Oncreate beginning");
        setTitle(status);
        
        final ImageButton left_button = (ImageButton) findViewById(R.id.arrowleft);
        System.out.println("Button initialized");
        left_button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				System.out.println("Left Arrow Pushed");
				try	{ 
		    		OutputStreamWriter out = new OutputStreamWriter(s.getOutputStream());
		    		String str = "1";
		    		out.write(str);
		    		out.flush();
		    	}catch (IOException e){
		    		e.printStackTrace();
		    	}
			}
		});
        final ImageButton top_button = (ImageButton) findViewById(R.id.arrowup);
        System.out.println("Button initialized");
        top_button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				System.out.println("Up Arrow Pushed");
				try	{ 
		    		OutputStreamWriter out = new OutputStreamWriter(s.getOutputStream());
		    		String str = "2";
		    		out.write(str);
		    		out.flush();
		    	}catch (IOException e){
		    		e.printStackTrace();
		    	}
			}
		});
        final ImageButton right_button = (ImageButton) findViewById(R.id.arrowright);
        System.out.println("Button initialized");
        right_button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				System.out.println("Right Arrow Pushed");
				try	{ 
		    		OutputStreamWriter out = new OutputStreamWriter(s.getOutputStream());
		    		String str = "3";
		    		out.write(str);
		    		out.flush();
		    	}catch (IOException e){
		    		e.printStackTrace();
		    	}
			}
		});
        final ImageButton bottom_button = (ImageButton) findViewById(R.id.arrowdown);
        System.out.println("Button initialized");
        bottom_button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				System.out.println("Down Arrow Pushed");
				try	{ 
		    		OutputStreamWriter out = new OutputStreamWriter(s.getOutputStream());
		    		String str = "4";
		    		out.write(str);
		    		out.flush();
		    	}catch (IOException e){
		    		e.printStackTrace();
		    	}
			}
		});
        final ImageButton left_turn_button = (ImageButton) findViewById(R.id.leftturn);
        System.out.println("Button initialized");
        left_turn_button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				System.out.println("Left Turn Arrow Pushed");
				try	{ 
		    		OutputStreamWriter out = new OutputStreamWriter(s.getOutputStream());
		    		String str = "5";
		    		out.write(str);
		    		out.flush();
		    	}catch (IOException e){
		    		e.printStackTrace();
		    	}
			}
		});
        final ImageButton right_turn_button = (ImageButton) findViewById(R.id.rightturn);
        System.out.println("Button initialized");
        right_turn_button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				System.out.println("Right Turn Arrow Pushed");
				try	{ 
		    		OutputStreamWriter out = new OutputStreamWriter(s.getOutputStream());
		    		String str = "6";
		    		out.write(str);
		    		out.flush();
		    	}catch (IOException e){
		    		e.printStackTrace();
		    	}
			}
		});
        
        final ImageButton stop_button = (ImageButton) findViewById(R.id.stopButton);
        System.out.println("Button initialized");
        stop_button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				System.out.println("Right Turn Arrow Pushed");
				try	{ 
		    		OutputStreamWriter out = new OutputStreamWriter(s.getOutputStream());
		    		String str = "7";
		    		out.write(str);
		    		out.flush();
		    	}catch (IOException e){
		    		e.printStackTrace();
		    	}
			}
		});
        
        leftUltra = (TextView) findViewById(R.id.leftUltraData);
        leftIR = (TextView) findViewById(R.id.leftIRData);
        frontUltra = (TextView) findViewById(R.id.frontalUltraData);
        frontIR = (TextView) findViewById(R.id.frontIRData);
        rightUltra = (TextView) findViewById(R.id.rightUltraData);
        rightIR = (TextView) findViewById(R.id.rightIRData);
        heading = (TextView) findViewById(R.id.headingData);
        
        
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_connect)
        {
        	promptUser();
        	/*setTitle("Connected");
        	new Thread(new ClientThread()).start();*/
            return true;
        }
        if(id == R.id.action_disconnect)
        {
        	try
        	{
        		listen = false;
        		listenThread.interrupt();
        		OutputStreamWriter out = new OutputStreamWriter(s.getOutputStream());
	    		String str = "8";
	    		out.write(str);
	    		out.flush();
				s.close();
				setTitle("Disconnected");
			}
        	catch (IOException e)
        	{
				e.printStackTrace();
			}
        	return true;
        }
        return super.onOptionsItemSelected(item);
    }
    
    class ClientThread implements Runnable
    {
    	@Override
    	public void run()
    	{
    		try
    		{
    			s = new Socket(default_ip, conn_port);
    			status = "Connected";
    			updateTitle();
    			listenThread = new Thread(new ListenThread());
    			listenThread.start();
    		}
    		catch(UnknownHostException e1)
    		{
    			e1.printStackTrace();
    			status = "Connection Failed";
    			updateTitle();
    		}
    		catch(IOException e1)
    		{
    			e1.printStackTrace();
    			status = "Connection Failed";
    			updateTitle();
    		}
    	}
    	
    }
    
    class ListenThread implements Runnable
    {
		public void run()
		{
			try
			{
				BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
				while(listen)
				{
					String newLine = in.readLine();
					updateData(newLine);
				}
			}
			catch (IOException e)
			{
				e.printStackTrace();
			}
		}
    	
    }
    
    public void promptUser()
    {
    	AlertDialog.Builder alert = new AlertDialog.Builder(this);

    	alert.setTitle("Enter Connection Information");
    	alert.setMessage("Enter connection port below");

    	// Set an EditText view to get user input 
    	final EditText port_input = new EditText(this);
    	alert.setView(port_input);
    	//port_input.setText(default_port);

    	alert.setPositiveButton("Connect", new DialogInterface.OnClickListener(){
	    	public void onClick(DialogInterface dialog, int whichButton)
	    	{
	    		String new_port = port_input.getText().toString();
	    		System.out.println("Connection information entered: " + default_ip + ":" + new_port);
	    		conn_port = Integer.parseInt(new_port);
	    		new Thread(new ClientThread()).start();
			}
    	});

    	alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener(){
			public void onClick(DialogInterface dialog, int whichButton)
			{
				
			}
    	});

    	alert.show();
    }
    
    public void updateTitle()
    {
    	runOnUiThread(new Runnable() {
            public void run()
            {
                setTitle(status);
            }
        });
    }
    
    public void updateData(String data)
    {
    	final String inpData = data;
    	runOnUiThread(new Runnable() {
    		public void run()
    		{
    			try
    			{
					JSONObject jsonData = new JSONObject(inpData);
					JSONObject ultraData = jsonData.getJSONObject("ultra");
					JSONObject irData = jsonData.getJSONObject("ir");
					heading.setText(jsonData.getString("heading") + "°");
					leftUltra.setText(ultraData.getString("left") + " cm");
					leftIR.setText(irData.getString("left"));
					frontUltra.setText(ultraData.getString("front") + " cm");
					frontIR.setText(irData.getString("front"));
					rightUltra.setText(ultraData.getString("right") + " cm");
					rightIR.setText(irData.getString("right"));
					System.out.println("Data updated");
				}
    			catch (JSONException e)
    			{
					e.printStackTrace();
				}
    		}
    	});
    }
}


