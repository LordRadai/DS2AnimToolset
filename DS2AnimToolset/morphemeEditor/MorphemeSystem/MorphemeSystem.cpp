#include "MorphemeSystem.h"
#include "AssetLoader/AssetLoader.h"
#include "simpleBundle/simpleBundle.h"
#include "NMPlatform/NMFile.h"
#include "morpheme/Nodes/mrNodes.h"
#include "morpheme/Nodes/mrNodeAnimSyncEvents.h"
#include "AnimLoader/AnimLoader.h"

#include "extern.h"
#include "utils/RLog/RLog.h"

//----------------------------------------------------------------------------------------------------------------------
void MorphemeSystem::initMorpheme()
{
    NMP::Memory::init();

    //----------------------------
    // Initialise morpheme library
    g_appLog->DebugMessage(MsgLevel_Info, "Initialising Morpheme\n");
    MR::Manager::initMorphemeLib();

    //----------------------------
    // Dispatcher initialisation
    MR::Dispatcher* dispatcher = MR::DispatcherBasic::createAndInit();

    //----------------------------
    // Finalise semantic and core task registration
    MR::Manager::getInstance().allocateRegistry();
    MR::CoreTaskIDs::registerNMCoreTasks(dispatcher);

    MR::Manager::getInstance().finaliseInitMorphemeLib();

    //----------------------------
    // Tidy up the dispatcher instance since we have finished initialising the static data
    dispatcher->releaseAndDestroy();

    //----------------------------
    // Initialise animation file handling functions
    MR::Manager::getInstance().setAnimFileHandlingFunctions(
        AnimLoader::requestAnim,
        AnimLoader::releaseAnim);
}

//----------------------------------------------------------------------------------------------------------------------
// Unload any data stored in this MorphemeSystem, including CharacterDefs that have been loaded and unload the memory
// they store.
//
// Note that this isn't exactly robust - if we load bundle A and bundle B, and both contain the same rig, the rig will
// be registered only once (by A), but will be unregistered when B is torn down. If A is not immediately torn down it
// will reference the rig that has just been unregistered. Since the pointer is still valid this will not immediately
// fail, but may introduce subtle bugs. To get around this, either load and unload all your network assets at the same
// time, or use a ref counting scheme to register and unregister assets. See the Multiple Network tutorials for more
// details.
void MorphemeSystem::termMorpheme()
{
    g_appLog->DebugMessage(MsgLevel_Info, "Terminating Morpheme\n");

    if (m_characterData)
    {
        //----------------------------
        // Once we've finished with the binary file release it.
        Character::destroy(m_characterData);
    }

    if (m_characterDef && m_characterDef->isLoaded())
    {
        //----------------------------
        // Once we've finished with the binary file release it.
        CharacterDef::destroy(m_characterDef);
    }

    //----------------------------
    // Terminate morpheme library
    MR::DispatcherBasic::term();
    MR::Manager::termMorphemeLib();
    NMP::Memory::shutdown();

    g_appLog->DebugMessage(MsgLevel_Info, "Morpheme shutdown\n");
}

//----------------------------------------------------------------------------------------------------------------------
// Creates a CharacterDef and registers it with the manager.
CharacterDef* MorphemeSystem::createCharacterDef(const char* filename)
{
    if (this->m_characterDef)
        CharacterDef::destroy(this->m_characterDef);

    this->m_characterDef = nullptr;

    g_appLog->DebugMessage(MsgLevel_Info, "Creating CharacterDefBasic from file %s\n", filename);

    CharacterDef* gameCharacterDef = CharacterDef::create(filename);
    if (!gameCharacterDef || !gameCharacterDef->isLoaded())
    {
        NMP_ASSERT_FAIL();
        return NULL;
    }
    registerCharacterDef(gameCharacterDef); // The management of this gameCharacterDefs memory is is this CharacterManagers responsibility.
    return gameCharacterDef;
}

//----------------------------------------------------------------------------------------------------------------------
void MorphemeSystem::registerCharacterDef(CharacterDef* characterDef)
{
    //----------------------------
    // store a pointer to the character for use later
    m_characterDef = characterDef;
}

//----------------------------------------------------------------------------------------------------------------------
void MorphemeSystem::registerCharacter(Character* characterData)
{
    //----------------------------
    // store a pointer to the character for use later
    m_characterData = characterData;
}

//----------------------------------------------------------------------------------------------------------------------
void MorphemeSystem::update(float timeDelta)
{
    if (m_characterData)
    {
        m_characterData->update(timeDelta);
    }
}

CharacterDef* MorphemeSystem::GetCharacterDef()
{
    return this->m_characterDef;
}

Character* MorphemeSystem::GetCharacter()
{
    return this->m_characterData;
}

bool MorphemeSystem::ExportXML()
{
    if (this->m_characterDef == nullptr)
        return false;

    return true;
}