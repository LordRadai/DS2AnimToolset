------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

local convertJointIndexToName = function(jointIndex, animSet)
  return anim.getAnimChannelName(jointIndex, animSet)
end

------------------------------------------------------------------------------------------------------------------------
-- TwoBoneIK node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("TwoBoneIK",
  {
    displayName = "Two Bone IK",
    helptext = "Two-bone IK: make animation reach to a target position and orientation",
    group = "IK",
    image = "TwoBoneIK.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 120),
    version = 6,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time" },
          optional = { },
        }
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time" },
          optional = { },
        }
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["EffectorTarget"] = {
        input = true,
        array = false,
        type = "vector3",
      },
      ["TargetOrientation"] = {
        input = true,
        array = false,
        type = "vector4",
      },
      ["SwivelAngle"] = {
        input = true,
        array = false,
        type = "float",
      },
      ["IkFkBlendWeight"] = {
        input = true,
        array = false,
        type = "float",
      },
      ["SwivelContributionToOrientation"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source", "Result", "EffectorTarget", "TargetOrientation", "SwivelAngle", "IkFkBlendWeight", "SwivelContribution" },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "MidJointRotationAxisX", type = "float", value = 1.0, perAnimSet = true,
        helptext = "The axis to rotate the middle joint around, in the coordinate frame of its parent."
      },
      {
        name = "MidJointRotationAxisY", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis to rotate the middle joint around, in the coordinate frame of its parent."
      },
      {
        name = "MidJointRotationAxisZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis to rotate the middle joint around, in the coordinate frame of its parent."
      },
      {
        name = "FlipMidJointRotationDirection", type = "bool", value = false,  perAnimSet = true,
        helptext = "Change the sign of the hinge axis to flip the bending direction."
      },
      {
        name = "EndJointName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "Specifies the end joint and, by association through the rig, the root and middle joints."
      },
      {
        name = "TargetJointName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "Specifies the target joint for the IK solver."
      },
      -- Don't actually create attributes for mid and root - currently we are only allowed to use
      -- sequential IK chains, where the mid and root joints are the direct parent and grandparent
      -- of the end joint
      --{ name = "MidJointIndex", type = "int", value = 0 },
      --{ name = "RootJointIndex", type = "int", value = 0 },
      -- For similar reasons, 'assume simple hierarchy' must always be true
      --{ name = "AssumeSimpleHierarchy", type = "bool", value = true },

      {
        name = "KeepEndEffOrientation", type = "bool", value = false,
        helptext = "If turned on, the target orientation of the end joint is defined by its character-space orientation in the source animation."
      },
      {
        name = "GlobalReferenceAxis", type = "bool", value = true,  perAnimSet = true,
        helptext = "If turned on, the Mid Joint Reference Axis is specified in character space. Otherwise, it is specified relative to the root joint (its parent)."
      },
      {
        name = "MidJointReferenceAxisX", type = "float", value = 0,  perAnimSet = true,
        helptext = "The normal of the plane used with the swivel angle value.  For instance, to keep the elbow of an arm chain in the XZ plane specify the plane as <0, 1, 0> and set the swivel angle to 0."
      },
      {
        name = "MidJointReferenceAxisY", type = "float", value = 0,  perAnimSet = true,
        helptext = "The normal of the plane used with the swivel angle value.  For instance, to keep the elbow of an arm chain in the XZ plane specify the plane as <0, 1, 0> and set the swivel angle to 0."
      },
      {
        name = "MidJointReferenceAxisZ", type = "float", value = 0,  perAnimSet = true,
        helptext = "The normal of the plane used with the swivel angle value.  For instance, to keep the elbow of an arm chain in the XZ plane specify the plane as <0, 1, 0> and set the swivel angle to 0."
      },
      {
        name = "UpdateTargetByDeltas", type = "bool", value = false,
        helptext = "If turned on, assume that input target position and orientation were given relative to the character root at the previous update, and should be corrected for motion since then.\n\nThis attribute only applies when the target frame is set to Character Space."
      },
      {
        name = "UseReferenceAxis", type = "bool", value = false,  perAnimSet = true,
        helptext = "Choose if the swivel angle is relative to the source animation or a reference plane"
      },
      {
        name = "WorldSpaceTarget", type = "bool", value = false, perAnimSet = false,
        helptext = "If true, the target input control parameter is specified in world space; otherwise, it is expressed in character space (the coordinate frame of the trajectory joint)."
      },
      {
        name = "UseSpecifiedJointAsTarget", type = "bool", value = false, perAnimSet = false,
        helptext = "If true, the IK will use a specified joint as the target instead of the default effector target."
      },
      {
        name = "UseSpecifiedJointOrientation", type = "bool", value = false, perAnimSet = false,
        helptext = "If true, the IK will use the orientation of a specified joint instead of the default effector target."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local inputNode

      -- Validate connections
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = sourcePin, ResolveReferences = true }
        inputNode = nodesConnected[1]

        if isValid(inputNode) ~= true then
          return nil, ("TwoBoneIK node " .. node .. " requires a valid input node")
        end
      else
        return nil, ("TwoBoneIK node " .. node .. " is missing a required connection to In")
      end

      -- Validate Rig indices
      -- When this node supports non-sequential IK chains, this needs to be expanded
      local animSets = listAnimSets()
      for asIdx, asVal in animSets do

        local name = getAttribute(node, "EndJointName", asVal)
        local index = nil
        if name ~= nil then
          index = anim.getRigChannelIndex(name, asVal)
        end

        local rigSize = anim.getRigSize(asVal)
        for i = 1, 3 do
          if index == nil or index <= 0 or index >= rigSize then
            return nil, ("TwoBoneIK node " .. node .. " (animset " .. asVal .. ") requires a valid EndJointName with a valid parent and grandparent")
          end

          index = anim.getParentBoneIndex(index, asVal)
        end
      end

      local effectorTargetPin = string.format("%s.EffectorTarget", node)
      if isConnected{ SourcePin = effectorTargetPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = effectorTargetPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("TwoBoneIK node " .. node .. " requires a valid Effector Target")
        end

      else
        return nil, ("TwoBoneIK node " .. node .. " is missing a required connection to Effector Target")
      end

      local targetOrientationPin = string.format("%s.TargetOrientation", node)
      if isConnected{ SourcePin = targetOrientationPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = targetOrientationPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("TwoBoneIK node " .. node .. " requires a valid Target Orientation")
        end
      end

      local swivelAnglePin = string.format("%s.SwivelAngle", node)
      if isConnected{ SourcePin = swivelAnglePin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = swivelAnglePin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("TwoBoneIK node " .. node .. " requires a valid SwivelAngle")
        end
      end

      local blendWeightPin = string.format("%s.IkFkBlendWeight", node)
      if isConnected{ SourcePin = blendWeightPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = blendWeightPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("TwoBoneIK node " .. node .. " requires a valid IkFkBlendWeight")
        end
      end

      local swivelContributionToOrientationPin = string.format("%s.SwivelContributionToOrientation", node)
      if isConnected{ SourcePin = swivelContributionToOrientationPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = swivelContributionToOrientationPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("TwoBoneIK node " .. node .. " requires a valid SwivelContributionToOrientation")
        end
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputNodeID = -1
      local targetOrientationNodeID = -1
      local swivelAngleNodeID = -1
      local iKFkBlendWeightNodeID = -1
      local swivelContributionToOrientationNodeID = -1

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        inputNodeID = getConnectedNodeID(sourcePin)
      end
      Stream:writeNetworkNodeId(inputNodeID, "InputNodeID")

      local effectorTargetPin = string.format("%s.EffectorTarget", node)
      if isConnected{ SourcePin = effectorTargetPin, ResolveReferences = true } then
        effectorTargetNodeInfo = getConnectedNodeInfo(effectorTargetPin)
        Stream:writeNetworkNodeId(effectorTargetNodeInfo.id, "EffectorTarget", effectorTargetNodeInfo.pinIndex)
      end

      local targetOrientationPin = string.format("%s.TargetOrientation", node)
      if isConnected{ SourcePin = targetOrientationPin, ResolveReferences = true } then
        targetOrientationNodeInfo = getConnectedNodeInfo(targetOrientationPin)
        Stream:writeNetworkNodeId(targetOrientationNodeInfo.id, "TargetOrientation", targetOrientationNodeInfo.pinIndex)
      end

      local swivelAnglePin = string.format("%s.SwivelAngle", node)
      if isConnected{ SourcePin = swivelAnglePin, ResolveReferences = true } then
        swivelAngleNodeInfo = getConnectedNodeInfo(swivelAnglePin)
        Stream:writeNetworkNodeId(swivelAngleNodeInfo.id, "SwivelAngle", swivelAngleNodeInfo.pinIndex)
      end

      local blendWeightPin = string.format("%s.IkFkBlendWeight", node)
      if isConnected{ SourcePin = blendWeightPin, ResolveReferences = true } then
        iKFkBlendWeightNodeInfo = getConnectedNodeInfo(blendWeightPin)
        Stream:writeNetworkNodeId(iKFkBlendWeightNodeInfo.id, "IkFkBlendWeight", iKFkBlendWeightNodeInfo.pinIndex)
      end

      local swivelContributionToOrientationPin = string.format("%s.SwivelContributionToOrientation", node)
      if isConnected{ SourcePin = swivelContributionToOrientationPin, ResolveReferences = true } then
        swivelContributionToOrientationNodeInfo = getConnectedNodeInfo(swivelContributionToOrientationPin)
        Stream:writeNetworkNodeId(swivelContributionToOrientationNodeInfo.id, "SwivelContributionToOrientation", swivelContributionToOrientationNodeInfo.pinIndex)
      end

      local assumeSimpleHierarchy = true --getAttribute(node, "AssumeSimpleHierarchy")
      Stream:writeBool(assumeSimpleHierarchy, "AssumeSimpleHierarchy")

      local keepEndEffOrientation = getAttribute(node, "KeepEndEffOrientation")
      Stream:writeBool(keepEndEffOrientation, "KeepEndEffOrientation")

      local updateTargetByDeltas = getAttribute(node, "UpdateTargetByDeltas")
      Stream:writeBool(updateTargetByDeltas, "UpdateTargetByDeltas")
      
      local worldSpaceTarget = getAttribute(node, "WorldSpaceTarget")
      Stream:writeBool(worldSpaceTarget, "WorldSpaceTarget")

      local useSpecifiedJointAsTarget = getAttribute(node, "UseSpecifiedJointAsTarget")
      Stream:writeBool(useSpecifiedJointAsTarget, "UseSpecifiedJointAsTarget")

      local useSpecifiedJointOrientation = getAttribute(node, "UseSpecifiedJointOrientation")
      Stream:writeBool(useSpecifiedJointOrientation, "UseSpecifiedJointOrientation")

      local animSets = listAnimSets()
      for asIdx, asVal in animSets do
        local midJointRotationAxisX = getAttribute(node, "MidJointRotationAxisX", asVal)
        Stream:writeFloat(midJointRotationAxisX, "MidJointRotationAxisX_"..asIdx)

        local midJointRotationAxisY = getAttribute(node, "MidJointRotationAxisY", asVal)
        Stream:writeFloat(midJointRotationAxisY, "MidJointRotationAxisY_"..asIdx)

        local midJointRotationAxisZ = getAttribute(node, "MidJointRotationAxisZ", asVal)
        Stream:writeFloat(midJointRotationAxisZ, "MidJointRotationAxisZ_"..asIdx)

        local flipMidJointRotationDirection = getAttribute(node, "FlipMidJointRotationDirection", asVal)
        Stream:writeBool(flipMidJointRotationDirection, "FlipMidJointRotationDirection_"..asIdx)

        local globalReferenceAxis = getAttribute(node, "GlobalReferenceAxis", asVal)
        Stream:writeBool(globalReferenceAxis, "GlobalReferenceAxis_"..asIdx)

        local useReferenceAxis = getAttribute(node, "UseReferenceAxis", asVal)
        if useReferenceAxis == true then
          local midJointReferenceAxisX = getAttribute(node, "MidJointReferenceAxisX", asVal)
          Stream:writeFloat(midJointReferenceAxisX, "MidJointReferenceAxisX_"..asIdx)

          local midJointReferenceAxisY = getAttribute(node, "MidJointReferenceAxisY", asVal)
          Stream:writeFloat(midJointReferenceAxisY, "MidJointReferenceAxisY_"..asIdx)

          local midJointReferenceAxisZ = getAttribute(node, "MidJointReferenceAxisZ", asVal)
          Stream:writeFloat(midJointReferenceAxisZ, "MidJointReferenceAxisZ_"..asIdx)
        else
        --  not using the reference axis is specified by writing 0 for all channels
          Stream:writeFloat(0, "MidJointReferenceAxisX_"..asIdx)
          Stream:writeFloat(0, "MidJointReferenceAxisY_"..asIdx)
          Stream:writeFloat(0, "MidJointReferenceAxisZ_"..asIdx)
        end

        local endJointName = getAttribute(node, "EndJointName", asVal)
        local endJointIndex = anim.getRigChannelIndex(endJointName, asVal)
        Stream:writeUInt(endJointIndex, "EndJointIndex_"..asIdx)

        local targetJointName = getAttribute(node, "TargetJointName", asVal)

        if targetJointName ~= nil and targetJointName ~= "" then
          local targetJointIndex = anim.getRigChannelIndex(targetJointName, asVal)
          Stream:writeUInt(targetJointIndex, "TargetJointIndex_"..asIdx)
        end

        -- Don't actually retrieve values for mid and root - currently we are only allowed to use
        -- sequential IK chains, where the mid and root joints are the direct parent and grandparent
        -- of the end joint
        local midJointIndex = -1 --getAttribute(node, "MidJointIndex", asVal)
        Stream:writeUInt(midJointIndex, "MidJointIndex_"..asIdx)

        local rootJointIndex = -1 --getAttribute(node, "RootJointIndex", asVal)
        Stream:writeUInt(rootJointIndex, "RootJointIndex_"..asIdx)
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local sourceNodeChannels = { }
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = connections[1]
        sourceNodeChannels = anim.getTransformChannels(sourceNode, set)
      end

      -- Generate reverse map
      local reverseMap = { }
      for jointIndex,_ in sourceNodeChannels do
        reverseMap[jointIndex] = jointIndex
      end

      -- Add any channels output by this node that are not provided by the input
      local endJointName = getAttribute(node, "EndJointName", set)
      local endJointIndex = anim.getRigChannelIndex(endJointName, set)
      local midJointIndex = anim.getParentBoneIndex(endJointIndex, set)
      local rootJointIndex = anim.getParentBoneIndex(midJointIndex, set)
      reverseMap[endJointIndex] = endJointIndex
      reverseMap[midJointIndex] = midJointIndex
      reverseMap[rootJointIndex] = rootJointIndex

      -- Re-reverse to get output channels.  Sort to keep neat numerical order.
      local outputChannels = { }
      for jointIndex, val in reverseMap do
        outputChannels[jointIndex] = true
      end

      return outputChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        copyAnimSetAttribute(node, "deprecated_EndJointIndex", "EndJointName", convertJointIndexToName)
        --remove deprecated values
        removeAttribute(node, "deprecated_EndJointIndex")
      end

      if version < 3 then
        local useReferenceAxis = false
        local midJointReferenceAxisX = getAttribute(node, "MidJointReferenceAxisX")
        local midJointReferenceAxisY = getAttribute(node, "MidJointReferenceAxisY")
        local midJointReferenceAxisZ = getAttribute(node, "MidJointReferenceAxisZ")
        if midJointReferenceAxisX ~= 0 or midJointReferenceAxisY ~= 0 or midJointReferenceAxisZ ~= 0  then
          useReferenceAxis = true
        end
        setAttribute(node ..".UseReferenceAxis", useReferenceAxis)
      end

      if version < 3 then
        local sourcePath = string.format("%s.Source", node)
        setPinPassThrough(sourcePath, true)

        local resultPath = string.format("%s.Result", node)
        setPinPassThrough(resultPath, true)
      elseif version == 3 then
        local oldSourcePath = string.format("%s.In", node)
        local newSourcePath = string.format("%s.Source", node)
        pinLookupTable[oldSourcePath] = newSourcePath

        local oldResultPath = string.format("%s.Out", node)
        local newResultPath = string.format("%s.Result", node)
        pinLookupTable[oldResultPath] = newResultPath
      end
      
      if version < 6 then
        -- The following attributes were added in version 6
        --  WorldSpaceTarget
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- TwoBoneIK custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
attributeEditor.registerDisplayInfo(
    "TwoBoneIK",
    {
      {
        title = "Use Target Joint",
        usedAttributes = { "UseSpecifiedJointAsTarget" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "End Effector",
        usedAttributes = { "EndJointName" },
        displayFunc = function(...) safefunc(attributeEditor.animSetDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Target Joint",
        usedAttributes = { "TargetJointName" },
        displayFunc = function(...) safefunc(attributeEditor.animSetDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Hinge Axis",
        usedAttributes = {
          "MidJointRotationAxisX",
          "MidJointRotationAxisY",
          "MidJointRotationAxisZ"
        },
        displayFunc = function(...) safefunc(attributeEditor.twoBoneIkHingeAxisSection , unpack(arg)) end
      },
      {
        title = "Swivel Angle",
        usedAttributes = {
          "UseReferenceAxis",
          "MidJointReferenceAxisX",
          "MidJointReferenceAxisY",
          "MidJointReferenceAxisZ",
          "GlobalReferenceAxis"
        },
        displayFunc = function(...) safefunc(attributeEditor.twoBoneIkReferenceAxisSection, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = {
          "KeepEndEffOrientation",
          "UpdateTargetByDeltas",
          "FlipMidJointRotationDirection",
          "WorldSpaceTarget",
          "UseSpecifiedJointOrientation",
          "SpecifiedJointName",
        },
        displayFunc = function(...) safefunc(attributeEditor.twoBoneIkPropertiesSection, unpack(arg)) end
      },
    }
)
end

