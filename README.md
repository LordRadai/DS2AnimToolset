# DS2AnimToolset
A set of tools to load, inspect, edit and decompile morpheme runtime binary assets for Dark Souls II Scholar of the First Sin.

# morphemeEditor
Previously known as MorphemeConnect.
This program lets you open and edit Dark Souls II Scholar of the First Sin morpheme binaries alongside TimeAct files.
It can be used in the following ways:
1) Open an NMB file. The program will search for the Game folder in the parent path of the opened file, if it finds it it will then look for /timeact/chr and search for all the TimeAct files that share the NMB's character ID in the name and ask the user if they'd like to open one of them. It will also look for the character's BND in the /model/chr folder.
2) Open a TimeAct file. The program will parse the opened file and add the TimeAct list to the TimeAct tab in the Asset window. If the file opened belongs to an object, then it will also attempt to find that object's BND in the /model/obj folder.

## Preview Window
When opening an NMB, the program will attempt to find the character model in the parent path. If it finds one and it has valid vertex data in it, it will show the model in the Model Viewer window.

## TimeAct Templates, Tooltips
Templates are inside the `Data/res` folder. If you want to make changes to the templates, just edit `TimeActTemplate.xml`.
Tooltips are inside the `Data/res/tooltip` folder. They are shown when you hover an event in the editor windows.

## Export
You can export animations and models to FBX, glTF or XMD using the Export menu under File. Note that animation files do not contain the model within them, and that exporting animations will automatically export the model.

## Build Requirements
If you want to compile this project, you need the following things:
* DirectXTK UWP (install with VS GnuPackage)
* ICU
* ZLIB
* FBX SDK

# mcnGen
A set of scripts, run by a batch script, to decompile an export from morphemeEditor back to morphemeConnect source projects.

Instructions [here](https://github.com/LordRadai/DS2AnimToolset/blob/main/DS2AnimToolset/Tools/mcn/README.md)

# Decompiled Source Projects
Here's a shared folder with all the decompiled game morpheme projects as source:
https://drive.google.com/drive/folders/1N0WAoNuqFO-evxonbvBdZ9A_sjbv0JcV?usp=sharing

# Bugs
Report any bugs in the Discord server's bug report forum https://discord.gg/CJk2b5WMMF
