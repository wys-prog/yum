# Yum

YumYum! Let's run Lua differently!

You'll need a simple configuration file.
```lua
-- yum.lua (must be yum.lua, and located at the ROOT of your project!)
return {
    includes = {
        "libs/math.lua", -- Your libs goes there!
        "libs/io.lua"
    },
    main = "main.lua",
    output = "main.luac"
}
```
Then, you can just code in Lua, and launch with your binary.
DON'T FORGET TO PUT LIBRARIES IN 'includes' !!!
Have fun~ <3