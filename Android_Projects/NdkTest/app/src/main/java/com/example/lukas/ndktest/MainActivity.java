package com.example.lukas.ndktest;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import com.example.lukas.ndktest.MappedMemoryRegion;
import com.example.lukas.ndktest.MemoryAnalysis.MemoryAnalyzer;
import com.example.lukas.ndktest.MemoryAnalysis.MemoryInfo;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonElement;

import org.json.JSONObject;

import java.lang.reflect.Method;


public class MainActivity extends Activity
{
    public final String TAG = this.getClass().getName();

    private TextView mOutput;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.mOutput = (TextView) this.findViewById(R.id.output);

        this.dumpProcessMemoryMap();
        this.dumpProcAlignmentMap();
        this.dumpJEnvContent();

        this.testBreakpointAtoi();

        try
        {
            MemoryInfo info = MemoryAnalyzer.getMemoryInfo();
            Gson gson = new GsonBuilder().setPrettyPrinting().create();
            JsonElement serialized = gson.toJsonTree(info.Regions);
            this.mOutput.setText(serialized.toString());

            this.testHookingAtoi();
            this.testHookingAndroidLogPrint();
        }
        catch (Exception ex)
        {
            Log.e(TAG, String.format("Exception getting memory regions: %s ", ex.getMessage()));
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings)
        {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private native void testOverwriteAtoi();

    private native void testOverwriteJNIEnvFunc();

    private native void testOverwriteOwnFunction();

    private native void testBreakpointAtoi();

    private native void testHookingAtoi();

    private native void testHookingAndroidLogPrint();

    private native void dumpJEnvContent();

    private native void dumpProcessMemoryMap();

    private native void dumpProcAlignmentMap();

    private native void setMemoryProtection(long address, long size, boolean read, boolean write, boolean execute);


    static
    {
        System.loadLibrary("NdkTest");
    }
}
