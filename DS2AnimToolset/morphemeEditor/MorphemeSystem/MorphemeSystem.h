#pragma once
#include "Character/Character.h"
#include "CharacterDef/CharacterDef.h"
#include "MorphemeExport/MorphemeExport.h"

#include <vector>
#include <string>

#include "morpheme/mrAnimationSourceHandle.h"

class MorphemeSystem
{
public:

	MorphemeSystem() {}
	~MorphemeSystem() {}

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
	CharacterDef* createCharacterDef(const char* filename);  //< Name of simple bundle file where we should try and load this characters assets from.

	//----------------------------
	// Stores a pointer to a characterDef. This allows Game::GameCharacterManager to manage this character definition.
	void registerCharacterDef(CharacterDef* characterDef);

	//----------------------------
	// Pass a pointer to a Character to the CharacterList where it is stored.
	void registerCharacter(Character* character);

	//----------------------------
	// Update any networks that are available
	void update(float timeDelta);

	//----------------------------
	// Get character def
	CharacterDef* GetCharacterDef();

	//----------------------------
	// Get character instance
	Character* GetCharacter();

	bool ExportXML();
protected:
	CharacterDef* m_characterDef;   // Store a pointer to a morpheme character definition
	Character* m_characterData;  // Store a pointer to a morpheme character
};