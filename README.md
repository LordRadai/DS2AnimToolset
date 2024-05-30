# MorphemeConnect
This program lets you open and edit DS2 Morpheme binaries alongside TimeAct files.
It can be used in the following ways:
1) Open an NMB file. The program will search for the Game folder in the parent path of the opened file, if it finds it it will then look for /timeact/chr and search for all the TimeAct files that share the NMB's character ID in the name and ask the user if they'd like to open one of them. It will also look for the character's BND in the /model/chr folder.
2) Open a TimeAct file. The program will parse the opened file and add the TimeAct list to the TimeAct tab in the Asset window. If the file opened belongs to an object, then it will also attempt to find that object's BND in the /model/obj folder.

# Preview Window
When opening an NMB, the program will attempt to find the character model in the parent path. If it finds one and it has valid vertex data in it, it will show the model in the Preview window. Note that c0001 do not have a model, player equipments are not supported yet.

![Immagine 2023-08-31 211346](https://github.com/LordRadai/MorphemeConnect/assets/22768664/9eac1a67-eb14-4d2b-a3b1-c18dcf99c0b0)

# TimeAct Templates, Tooltips
Templates are inside the `MorphemeConnect/res/def/timeact` folder. If you want to make changes to the templates, just edit `timeact.json`.
Tooltips are inside the `MorphemeConnect/res/tooltip folder`. They are shown when you hover an event in the editor windows.

# TODO
* Implement project save as XML
* Implement NMB Network generation logic to allow the creation of new EventTrack bundles.
* Implement proper character skeleton loading
* Implement NSA animation preview

# Build Requirements
If you want to compile this project, you need the following things:
* DirectXTK UWP (install with VS GnuPackage)
* ICU
* ZLIB
* betstick's cfromloader

# Bugs
Report any bugs in the Discord server's bug report forum https://discord.gg/CJk2b5WMMF
