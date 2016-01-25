## Requirements ##

  * cmake 2.8
  * For Windows: Visual C++ 2008 (the Express version works fine); possibly a Windows SDK
  * For MacOS: Xcode

## Details ##

  * Check out a copy of the source
  * Make a build directory alongside your source tree (if you checked out to 'trunk', make a 'trunk\_build' at the same level)
  * cd to the build directory and run `cmake -G "Visual Studio 9 2008" ..\trunk` for MSVC or `cmake -G Xcode ..\trunk` for Xcode
  * Open 'Project.sln' and build the 'INSTALL' target in MSVC, or open 'Project.xcodeproj' and build the 'wavein' target in Xcode
  * The DLL/Bundle will be installed automatically at the correct location in the Unity3D project directory