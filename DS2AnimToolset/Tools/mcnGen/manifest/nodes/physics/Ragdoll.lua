------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- Ragdoll node definition.
------------------------------------------------------------------------------------------------------------------------
registerPhysicsNode("Ragdoll",
  {
    displayName = "Ragdoll",
    helptext = "Physics rig collapses given the current local space transforms as starting point. Animation input is used only to set the non-physical bones.",
    group = "Physics",
    image = "Ragdoll.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 123),
    version = 3,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
    },

    pinOrder = { "Source", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "DampingMultiplier", type = "float", value = 1, perAnimSet = true,
        displayName = "Damping",
        helptext = "If the character has been set up correctly, then the joint damping will represent damping due to the skeletal joints and clothes and so on, so that the limbs are not completely floppy. This joint damping will be modulated by the damping multiplier, which would normally be set to 1 (its default value)."
      },
      {
        name = "PoseStrengthMultiplier", type = "float", value = 0.02, perAnimSet = true,
        displayName = "Strength",
        helptext = "The maximum strength of each joint is multiplied by this value and then used to drive towards the centre of the rigs joint limits. This represents the tendency for joints to relax weakly towards a central position. If the centre of the joint limit isn't a suitable target, then the same effect can be achieved with the Active Animation node using a suitable pose."
      },
      {
        name = "EnableJointLimits", type = "bool", value = true, perAnimSet = true,
        helptext = "Enable joint limit clamping, keeping the physics joints inside the joint limits."
      },      
      {
        name = "DisableSleeping", type = "bool", value = false, perAnimSet = true,
        helptext = "Disable sleeping when stationary."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)

      -- Serialise Per Node Attributes

      local inputNodeID = -1
      if isConnected{ SourcePin = node .. ".Source", ResolveReferences = true } then
        inputNodeID = getConnectedNodeID(node, "Source")
        if (inputNodeID == nil) then 
          inputNodeID = -1
        end
      end

      local KWeightNodeID = -1
      local AAWeightNodeID = -1
      local EnableCollision = false
      local UseActiveAnimationAsKeyframeAnchor = false
      local UseRootAsAnchor = false
      local OutputSourceAnimation = false
      local PreserveMomentum = true

      stream:writeNetworkNodeId(inputNodeID, "InputNodeID")
      stream:writeNetworkNodeId(KWeightNodeID, "KWeightNodeID")
      stream:writeNetworkNodeId(AAWeightNodeID, "AAWeightNodeID")
      stream:writeBool(EnableCollision, "EnableCollision")
      stream:writeBool(UseActiveAnimationAsKeyframeAnchor, "UseActiveAnimationAsKeyframeAnchor")
      stream:writeBool(UseRootAsAnchor, "UseRootAsAnchor")
      stream:writeBool(OutputSourceAnimation, "OutputSourceAnimation")
      stream:writeBool(PreserveMomentum, "PreserveMomentum")
      stream:writeInt(0, "Method")

      -- Serialise Per Anim Set Attributes

      local SoftKeyFramingMaxAccel = -1
      local SoftKeyFramingMaxAngAccel = -1
      local GravityCompensation = 0
      local InternalJointCompliance = 1
      local ExternalJointCompliance = 1
      local ControllerRadiusFraction = -1
      local ControllerHeightFraction = -1
      local EnableCharacterControllerCollision = true

      local animSets = listAnimSets()
      
      for asIdx, asVal in ipairs(animSets) do
        local DisableSleeping = getAttribute(node, "DisableSleeping", asVal)
        local StrengthMultiplier = getAttribute(node, "PoseStrengthMultiplier", asVal)
        local DampingMultiplier = getAttribute(node, "DampingMultiplier", asVal)
        local EnableJointLimits = getAttribute(node, "EnableJointLimits", asVal)

        stream:writeBool(DisableSleeping, string.format("DisableSleeping_%d", asIdx))
        stream:writeBool(EnableJointLimits, string.format("EnableJointLimits_%d", asIdx))
        stream:writeFloat(SoftKeyFramingMaxAccel, string.format("SoftKeyFramingMaxAccel_%d", asIdx))
        stream:writeFloat(SoftKeyFramingMaxAngAccel, string.format("SoftKeyFramingMaxAngAccel_%d", asIdx))
        stream:writeFloat(GravityCompensation, string.format("GravityCompensation_%d", asIdx))
        stream:writeFloat(StrengthMultiplier, string.format("StrengthMultiplier_%d", asIdx))
        stream:writeFloat(DampingMultiplier, string.format("DampingMultiplier_%d", asIdx))
        stream:writeFloat(InternalJointCompliance, string.format("InternalCompliance_%d", asIdx))
        stream:writeFloat(ExternalJointCompliance, string.format("ExternalCompliance_%d", asIdx))
        stream:writeFloat(ControllerRadiusFraction, string.format("ControllerRadiusFraction_%d", asIdx))
        stream:writeFloat(ControllerHeightFraction, string.format("ControllerHeightFraction_%d", asIdx))
        stream:writeBool(EnableCharacterControllerCollision, string.format("EnableCharacterControllerCollision_%d", asIdx))
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local transformChannels = { }
      local rigSize = anim.getRigSize(set)

      -- set the size of the transform channels array
      table.setn(transformChannels, rigSize)
      -- set the elements in the array
      for i=1, rigSize do
        transformChannels[i - 1] = true
      end

      return transformChannels
    end,

  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of Ragdoll node definition.
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "Ragdoll",
    {
      {
        title = "Joint Properties",
        usedAttributes = {"PoseStrengthMultiplier", "DampingMultiplier", "EnableJointLimits"},
        displayFunc = function(...) safefunc(attributeEditor.jointPropertiesDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = {"DisableSleeping"},
        displayFunc = function(...) safefunc(attributeEditor.perAnimSetTickboxListDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end