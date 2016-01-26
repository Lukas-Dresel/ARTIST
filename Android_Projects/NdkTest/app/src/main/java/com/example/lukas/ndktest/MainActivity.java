package com.example.lukas.ndktest;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import dalvik.system.BaseDexClassLoader;


public class MainActivity extends Activity
{
    public final String TAG = this.getClass().getName();

    private TextView mOutput;

    private void logCoolNumber()
    {
        Log.d("MainActivity", "42.42");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.mOutput = (TextView) this.findViewById(R.id.output);

        this.dumpProcessMemoryMap();

        Class c = BaseDexClassLoader.class;

        Log.i(TAG, String.format("DexClassLoader: Canonical Name: %s", c.getCanonicalName()));
        Log.i(TAG, String.format("DexClassLoader: Name:           %s", c.getName()));
        Log.i(TAG, String.format("DexClassLoader: Package Name:   %s", c.getPackage().getName()));

        this.dumpQuickEntryPointsInfo();

        //this.testHookingAOTCompiledFunction();
        //this.testHookingInterpretedFunction();
        //this.dumpMainOatInternals();
        //this.testBreakpointAtoi();

        this.mOutput.setText(String.format("Bits of %d: %d", 10, Integer.bitCount(10)));

        /*Log.d(TAG, "Overwriting java code.");
        this.tryNukeDexContent();
        Log.d(TAG, "Overwrote java code.");
        this.logCoolNumber();

        try
        {
            Log.d(TAG, "After this comes all the dexclassloader bullshit!");
            DexClassLoader loader;
            loader = new DexClassLoader("", "", "", this.getClassLoader());
            loader.loadClass("abc");
        }
        catch(Exception ex)
        {
            Log.e(TAG, String.format("Exception making class loader: %s", ex.getMessage()));
        }*/

        /*try
        {
            MemoryInfo info = MemoryAnalyzer.getMemoryInfo();
            Gson gson = new GsonBuilder().setPrettyPrinting().create();
            JsonElement serialized = gson.toJsonTree(info.Regions);
            this.mOutput.setText(serialized.toString());
        }
        catch (Exception ex)
        {
            Log.e(TAG, String.format("Exception getting memory regions: %s", ex.getMessage()));
        }*/
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

    private native void dumpQuickEntryPointsInfo();
    private native void testHookingAOTCompiledFunction();
    private native void testHookingInterpretedFunction();
    private native void testSingleStep();
    private native void testBreakpointAtoi();
    private native void tryNukeDexContent();

    private native void dumpMainOatInternals();
    private native void dumpProcessMemoryMap();

    static
    {
        System.loadLibrary("NdkTest");
    }
}
