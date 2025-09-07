using System;
using System.Runtime.InteropServices;

namespace Yum
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate IntPtr YumCSharpFunc(IntPtr vec);
    internal static class Native
    {


        private const string DllName = "yum"; // <-- replace with actual .dll/.so/.dylib

        // ==============================
        // Subsystem
        // ==============================
        [DllImport(DllName)] internal static extern int Yum_initSubsystem();
        [DllImport(DllName)] internal static extern void Yum_shutdownSubsystem();
        [DllImport(DllName)] internal static extern void Yum_pushInt(long a);
        [DllImport(DllName)] internal static extern void Yum_pushNumber(double a);
        [DllImport(DllName)] internal static extern void Yum_pushString(string a);
        [DllImport(DllName)] internal static extern void Yum_pushBoolean(int a);
        [DllImport(DllName)] internal static extern int Yum_loadString(string str);
        [DllImport(DllName)] internal static extern int Yum_loadFile(string src);
        [DllImport(DllName)] internal static extern int Yum_call(string name);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int Yum_registerCSCallback(string name, YumCSharpFunc func);


        // ==============================
        // Variant
        // ==============================
        [DllImport(DllName)] internal static extern IntPtr Yum_newVariant();
        [DllImport(DllName)] internal static extern void Yum_deleteVariant(IntPtr v);
        [DllImport(DllName)] internal static extern void Yum_setInt(IntPtr v, long i);
        [DllImport(DllName)] internal static extern void Yum_setNumber(IntPtr v, double d);
        [DllImport(DllName)] internal static extern void Yum_setBool(IntPtr v, int b);
        [DllImport(DllName)] internal static extern void Yum_setString(IntPtr v, string s);
        [DllImport(DllName)] internal static extern IntPtr Yum_typeOf(IntPtr v);
        [DllImport(DllName)] internal static extern long Yum_asInt(IntPtr v);
        [DllImport(DllName)] internal static extern double Yum_asNumber(IntPtr v);
        [DllImport(DllName)] internal static extern int Yum_asBool(IntPtr v);
        [DllImport(DllName)] internal static extern IntPtr Yum_asString(IntPtr v);

        // ==============================
        // Vec<Variant>
        // ==============================
        [DllImport(DllName)] internal static extern IntPtr Yum_newVec();
        [DllImport(DllName)] internal static extern void Yum_deleteVec(IntPtr v);
        [DllImport(DllName)] internal static extern void Yum_pushIntVec(long a, IntPtr v);
        [DllImport(DllName)] internal static extern void Yum_pushNumberVec(double a, IntPtr v);
        [DllImport(DllName)] internal static extern void Yum_pushBooleanVec(int a, IntPtr v);
        [DllImport(DllName)] internal static extern void Yum_pushStringVec(string a, IntPtr v);
        [DllImport(DllName)] internal static extern long Yum_sizeVec(IntPtr v);
        [DllImport(DllName)] internal static extern IntPtr Yum_typeAtVec(IntPtr v, long idx);
        [DllImport(DllName)] internal static extern long Yum_intAtVec(IntPtr v, long idx);
        [DllImport(DllName)] internal static extern double Yum_numberAtVec(IntPtr v, long idx);
        [DllImport(DllName)] internal static extern int Yum_booleanAtVec(IntPtr v, long idx);
        [DllImport(DllName)] internal static extern IntPtr Yum_stringAtVec(IntPtr v, long idx);
        [DllImport(DllName)] internal static extern void Yum_clearVec(IntPtr v);
        [DllImport(DllName)] internal static extern void Yum_popBackVec(IntPtr v);
    }

    // ==============================
    // Managed API: Subsystem
    // ==============================
    public static class YumSystem
    {
        public static void Init() => Native.Yum_initSubsystem();
        public static void Shutdown() => Native.Yum_shutdownSubsystem();
        public static void Push(long v) => Native.Yum_pushInt(v);
        public static void Push(double v) => Native.Yum_pushNumber(v);
        public static void Push(string v) => Native.Yum_pushString(v);
        public static void Push(bool v) => Native.Yum_pushBoolean(v ? 1 : 0);
        public static int LoadString(string code) => Native.Yum_loadString(code);
        public static int LoadFile(string path) => Native.Yum_loadFile(path);
        public static int Call(string func) => Native.Yum_call(func);
        public static int Call(string func, params object[] args)
        {
            foreach (var arg in args)
            {
                switch (arg)
                {
                    case int i: Push((long)i); break;
                    case long l: Push(l); break;
                    case double d: Push(d); break;
                    case float f: Push((double)f); break;
                    case bool b: Push(b); break;
                    case string s: Push(s); break;
                    case null: Push("null"); break; // fallback if Lua doesn’t support nil easily
                    default:
                        throw new ArgumentException(
                            $"Unsupported argument type {arg.GetType().Name} for Lua call");
                }
            }

            return Native.Yum_call(func);
        }

        public static int RegisterCallback(string name, YumCSharpFunc func) => Native.Yum_registerCSCallback(name, func);
    }

    // ==============================
    // Managed API: Variant
    // ==============================
    public sealed class Variant : IDisposable
    {
        private IntPtr handle;
        internal IntPtr Handle => handle;


        public Variant()
        {
            handle = Native.Yum_newVariant();
            if (handle == IntPtr.Zero)
                throw new Exception("Failed to create Variant");
        }

        public Variant(long a)
        {
            handle = Native.Yum_newVariant();
            if (handle == IntPtr.Zero)
                throw new Exception("Failed to create Variant");
            Set(a);
        }

        public void Set(long v) => Native.Yum_setInt(handle, v);
        public void Set(double v) => Native.Yum_setNumber(handle, v);
        public void Set(bool v) => Native.Yum_setBool(handle, v ? 1 : 0);
        public void Set(string v) => Native.Yum_setString(handle, v);

        public string Type => Marshal.PtrToStringAnsi(Native.Yum_typeOf(handle)) ?? "null";

        public long AsInt() => Native.Yum_asInt(handle);
        public double AsNumber() => Native.Yum_asNumber(handle);
        public bool AsBool() => Native.Yum_asBool(handle) != 0;
        public string AsString() => Marshal.PtrToStringAnsi(Native.Yum_asString(handle)) ?? "";

        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                Native.Yum_deleteVariant(handle);
                handle = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        ~Variant() => Dispose();
    }

    // ==============================
    // Managed API: Vector<Variant>
    // ==============================
    public sealed class VariantVector : IDisposable
    {
        private IntPtr handle;
        private readonly bool ownsHandle;

        internal IntPtr Handle => handle;

        public VariantVector()
        {
            handle = Native.Yum_newVec();
            if (handle == IntPtr.Zero)
                throw new Exception("Failed to create Vector<Variant>");
            ownsHandle = true;
        }

        public VariantVector(IntPtr existing, bool ownsHandle)
        {
            handle = existing;
            this.ownsHandle = ownsHandle;
        }

        public void Push(long v) => Native.Yum_pushIntVec(v, handle);
        public void Push(double v) => Native.Yum_pushNumberVec(v, handle);
        public void Push(bool v) => Native.Yum_pushBooleanVec(v ? 1 : 0, handle);
        public void Push(string v) => Native.Yum_pushStringVec(v, handle);

        public long Size => Native.Yum_sizeVec(handle);

        public string TypeAt(long i) =>
            Marshal.PtrToStringAnsi(Native.Yum_typeAtVec(handle, i)) ?? "null";

        public long IntAt(long i) => Native.Yum_intAtVec(handle, i);
        public double NumberAt(long i) => Native.Yum_numberAtVec(handle, i);
        public bool BoolAt(long i) => Native.Yum_booleanAtVec(handle, i) != 0;
        public string StringAt(long i) => Marshal.PtrToStringAnsi(Native.Yum_stringAtVec(handle, i)) ?? "";

        public void Clear() => Native.Yum_clearVec(handle);
        public void PopBack() => Native.Yum_popBackVec(handle);

        public void Dispose()
        {
            if (ownsHandle && handle != IntPtr.Zero)
            {
                Native.Yum_deleteVec(handle);
                handle = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        ~VariantVector() => Dispose();
    }
}
