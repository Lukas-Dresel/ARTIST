package com.example.lukas.ndktest.MemoryAnalysis;

import com.example.lukas.ndktest.MappedMemoryRegion;
import com.google.common.collect.ImmutableListMultimap;
import com.google.common.collect.Multimap;

import java.util.LinkedList;
import java.util.Map;

/**
 * Created by Lukas on 4/20/2015.
 */
public class MemoryInfo
{
    public final MappedMemoryRegion[] Regions;
    public final ImmutableListMultimap<String, MappedMemoryRegion> DescriptionLookupTable;

    public MemoryInfo(MappedMemoryRegion[] regions)
    {
        ImmutableListMultimap.Builder<String, MappedMemoryRegion> builder = ImmutableListMultimap.builder();
        this.Regions = regions.clone();
        for(MappedMemoryRegion region : this.Regions)
        {
            if(region.Description != null && !region.Description.isEmpty())
            {
                builder.put(region.Description, region);
            }
        }
        this.DescriptionLookupTable = builder.build();
    }
}
