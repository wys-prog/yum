using Godot;
using System;

public partial class MainCaca : Control
{
  public override void _Ready()
  {
    Yum.Yum_initSubsystem();

    Yum.Yum_loadString("print('Mommy i luv u <3')");
  }
}