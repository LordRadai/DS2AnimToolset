# DS2AnimToolset
Currently contains morphemeEditor only. Things will come with future patches.

# morphemeEditor
Previously known as MorphemeConnect.
This program lets you open and edit DS2 Morpheme binaries alongside TimeAct files.
It can be used in the following ways:
1) Open an NMB file. The program will search for the Game folder in the parent path of the opened file, if it finds it it will then look for /timeact/chr and search for all the TimeAct files that share the NMB's character ID in the name and ask the user if they'd like to open one of them. It will also look for the character's BND in the /model/chr folder.
2) Open a TimeAct file. The program will parse the opened file and add the TimeAct list to the TimeAct tab in the Asset window. If the file opened belongs to an object, then it will also attempt to find that object's BND in the /model/obj folder.

**IMPORTANT NOTE**: This version will NOT let you modify Event Tracks. That feature had to be disabled because of the Morpheme SDK integration. If you wish to have this feature, use MorphemeConnect instead: https://github.com/LordRadai/MorphemeConnect

# Preview Window
When opening an NMB, the program will attempt to find the character model in the parent path. If it finds one and it has valid vertex data in it, it will show the model in the Preview window. Note that c0001 does not have a model, player equipments are not supported yet.

![Immagine 2024-05-30 194747](https://github.com/LordRadai/DS2AnimToolset/assets/22768664/63fd3aad-6612-4b50-85ff-7b935350156b)

# TimeAct Templates, Tooltips
Templates are inside the `MorphemeConnect/res` folder. If you want to make changes to the templates, just edit `TimeActTemplate.xml`.
Tooltips are inside the `MorphemeConnect/res/tooltip folder`. They are shown when you hover an event in the editor windows.

# Export
You can export animations and models to FBX using the Export menu under File. Note that animation files do not contain the model within them, and that exporting animations will automatically export models

# TODO
* Implement project save as XML
* Implement NMB generation logic to allow the creation of new EventTrack bundles.
* Implement EventTrack editing

# Build Requirements
If you want to compile this project, you need the following things:
* DirectXTK UWP (install with VS GnuPackage)
* ICU
* ZLIB
* FBX SDK
* betstick's cfromloader

# Bugs
Report any bugs in the Discord server's bug report forum https://discord.gg/CJk2b5WMMF

![Immagine 2024-05-30 194830](https://github.com/LordRadai/DS2AnimToolset/assets/22768664/218df726-5ad0-41d5-a885-fc955c4e3678)
