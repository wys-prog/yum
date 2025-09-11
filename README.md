# Yum

Maybe we can use both Godot and Lua ?

YumEngine is written in both C and C++.
The engine's backend is mainely in C++, while the API is fully C-Safe (without exceptions) code.
Idea of YumEngine is to provide a specific lua version (called Yum.InternalLua), with a simple and universal interface.
While the API is in C, all internal classes have their own C version (e.g. Yumcxx::Vecot, Yumcxx::Variant)
The second idea of YumEngine is to provide an Engine, customizable, and easy to integrate (even compilation).
Generally, official builds will contain:
  - Yum : Dynamic Library
  - YumEngine : The whole engine + Yum library (linked in)
  - YumiEngine : Simply the engine, dependent of Yum library

Requirements too, are pretty simple. The maximum that you'll need is: 
  - a C++ compiler that supports at least C++20
  - a C compiler that supports at least C-17

Suggested compilers:
  - GCC/MinGW, with standard 20.
  Note: On MacOS, most of binaries provided by that project will be compiled with 
  g++-15 and the -std=c++23 argument, the one from homebrew.

Compiling:
  You don't need ! 
  But if you really need, you can compile with:
  build.lua (provided file)
  You just have to make sure that you have Lua (or something that can execute Lua code!) 
  and to modify compilation flags/compilers/studio name.

