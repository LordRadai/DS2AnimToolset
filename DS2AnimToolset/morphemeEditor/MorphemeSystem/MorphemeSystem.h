#pragma once
#include "MorphemeCharacter/MorphemeCharacter.h"
#include "MorphemeCharacterDef/MorphemeCharacterDef.h"
#include "MorphemeDecompiler/MorphemeDecompiler.h"

#include <vector>
#include <string>

namespace MorphemeSystem
{
	//----------------------------
	// Initialise function for MorphemeSystem. Any core morpheme or animation specific initialisation
	// should occur in this functions.
	void initMorpheme();
	
	//----------------------------
	// Free allocated memory and shutdown
	void termMorpheme();

	//----------------------------
	// Create and initialise a CharacterDef; loading its assets a specified simple bundle source file.
	//
	// CharacterDefs that are created via the Manager are automatically registered with the manager
	//  and the memory management of the Character remains the responsibility of the Manager.
	MorphemeCharacterDef* createCharacterDef(const char* filename);  //< Name of simple bundle file where we should try and load this characters assets from.
};