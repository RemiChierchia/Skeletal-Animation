# COMPUTER GRAPHICS PROJECT
# Skeletal Animation

This project draws, thanks to OpenGL, a skeletal animation reading through json files.

## Requirements
 - Visual Studio
 - GPU
 
# Project structure
```
bin/
    intermediates/Win32/Debug
    Win32/Debug
Dependencies/
             GLEW
             GLFW
Skeletal Animation/
                   res/
                       shaders
                       objects
                       Link to the resources.txt
                   src/
Skeletal Animation.sln
```

## Libraries
The `bin` folder contains the `Debug` folders that Visual Studio makes. To get ASSIMP library working correctly is important to put `assimp-vc142-mtd.dll` file in the `bin/Win32/Debug/` directory.
In `Depencencies` folder are placed the main libraries to set up the window management with OpenGL. Inside both of them there are the `include` and `lib` folders to properly link in the Visual Studio project properties. I built the project based on 32-bit architecture, so refer to `Win32` folders.
The other additional libraries are placed in the `Link to the resources.txt` file. Al the precompiled libraries has `include` and `lib` folders, while `glm` and `json` are header only libraries.
The `boost` library is the only one that I built.

# Links
 - [GLFW]   (https://www.glfw.org/download.html)
 - [GLEW]   (http://glew.sourceforge.net/)
 - [GLM]    (https://glm.g-truc.net/0.9.9/index.html)
 - [SOIL2]  (https://github.com/SpartanJ/SOIL2)
 - [ASSIMP] (http://assimp.org/)
 - [JSON]   (https://github.com/nlohmann/json)
 - [BOOST]  (https://www.boost.org/)
 
# Usage

# How-to


## Credits

# Notes and References
- https://rogerdudler.github.io/git-guide/
- http://therandomsecurityguy.com/openvswitch-cheat-sheet/
- https://www.cyberciti.biz/faq/howto-linux-configuring-default-route-with-ipcommand/
- https://www.vagrantup.com/intro/getting-started/

## License
TODO: Write license

