package com.example.lukas.ndktest.MemoryAnalysis;

import android.provider.MediaStore;

import com.example.lukas.ndktest.MappedMemoryRegion;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

/**
 * Created by Lukas on 4/20/2015.
 */
public class MemoryAnalyzer
{
    public static MemoryInfo getMemoryInfo() throws IOException
    {
        return new MemoryInfo(getMemoryRegions());
    }
    public static MappedMemoryRegion[] getMemoryRegions() throws IOException
    {
        ArrayList<MappedMemoryRegion> list = new ArrayList<>();
        BufferedReader reader = new BufferedReader(new FileReader("/proc/self/maps"));
        String line = null;
        while ((line = reader.readLine()) != null)
        {
            list.add(MemoryRegionParser.getMemoryRegionFromMapsFileLine(line));
        }
        MappedMemoryRegion[] result = new MappedMemoryRegion[list.size()];
        return list.toArray(result);
    }
}
