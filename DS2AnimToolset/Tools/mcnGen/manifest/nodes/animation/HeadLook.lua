------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/HeadLookDisplayInfo.lua"
require "ui/AttributeEditor/HeadLookPropertiesDisplayInfo.lua"
require "luaAPI/debugDataAPI.lua"

local convertJointIndexToName = function(jointIndex, animSet)
  return anim.getAnimChannelName(jointIndex, animSet)
end

------------------------------------------------------------------------------------------------------------------------
-- HeadLook node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("HeadLook",
  {
    displayName = "Head Look",
    helptext = "Head Look: points chosen end effector at target along a given pointing direction",
    group = "IK",
    image = "HeadLook.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 122),
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
      ["Target"] = {
        input = true,
        array = false,
        type = "vector3",
      },
      ["BlendWeight"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source", "Result", "Target", "BlendWeight" },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "EndJointName", displayName = "EndJoint", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The end joint in the chain to be modified."
      },
      {
        name = "RootJointName", displayName = "RootJoint", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The root joint of the chain to be modified. Must be an ancestor of the end joint."
      },
      {
        name = "PointingVectorX", type = "float", value = 0, perAnimSet = true,
        helptext = "The vector defining which direction must be pointing towards the target."
      },
      {
        name = "PointingVectorY", type = "float", value = 0, perAnimSet = true,
        helptext = "The vector defining which direction must be pointing towards the target."
      },
      {
        name = "PointingVectorZ", type = "float", value = 1, perAnimSet = true,
        helptext = "The vector defining which direction must be pointing towards the target."
      },
      {
        name = "EndEffectorOffsetX", type = "float", value = 0, perAnimSet = true,
        helptext = "The position of the end effector with respect to the local frame of the end joint."
      },
      {
        name = "EndEffectorOffsetY", type = "float", value = 0, perAnimSet = true,
        helptext = "The position of the end effector with respect to the local frame of the end joint."
      },
      {
        name = "EndEffectorOffsetZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The position of the end effector with respect to the local frame of the end joint."
      },
      {
        name = "Bias", type = "float", value = 1, perAnimSet = true,
        helptext = "A float parameter between -1 and 1, defining the amount of weight given to each joint for achieving the goal. -1 is fully biased towards the root joint, 0 is an even spread, and 1 is fully biased towards the end joint."
      },
      {
        name = "UpdateTargetByDeltas", type = "bool", value = false,
        helptext = "If previous, assume that the input target position was given relative to the character's trajectory root at the previous update, and should be corrected for motion since then.\n\nThis attribute only applies when the Target Frame is set to Character Space."
      },
      {
        name = "ApplyJointLimits", type = "bool", value = false, perAnimSet = false,
        helptext = "Enforce joint limits defined on your animation rig."
      },
      {
        name = "KeepUpright", type = "bool", value = false, perAnimSet = false,
        helptext = "Minimise tilt around the pointing direction.  This relies on the input animation providing an upright orientation for reference.\n\nThere will be a discontinuity between turning around to the left or the right when the target passes behind the character."
      },
      {
        name = "DebugDraw", type = "bool", value = false, perAnimSet = false,
        helptext = "If set, then debug lines will be drawn in the viewport.\n\nIn the Preview Script an update handler will need to call viewport.debugDraw.update() to see the debug information."
      },
      {
        name = "MinimiseRotation", type = "bool", value = true, perAnimSet = false,
        helptext = "Overall rotation of joints is minimised, rather than translation of the end effector.  The output usually has fewer discontinuities when there is a lot of rotation when this is switched on.  This attribute is hidden from the user at the moment because there are not clear enough use cases for turning this off."
      },
      {
        name = "WorldSpaceTarget", type = "bool", value = false, perAnimSet = false,
        helptext = "If true, the target input control parameter is specified in world space; otherwise, it is expressed in character space (the coordinate frame of the trajectory joint)."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local inputNode = nil

      -- Validate connections
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = sourcePin, ResolveReferences = true }

        inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("HeadLook node " .. node .. " requires a valid input node")
        end
      else
        return nil, ("HeadLook node " .. node .. " is missing a required connection to Source")
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

        local rootName = getAttribute(node, "RootJointName", asVal)
        local rootIndex = nil
        if rootName ~= nil then
          rootIndex = anim.getRigChannelIndex(rootName, asVal)
        end

        local rigSize = anim.getRigSize(asVal)

        if index == nil or index <= 0 or index >= rigSize then
          return nil, ("HeadLook node " .. node .. " (animset " .. asVal .. ") requires a valid EndJointName")
        end

        if rootIndex == nil or rootIndex <= 0 or rootIndex >= rigSize then
          return nil, ("HeadLook node " .. node .. " (animset " .. asVal .. ") requires a valid RootJointName")
        end

        while index ~= rootIndex do
          if index == nil or index <= 0 or index >= rigSize then
            return nil, ("HeadLook node " .. node .. " (animset " .. asVal .. "): the root joint is not an ancestor of the end joint")
          end

          index = anim.getParentBoneIndex(index, asVal)
        end
      end

      local targetPin = string.format("%s.Target", node)
      if isConnected{ SourcePin = targetPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = targetPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]

        if isValid(inputNode) ~= true then
          return nil, ("HeadLook node " .. node .. " requires a valid Look-At Target")
        end
      else
        return nil, ("HeadLook node " .. node .. " is missing a required connection to Target")
      end

      local blendWeightPin = string.format("%s.BlendWeight", node)
      if isConnected{ SourcePin = blendWeightPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = blendWeightPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("HeadLook node " .. node .. " requires a valid Blend Weight")
        end
      end

      local worldUpAxis = preferences.get("WorldUpAxis")
      if not (worldUpAxis == "X Axis" or worldUpAxis == "Y Axis" or worldUpAxis == "Z Axis") then
        return nil, ("HeadLook node " .. node  .. " failed because the world up axis preference returned an unexpected value")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputNodeID = -1
      local targetNodeInfo = nil
      local blendWeightNodeInfo = nil

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        inputNodeID = getConnectedNodeID(sourcePin)
      end

      local targetPin = string.format("%s.Target", node)
      if isConnected{ SourcePin = targetPin, ResolveReferences = true } then
        targetNodeInfo = getConnectedNodeInfo(targetPin)
      end

      local blendWeightPin = string.format("%s.BlendWeight", node)
      if isConnected{ SourcePin = blendWeightPin, ResolveReferences = true } then
        blendWeightNodeInfo = getConnectedNodeInfo(blendWeightPin)
      end

      Stream:writeNetworkNodeId(inputNodeID, "InputNodeID")
      if targetNodeInfo then
        Stream:writeNetworkNodeId(targetNodeInfo.id, "Target", targetNodeInfo.pinIndex)
      else
        Stream:writeNetworkNodeId(-1, "Target")
      end
      if blendWeightNodeInfo then
        Stream:writeNetworkNodeId(blendWeightNodeInfo.id, "BlendWeight", blendWeightNodeInfo.pinIndex)
      else
        Stream:writeNetworkNodeId(-1, "BlendWeight")
      end

      local updateTargetByDeltas = getAttribute(node, "UpdateTargetByDeltas")
      Stream:writeBool(updateTargetByDeltas, "UpdateTargetByDeltas")

      local applyJointLimits = getAttribute(node, "ApplyJointLimits")
      Stream:writeBool(applyJointLimits, "ApplyJointLimits")
      
      local minimiseRotation = getAttribute(node, "MinimiseRotation")
      Stream:writeBool(minimiseRotation, "MinimiseRotation")

      Stream:writeBool(getAttribute(node, "KeepUpright"), "KeepUpright")

      Stream:writeBool(getAttribute(node, "WorldSpaceTarget"), "WorldSpaceTarget")

      -- Serialise out the up axis based on morpheme settings
      local worldUpAxis = preferences.get("WorldUpAxis")
      local upX, upY, upZ = 0, 0, 0
      if worldUpAxis == "X Axis" then
        upX = 1.0
      elseif worldUpAxis == "Y Axis" then
        upY = 1.0
      elseif worldUpAxis == "Z Axis" then
        upZ = 1.0
      end
      Stream:writeFloat(upX, "WorldUpAxisX")
      Stream:writeFloat(upY, "WorldUpAxisY")
      Stream:writeFloat(upZ, "WorldUpAxisZ")

      local animSets = listAnimSets()
      for asIdx, asVal in animSets do
        local pointingVectorX = getAttribute(node, "PointingVectorX", asVal)
        Stream:writeFloat(pointingVectorX, "PointingVectorX_"..asIdx)

        local pointingVectorY = getAttribute(node, "PointingVectorY", asVal)
        Stream:writeFloat(pointingVectorY, "PointingVectorY_"..asIdx)

        local pointingVectorZ = getAttribute(node, "PointingVectorZ", asVal)
        Stream:writeFloat(pointingVectorZ, "PointingVectorZ_"..asIdx)

        local offsetX = getAttribute(node, "EndEffectorOffsetX", asVal)
        offsetX = mcn.convertWorldToPhysics(offsetX)
        Stream:writeFloat(offsetX, "EndEffectorOffsetX_"..asIdx)

        local offsetY = getAttribute(node, "EndEffectorOffsetY", asVal)
        offsetY = mcn.convertWorldToPhysics(offsetY)
        Stream:writeFloat(offsetY, "EndEffectorOffsetY_"..asIdx)

        local offsetZ = getAttribute(node, "EndEffectorOffsetZ", asVal)
        offsetZ = mcn.convertWorldToPhysics(offsetZ)
        Stream:writeFloat(offsetZ, "EndEffectorOffsetZ_"..asIdx)

        local bias = getAttribute(node, "Bias", asVal)
        Stream:writeFloat(bias, "Bias_"..asIdx)

        local endJointName = getAttribute(node, "EndJointName", asVal)
        local endJointIndex = anim.getRigChannelIndex(endJointName, asVal)
        Stream:writeUInt(endJointIndex, "EndJointIndex_"..asIdx)

        local rootJointName = getAttribute(node, "RootJointName", asVal)
        local rootJointIndex = anim.getRigChannelIndex(rootJointName, asVal)
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
      local rootJointName = getAttribute(node, "RootJointName", set)
      local rootJointIndex = anim.getRigChannelIndex(rootJointName, set)
      local j = endJointIndex
      repeat
        reverseMap[j] = j
        j = anim.getParentBoneIndex(j, set)
      until j < rootJointIndex

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
        copyAnimSetAttribute(node, "deprecated_RootJointIndex", "RootJointName", convertJointIndexToName)

        --remove deprecated values
        removeAttribute(node, "deprecated_EndJointIndex")
        removeAttribute(node, "deprecated_RootJointIndex")
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
        --  ApplyJointLimits
        --  KeepUpright
        --  MinimiseRotation
        --  WorldSpaceTarget
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- HeadLook custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "HeadLook",
    {
      {
        title = "Joint Names",
        usedAttributes = { "RootJointName", "EndJointName" },
        displayFunc = function(...) safefunc(attributeEditor.animSetDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "End Effector Vectors",
        usedAttributes = {
          "PointingVectorX",
          "PointingVectorY",
          "PointingVectorZ",
          "EndEffectorOffsetX",
          "EndEffectorOffsetY",
          "EndEffectorOffsetZ",
        },
        displayFunc = function(...) safefunc(attributeEditor.headLookDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = {
          "Bias",
          "UpdateTargetByDeltas",
          -- No widget is actually created, and therefore this attribute is hidden.  Comment next line out to unhide.
          "MinimiseRotation",
          "KeepUpright",
          "ApplyJointLimits",
          "WorldSpaceTarget",
        },
        displayFunc = function(...) safefunc(attributeEditor.headlookPropertiesDisplayInfoSection, unpack(arg)) end
      }
    }
  )

  previewScript.debugData.registerProvider(
    "HeadLook",
    function(self)
      if getAttribute(self, "DebugDraw") then
        local animSets = listAnimSets()

        -- we scale the offset into the runtime unit scale
        local runtimeAssetScaleFactor = 1 / preferences.get("RuntimeAssetScaleFactor")
        
        for asIdx, asVal in ipairs(animSets) do
          local hier = anim.getRigHierarchy(asVal)
          local endJointName = getAttribute(self, "EndJointName", asVal)
          local endJointIndex = anim.getRigChannelIndex(endJointName, asVal)

          local offset={ }
          local vector={ }
          -- the offset is already in the runtime unit scale, as it was set explicitly by the user
          offset.x = getAttribute(self, "EndEffectorOffsetX")
          offset.y = getAttribute(self, "EndEffectorOffsetY")
          offset.z = getAttribute(self, "EndEffectorOffsetZ")
          
          vector.x = getAttribute(self, "PointingVectorX")
          vector.y = getAttribute(self, "PointingVectorY")
          vector.z = getAttribute(self, "PointingVectorZ")
          
          -- make the vector about 1m in runtime asset units (so 100 cm if the runtime uses centimeters)
          local length = 0.1 * math.sqrt( vector.x * vector.x + vector.y * vector.y + vector.z * vector.z )
          vector.x = (vector.x / length) * runtimeAssetScaleFactor;
          vector.y = (vector.y / length) * runtimeAssetScaleFactor;
          vector.z = (vector.z / length) * runtimeAssetScaleFactor;
          
          local colour= { r=255, g=255, b=0 }
          local scale = 0.1

          previewScript.debugData.addPoint(self, asVal, endJointIndex, offset, scale * runtimeAssetScaleFactor, colour)
          previewScript.debugData.addLine(self, asVal, endJointIndex, offset, vector, colour)
        end -- Anim Set loop
      end -- If DebugDraw is true
    end -- Debug Draw function
  )
end