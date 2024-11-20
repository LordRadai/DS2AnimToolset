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
#include "MorphemeCharacterDef.h"
#include "extern.h"
#include "RCore.h"

#include "MorphemeSystem/MorphemeSystem.h"
#include "MorphemeSystem/MorphemeUtils/MorphemeUtils.h"
#include "../AssetLoader/AssetLoader.h"
//----------------------------------------------------------------------------------------------------------------------

namespace
{
    bool compareAnimObjs(AnimObject* first, AnimObject* second)
    {
        return std::string(first->getAnimName()) < std::string(second->getAnimName());
    }

    int getChrIdFromNmbFileName(std::wstring name)
    {
        std::wstring chr_id_str;
        int chrId = -1;

        int lastCPos = name.find_last_of(L"\\");

        if (name.substr(lastCPos + 1, 1).compare(L"c") != 0)
            return -1;

        chr_id_str = name.substr(lastCPos + 2, 4);

        chrId = stoi(chr_id_str);

        return chrId;
    }

}

//----------------------------------------------------------------------------------------------------------------------
MorphemeCharacterDef* MorphemeCharacterDef::create(const char* filename)
{
  //----------------------------
  // Create and initialise an instance of our character definition
  MorphemeCharacterDef* const instance = static_cast<MorphemeCharacterDef*>(NMPMemoryAlloc(sizeof(MorphemeCharacterDef)));
  new(instance) MorphemeCharacterDef();

  std::filesystem::path filepath(filename);
  std::filesystem::path file_name = filepath.filename();

  strcpy(instance->m_metadata.m_bundleDir, filepath.parent_path().string().c_str());
  strcpy(instance->m_filename, file_name.string().c_str());

  instance->m_chrId = getChrIdFromNmbFileName(filepath.c_str());
  g_appLog->debugMessage(MsgLevel_Debug, "\tChr ID: %d\n", instance->m_chrId);

  //----------------------------
  // Load the given bundle file into memory and load the bundle.

  //----------------------------
  // In this tutorial we no longer hold bundle information in GameCharacterDef as the unloading is handled
  // by the GameAssetLoader.
  void* bundle = NULL;
  int64_t bundleSize = 0;

  g_appLog->debugMessage(MsgLevel_Debug, "\tLoading SimpleBundle at \"%s\":\n", filename);

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
      MorphemeCharacterDef::destroy(instance);

      NMP_ASSERT_FAIL();
    }
  }

  //----------------------------
  // We have loaded the information in the buffer now so we can free it.
  NMP::Memory::memFree(bundle);

  return instance;
}

//--------------------------------------------------------------------------------------------------------------------
bool MorphemeCharacterDef::destroy(MorphemeCharacterDef* characterDef)
{
  NMP_ASSERT(characterDef);

  characterDef->term();

  NMP::Memory::memFree(characterDef);
  characterDef = NULL;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MorphemeCharacterDef::loadAnimations()
{  
  //----------------------------
  // Load animations listed in this network definition
  for (UINT i = 0; i < m_netDef->getNumAnimSets(); ++i)
  {
    g_appLog->debugMessage(MsgLevel_Debug, "Loading animations for anim set %d\n", i);
    m_netDef->loadAnimations((MR::AnimSetIndex)i, &m_metadata);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MorphemeCharacterDef::init(void* bundle, size_t bundleSize)
{  
  if (!bundle || !bundleSize)
  {
    return NULL;
  }

  //----------------------------
  // Evaluate the number of registered (core) and client assets and allocate appropriate storage.
  AssetLoader::evalBundleRequirements(m_numRegisteredAssets,
    m_numClientAssets,
    bundle,
    bundleSize);

  NMP_ASSERT(m_numRegisteredAssets > 0);

  //----------------------------
  // Allocate arrays for storing asset information based on the bundle requirements
  m_registeredAssetIDs = (UINT*)NMPMemoryCalloc(m_numRegisteredAssets * sizeof(UINT));
  m_clientAssets = (void**)NMPMemoryCalloc(m_numClientAssets * sizeof(void*));
  m_rigToAnimMaps.clear();

  g_appLog->debugMessage(MsgLevel_Info, "\t\tBundle size: %d\n", bundleSize);

  //----------------------------
  // Process the bundle and extract the contents into memory
  m_netDef = AssetLoader::loadBundle(
                   bundle,
                   bundleSize,
                   m_registeredAssetIDs,
                   m_clientAssets,
                   m_numRegisteredAssets,
                   m_numClientAssets,
                   m_rigToAnimMaps,
                   m_characterControllerDefs,
                   m_metadata.m_animFileLookUp);

  if (!m_netDef)
  {
    return false;
  }

  m_isLoaded = true;

  return true;
}

// --------------------------------------------------------------------------------------------------------------------
bool MorphemeCharacterDef::term()
{
  if( m_isLoaded )
  {
    for (UINT i = 0; i < m_netDef->getNumAnimSets(); ++i)
    {
      g_appLog->debugMessage(MsgLevel_Info, "Unloading animations for anim set %d\n", i);

      if (!m_netDef->unloadAnimations((MR::AnimSetIndex)i, NULL))
      {
        return false;
      }
    }

    g_appLog->debugMessage(MsgLevel_Info, "Unloading bundles\n");

    //----------------------------
    // In the same theme as loadBundle above we call a function that can be cut-and-paste into any program.
    AssetLoader::unLoadBundle(m_registeredAssetIDs, m_numRegisteredAssets, m_clientAssets, m_numClientAssets);
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

MR::RigToAnimMap* MorphemeCharacterDef::getRigToAnimMap(int idx)
{
    if (idx < m_rigToAnimMaps.size())
        return m_rigToAnimMaps[idx];

    return nullptr;
}

MR::CharacterControllerDef* MorphemeCharacterDef::getCharacterController(int idx)
{
    if (idx < m_characterControllerDefs.size())
        return m_characterControllerDefs[idx];

    return nullptr;
}

AnimObject* MorphemeCharacterDef::getAnimation(int idx)
{
    if (idx < m_anims.size())
        return m_anims[idx];

    return nullptr;
}

AnimObject* MorphemeCharacterDef::getAnimationById(int id)
{
    for (size_t i = 0; i < this->m_anims.size(); i++)
    {
        if (this->m_anims[i]->getAnimID() == id)
            return this->m_anims[i];
    }

    return nullptr;
}

void MorphemeCharacterDef::addAnimation(const char* filename, int animSetIdx)
{
    g_appLog->debugMessage(MsgLevel_Debug, "\Adding animation \"%s\"\n", filename);

    const int idx = m_anims.size();
    
    m_anims.push_back(AnimObject::createFromMorphemeAssets(this, this->m_netDef->getRig(animSetIdx), MorphemeUtils::getRigToAnimMapByAnimID(this->m_netDef, idx, animSetIdx), filename, idx));
}

void MorphemeCharacterDef::sortAnimations()
{
    std::sort(m_anims.begin(), m_anims.end(), compareAnimObjs);
}