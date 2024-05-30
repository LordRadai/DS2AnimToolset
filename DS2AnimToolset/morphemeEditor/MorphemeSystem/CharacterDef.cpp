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
#include "CharacterDef.h"

#include "MorphemeSystem.h"
#include "AssetLoader.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
CharacterDefBasic* CharacterDefBasic::create(const char* filename)
{
  //----------------------------
  // Create and initialise an instance of our character definition
  CharacterDefBasic* const instance = static_cast<CharacterDefBasic*>(NMPMemoryAlloc(sizeof(CharacterDefBasic)));
  new(instance) CharacterDefBasic();

  std::filesystem::path filepath(filename);
  std::filesystem::path file_name = filepath.filename();

  strcpy(instance->m_metadata.m_bundleDir, filepath.parent_path().string().c_str());
  strcpy(instance->m_filename, file_name.string().c_str());

  //----------------------------
  // Load the given bundle file into memory and load the bundle.

  //----------------------------
  // In this tutorial we no longer hold bundle information in GameCharacterDef as the unloading is handled
  // by the GameAssetLoader.
  void* bundle = NULL;
  int64_t bundleSize = 0;

  //----------------------------
  // Load binary bundle into memory
  int64_t fileSize = NMP::NMFile::allocAndLoad(filename, &bundle, &bundleSize);
  if (fileSize <= -1)
  {
    NMP_ASSERT_FAIL();

    return NULL;
  }
  else
  {
    NMP_ASSERT(bundleSize >= fileSize);

    //----------------------------
    // Allocate memory and unpack bundle
    if(!(instance->init(bundle, (size_t)bundleSize)))
    {
      CharacterDefBasic::destroy(instance);

      NMP_ASSERT_FAIL();
    }
  }

  //----------------------------
  // We have loaded the information in the buffer now so we can free it.
  NMP::Memory::memFree(bundle);

  return instance;
}

//--------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::destroy(CharacterDefBasic* characterDef)
{
  NMP_ASSERT(characterDef);

  characterDef->term();

  NMP::Memory::memFree(characterDef);
  characterDef = NULL;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::loadAnimations()
{  
  //----------------------------
  // Load animations listed in this network definition
  for (UINT i = 0; i < m_netDef->getNumAnimSets(); ++i)
  {
    m_netDef->loadAnimations((MR::AnimSetIndex)i, &m_metadata);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::init(void* bundle, size_t bundleSize)
{  
  if (!bundle || !bundleSize)
  {
    return NULL;
  }

  //----------------------------
  // Evaluate the number of registered (core) and client assets and allocate appropriate storage.
  AssetLoaderBasic::evalBundleRequirements(m_numRegisteredAssets,
    m_numClientAssets,
    bundle,
    bundleSize);

  NMP_ASSERT(m_numRegisteredAssets > 0);

  //----------------------------
  // Allocate arrays for storing asset information based on the bundle requirements
  m_registeredAssetIDs = (UINT*)NMPMemoryCalloc(m_numRegisteredAssets * sizeof(UINT));
  m_clientAssets = (void**)NMPMemoryCalloc(m_numClientAssets * sizeof(void*));
  m_rigToAnimMaps.clear();

  //----------------------------
  // Process the bundle and extract the contents into memory
  m_netDef = AssetLoaderBasic::loadBundle(
                   bundle,
                   bundleSize,
                   m_registeredAssetIDs,
                   m_clientAssets,
                   m_numRegisteredAssets,
                   m_numClientAssets,
                   m_rigToAnimMaps,
                   m_metadata.m_animFileLookUp);

  if (!m_netDef)
  {
    return false;
  }

  m_isLoaded = true;

  return true;
}

// --------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::term()
{
  if( m_isLoaded )
  {
    for (UINT i = 0; i < m_netDef->getNumAnimSets(); ++i)
    {
      if (!m_netDef->unloadAnimations((MR::AnimSetIndex)i, NULL))
      {
        return false;
      }
    }

    //----------------------------
    // In the same theme as loadBundle above we call a function that can be cut-and-paste into any program.
    AssetLoaderBasic::unLoadBundle(m_registeredAssetIDs, m_numRegisteredAssets, m_clientAssets, m_numClientAssets);
  }

  if (m_registeredAssetIDs)
  {
    NMP::Memory::memFree(m_registeredAssetIDs);
  }

  if(m_clientAssets)
  {
    NMP::Memory::memFree(m_clientAssets);
  }
  //----------------------------
  // Free any memory that may be allocated in this class here
  return true;
}

MR::RigToAnimMap* CharacterDefBasic::getRigToAnimMap(int idx)
{
    if (idx < m_rigToAnimMaps.size())
        return m_rigToAnimMaps[idx];

    return nullptr;
}

AnimSourceInterface* CharacterDefBasic::getAnimation(int idx)
{
    if (idx < m_anims.size())
        return m_anims[idx];

    return nullptr;
}

AnimSourceInterface* CharacterDefBasic::getAnimationById(int id)
{
    for (size_t i = 0; i < this->m_anims.size(); i++)
    {
        if (this->m_anims[i]->GetID() == id)
            return this->m_anims[i];
    }

    return nullptr;
}

void CharacterDefBasic::addAnimation(const char* filename)
{
    int idx = m_anims.size();

    m_anims.push_back(new AnimSourceInterface(this->m_netDef->getRig(0), this->m_rigToAnimMaps[0], filename, idx));
}

void CharacterDefBasic::sortAnimations()
{
    std::sort(m_anims.begin(), m_anims.end(), AnimSourceInterface::lessThan);
}