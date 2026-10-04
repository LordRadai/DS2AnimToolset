------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- SoftKeyFrame node definition.
------------------------------------------------------------------------------------------------------------------------
registerPhysicsNode("SoftKeyFrame",
  {
    displayName = "Soft Key Frame",
    helptext = "Drives physics rig to reach towards source animation global transforms.",
    group = "Physics",
    image = "SoftKeyFrame.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 123), -- 123 is same as physics node.
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
        name = "EnableCollision", type = "bool", value = true,
        helptext = "Whether or not the hard keyframed physics parts will collide with/push other dynamic objects in the scene."
      },
      {
        name = "UseRootAsAnchor", type = "bool", value = false,
        helptext = "Whether the input animation should be converted into world space by using the character root directly, or if it should be anchored to the next body group that is closer to the root."
      },
      {
        name = "OutputSourceAnimation", type = "bool", value = false,
        helptext = "If true then the output will be identical to the input, but physics will still run to provide soft-keyframe collisions."
      },
      {
        name = "GravityCompensation", type = "float", value = 0, min = 0, max = 1, perAnimSet = true,
        helptext = "The fraction that gravity should be compensated for. This will be scaled by the node's input weight. Note that using this allows much smaller (and therefore softer) max acceleration values to be used without the character sagging under gravity."
      },
      {
        name = "UseMaxAcceleration", type = "bool", value = false, perAnimSet = true,
        helptext = "Whether or not the following Max acceleration attribute should be used. If false, then there is no limit to the acceleration that soft keyframing can produce."
      },
      {
        name = "UseMaxAngularAcceleration", type = "bool", value = false, perAnimSet = true,
        helptext = "Whether or not the following Max angular acceleration attribute should be used. If false, then there is no limit to the angular acceleration that soft keyframing can produce."
      },
      {
        name = "MaxAcceleration", type = "float", min = 0, value = 150, perAnimSet = true,
        helptext = "This clamps the forces so that the body parts do not accelerate due to soft keyframing by more than this amount. This will be scaled by the node's input weight."
      },
      {
        name = "MaxAngularAcceleration", type = "float", min = 0, value = 150, perAnimSet = true,
        helptext = "This clamps the forces so that the body parts do not accelerate due to soft keyframing by more than this amount. This will be scaled by the node's input weight."
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
        name = "ExternalJointCompliance", type = "float", value = 1, min = 0, max = 1, perAnimSet = true,
        helptext = "PhysX 3 only: Compliance for externally generated motion.\n\nCompliance affects how well the motion of a body part propagates through the joint, for instance whether pushing on the hand will move the whole arm or just bend the elbow.",
      },
      {
        name = "EnableJointLimits", type = "bool", value = true, perAnimSet = true,
        helptext = "Enable joint limits."
      },
      {
        name = "EnableCharacterControllerCollision", type = "bool", value = false, perAnimSet = true,
        helptext = "Allow dynamic objects to collide with the character controller."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      if isConnected{ SourcePin = node..".Source", ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = node..".Source", ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("SoftKeyFrame node " .. node .. " requires a valid input node")
        end

      else
        return nil, ("SoftKeyFrame node " .. node .. " is missing a required connection to Source")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)

      -- Serialise Per Node Attributes

      local inputNodeID = -1
      local AAWeightNodeID = -1
      local KWeightNodeInfo = nil

      if isConnected{ SourcePin = node .. ".Source", ResolveReferences = true } then
        inputNodeID = getConnectedNodeID(node, "Source")
      end

      if isConnected{ SourcePin = node .. ".Weight", ResolveReferences = true } then
        KWeightNodeInfo = getConnectedNodeInfo(node, "Weight")
      end

      local EnableCollision = getAttribute(node, "EnableCollision")
      local UseActiveAnimationAsKeyframeAnchor = false
      local UseRootAsAnchor = getAttribute(node, "UseRootAsAnchor")
      local OutputSourceAnimation = getAttribute(node, "OutputSourceAnimation")
      local PreserveMomentum = false

      stream:writeNetworkNodeId(inputNodeID, "InputNodeID")
      if KWeightNodeInfo ~= nil then
        stream:writeNetworkNodeId(KWeightNodeInfo.id, "KWeightNodeID", KWeightNodeInfo.pinIndex)
      else
        stream:writeNetworkNodeId(-1, "KWeightNodeID")
      end
      stream:writeNetworkNodeId(AAWeightNodeID, "AAWeightNodeID")

      stream:writeBool(EnableCollision, "EnableCollision")
      stream:writeBool(UseActiveAnimationAsKeyframeAnchor, "UseActiveAnimationAsKeyframeAnchor")
      stream:writeBool(UseRootAsAnchor, "UseRootAsAnchor")
      stream:writeBool(OutputSourceAnimation, "OutputSourceAnimation")
      stream:writeBool(PreserveMomentum, "PreserveMomentum")
      stream:writeInt(3, "Method")

      -- Serialise Per Anim Set Attributes

      local animSets = listAnimSets()
      
      for asIdx, asVal in ipairs(animSets) do
        local DisableSleeping = true;
        local SoftKeyFramingMaxAccel = getAttribute(node, "MaxAcceleration", asVal)
        local SoftKeyFramingMaxAngAccel = getAttribute(node, "MaxAngularAcceleration", asVal)
        local GravityCompensation = getAttribute(node, "GravityCompensation", asVal)
        local StrengthMultiplier = 1
        local DampingMultiplier = 1
        local InternalJointCompliance = 1
        local ExternalJointCompliance = getAttribute(node, "ExternalJointCompliance", asVal)
        local ControllerRadiusFraction = getAttribute(node, "ControllerRadiusFraction", asVal)
        local ControllerHeightFraction = getAttribute(node, "ControllerHeightFraction", asVal)
        local EnableCharacterControllerCollision = getAttribute(node, "EnableCharacterControllerCollision", asVal)
        local EnableJointLimits = getAttribute(node, "EnableJointLimits", asVal)

        local UseMaxAcceleration = getAttribute(node, "UseMaxAcceleration", asVal)
        if (not UseMaxAcceleration) then
          SoftKeyFramingMaxAccel = -1
        end
        
        local UseMaxAngularAcceleration = getAttribute(node, "UseMaxAngularAcceleration", asVal)
        if (not UseMaxAngularAcceleration) then
          SoftKeyFramingMaxAngAccel = -1
        end

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

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        local MaxAcceleration = getAttribute(node..".MaxAcceleration")
        if (MaxAcceleration < 0) then
          setAttribute(node..".MaxAcceleration", 0)
          setAttribute(node..".UseMaxAcceleration", false)
        else
          setAttribute(node..".UseMaxAcceleration", true)
        end

        local MaxAngularAcceleration = getAttribute(node..".MaxAngularAcceleration")
        if (MaxAngularAcceleration < 0) then
          setAttribute(node..".MaxAngularAcceleration", 0)
          setAttribute(node..".UseMaxAngularAcceleration", false)
        else
          setAttribute(node..".UseMaxAngularAcceleration", true)
        end
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    onNodeCreated = function(node)
      local distScaleFactor = preferences.get("RuntimeAssetScaleFactor")

      -- scale the default max acceleration to the runtime asset scale.
      local maxAcc = getAttribute(node..".MaxAcceleration") / distScaleFactor
      setAttribute(node..".MaxAcceleration", maxAcc)
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of SoftKeyFrame node definition.
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "SoftKeyFrame",
    {
      {
        title = "Anchor",
        usedAttributes = { "UseRootAsAnchor" },
        displayFunc = function(...) safefunc(attributeEditor.SoftKeyFrameAnchorInfoSection, unpack(arg)) end
      },
      {
        title = "Character Controller Scale",
        usedAttributes = {
          "ControllerRadiusFraction",
          "ControllerHeightFraction"
        },
        displayFunc = function(...) safefunc(attributeEditor.controllerScaleDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Gravity Compensation",
        usedAttributes = { "GravityCompensation" },
        displayFunc = function(...) safefunc(attributeEditor.gravityCompensationDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Acceleration Limits",
        usedAttributes = {
          "UseMaxAcceleration",
          "MaxAcceleration",
          "UseMaxAngularAcceleration",
          "MaxAngularAcceleration"
        },
        displayFunc = function(...) safefunc(attributeEditor.SoftKeyFrameAccelerationLimitsInfoSection, unpack(arg)) end
      },
      {
        title = "PhysX 3",
        usedAttributes = {
          "ExternalJointCompliance",
        },
        displayFunc = function(...) safefunc(attributeEditor.SoftKeyFrameJointComplianceDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = {
          "EnableJointLimits",
          "EnableCharacterControllerCollision"
        },
        displayFunc = function(...) safefunc(attributeEditor.perAnimSetTickboxListDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "General",
        usedAttributes = {
          "EnableCollision",
          "OutputSourceAnimation"
        },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end
