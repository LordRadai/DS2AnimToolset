------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- HardKeyFrame node definition.
------------------------------------------------------------------------------------------------------------------------
registerPhysicsNode("HardKeyFrame",
  {
    displayName = "Hard Key Frame",
    helptext = "Drives physics rig to exactly match source animation global transforms.",
    group = "Physics",
    image = "HardKeyFrame.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 123), -- 123 is same as physics node.
    version = 2,

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

    pinOrder = { "Source", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "EnableCollision", type = "bool", value = true,
        helptext = "Whether or not the hard keyframed physics parts will collide with/push other dynamic objects in the scene."
      },
      {
        name = "UseRootAsAnchor", type = "bool", value = false,
        helptext = "Whether the input animation should be converted into world space by using the character root directly, or if it should be anchored to the next body group that is closer to the root."
      },
      {
        name = "OutputSourceAnimation", type = "bool", value = false,
        helptext = "If true then the output will be identical to the input, but physics will still run to provide collisions. This will provide a small performance benefit. Note that when using HardKeyFraming inside a physics compositor this can generate incorrect output."
      },
      {
        name = "ControllerRadiusFraction", type = "float", value = 1, min = 0, perAnimSet = true,
        helptext = "Scales the character controller radius."
      },
      {
        name = "ControllerHeightFraction", type = "float", value = 1, min = 0, perAnimSet = true,
        helptext = "Scales the character controller height."
      },
      {
        name = "EnableCharacterControllerCollision", type = "bool", value = false, perAnimSet = true,
        helptext = "Allow dynamic objects to collide with the character controller."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      if isConnected{ SourcePin = node .. ".Source", ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = node .. ".Source", ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("HardKeyFrame node " .. node .. " requires a valid input node")
        end

      else
        return nil, ("HardKeyFrame node " .. node .. " is missing a required connection to Source")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)

      -- Serialise Per Node Attributes
      
      local inputNodeID = -1
      local KWeightNodeID = -1
      local AAWeightNodeID = -1

      if isConnected{ SourcePin = node .. ".Source", ResolveReferences = true } then
        inputNodeID = getConnectedNodeID(node, "Source")
      end

      local EnableCollision = getAttribute(node, "EnableCollision")
      local UseActiveAnimationAsKeyframeAnchor = false
      local UseRootAsAnchor = getAttribute(node, "UseRootAsAnchor")
      local OutputSourceAnimation = getAttribute(node, "OutputSourceAnimation")
      local PreserveMomentum = true
      
      stream:writeNetworkNodeId(inputNodeID, "InputNodeID")
      stream:writeNetworkNodeId(KWeightNodeID, "KWeightNodeID")
      stream:writeNetworkNodeId(AAWeightNodeID, "AAWeightNodeID")
      stream:writeBool(EnableCollision, "EnableCollision")
      stream:writeBool(UseActiveAnimationAsKeyframeAnchor, "UseActiveAnimationAsKeyframeAnchor")
      stream:writeBool(UseRootAsAnchor, "UseRootAsAnchor")
      stream:writeBool(OutputSourceAnimation, "OutputSourceAnimation")
      stream:writeBool(PreserveMomentum, "PreserveMomentum")
      stream:writeInt(4, "Method")

      -- Serialise Per Anim Set Attributes

      local animSets = listAnimSets()
      
      for asIdx, asVal in ipairs(animSets) do
        local DisableSleeping = true
        local SoftKeyFramingMaxAccel = -1
        local SoftKeyFramingMaxAngAccel = -1
        local GravityCompensation = 0
        local StrengthMultiplier = 1
        local DampingMultiplier = 1
        local InternalJointCompliance = 1
        local ExternalJointCompliance = 1
        local ControllerRadiusFraction = getAttribute(node, "ControllerRadiusFraction", asVal)
        local ControllerHeightFraction = getAttribute(node, "ControllerHeightFraction", asVal)
        local EnableCharacterControllerCollision = getAttribute(node, "EnableCharacterControllerCollision", asVal)
        local EnableJointLimits = false

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
      local inputNodeChannels = { }
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local SourceTable = listConnections{ Object = sourcePin, ResolveReferences = true }
        local NodeConnected = SourceTable[1]
        inputNodeChannels = anim.getTransformChannels(NodeConnected, set)
      end

      return inputNodeChannels
    end,

  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of HardKeyFrame node definition.
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "HardKeyFrame",
    {
      {
        title = "Anchor",
        usedAttributes = { "UseRootAsAnchor" },
        displayFunc = function(...) safefunc(attributeEditor.SoftKeyFrameAnchorInfoSection, unpack(arg)) end
      },
      {
        title = "Character Controller Scale",
        usedAttributes = { "ControllerRadiusFraction", "ControllerHeightFraction" },
        displayFunc = function(...) safefunc(attributeEditor.controllerScaleDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "General",
        usedAttributes = { "EnableCollision", "OutputSourceAnimation" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Hidden",
        usedAttributes = { "EnableCharacterControllerCollision" },
        displayFunc = function(...) safefunc(attributeEditor.hiddenDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end