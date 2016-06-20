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

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by Lukas on 4/19/2015.
 */
public class MemoryRegionParser
{
    /**
     * Regular Expression for /proc/self/maps line is
     *
     *      ([0-9a-f]+)  -   ([0-9a-f]+)  \s  ([r-]) ([w-]) ([x-]) ([sp])   \s    ([0-9a-f]+)   \s  ([0-9a-f]+) :   ([0-9a-f]+)     \s  (\d+)   \s?  (.*)
     *      StartAddress        EndAddress    Read   Write  Execute Shared        Filemap Offset   Major devnum    Minor devnum         Inode       Description
     */
    public static final String pattern = "([0-9a-f]+)-([0-9a-f]+)\\s([r-])([w-])([x-])([sp])\\s([0-9a-f]+)\\s([0-9a-f]+):([0-9a-f]+)\\s(\\d+)\\s?(.*)";
    public final static Pattern MAPS_LINE_PATTERN = Pattern.compile(pattern, Pattern.CASE_INSENSITIVE);

    private static long parseHex(String s)
    {
        return Long.parseLong(s, 16);
    }

    public static MappedMemoryRegion getMemoryRegionFromMapsFileLine(String line)
    {
        line = line.trim();
        Matcher m = MAPS_LINE_PATTERN.matcher(line);
        if(!m.matches())
        {
            throw new IllegalArgumentException(String.format("The provided line does not match the pattern for /proc/$pid/maps lines. Given: %s", line));
        }

        if(m.groupCount() != 11) // group(0) not included in this.
        {
            throw new InternalError(String.format("Invalid group count: Found %d, but expected %d", m.groupCount(), 12));
        }

        long start       =   parseHex(m.group(1));
        long end         =   parseHex(m.group(2));
        boolean read     =            m.group(3) == "r";
        boolean write    =            m.group(4) == "w";
        boolean exec     =            m.group(5) == "x";
        boolean shared   =            m.group(6) == "s";
        long fileOffset  =   parseHex(m.group(7));
        long majorDevNum =   parseHex(m.group(8));
        long minorDevNum =   parseHex(m.group(9));
        long inode       =   parseHex(m.group(10));
        String desc      =            m.group(11);

        return new MappedMemoryRegion(start, end, read, write, exec, shared, fileOffset, majorDevNum, minorDevNum, inode, desc);
    }
}
