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

package com.example.lukas.ndktest.MemoryAnalysis;

import com.example.lukas.ndktest.MappedMemoryRegion;
import com.google.common.collect.ImmutableListMultimap;

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
