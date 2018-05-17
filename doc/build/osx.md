# Build on OSX

## Prerequisites

If you haven't already, install Xcode Dev Tools, then Cmake. Xcode can be found in the app store, and
Cmake has installation instructions [here](https://cmake.org/install/).
These instructions will require both of these toolkits.

## Installing

* Open Terminal, OSX's default command line tool.
* Cd into the folder you wish to host your local repo.
```
cd /path/to/folder
```
* Create a folder specific to the repo.
```
mkdir Ketsji
cd Ketsji
```
* Clone the project.
```
git clone https://github.com/bge-community/Ketsji-Game-Engine.git
```
* Cd into the repo
```
cd Ketsji-Game-Engine
```
* Update all packages
```
make update
```
* Open Cmake
* Browse to the repo with "Browse Source" button
* ex: /path/to/folder/Ketsji/Ketsji-Game-Engine
* Browse to a location to build the Xcode project with "Browse Build" button
* ex: /path/to/folder/Ketji/KetsjiGE
* Click "Configure" button. This will generate a list of options you may enable or disable to your preferences.
* Once complete, click "Configure" again, they will change from red to white.
* Click "Generate", and once complete click "open"
* Xcode will open with your blender project.

## First Build

In order to build successfully on Mac, follow these steps.

* In terminal, from the repo directory, switch to the Osx master branch
```
git checkout OSX/master-build
```
* Open the project in XCode.
* Click "Autogenerate Schemes" in the alert dialog that appears.
* Click the text to the right of the "Stop" button in the top right and select "install" from the drop down.
* Navigate to the menu Product-> Scheme-> Edit Scheme  (Command + shift + '<')
* Click "Run" in the sidebar
* Select "Blender App" from the dropdown next to "Executable"
* Back to the main panel, click the "play" button in the top left to build in debug mode.

## Developer settings
* This part is important to prevent you from pushing Mac specific configuration changes to the main branches.  ONLY OSX/master-build should have these files/settings
* In terminal, from the repo directory, switch to master branch and remove the following file from local tracking.
```
git checkout master
git update-index intern/mathfu/mathfu/utilities.h
```
* Now when you push a branch, make sure that those two files are not staged to be committed. Otherwise your push will be rejected during code review.
* Good luck, and have fun!
