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
