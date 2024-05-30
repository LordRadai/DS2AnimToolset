// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef GAME_CHARACTER_DEF_H
#define GAME_CHARACTER_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrDispatcher.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
#include <vector>
#include <filesystem>
#include "AnimSourceInterface/AnimSourceInterface.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Game::CharacterDef is a wrapper class for a morpheme 'network' or 'character' definition. Each instance of
// CharacterDef should contain a unique set of definitions of which to create network/character instances from. In a
// game scenario CharacterDef could also be used for other character definition data (model/texture assets for
// example).
class CharacterDefBasic
{
public:
    struct AnimData
    {
        char m_bundleDir[500];
        MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* m_animFileLookUp;
    };

  virtual ~CharacterDefBasic() {};

  //----------------------------
  // Static method to load the network file (a SimpleBundle), relocate the objects and pointers it contains, and register
  // them with morpheme. This information is then stored in the CharacterDef instance that is passed in - A pointer to
  // the gameCharacterDef is stored in this Game::World for management.
  static CharacterDefBasic* create(const char* filename);

  //----------------------------
  // term and free a game character def instance
  static bool destroy(CharacterDefBasic* characterDef);  

  //----------------------------
  // Load the animations listed for this character definition
  bool loadAnimations();

  //----------------------------
  // Accessors
  bool  isLoaded()  const { return m_isLoaded; }

  MR::NetworkDef* getNetworkDef() const { return m_netDef; }

  UINT* getRegisteredAssetIDs()  const { return m_registeredAssetIDs; }
  void**    getClientAssets()        const { return m_clientAssets; }
  UINT  getNumRegisteredAssets() const { return m_numRegisteredAssets; }
  UINT  getNumClientAssets()     const { return m_numClientAssets; }

  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* getAnimFileLookUp() const { return m_metadata.m_animFileLookUp; }
  const char* getBundleFilepath() const { return m_metadata.m_bundleDir; }
  const char* getFilename() const { return m_filename; }
  MR::RigToAnimMap* getRigToAnimMap(int idx);
  AnimSourceInterface* getAnimation(int idx);
  AnimSourceInterface* getAnimationById(int id);
  void addAnimation(const char* filename);
  void sortAnimations();

protected:

  CharacterDefBasic():
    m_isLoaded(false),
    m_netDef(NULL),
    m_metadata{ "", NULL},
    m_registeredAssetIDs(NULL),
    m_clientAssets(NULL),
    m_numRegisteredAssets(0),
    m_numClientAssets(0)
  {
  };

  //----------------------------
  // Initialise required data
  bool init(void* bundle, size_t bundleSize);

  //----------------------------
  // Free memory
  bool term();

protected:

  bool  m_isLoaded;

  std::vector<MR::RigToAnimMap*> m_rigToAnimMaps;
  MR::NetworkDef* m_netDef;

  UINT* m_registeredAssetIDs;
  void**    m_clientAssets;
  UINT  m_numRegisteredAssets;
  UINT  m_numClientAssets;

  AnimData m_metadata;
  char m_filename[255];

  std::vector<AnimSourceInterface*> m_anims;
};

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_DEF_H
//----------------------------------------------------------------------------------------------------------------------
