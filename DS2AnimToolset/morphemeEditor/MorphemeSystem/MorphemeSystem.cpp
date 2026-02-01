#include "MorphemeSystem.h"
#include "AnimLoader/AnimLoader.h"
#include "XMD/Model.h"

#include "extern.h"
#include "RCore.h"

#define NUM_EXPECTED_ATTRIB_SEMANTICS 74

void MorphemeSystem::initMorpheme()
{
    NMP::Memory::init();

    //----------------------------
    // Initialise morpheme library
    g_appLog->debugMessage(MsgLevel_Info, "Initialising Morpheme\n");
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

    XMD::XMDInit();

    const int numRegisteredAttribSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();

    //----------------------------
    // Dark Souls II Scholar of the First Sin registers exactly 74 ATTRIB_SEMANTICs. We must ensure that the count matches or else the output nmb will be incorrect
    if (numRegisteredAttribSemantics != NUM_EXPECTED_ATTRIB_SEMANTICS)
        g_appLog->panicMessage("Invalid amount of registered ATTRIB_SEMANTIC (expecting %d, got %d)\n", NUM_EXPECTED_ATTRIB_SEMANTICS, numRegisteredAttribSemantics);
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
    g_appLog->debugMessage(MsgLevel_Info, "Terminating Morpheme\n");

    //----------------------------
    // Terminate morpheme library
    XMD::XMDCleanup();
    MR::DispatcherBasic::term();
    MR::Manager::termMorphemeLib();
    NMP::Memory::shutdown();

    g_appLog->debugMessage(MsgLevel_Info, "Morpheme shutdown\n");
}

//----------------------------------------------------------------------------------------------------------------------
// Creates a CharacterDef and registers it with the manager.
MorphemeCharacterDef* MorphemeSystem::createCharacterDef(const char* filename)
{
    g_appLog->debugMessage(MsgLevel_Info, "Creating CharacterDef from file \"%s\"\n", filename);

    MorphemeCharacterDef* characterDef = MorphemeCharacterDef::create(filename);
    if (!characterDef || !characterDef->isLoaded())
    {
        NMP_ASSERT_FAIL();
        return NULL;
    }

    return characterDef;
}