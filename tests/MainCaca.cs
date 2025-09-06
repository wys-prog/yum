using Godot;
using System;

public partial class MainCaca : Control
{
  public override void _Ready()
  {
    Yum.Yum_initSubsystem();

    Yum.Yum_loadString("function badbro(str) print('hello from Lua guys') end");
    Yum.Yum_pushString("haha");
    Yum.Yum_call("badbro");
    Yum.Yum_call("badbro");
    Yum.Yum_call("badbro");
    Yum.Yum_loadString("print('hello', 123, true)");
    GD.Print("(from Godot.C#) end of ready");
  }
}