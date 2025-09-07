using Godot;
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Yum;

public partial class MainCaca : Control
{
  static void RegisterArg<T>(T value, [CallerArgumentExpression(nameof(value))] string name = null)
  {
    Console.WriteLine($"Param: {name} = {value}");
    // You could call C++ API here: map[name] = value
  }

  public override void _Ready()
  {
    YumSystem.Init();


    string code = $"""
    function friendof()
      print('oki')
    end

    function main(arg1)
      print(arg1)
      print('btw i got smth from C#, look: {"haha"}')
      friendof()
    end
    """;

    YumSystem.LoadString($"print('hilo')\n{code}");
    YumSystem.Call("main"); // TODO: Call(name, ...)
    YumSystem.RegisterCallback("foo", vecHandle =>
    {
      Console.WriteLine("Callback 'foo' invoked from C++/Lua!");

      // Wrap vecHandle to inspect arguments if needed
      using var argsVec = new VariantVector(vecHandle, ownsHandle: false);

      Console.WriteLine($"Args count: {argsVec.Size}");

      // Create return Variant
      var result = new Yum.Variant(42);

      // IMPORTANT: Don't Dispose result here, you must pass ownership to native side
      return result.Handle;
    });
    YumSystem.Call("foo");
  }

  public override void _ExitTree()
  {
    YumSystem.Shutdown();
  }
}