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

import java.util.Collections.*;

/**
 * Created by Lukas on 4/19/2015.
 */
public class MappedMemoryRegion
{
    public static class FileMapping
    {
        public final long Offset;
        public final long MajorDeviceNumber;
        public final long MinorDeviceNumber;
        public final long INode;
        public FileMapping(long off, long major, long minor, long inode)
        {
            this.Offset = off;
            this.MajorDeviceNumber = major;
            this.MinorDeviceNumber = minor;
            this.INode = inode;
        }
    }
    private static enum MemoryStatus
    {
        SHARED,
        PRIVATE
    }

    public final long StartingAddress;
    public final long EndAddress;
    public final boolean IsReadable;
    public final boolean IsWritable;
    public final boolean IsExecutable;
    public final boolean IsShared;

    public final FileMapping FileMapInfo;

    public final String Description;

    public MappedMemoryRegion(long start, long end, boolean read, boolean write, boolean exec, boolean shared, long off, long majorDevNum, long minorDevNum, long inode, String desc)
    {
        this.StartingAddress = start;
        this.EndAddress = end;
        this.IsReadable = read;
        this.IsWritable = write;
        this.IsExecutable = exec;
        this.IsShared = shared;
        this.FileMapInfo = (inode == 0) ? null : new FileMapping(off, majorDevNum, minorDevNum, inode);
        this.Description = desc;
    }

    public boolean isMappedFromFile()
    {
        return this.FileMapInfo != null;
    }
}
