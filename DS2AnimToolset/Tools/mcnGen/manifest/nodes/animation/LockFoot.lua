------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- LockFoot node definition.
------------------------------------------------------------------------------------------------------------------------

local convertJointIndexToName = function(jointIndex, animSet)
  return anim.getAnimChannelName(jointIndex, animSet)
end

registerNode("LockFoot",
  {
    displayName = "Lock Foot",
    helptext = "Lock Foot: prevents horizontal translation of a chosen joint during a footfall duration event",
    group = "IK",
    image = "LockFoot.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 121),
    version = 8,

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

    pinOrder = { "Source", "Result", "IkFkBlendWeight", "SwivelContributionToOrientation", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "KneeRotationAxisX", type = "float", value = 1, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "KneeRotationAxisY", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "KneeRotationAxisZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "FlipKneeRotationDirection", type = "bool", value = false, perAnimSet = true,
        helptext = "Change the sign of the knee axis, which will flip its bending direction."
      },

      -- Don't actually create attributes for hip and knee - currently we are only allowed to use
      -- sequential IK chains, where the knee and hip joints are the direct parent and grandparent
      -- of the end joint
      --{ name = "HipIndex", type = "int", value = -1 },
      --{ name = "KneeIndex", type = "int", value = -1 },

      {
        name = "AnkleName", displayName = "Ankle", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "If Use Ball Joint is turned off, this must be specified to identify the IK chain (ankle, knee and hip)."
      },
      {
        name = "BallName", displayName = "Ball", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "If Use Ball Joint is turned on, this must be specified to identify the IK chain (ball, ankle, knee and hip)."
      },
      {
        name = "ToeName", displayName = "Toe", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "Identifies the toe joint for use when Fix Toe Ground Penetration is turned on. Must be parented to the ball joint, which must be in use."
      },
      {
        name = "UseBallJoint", type = "bool", value = true, perAnimSet = true,
        helptext = "If turned on, it is the ankle joint which is fixed during a footfall. If turned off, the ball joint is fixed."
      },
      -- For similar reasons, 'assume simple hierarchy' must always be true
      --{ name = "AssumeSimpleHierarchy", type = "bool", value = true },

      {
        name = "FootfallEventID", type = "int", value = 0, perAnimSet = true,
        helptext = "A whole number which identifies the footfall duration events for this leg. Only duration events with this User ID will trigger footlocking."
      },
      {
        name = "CatchUpSpeedFactor", type = "float", value = 0.1, displayName = "Damping", perAnimSet = true,
        helptext = "A float number between 0 and 1 which controls how quickly the trailing leg will match up with the source animation again after a footfall. 0 means that the leg will snap instantly back onto the source, and at 1 it will never catch up. Good values lie between 0.1 and 0.8, but it does depend on the nature of the source animation."
      },
      {
        name = "SnapToSourceDistance", type = "float", value = 0.01, displayName = "Tolerance", perAnimSet = true,
        helptext = "When the controlled joint (either ball or ankle) is this close to its position in the source animation, the output animation matches the input from then until the next footfall."
      },
      {
        name = "StraightestLegFactor", type = "float", value = 1.0, displayName = "Max Extension", perAnimSet = true,
        helptext = "How straight the leg is allowed to become, as a percentage of fully straightened (hip, knee and ankle all lined up). When the foot is locked to a point out of reach (usually towards the end of a footfall), the leg will go straight and this can look unnatural."
      },
      {
        name = "UpAxisX", type = "bool", value = false,
        helptext = "The vertical axis is in the x-direction.  This attribute is hidden, and set from preferences."
      },
      {
        name = "UpAxisY", type = "bool", value = false,
        helptext = "The vertical axis is in the y-direction.  This attribute is hidden, and set from preferences."
      },
      {
        name = "UpAxisZ", type = "bool", value = false,
        helptext = "The vertical axis is in the z-direction.  This attribute is hidden, and set from preferences."
      },
      {
        name = "LockVerticalMotion", type = "bool", value = false, perAnimSet = false,
        helptext = "If turned off (the default), the foot's horizontal motion is locked during a footplant, but not vertical motion, which is usually preferred."
      },
      {
        name = "FixGroundPenetration", type = "bool", value = false, perAnimSet = true,
        helptext = "If turned on, the locking position is prevented from dropping below the Lower Height Bound."
      },
      {
        name = "AnkleLowerHeightBound", type = "float", value = 0, perAnimSet = true,
        helptext = "The minimum allowable height of the ankle joint (if using the ball joint), before there is apparent ground penetration."
      },
      {
        name = "LowerHeightBound", type = "float", value = 0, perAnimSet = true,
        helptext = "The minimum allowable height of the controlled joint (ankle or ball), before there is apparent ground penetration."
      },
      {
        name = "FixToeGroundPenetration", type = "bool", value = false, perAnimSet = true,
        helptext = "If turned on, the toe joint is prevented from dropping below the Toe Lower Height Bound by rotation of the ball joint."
      },
      {
        name = "ToeLowerHeightBound", type = "float", value = 0, perAnimSet = true,
        helptext = "The minimum allowable height of the toe joint before there is apparent ground penetration."
      },
      {
        name = "BallRotationAxisX", type = "float", value = 1, perAnimSet = true,
        helptext = "The axis around which the ball joint can rotate, local to its parent."
      },
      {
        name = "BallRotationAxisY", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis around which the ball joint can rotate, local to its parent."
      },
      {
        name = "BallRotationAxisZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis around which the ball joint can rotate, local to its parent."
      },
      {
        name = "FixFootOrientation", type = "bool", value = false, perAnimSet = true,
        helptext = "If turned on, the ankle is allowed to rotate to level the foot where appropriate."
      },
      {
        name = "FootLevelVectorX", type = "float", value = 1, perAnimSet = true,
        helptext = "The direction side-on to the foot which is parallel to the ground when the foot is flat, used for correcting foot roll.  Specified in the coordinate frame of the ankle."
      },
      {
        name = "FootLevelVectorY", type = "float", value = 0, perAnimSet = true,
        helptext = "The direction side-on to the foot which is parallel to the ground when the foot is flat, used for correcting foot roll.  Specified in the coordinate frame of the ankle."
      },
      {
        name = "FootLevelVectorZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The direction side-on to the foot which is parallel to the ground when the foot is flat, used for correcting foot roll.  Specified in the coordinate frame of the ankle."
      },
      {
        name = "FootPivotResistance", type = "float", value = 0, max = 1, min = 0, perAnimSet = true,
        helptext = "A value between 0 and 1 indicating the resistance of the foot to pivoting about the vertical when locked.  Only applies when Fix Foot Orientation is turned on.  When 0, the foot can pivot freely.  When 1, the foot will never pivot while locked."
      },
      {
        name = "TrackCharacterController", type = "bool", value = false, perAnimSet = false,
        helptext = "Enabling this will help eliminate footslip caused by movement of the character controller in nodes above this one, or in game code."
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
          return nil, ("LockFoot node " .. node .. " requires a valid input node")
        end
      else
        return nil, ("LockFoot node " .. node .. " is missing a required connection to Source")
      end

      local weightPin = string.format("%s.IkFkBlendWeight", node)
      if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightPin, ResolveReferences = true }
        local weightNode = connections[1]
        if isValid(weightNode) ~= true then
          return nil, string.format("LockFoot node %s requires a valid input to IkFkBlendWeight, node %s is not valid", node, weightNode)
        end
      else
        return nil, string.format("LockFoot node %s is missing a required connection to IkFkBlendWeight", node)
      end

      local swivelContributionToOrientationPin = string.format("%s.SwivelContributionToOrientation", node)
      if isConnected{ SourcePin = swivelContributionToOrientationPin, ResolveReferences = true } then
        local connections = listConnections{ Object = swivelContributionToOrientationPin, ResolveReferences = true }
        local weightNode = connections[1]
        if isValid(weightNode) ~= true then
          return nil, string.format("LockFoot node %s requires a valid input to SwivelContributionToOrientation, node %s is not valid", node, weightNode)
        end
      else
        -- DS2 (FRPG2): SwivelContributionToOrientation is optional
        -- return nil, string.format("LockFoot node %s is missing a required connection to SwivelContributionToOrientation", node)
      end

      -- Validate Rig indices
      -- When this node supports non-sequential IK chains, this needs to be expanded
      local animSets = listAnimSets()
      for asIdx, asVal in animSets do
        local ankleName = getAttribute(node, "AnkleName", asVal)
        local ankleIndex = nil
        if ankleName ~= nil then
          ankleIndex = anim.getRigChannelIndex(ankleName, asVal)
        end

        local rigSize = anim.getRigSize(asVal)

        if not getAttribute(node, "UseBallJoint", asVal) then

          if ankleIndex == nil or ankleIndex <= 0 or ankleIndex >= rigSize then
            return nil, ("LockFoot node " .. node .. " (animset " .. asVal .. ") requires a valid AnkleName when UseBallJoint is FALSE")
          end

        else

          local toeName = getAttribute(node, "ToeName", asVal)
          local toeIndex = nil
          if toeName ~= nil then
            toeIndex = anim.getRigChannelIndex(toeName, asVal)
          end

          local ballName = getAttribute(node, "BallName", asVal)
          local ballIndex = nil
          if ballName ~= nil then
            ballIndex = anim.getRigChannelIndex(ballName, asVal)
          end

          if ballIndex == nil or ballIndex <= 0 or ballIndex >= rigSize then
            return nil, ("LockFoot node " .. node .. " (animset " .. asVal .. ") requires a valid BallName when UseBallJoint is TRUE")
          end

          if getAttribute(node, "FixToeGroundPenetration", asVal) then
            if toeIndex == nil or toeIndex <= 0 or toeIndex >= rigSize then
              return nil, ("LockFoot node " .. node .. " (animset " .. asVal .. ") requires a valid ToeName when fixing toe ground penetration")
            end

            if ballIndex ~= anim.getParentBoneIndex(toeIndex, asVal) then
              return nil, ("LockFoot node " .. node .. " (animset " .. asVal .. "): ball joint must be the parent of the toe when it is being used")
            end
          end
          ankleIndex = anim.getParentBoneIndex(ballIndex, asVal)
        end
        local kneeIndex = anim.getParentBoneIndex(ankleIndex, asVal)
        local hipIndex = anim.getParentBoneIndex(kneeIndex, asVal)
        if kneeIndex == nil or kneeIndex <= 0 or kneeIndex >= rigSize or
           hipIndex == nil or hipIndex <= 0 or hipIndex >= rigSize then
          return nil, ("LockFoot node " .. node .. " (animset " .. asVal .. "): no valid trace through the hierarchy to the hip joint. Check your joint names.")
        end

        if getAttribute(node, "FixToeGroundPenetration", asVal) and not (getAttribute(node, "UseBallJoint", asVal) and getAttribute(node, "FixGroundPenetration", asVal)) then
          return true, ("LockFoot node " .. node .. ": attribute 'Fix toe ground penetration' ignored - requires 'Use ball joint' and 'Fix ground penetration'")
        end

      end

      if isConnected{ SourcePin = (node .. ".IkFkBlendWeight"), ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = (node .. ".IkFkBlendWeight"), ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("LockFoot node " .. node .. " requires a valid IkFkBlendWeight node")
        end
      end

      local swivelContributionToOrientationPin = string.format("%s.SwivelContributionToOrientation", node)
      if isConnected{ SourcePin = swivelContributionToOrientationPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = swivelContributionToOrientationPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("LockFoot node " .. node .. " requires a valid SwivelContributionToOrientation node")
        end
      end

      local worldUpAxis = preferences.get("WorldUpAxis")
      if not (worldUpAxis == "X Axis" or worldUpAxis == "Y Axis" or worldUpAxis == "Z Axis") then
        return nil, ("GunAimIK node " .. node  .. " failed because the world up axis preference returned an unexpected value")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputNodeID = -1
      local iKFkBlendWeightNodeInfo = nil
      local swivelContributionToOrientationNodeInfo = nil

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        inputNodeID = getConnectedNodeID(sourcePin)
      end

      local blendWeightPin = string.format("%s.IkFkBlendWeight", node)
      if isConnected{ SourcePin = blendWeightPin, ResolveReferences = true } then
        iKFkBlendWeightNodeInfo = getConnectedNodeInfo(blendWeightPin)
      end

      local swivelContributionToOrientationPin = string.format("%s.SwivelContributionToOrientation", node)
      if isConnected{ SourcePin = swivelContributionToOrientationPin, ResolveReferences = true } then
        swivelContributionToOrientationNodeInfo = getConnectedNodeInfo(swivelContributionToOrientationPin)
      end

      Stream:writeNetworkNodeId(inputNodeID, "InputNodeID")
      if iKFkBlendWeightNodeInfo then
        Stream:writeNetworkNodeId(iKFkBlendWeightNodeInfo.id, "IkFkBlendWeight", iKFkBlendWeightNodeInfo.pinIndex)
      end
      if swivelContributionToOrientationNodeInfo then
        Stream:writeNetworkNodeId(swivelContributionToOrientationNodeInfo.id, "SwivelContributionToOrientation", swivelContributionToOrientationNodeInfo.pinIndex)
      end

      local assumeSimpleHierarchy = true --getAttribute(node, "AssumeSimpleHierarchy")
      Stream:writeBool(assumeSimpleHierarchy, "AssumeSimpleHierarchy")

      -- Serialise world up axis as an index into a Cartesian 3-vector
      --  The UpAxis is now set from preferences.  To recover manual editing of the up axis, modify
      --  this section to get the axis from the UpAxis attributes, and edit custom UI to provide authoring.
      local worldUpAxis = preferences.get("WorldUpAxis")
      local upAxisIndex = 0
      if worldUpAxis == "Y Axis" then
        upAxisIndex = 1
      elseif worldUpAxis == "Z Axis" then
        upAxisIndex = 2
      end
      Stream:writeUInt(upAxisIndex, "UpAxisIndex")
      
      local lockVerticalMotion = getAttribute(node, "LockVerticalMotion")
      Stream:writeBool(lockVerticalMotion, "LockVerticalMotion")
      
      local trackCharacterController = getAttribute(node, "TrackCharacterController")
      Stream:writeBool(trackCharacterController, "TrackCharacterController")

      local animSets = listAnimSets()
      for asIdx, asVal in animSets do
        local ankleName = getAttribute(node, "AnkleName", asVal)
        local ankleIndex = anim.getRigChannelIndex(ankleName, asVal)
        if ankleIndex == nil then
          ankleIndex = -1
        end
        Stream:writeUInt(ankleIndex, "AnkleIndex_"..asIdx)

        local ballName = getAttribute(node, "BallName", asVal)
        local ballIndex = anim.getRigChannelIndex(ballName, asVal)
        if ballIndex == nil then
          ballIndex = -1
        end
        Stream:writeUInt(ballIndex, "BallIndex_"..asIdx)

        local toeName = getAttribute(node, "ToeName", asVal)
        local toeIndex = anim.getRigChannelIndex(toeName, asVal)
        if toeIndex == nil then
          toeIndex = -1
        end
        Stream:writeUInt(toeIndex, "ToeIndex_"..asIdx)

        local footfallEventID = getAttribute(node, "FootfallEventID", asVal)
        Stream:writeInt(footfallEventID, "FootfallEventID_"..asIdx)

        local kneeRotationAxisX = getAttribute(node, "KneeRotationAxisX", asVal)
        Stream:writeFloat(kneeRotationAxisX, "KneeRotationAxisX_"..asIdx)

        local kneeRotationAxisY = getAttribute(node, "KneeRotationAxisY", asVal)
        Stream:writeFloat(kneeRotationAxisY, "KneeRotationAxisY_"..asIdx)

        local kneeRotationAxisZ = getAttribute(node, "KneeRotationAxisZ", asVal)
        Stream:writeFloat(kneeRotationAxisZ, "KneeRotationAxisZ_"..asIdx)

        local flipKneeRotationDirection = getAttribute(node, "FlipKneeRotationDirection", asVal)
        Stream:writeBool(flipKneeRotationDirection, "FlipKneeRotationDirection_"..asIdx)

        local useBallJoint = getAttribute(node, "UseBallJoint", asVal)
        Stream:writeBool(useBallJoint, "UseBallJoint_"..asIdx)

        local catchupSpeedFactor = getAttribute(node, "CatchUpSpeedFactor", asVal)
        Stream:writeFloat(catchupSpeedFactor, "CatchUpSpeedFactor_"..asIdx)

        local snapToSourceDistance = getAttribute(node, "SnapToSourceDistance", asVal)
        snapToSourceDistance = mcn.convertWorldToPhysics(snapToSourceDistance)
        Stream:writeFloat(snapToSourceDistance, "SnapToSourceDistance_"..asIdx)

        local straightestLegFactor = getAttribute(node, "StraightestLegFactor", asVal)
        Stream:writeFloat(straightestLegFactor, "StraightestLegFactor_"..asIdx)

        local fixGroundPenetration = getAttribute(node, "FixGroundPenetration", asVal)
        Stream:writeBool(fixGroundPenetration, "FixGroundPenetration_"..asIdx)

        local ankleLowerHeightBound = getAttribute(node, "AnkleLowerHeightBound", asVal)
        Stream:writeFloat(ankleLowerHeightBound, "AnkleLowerHeightBound_"..asIdx)

        local lowerHeightBound = getAttribute(node, "LowerHeightBound", asVal)
        lowerHeightBound = mcn.convertWorldToPhysics(lowerHeightBound)
        Stream:writeFloat(lowerHeightBound, "LowerHeightBound_"..asIdx)

        local fixToeGroundPenetration = getAttribute(node, "FixToeGroundPenetration", asVal) and useBallJoint and fixGroundPenetration
        Stream:writeBool(fixToeGroundPenetration, "FixToeGroundPenetration_"..asIdx)

        local toeLowerHeightBound = getAttribute(node, "ToeLowerHeightBound", asVal)
        toeLowerHeightBound = mcn.convertWorldToPhysics(toeLowerHeightBound)
        Stream:writeFloat(toeLowerHeightBound, "ToeLowerHeightBound_"..asIdx)

        local ballRotationAxisX = getAttribute(node, "BallRotationAxisX", asVal)
        Stream:writeFloat(ballRotationAxisX, "BallRotationAxisX_"..asIdx)

        local ballRotationAxisY = getAttribute(node, "BallRotationAxisY", asVal)
        Stream:writeFloat(ballRotationAxisY, "BallRotationAxisY_"..asIdx)

        local ballRotationAxisZ = getAttribute(node, "BallRotationAxisZ", asVal)
        Stream:writeFloat(ballRotationAxisZ, "BallRotationAxisZ_"..asIdx)

        local footLevelVectorX = getAttribute(node, "FootLevelVectorX", asVal)
        Stream:writeFloat(footLevelVectorX, "FootLevelVectorX_"..asIdx)

        local footLevelVectorY = getAttribute(node, "FootLevelVectorY", asVal)
        Stream:writeFloat(footLevelVectorY, "FootLevelVectorY_"..asIdx)

        local footLevelVectorZ = getAttribute(node, "FootLevelVectorZ", asVal)
        Stream:writeFloat(footLevelVectorZ, "FootLevelVectorZ_"..asIdx)

        local fixFootOrientation = getAttribute(node, "FixFootOrientation", asVal)
        Stream:writeBool(fixFootOrientation, "FixFootOrientation_"..asIdx)

        local footPivotResistance = getAttribute(node, "FootPivotResistance", asVal)
        Stream:writeFloat(footPivotResistance, "FootPivotResistance_"..asIdx)
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

      -- Generate reverse map
      local reverseMap = { }
      for jointIndex,_ in inputNodeChannels do
        reverseMap[jointIndex] = jointIndex
      end

      -- Add any channels output by this node that are not provided by the input
      local ankleName = getAttribute(node, "AnkleName", set)
      local furthestIndex = anim.getRigChannelIndex(ankleName, set)
      local kneeIndex = anim.getParentBoneIndex(furthestIndex, set)
      local hipIndex = anim.getParentBoneIndex(kneeIndex, set)
      if getAttribute(node, "UseBallJoint", set) then
        local ballName = getAttribute(node, "BallName", set)
        furthestIndex = anim.getRigChannelIndex(ballName, set)
      end
      local j = furthestIndex
      repeat
        reverseMap[j] = j
        j = anim.getParentBoneIndex(j, set)
      until j < hipIndex

      -- Re-reverse to get output channels.  Sort to keep neat numerical order.
      local outputChannels = { }
      for jointIndex, val in reverseMap do
        outputChannels[jointIndex] = true
      end

      return outputChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version == 0 then
        local upAxis = getAttribute(node .. ".deprecated_UpAxis")
        setAttribute(node .. ".UpAxisX", upAxis[1])
        setAttribute(node .. ".UpAxisY", upAxis[2])
        setAttribute(node .. ".UpAxisZ", upAxis[3])
        removeAttribute(node, "deprecated_UpAxis")
      end

      if version < 2 then
        copyAnimSetAttribute(node, "deprecated_AnkleIndex", "AnkleName", convertJointIndexToName)
        copyAnimSetAttribute(node, "deprecated_BallIndex", "BallName", convertJointIndexToName)
        copyAnimSetAttribute(node, "deprecated_ToeIndex", "ToeName", convertJointIndexToName)

        --remove deprecated values
        removeAttribute(node, "deprecated_AnkleIndex")
        removeAttribute(node, "deprecated_BallIndex")
        removeAttribute(node, "deprecated_ToeIndex")
      end

      if version < 3 then
        local sourcePath = string.format("%s.Source", node)
        setPinPassThrough(sourcePath, true)

        local resultPath = string.format("%s.Result", node)
        setPinPassThrough(resultPath, true)
      elseif version >= 3 and version < 6 then
        local oldSourcePath = string.format("%s.In", node)
        local newSourcePath = string.format("%s.Source", node)
        pinLookupTable[oldSourcePath] = newSourcePath

        local oldResultPath = string.format("%s.Out", node)
        local newResultPath = string.format("%s.Result", node)
        pinLookupTable[oldResultPath] = newResultPath
      end

      if version < 4 then
        -- The following attributes were added in version 4
        --  AnkleLowerHeightBound
        --  FixFootOrientation
        --  FootLevelVectorX
        --  FootLevelVectorY
        --  FootLevelVectorZ
      end

      if version < 5 then
        -- The following attributes were added in version 5
        --  FootPivotResistance
      end

      if version < 6 then
        -- FilterNodes were removed so pins In and Out were renamed to Source and Result.
      end

      if version < 8 then
        -- The following attributes were added in version 8
        --  LockVerticalMotion
        --  TrackCharacterController
        
        -- The following attribute was hidden in version 8
        --  UpAxis  - this is now set automatically from project preferences
        
        -- Determine LockVerticalMotion attribute
        local noUpAxisSpecified = false
        if not (getAttribute(node .. ".UpAxisX") or getAttribute(node .. ".UpAxisY") or getAttribute(node .. ".UpAxisZ")) then
          setAttribute(node .. ".LockVerticalMotion", true)
          noUpAxisSpecified = true
        end
        
        --  Warn if your up axis is going to be changed
        local worldUpAxis = preferences.get("WorldUpAxis")
        if ((worldUpAxis == "X Axis" and not getAttribute(node .. ".UpAxisX")) or
            (worldUpAxis == "Y Axis" and not getAttribute(node .. ".UpAxisY")) or
            (worldUpAxis == "Z Axis" and not getAttribute(node .. ".UpAxisZ"))) and
           not noUpAxisSpecified then
          app.warning("While upgrading LockFoot node " .. node .. ", the UpAxis attribute was changed to match your preferences.")
        end

        --  The UpAxis attributes are overridden by preferences and hidden
        setAttribute(node .. ".UpAxisX", false)
        setAttribute(node .. ".UpAxisY", false)
        setAttribute(node .. ".UpAxisZ", false)
        if worldUpAxis == "X Axis" then
          setAttribute(node .. ".UpAxisX", true)
        elseif worldUpAxis == "Y Axis" then
          setAttribute(node .. ".UpAxisY", true)
        elseif worldUpAxis == "Z Axis" then
          setAttribute(node .. ".UpAxisZ", true)
        end
      end

    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- LockFoot custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "LockFoot",
    {
      {
        title = "Joint Names",
        usedAttributes = { "AnkleName", "BallName", "ToeName", "FootfallEventID" },
        displayFunc = function(...) safefunc(attributeEditor.animSetDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Joint Axis",
        usedAttributes = {
          "KneeRotationAxisX",
          "KneeRotationAxisY",
          "KneeRotationAxisZ",
          "BallRotationAxisX",
          "BallRotationAxisY",
          "BallRotationAxisZ",
          "FlipKneeRotationDirection",
          "UseBallJoint"
        },
        displayFunc = function(...) safefunc(attributeEditor.lockFootJointAxisDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Ground Plane",
        usedAttributes =
        {
          "FixGroundPenetration",
          "AnkleLowerHeightBound",
          "LowerHeightBound",
          "FixToeGroundPenetration",
          "ToeLowerHeightBound"
        },
        displayFunc = function(...) safefunc(attributeEditor.lockFootGroundPlaneDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Foot Orientation",
        usedAttributes = { "FixFootOrientation", "FootLevelVectorX", "FootLevelVectorY", "FootLevelVectorZ", "FootPivotResistance" },
        displayFunc = function(...) safefunc(attributeEditor.lockFootOrientationDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = { "StraightestLegFactor", "SnapToSourceDistance", "CatchUpSpeedFactor" },
        displayFunc = function(...) safefunc(attributeEditor.footlockPropertiesDisplayInfoSection, unpack(arg)) end
      },
      -- The up axis is now set from preferences.  To recover manual editing of the up axis, remove the UpAxis
      -- attributes from this panel and make relevant edits to the serialise function.
      {
        title = "General",
        usedAttributes = { "LockVerticalMotion", "UpAxisX", "UpAxisY", "UpAxisZ", "TrackCharacterController" },
        displayFunc = function(...) safefunc(attributeEditor.footlockGeneralDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end

