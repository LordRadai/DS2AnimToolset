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
#include "AssetLoader.h"

#include "MorphemeSystem.h"
#include "simpleBundle/simpleBundle.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void AssetLoaderBasic::evalBundleRequirements(
  UINT& numRegisteredAssets,
  UINT& numClientAssets,
  void*     buffer,
  size_t    bufferSize)
{
  numRegisteredAssets = 0;
  numClientAssets = 0;

  if (!buffer || !bufferSize)
  {
    NMP_DEBUG_MSG("error: Valid bundle data expected (%p, size=%u)!\n", buffer, bufferSize);
    return;
  }

  //----------------------------
  // Start parsing the bundle. 
  MR::UTILS::SimpleBundleReader bundleReader(buffer, bufferSize);

  MR::Manager::AssetType assetType;
  UINT assetID;
  uint8_t* fileGuid;
  void* asset;
  NMP::Memory::Format assetMemReqs;

  while (bundleReader.readNextAsset(assetType, assetID, fileGuid, asset, assetMemReqs))
  {
    if (assetType < MR::Manager::kAsset_NumAssetTypes)
    {
      // The pluginList is used only when loading the bundle and isn't registered with the manager
      if (assetType != MR::Manager::kAsset_PluginList)
      {
        ++numRegisteredAssets;
      }
    }
    else
    {
      ++numClientAssets;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* AssetLoaderBasic::loadBundle(
  void*            bundle,
  size_t           bundleSize,
  UINT*        registeredAssetIDs,
  void**           clientAssets,
  UINT         NMP_USED_FOR_ASSERTS(numRegisteredAssets),
  UINT         NMP_USED_FOR_ASSERTS(numClientAssets),
  std::vector<MR::RigToAnimMap*>& rigToAnimMaps,
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup*& animFileLookup)
{
  animFileLookup = NULL;

  if (!bundle || !bundleSize)
  {
    NMP_DEBUG_MSG("error: Valid bundle data expected (%p, size=%u)!\n", bundle, bundleSize);
    return NULL;
  }

  //----------------------------
  // Start parsing the bundle. The simple bundle has been written by the asset compiler and contains various types of 
  // assets like the network definition, rig definitions and animation markup but not the actual animation data.
  MR::UTILS::SimpleBundleReader bundleReader(bundle, (UINT)bundleSize);

  MR::Manager::AssetType assetType;
  void* asset;
  NMP::Memory::Format assetMemReqs;
  MR::RuntimeAssetID assetID;
  uint8_t* fileGuid = 0;

  UINT registeredAssetIndex = 0;
  UINT clientAssetIndex = 0;

  MR::NetworkDef* netDef = NULL;

  while (bundleReader.readNextAsset(assetType, assetID, fileGuid, asset, assetMemReqs))
  {
    //----------------------------
    // Only consider core runtime asset for registration with the manager. The locate process is also different for 
    // core and client assets, while core assets can be located using the manager, client assets need to be located 
    // explicitly - which could also be handled outside this method
    if (assetType < MR::Manager::kAsset_NumAssetTypes)
    {
      //----------------------------
      // Special case for plugin list.
      if (assetType == MR::Manager::kAsset_PluginList)
      {
        // We don't care about plugins. DS2 bundles use no physics
        continue;
      }

      // Grab locate function for this asset type
      const MR::AssetLocateFn locateFn = MR::Manager::getInstance().getAssetLocateFn(assetType);
      if (!locateFn)
      {
        //----------------------------
        // This may happen if you compiled your assets using an asset compiler with additional plug-ins registered
        // but are using a runtime with different modules. For more details see MR::registerCoreAssets() (called from
        // MR::Manager::initMorphemeLib()) and MR::initMorphemePhysics()
        NMP_DEBUG_MSG("error: Failed to locate core asset (type=%u, ID=%u)!\n", assetType, assetID);
        return NULL;
      }

      //----------------------------
      // If the asset is already registered just bump the reference count, if its a new ID the asset is loaded below
      void* const registeredAsset = (void*)MR::Manager::getInstance().getObjectPtrFromObjectID(assetID);
      if (registeredAsset)
      {
        asset = registeredAsset;
      }
      else
      {
        //----------------------------
        // Allocate memory to store the asset for runtime use. The memory is freed as the reference count goes to 
        // zero in unloadAssets() while the bundle memory can be freed right after this methods has completed
        void* const bundleAsset = asset;
        asset = NMPMemoryAllocateFromFormat(assetMemReqs).ptr;
        NMP::Memory::memcpy(asset, bundleAsset, assetMemReqs.size);

        //----------------------------
        // Locate the asset (in-place pointer fix-up)
        if (!locateFn(assetType, asset))
        {
          NMP_DEBUG_MSG("error: Failed to locate core asset (type=%u, ID=%u)!\n", assetType, assetID);
          return NULL;
        }

        //----------------------------
        // Register the object (initialises the reference count to zero)
        if (!MR::Manager::getInstance().registerObject(asset, assetType, assetID))
        {
          NMP_DEBUG_MSG("error: Failed to register asset (type=%u, ID=%u)!\n", assetType, assetID);
          return NULL;
        }
      }

      //----------------------------
      // Increment reference count
      MR::Manager::incObjectRefCount(assetID);

      switch (assetType)
      {
      case MR::Manager::kAsset_Rig:
          break;
      case MR::Manager::kAsset_RigToAnimMap:
          rigToAnimMaps.push_back((MR::RigToAnimMap*)asset);
          break;
      case MR::Manager::kAsset_EventTrackDiscrete:
          break;
      case MR::Manager::kAsset_EventTrackDuration:
          break;
      case MR::Manager::kAsset_EventTrackCurve:
          break;
      case MR::Manager::kAsset_PhysicsRigDef:
          break;
      case MR::Manager::kAsset_CharacterControllerDef:
          break;
      case MR::Manager::kAsset_InteractionProxyDef:
          break;
      case MR::Manager::kAsset_BodyDef:
          break;
      case MR::Manager::kAsset_NetworkDef:
          NMP_ASSERT(!netDef);  // We only expect one network definition per bundle
          netDef = (MR::NetworkDef*)asset;
          break;
      case MR::Manager::kAsset_NetworkPredictionDef:
          break;
      default:
          break;
      }

      //----------------------------
      // Log the asset ID for use in UnloadMorphemeNetwork().
      NMP_ASSERT(registeredAssetIndex < numRegisteredAssets);
      registeredAssetIDs[registeredAssetIndex++] = assetID;
    }
    else
    {
      //----------------------------
      // Allocate memory to store the asset for runtime use. The memory is freed in unLoadBundle() while the 
      // bundle memory can be freed right after this methods has completed.
      void* const bundleAsset = asset;
      asset = NMPMemoryAllocateFromFormat(assetMemReqs).ptr;
      NMP::Memory::memcpy(asset, bundleAsset, assetMemReqs.size);

      //----------------------------
      // Locate the asset (in-place pointer fix-up).
      switch (assetType)
      {
      case MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup::kAsset_SimpleAnimRuntimeIDtoFilenameLookup:
        NMP_ASSERT(!animFileLookup); // We only expect one filename lookup per bundle.
        animFileLookup = (MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup*)asset;
        animFileLookup->locate();
        break;

      default:
        NMP_DEBUG_MSG("warning: Failed to locate client asset (type=%u, ID=%u)!\n", assetType, assetID);
        break;
      }

      //----------------------------
      // Log the asset pointer for use in UnloadMorphemeNetwork().
      NMP_ASSERT(clientAssetIndex < numClientAssets);
      clientAssets[clientAssetIndex++] = asset;
    }
  }

  return netDef;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetLoaderBasic::unLoadBundle(
  const UINT* registeredAssetIDs,
  UINT        numRegisteredAssets,
  void* const*    clientAssets,
  UINT        numClientAssets)
{
  //----------------------------
  // Release registered assets but only free the associated memory if the reference count goes to zero.
  for (UINT i = 0; i < numRegisteredAssets; ++i)
  {
    const UINT assetId = registeredAssetIDs[i];

    //----------------------------
    // Unregister and free the asset if it's no longer referenced.
    if (MR::Manager::decObjectRefCount(assetId) == 0)
    {
      void* const asset = const_cast<void*>(MR::Manager::getInstance().getObjectPtrFromObjectID(assetId));
      MR::Manager::getInstance().unregisterObject(assetId);
      NMP::Memory::memFree(asset);
    }
  }

  //----------------------------
  // Free client assets.
  for (UINT i = 0; i < numClientAssets; ++i)
  {
    NMP::Memory::memFree(clientAssets[i]);
  }
}