# COMPUTER GRAPHICS PROJECT
# Skeletal Animation

This project draws, thanks to OpenGL, a skeletal animation reading through json files.
At https://www.youtube.com/watch?v=BJqw10MVvpQ&feature=youtu.be&ab_channel=RemiChierchia is possible to see a video of the current last version of the project.

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

## Usage
Make sure to have Visual Studio installed, I have used the version 2019, and a GPU that supports GLFW and GLEW libraries. Nowadays GPUs are all suitable to this application, but is always better to double check.

## Project properties
```
Platform Win32 

C/C++:
  General:
    Additional directories:
      res\boost;
      res\json\include;
      res\SOIL2\include;
      res\assimp\include;
      res;
      $(SolutionDir)Dependencies\GLEW\include;
      $(SolutionDir)Dependencies\GLFW\include;
Linker:
  General:
    Additional directories:
      res\boost\lib;
      res\SOIL2\lib;
      res\assimp\lib;
      $(SolutionDir)Dependencies\GLEW\lib\Release\Win32;
      $(SolutionDir)Dependencies\GLFW\lib-vc2019;
  Input:
    Additional dependences:
      soil2.lib;
      assimp-vc142-mtd.lib;
      glfw3.lib;
      glew32s.lib;
      opengl32.lib;
      user32.lib;
      gdi32.lib;
      shell32.lib
```

# Libraries links
 - [GLFW]   (https://www.glfw.org/download.html)
 - [GLEW]   (http://glew.sourceforge.net/)
 - [GLM]    (https://glm.g-truc.net/0.9.9/index.html)
 - [SOIL2]  (https://github.com/SpartanJ/SOIL2)
 - [ASSIMP] (http://assimp.org/)
 - [JSON]   (https://github.com/nlohmann/json)
 - [BOOST]  (https://www.boost.org/)

# Credits and References
- https://open.gl/
- https://docs.gl/
- http://ogldev.org/www/tutorial38/tutorial38.html
- https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&ab_channel=TheCherno
- http://www.songho.ca/opengl/index.html
- https://www.3dgep.com/loading-and-animating-md5-models-with-opengl/
- https://learnopengl.com/

# License
TODO.

