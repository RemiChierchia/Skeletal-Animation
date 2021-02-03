# COMPUTER GRAPHICS PROJECT
# Skeletal Animation

This project draws, thanks to OpenGL, a skeletal animation reading through json files.

# Requirements
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
 
## Libraries installation
The 'bin' folder contains the 'Debug' folders that Visual Studio makes. To get ASSIMP library working correctly is important to put 'assimp-vc142-mtd.dll' file in the 'bin/Win32/Debug/' directory.
In 'Depencencies' folder are placed the main libraries to set up the window management with OpenGL. Inside both of them there are the 'include' and 'lib' folders to properly include in Visual Studio properties. I built the project based on 32-bit architecture, so refer to 'Win32' folders.
The file "Link to the resources.txt".
## Usage
TODO: Write usage instructions
# How-to
 - Install Virtualbox and Vagrant
 - Clone this repository
`git clone https://github.com/dustnic/dncs-lab`
 - You should be able to launch the lab from within the cloned repo folder.
```
cd dncs-lab
[~/dncs-lab] vagrant up
```
Once you launch the vagrant script, it may take a while for the entire topology to become available.
 - Verify the status of the 4 VMs
 ```
 [dncs-lab]$ vagrant status                                                                                                                                                                
Current machine states:

router                    running (virtualbox)
switch                    running (virtualbox)
host-a                    running (virtualbox)
host-b                    running (virtualbox)
```
- Once all the VMs are running verify you can log into all of them:
`vagrant ssh router`
`vagrant ssh switch`
`vagrant ssh host-a`
`vagrant ssh host-b`
`vagrant ssh host-c`

## Assignment
cosa dovevo fare

## Credits
TODO: Write credits
# Notes and References
- https://rogerdudler.github.io/git-guide/
- http://therandomsecurityguy.com/openvswitch-cheat-sheet/
- https://www.cyberciti.biz/faq/howto-linux-configuring-default-route-with-ipcommand/
- https://www.vagrantup.com/intro/getting-started/

## License
TODO: Write license

