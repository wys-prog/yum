using System;
using System.Runtime.InteropServices;

public static class Yum
{
    private const string DllName = "yum.dylib"; // yum.dll (Windows), libyum.so (Linux), libyum.dylib (macOS)

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Yum_initSubsystem();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Yum_shutdownSubsystem();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Yum_pushInt(long a);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Yum_pushNumber(double a);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Yum_pushString([MarshalAs(UnmanagedType.LPStr)] string a);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Yum_pushBoolean(short a);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Yum_loadString([MarshalAs(UnmanagedType.LPStr)] string str);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Yum_loadFile([MarshalAs(UnmanagedType.LPStr)] string src);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int Yum_call([MarshalAs(UnmanagedType.LPStr)] string name);
}
