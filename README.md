# Zenith

Zenith is an extensible, **C++ based game engine** replete with **PBR**, built in **UI functionality**, **Lua scripting** and a **custom object format (.zof)**. The project is currently in an inception period but there are many other features and additions that I will implement as time passes. Keep reading to get started.

![Short Zenith Demo](zenith.gif)

## Requirements

* CMake, available for download at [https://cmake.org/download/](https://cmake.org/download/)
* An Apple computer with any of the latest macOS images

  **OR**
  
* A 64 bit Windows installation, in which case you will also need Visual Studio, available for download at [https://visualstudio.microsoft.com/vs/community/](https://visualstudio.microsoft.com/vs/community/)

## Downloading

Please make sure to clone the repository from a command line using 

`git clone https://github.com/gitbetter/Zenith.git`

or using your favorite Git desktop GUI client. Zenith uses LFS to store certain sizable files, so downloading directly via the ZIP option will not resolve the git LFS links, and you will most likely encounter errors when attempting to run the project.

## Building and Running

Make sure that you have met the requirements above. There is currently no way to build the project without CMake, and you might be left out in the cold if you try to run on any Linux distro or older Windows/OSX versions, as these have not yet been tested.

### macOS
* Run the `build.sh` or `rebuild.sh` scripts from within the project directory to generate the necessary Makefile and build configuration. You can optionally specify the `--xcode` command line flag when building with either command to generate an Xcode project if you prefer to work with Xcode.
* Once the build succeeds, run the `run.sh` script any time to run the engine.

### Windows
* Open a powershell window, then go the project directory using `cd your\project\path`. Run the `build.ps1` or `rebuild.ps1` powershell script to generate a Visual Studio project. You can then open the `Zenith.sln` solution to run and build the project as normal

  **OR**
  
* Use the CMake GUI to generate the Visual Studio project files, making sure to select *Visual Studio 15 2017 Win64* as the generator.

**Note:** You can include `-development` (`--development` on Unix) as a command line flag for both the `build` and `rebuild` commands. This has some additional implications on how resources are expected to be bundled. For more information check out the [Resources and Assets](https://github.com/gitbetter/Zenith/wiki/Resources-and-Assets) wiki page.

## Overview

Take a look at the **[Zenith Wiki](https://github.com/gitbetter/Zenith/wiki)** for more info, but keep in mind that it is not  an authoritative API reference. To that end, perusing the headers and source files is a much better idea. In case you're wondering why the project size is so large, most if it resides in the Assets.zip file, which contains some hefty demo textures, but they are by no means required for the engine to run properly, and you are free to play with your own assets if you have any laying around. Stay tuned for more developments.

## License
Zenith is available under the GNU General Public License, and therefore anyone is free to fork the project and modify it to their hearts content as long as they maintain attributions to the original author of the project.

## Contributing
If you wish to contribute to this project (at which you have my many thanks), please submit any pull requests you may have to the Gold branch and not master, as it is easier for me to integrate changes selectively if need be. If you have any inquiries or particularly nice ideas, feel free to contact me at adrian@pervasivesense.com
