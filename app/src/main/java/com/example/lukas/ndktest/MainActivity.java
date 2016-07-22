/*
 * Copyright 2016 Lukas Dresel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
 package com.example.lukas.ndktest;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import com.example.lukas.ndktest.MappedMemoryRegion;
import com.example.lukas.ndktest.MemoryAnalysis.MemoryAnalyzer;
import com.example.lukas.ndktest.MemoryAnalysis.MemoryRegionParser;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.lang.reflect.Method;

import dalvik.system.BaseDexClassLoader;
import dalvik.system.DexClassLoader;
import dalvik.system.DexFile;


public class MainActivity extends Activity {
    public final String TAG = this.getClass().getName();

    private TextView mOutput;

    private void logCoolNumber() {
        Log.d("MainActivity", "42.42");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.mOutput = (TextView) this.findViewById(R.id.output);

        this.testHookingThreadEntryPoints();

        try {
            BufferedReader reader = new BufferedReader(new FileReader("/proc/self/maps"));
            String line;
            while ((line = reader.readLine()) != null)
            {
                Log.i(TAG, line);
            }
        }
        catch (IOException ex)
        {

        }


        String a = "NdkTest";
        testtest(a, 10, a, a, 10);
        System.loadLibrary(a);


        /*try
        {
            for(MappedMemoryRegion mr : MemoryAnalyzer.getMemoryRegions())
            {
                Log.d(TAG, String.format("%x-%x: %s", mr.StartingAddress, mr.EndAddress, mr.Description));
            }
        }
        catch(Throwable t)
        {
            Log.e(TAG, t.getMessage());
        }

        this.dumpProcessMemoryMap();

        Class c = BaseDexClassLoader.class;

        Log.i(TAG, String.format("DexClassLoader: Canonical Name: %s", c.getCanonicalName()));
        Log.i(TAG, String.format("DexClassLoader: Name:           %s", c.getName()));
        Log.i(TAG, String.format("DexClassLoader: Package Name:   %s", c.getPackage().getName()));


        this.dumpProcessMemoryMap();
        this.testHookingAOTCompiledFunction();
        this.dumpProcessMemoryMap();


        this.dumpQuickEntryPointsInfo();
        this.dumpSystemLoadLibraryState();
        this.testHookingThreadEntryPoints();

        try
        {
            Method findLib = BaseDexClassLoader.class.getDeclaredMethod("findLibrary", String.class);
            Log.d(TAG, "dalvik.system.BaseDexClassLoader.findLibrary[(Ljava/lang/String;)Ljava/lang/String;]'s Access flags before overwrite: " + findLib.getModifiers());
            this.testHookingInterpretedFunction();
            findLib = BaseDexClassLoader.class.getDeclaredMethod("findLibrary", String.class);
            Log.d(TAG, "dalvik.system.BaseDexClassLoader.findLibrary[(Ljava/lang/String;)Ljava/lang/String;]'s Access flags after overwrite:  " + findLib.getModifiers());

            //registerNativeHookForFindLibrary();

            new dalvik.system.BaseDexClassLoader("blub", null, "blub", this.getClassLoader()).findLibrary("abcdef");

            this.testHookingDexLoadClass();



            Method m = DexFile.class.getDeclaredMethod("loadClass", String.class, ClassLoader.class);
            Log.d(TAG, "DexFile.loadClass modifiers: " + m.getModifiers());

            this.registerNativeHookForDexFileLoadClass();

        }
        catch(NoSuchMethodException nometh)
        {
            Log.d(TAG, nometh.getMessage());
        }

        this.mOutput.setText(String.format("Bits of %d: %d", 10, Integer.bitCount(10)));

        String towelroot_path = "/storage/sdcard0/Download/tr.apk";
        try {
            DexFile towelroot_dex = new DexFile(towelroot_path);
            Class towelroot_main = towelroot_dex.loadClass("com.geohot.towelroot.TowelRoot", this.getClassLoader());
            Method[] methods = towelroot_main.getDeclaredMethods();
            for (Method m : methods) {
                Log.d(TAG, "Found method: " + m.toString());
            }
        } catch (IOException ioex) {
            Log.wtf(TAG, ioex.getMessage());
        }*/

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
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private native void testtest(String a, int b, String c, String d, int e);
    private native void registerNativeHookForFindLibrary();
    private native void registerNativeHookForDexFileLoadClass();

    private native void testHookingAOTCompiledFunction();
    private native void testHookingInterpretedFunction();
    private native void testHookingDexLoadClass();

    private native void testHookingThreadEntryPoints();

    private native void dumpQuickEntryPointsInfo();

    private native void dumpLibArtInterpretedFunctionsInNonAbstractClasses();
    private native void dumpMainOatInternals();
    private native void dumpSystemLoadLibraryState();

    private native void dumpProcessMemoryMap();

    static
    {
        System.loadLibrary("NdkTest");
    }
}
