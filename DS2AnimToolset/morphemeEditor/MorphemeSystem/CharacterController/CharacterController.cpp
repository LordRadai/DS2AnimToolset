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
#include "CharacterController.h"

#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
CharacterController* CharacterController::create()
{
  CharacterController* const instance = 
    static_cast<CharacterController*>(NMPMemoryAlloc(sizeof(CharacterController)));

  new(instance) CharacterController();

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterController::destroy(CharacterController* characterController)
{
  NMP_ASSERT(characterController);

  NMP::Memory::memFree(characterController);
  characterController = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t CharacterController::init(MR::Network* network)
{
  m_net = network;

  // Set the world root transform to the character position/orientation
  m_net->updateCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_characterOrientation, m_characterPosition), false);

  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterController::updateTransform(
  const NMP::Vector3* requestedDeltaWorldTranslation,
  const NMP::Quat*    requestedDeltaWorldOrientation)
{
  if (requestedDeltaWorldTranslation && requestedDeltaWorldOrientation)
  {
    //----------------------------
    // Update the controller transform using the delta values that have been supplied.
    //  This path allows the caller to override 
    m_characterPosition += (*requestedDeltaWorldTranslation);
    m_characterOrientation *= (*requestedDeltaWorldOrientation);
  }
  else
  {
    //----------------------------
    // Update the controller transform using the delta values directly calculated by the Network.
    m_characterPosition += m_characterOrientation.rotateVector(m_net->getTranslationChange());
    m_characterOrientation *= m_net->getOrientationChange();
    m_characterOrientation.normalise();
  }

  //----------------------------
  // Update the Networks world RootTransform with the new position and orientation.
  m_net->updateCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_characterOrientation, m_characterPosition), true);
  m_net->setCharacterPropertiesAchievedRequestedMovement(true);
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterController::setPosition(const NMP::Vector3& position)
{
  m_characterPosition = position;
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterController::setOrientation(const NMP::Quat& orientation)
{
  m_characterOrientation = orientation;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterController::CharacterController():
m_characterPosition(NMP::Vector3::InitZero),
m_characterOrientation(NMP::Quat::kIdentity),
m_net(NULL)
{
}