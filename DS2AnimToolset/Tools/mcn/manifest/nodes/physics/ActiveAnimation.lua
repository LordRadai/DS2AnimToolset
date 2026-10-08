------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- ActiveAnimation node definition.
------------------------------------------------------------------------------------------------------------------------
registerPhysicsNode("ActiveAnimation",
  {
    displayName = "Active Animation",
    helptext = "Drives physics rig to reach towards source animation local transforms.",
    group = "Physics",
    image = "ActiveAnimation.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 123), -- 123 is same as physics node.
    version = 4,

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
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
    },

    dataPins =
    {
      ["Weight"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source", "Weight", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "PoseStrengthMultiplier", type = "float", value = 1, perAnimSet = true,
        displayName = "Strength",
        helptext = "The maximum strength of each joint is multiplied by this value and then used to drive towards the animation source."
      },
      {
        name = "DampingMultiplier", type = "float", value = 1, perAnimSet = true,
        displayName = "Damping",
        helptext = "If the character has been set up correctly, then the joint damping will represent damping due to the skeletal joints and clothes and so on, so that the limbs are not completely floppy. This joint damping will be modulated by the damping multiplier, which would normally be set to 1 (its default value)."
      },
      {
        name = "InternalJointCompliance", type = "float", value = 1, min = 0, max = 1, perAnimSet = true,
        helptext = "PhysX 3 only: When Weight control parameter is 1, compliance for internally generated motion.\n\nCompliance affects how well the motion of a body part propagates through the joint, for instance whether pushing on the hand will move the whole arm or just bend the elbow.",
      },
      {
        name = "ExternalJointCompliance", type = "float", value = 1, min = 0, max = 1, perAnimSet = true,
        helptext = "PhysX 3 only: When Weight control parameter is 1, compliance for externally generated motion.\n\nCompliance affects how well the motion of a body part propagates through the joint, for instance whether pushing on the hand will move the whole arm or just bend the elbow.",
      },
      {
        name = "UseAsKeyframeAnchor", type = "bool", value = false,
        helptext = "Whether the input animation or the bind pose should be used to 'anchor' any keyframe methods that are being applied to other body groups."
      },
      {
        name = "EnableJointLimits", type = "bool", value = true, perAnimSet = true,
        helptext = "Enable joint limits."
      },
      {
        name = "DisableSleeping", type = "bool", value = true, perAnimSet = true,
        helptext = "Disable sleeping when stationary."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      if isConnected{ SourcePin = node .. ".Source", ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = node .. ".Source", ResolveReferences = true }
        if table.getn(nodesConnected) ~= 1 then
          return nil, ("ActiveAnimation node " .. node .. " requires a valid input node")
        end

        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("ActiveAnimation node " .. node .. " requires a valid input node")
        end

      else
        return nil, ("ActiveAnimation node " .. node .. " is missing a required connection to Source")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)

      -- Serialise Per Node Attributes

      local inputNodeID = getConnectedNodeID(node, "Source")
      stream:writeNetworkNodeId(inputNodeID, "InputNodeID")

      -- this is always -1 for active animation nodes
      stream:writeNetworkNodeId(-1, "KWeightNodeID")

      local weightPin = string.format("%s.Weight", node)
      if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
        local weightNodeInfo = getConnectedNodeInfo(weightPin)
        stream:writeNetworkNodeId(weightNodeInfo.id, "AAWeightNodeID", weightNodeInfo.pinIndex)
      else
        stream:writeNetworkNodeId(-1, "AAWeightNodeID")
      end

      local enableCollision = true
      local useActiveAnimationAsKeyframeAnchor = getAttribute(node, "UseAsKeyframeAnchor")
      local useRootAsAnchor = false
      local outputSourceAnimation = false
      local PreserveMomentum = true

      stream:writeBool(enableCollision, "EnableCollision")
      stream:writeBool(useRootAsAnchor, "UseRootAsAnchor")
      stream:writeBool(useActiveAnimationAsKeyframeAnchor, "UseActiveAnimationAsKeyframeAnchor")
      stream:writeBool(outputSourceAnimation, "OutputSourceAnimation")
      stream:writeBool(PreserveMomentum, "PreserveMomentum")
      stream:writeInt(1, "Method")

      -- Serialise Per Anim Set Attributes

      local softKeyFramingMaxAccel = -1
      local softKeyFramingMaxAngAccel = -1
      local gravityCompensation = 0

      local controllerRadiusFraction = -1
      local controllerHeightFraction = -1
      local enableCharacterControllerCollision = false

      local animSets = listAnimSets()
      for asIdx, asVal in ipairs(animSets) do
        local disableSleeping = getAttribute(node, "DisableSleeping", asVal)
        local enableJointLimits = getAttribute(node, "EnableJointLimits", asVal)
        local strengthMultiplier = getAttribute(node, "PoseStrengthMultiplier", asVal)
        local dampingMultiplier = getAttribute(node, "DampingMultiplier", asVal)
        local internalJointCompliance = getAttribute(node, "InternalJointCompliance", asVal)
        local externalJointCompliance = getAttribute(node, "ExternalJointCompliance", asVal)

        stream:writeBool(disableSleeping, string.format("DisableSleeping_%d", asIdx))
        stream:writeBool(enableJointLimits, string.format("EnableJointLimits_%d", asIdx))
        stream:writeFloat(softKeyFramingMaxAccel, string.format("SoftKeyFramingMaxAccel_%d", asIdx))
        stream:writeFloat(softKeyFramingMaxAngAccel, string.format("SoftKeyFramingMaxAngAccel_%d", asIdx))
        stream:writeFloat(gravityCompensation, string.format("GravityCompensation_%d", asIdx))
        stream:writeFloat(strengthMultiplier, string.format("StrengthMultiplier_%d", asIdx))
        stream:writeFloat(dampingMultiplier, string.format("DampingMultiplier_%d", asIdx))
        stream:writeFloat(internalJointCompliance, string.format("InternalCompliance_%d", asIdx))
        stream:writeFloat(externalJointCompliance, string.format("ExternalCompliance_%d", asIdx))
        stream:writeFloat(controllerRadiusFraction, string.format("ControllerRadiusFraction_%d", asIdx))
        stream:writeFloat(controllerHeightFraction, string.format("ControllerHeightFraction_%d", asIdx))
        stream:writeBool(enableCharacterControllerCollision, string.format("EnableCharacterControllerCollision_%d", asIdx))
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local inputNodeChannels = { }
      local sourcePin = string.format("%s.Source")
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local SourceTable = listConnections{ Object = sourcePin, ResolveReferences = true }
        local NodeConnected = SourceTable[1]
        inputNodeChannels = anim.getTransformChannels(NodeConnected, set)
      end

      return inputNodeChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 4 then
        -- version 4, StrengthMultiplier was moved to PoseStrengthMultiplier to match the ragdoll node.
        if attributeExists(node, "deprecated_StrengthMultiplier") then
          local value = getAttribute(node, "deprecated_StrengthMultiplier")
          setAttribute(string.format("%s.PoseStrengthMultiplier", node), value)
          removeAttribute(node, "deprecated_StrengthMultiplier")
        end
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of ActiveAnimation node definition.
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "ActiveAnimation",
    {
      {
        title = "Anchors",
        usedAttributes = {"UseAsKeyframeAnchor"},
        displayFunc = function(...) safefunc(attributeEditor.activeAnimAnchorsDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Joint Properties",
        usedAttributes = {"PoseStrengthMultiplier", "DampingMultiplier", "EnableJointLimits"},
        displayFunc = function(...) safefunc(attributeEditor.jointPropertiesDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "PhysX 3",
        usedAttributes = {"InternalJointCompliance", "ExternalJointCompliance"},
        displayFunc = function(...) safefunc(attributeEditor.SoftKeyFrameJointComplianceDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = {"DisableSleeping"},
        displayFunc = function(...) safefunc(attributeEditor.perAnimSetTickboxListDisplayInfoSection, unpack(arg)) end
      },

    }
  )
end

