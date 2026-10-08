------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "luaAPI/debugDataAPI.lua"
require "previewScripts/NetworkNodeDebugDraw.lua"
require "ui/AttributeEditor/GunAimIKDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- GunAimIK node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("GunAimIK",
  {
    displayName = "Gun Aim IK",
    helptext = "GunAimIK: points gun at target along a given pointing direction",
    group = "IK",
    image = "GunAimIK.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 150),
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
        name = "SpineRootJointName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The root joint of the chain to be modified, where the IK aim begins to take effect"
      },
      {
        name = "GunJointName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The joint to which the gun mesh is attached."
      },
      {
        name = "GunBindJointName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The joint to which the stock of the gun is bound.  The transform of the stock of the gun will remain relative to the transform of the selected joint, moving the gun with it.  For example, bracing the stock of a rifle against the primary shoulder."
      },
      {
        name = "SecondaryWristJointName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The secondary wrist end joint in the chain to be modified."
      },
      {
        name = "PrimaryWristJointName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The primary wrist end joint of the chain to be modified."
      },
      {
        name = "GunPivotOffsetX", type = "float", value = 0, perAnimSet = true,
        helptext = "The offset from the Gun Joint to the position of the gun stock in the coordinate system of the gun."
      },
      {
        name = "GunPivotOffsetY", type = "float", value = 0, perAnimSet = true,
        helptext = "The offset from the Gun Joint to the position of the gun stock in the coordinate system of the gun."
      },
      {
        name = "GunPivotOffsetZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The offset from the Gun Joint to the position of the gun stock in the coordinate system of the gun."
      },
      {
        name = "GunBarrelOffsetX", type = "float", value = 0, perAnimSet = true,
        helptext = "The offset from the Gun Joint to a point on the gun barrel in the coordinate system of the gun."
      },
      {
        name = "GunBarrelOffsetY", type = "float", value = 0, perAnimSet = true,
        helptext = "The offset from the Gun Joint to a point on the gun barrel in the coordinate system of the gun."
      },
      {
        name = "GunBarrelOffsetZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The offset from the Gun Joint to a point on the gun barrel in the coordinate system of the gun."
      },
      {
        name = "GunPointingVectorX", type = "float", value = 1, perAnimSet = true,
        helptext = "The vector defining which direction the gun barrel is pointing in the coordinate system of the gun."
      },
      {
        name = "GunPointingVectorY", type = "float", value = 0, perAnimSet = true,
        helptext = "The vector defining which direction the gun barrel is pointing in the coordinate system of the gun."
      },
      {
        name = "GunPointingVectorZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The vector defining which direction the gun barrel is pointing in the coordinate system of the gun."
      },
      {
        name = "SecondaryArmHingeAxisX", type = "float", value = 0, perAnimSet = true,
        helptext = "The vector defining the hinge axis for the secondary elbow, in the coordinate frame of the shoulder."
      },
      {
        name = "SecondaryArmHingeAxisY", type = "float", value = 0, perAnimSet = true,
        helptext = "The vector defining the hinge axis for the secondary elbow, in the coordinate frame of the shoulder."
      },
      {
        name = "SecondaryArmHingeAxisZ", type = "float", value = 1, perAnimSet = true,
        helptext = "The vector defining the hinge axis for the secondary elbow, in the coordinate frame of the shoulder."
      },
      {
        name = "PrimaryArmHingeAxisX", type = "float", value = 0, perAnimSet = true,
        helptext = "The vector defining the hinge axis for the primary elbow, in the coordinate frame of the shoulder."
      },
      {
        name = "PrimaryArmHingeAxisY", type = "float", value = 0, perAnimSet = true,
        helptext = "The vector defining the hinge axis for the primary elbow, in the coordinate frame of the shoulder."
      },
      {
        name = "PrimaryArmHingeAxisZ", type = "float", value = 1, perAnimSet = true,
        helptext = "The vector defining the hinge axis for the primary elbow, in the coordinate frame of the shoulder."
      },
      {
        name = "SpineBias", type = "float", value = 0, perAnimSet = true,
        helptext = "A float parameter between -1 and 1, defining the amount of weight given to each joint for achieving the goal. -1 is fully biased towards the root joint, 0 is an even spread, and 1 is fully biased towards the end joint."
      },
      {
        name = "FlipSecondaryHinge", type = "bool", value = false, perAnimSet = true,
        helptext = "If set, the sign of the mid joint axis of the secondary arm is changed, which will flip its bending direction."
      },
      {
        name = "FlipPrimaryHinge", type = "bool", value = false, perAnimSet = true,
        helptext = "If set, the sign of the mid joint axis of the primary arm is changed, which will flip its bending direction."
      },
      {
        name = "UpdateTargetByDeltas", type = "bool", value = false, perAnimSet = true,
        helptext = "If Previous is set, assume that the input target position was given relative to the character trajectory root at the previous update, and should be corrected for motion since then."
      },
      {
        name = "UseSecondaryArm", type = "bool", value = true, perAnimSet = true,
        helptext = "If set, the secondary arm is bound to the gun, otherwise it is free.  Use for two-handed guns."
      },
      {
        name = "KeepUpright", type = "bool", value = false, perAnimSet = false,
        helptext = "Minimise tilt of the gun around the barrel direction.  This relies on the input animation providing an upright orientation as reference.\n\nThere will be a discontinuity between aiming around to the left or the right when the target passes behind the character."
      },
      {
        name = "ApplyJointLimits", type = "bool", value = false, perAnimSet = false,
        helptext = "Enforce joint limits defined on your animation rig.\n\nOnly joint limits on the primary arm can prevent the character from successfully aiming at any target."
      },
      {
        name = "DebugDraw", type = "bool", value = false, perAnimSet = false,
        helptext = "If set, then debug lines will be drawn in the viewport.\n\nYellow = Barrel position & Orientation, Red = Arm hinge axes, Cyan = Stock position, Green = Brace Joint\n\nIn the Preview Script an update handler will need to call viewport.debugDraw.update() to see the debug information."
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
        if not isValid(inputNode) then
          return nil, string.format("GunAimIK node %s requires a valid input node", node)
        end
      else
        return nil, string.format("GunAimIK node %s is missing a required connection to Source", node)
      end

      -- Validate Rig indices
      -- When this node supports non-sequential IK chains, this needs to be expanded
      local animSets = listAnimSets()
      for asIdx, asVal in animSets do

        local spineRootName = getAttribute(node, "SpineRootJointName", asVal)
        local spineRootIndex = nil
        if spineRootName ~= nil then
          spineRootIndex = anim.getRigChannelIndex(spineRootName, asVal)
        end

        local gunJointName = getAttribute(node, "GunJointName", asVal)
        local gunJointIndex = nil
        if gunJointName ~= nil then
          gunJointIndex = anim.getRigChannelIndex(gunJointName, asVal)
        end

        local gunBindJointName = getAttribute(node, "GunBindJointName", asVal)
        local gunBindJointIndex = nil
        if gunBindJointName ~= nil then
          gunBindJointIndex = anim.getRigChannelIndex(gunBindJointName, asVal)
        end

        local secondaryWristIndex = nil
        if getAttribute(node, "UseSecondaryArm", asVal) == true then
          local secondaryWristName = getAttribute(node, "SecondaryWristJointName", asVal)
          if secondaryWristName ~= nil then
            secondaryWristIndex = anim.getRigChannelIndex(secondaryWristName, asVal)
          end
        end

        local primaryWristName = getAttribute(node, "PrimaryWristJointName", asVal)
        local primaryWristIndex = nil
        if primaryWristName ~= nil then
          primaryWristIndex = anim.getRigChannelIndex(primaryWristName, asVal)
        end

        local rigSize = anim.getRigSize(asVal)

        if spineRootIndex == nil or spineRootIndex <= 0 or spineRootIndex >= rigSize then
          return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. ") requires a valid SpineRootJointName")
        end

        if (gunJointIndex == nil or gunJointIndex <= 0 or gunJointIndex >= rigSize) then
          return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. ") requires a valid GunJointName")
        end

        if (gunBindJointIndex == nil or gunBindJointIndex <= 0 or gunBindJointIndex >= rigSize) then
          return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. ") requires a valid GunBindJointName")
        end

        if primaryWristIndex == nil or primaryWristIndex <= 0 or primaryWristIndex >= rigSize then
          return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. ") requires a valid PrimaryWristJointName")
        end

        local numParents = 0
        while primaryWristIndex ~= spineRootIndex do
          if primaryWristIndex == nil or primaryWristIndex <= 0 or primaryWristIndex >= rigSize then
            return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. "): the spine root joint is not an ancestor of the primary wrist joint")
          end
          primaryWristIndex = anim.getParentBoneIndex(primaryWristIndex, asVal)
          numParents = numParents + 1
        end
        if numParents < 2 then
          return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. "): the primary wrist joint must have at least two parent joints up to and including the spine root")
        end

        local useSecondaryArm = getAttribute(node, "UseSecondaryArm", asVal)
        if useSecondaryArm == true then
          if secondaryWristIndex == nil or secondaryWristIndex <= 0 or secondaryWristIndex >= rigSize then
            return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. ") requires a valid SecondaryWristJointName")
          end

          numParents = 0
          while secondaryWristIndex ~= spineRootIndex do
            if secondaryWristIndex == nil or secondaryWristIndex <= 0 or secondaryWristIndex >= rigSize then
              return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. "): the spine root joint is not an ancestor of the secondary wrist joint")
            end
            secondaryWristIndex = anim.getParentBoneIndex(secondaryWristIndex, asVal)
            numParents = numParents + 1
          end
          if numParents < 2 then
            return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. "): the secondary wrist joint must have at least two parent joints up to and including the spine root")
          end
        end

        while gunBindJointIndex ~= spineRootIndex do
          if gunBindJointIndex == nil or gunBindJointIndex <= 0 or gunBindJointIndex >= rigSize then
            return nil, ("GunAimIK node " .. node .. " (animset " .. asVal .. "): the spine root joint is not an ancestor of the gun bind joint")
          end
          gunBindJointIndex = anim.getParentBoneIndex(gunBindJointIndex, asVal)
        end
      end

      local targetPin = string.format("%s.Target", node)
      if isConnected{ SourcePin = targetPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = targetPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("GunAimIK node " .. node .. " requires a valid Aim-At Target")
        end
      else
        return nil, ("GunAimIK node " .. node .. " is missing a required connection to Target")
      end

      local blendWeightPin = string.format("%s.BlendWeight", node)
      if isConnected{ SourcePin = blendWeightPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = blendWeightPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if isValid(inputNode) ~= true then
          return nil, ("GunAimIK node " .. node .. " requires a valid FK to IK Blend Weight")
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
      
      local keepUpright = getAttribute(node, "KeepUpright")
      Stream:writeBool(keepUpright, "KeepUpright")

      local applyJointLimits = getAttribute(node, "ApplyJointLimits")
      Stream:writeBool(applyJointLimits, "ApplyJointLimits")

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
      for asIdx, asVal in ipairs(animSets) do
        local spineRootJointName = getAttribute(node, "SpineRootJointName", asVal)
        local spineRootJointIndex = anim.getRigChannelIndex(spineRootJointName, asVal)
        Stream:writeUInt(spineRootJointIndex, string.format("SpineRootJointIndex_%d", asIdx))

        local gunJointName = getAttribute(node, "GunJointName", asVal)
        local gunJointIndex = anim.getRigChannelIndex(gunJointName, asVal)
        Stream:writeUInt(gunJointIndex, string.format("GunJointIndex_%d", asIdx))

        local gunBindJointName = getAttribute(node, "GunBindJointName", asVal)
        local gunBindJointIndex = anim.getRigChannelIndex(gunBindJointName, asVal)
        Stream:writeUInt(gunBindJointIndex, string.format("GunBindJointIndex_%d", asIdx))

        local gunPivotOffsetX = getAttribute(node, "GunPivotOffsetX", asVal)
        gunPivotOffsetX = mcn.convertWorldToPhysics(gunPivotOffsetX)
        Stream:writeFloat(gunPivotOffsetX, string.format("GunPivotOffsetX_%d", asIdx))

        local gunPivotOffsetY = getAttribute(node, "GunPivotOffsetY", asVal)
        gunPivotOffsetY = mcn.convertWorldToPhysics(gunPivotOffsetY)
        Stream:writeFloat(gunPivotOffsetY, string.format("GunPivotOffsetY_%d", asIdx))

        local gunPivotOffsetZ = getAttribute(node, "GunPivotOffsetZ", asVal)
        gunPivotOffsetZ = mcn.convertWorldToPhysics(gunPivotOffsetZ)
        Stream:writeFloat(gunPivotOffsetZ, string.format("GunPivotOffsetZ_%d", asIdx))

        local gunBarrelOffsetX = getAttribute(node, "GunBarrelOffsetX", asVal)
        gunBarrelOffsetX = mcn.convertWorldToPhysics(gunBarrelOffsetX)
        Stream:writeFloat(gunBarrelOffsetX, string.format("GunBarrelOffsetX_%d", asIdx))

        local gunBarrelOffsetY = getAttribute(node, "GunBarrelOffsetY", asVal)
        gunBarrelOffsetY = mcn.convertWorldToPhysics(gunBarrelOffsetY)
        Stream:writeFloat(gunBarrelOffsetY, string.format("GunBarrelOffsetY_%d", asIdx))

        local gunBarrelOffsetZ = getAttribute(node, "GunBarrelOffsetZ", asVal)
        gunBarrelOffsetZ = mcn.convertWorldToPhysics(gunBarrelOffsetZ)
        Stream:writeFloat(gunBarrelOffsetZ, string.format("GunBarrelOffsetZ_%d", asIdx))

        local gunPointingVectorX = getAttribute(node, "GunPointingVectorX", asVal)
        Stream:writeFloat(gunPointingVectorX, string.format("GunPointingVectorX_%d", asIdx))

        local gunPointingVectorY = getAttribute(node, "GunPointingVectorY", asVal)
        Stream:writeFloat(gunPointingVectorY, string.format("GunPointingVectorY_%d", asIdx))

        local gunPointingVectorZ = getAttribute(node, "GunPointingVectorZ", asVal)
        Stream:writeFloat(gunPointingVectorZ, string.format("GunPointingVectorZ_%d", asIdx))

        local primaryWristJointName = getAttribute(node, "PrimaryWristJointName", asVal)
        local primaryWristJointIndex = anim.getRigChannelIndex(primaryWristJointName, asVal)
        Stream:writeUInt(primaryWristJointIndex, string.format("PrimaryWristJointIndex_%d", asIdx))

        local primaryArmHingeAxisX = getAttribute(node, "PrimaryArmHingeAxisX", asVal)
        Stream:writeFloat(primaryArmHingeAxisX, string.format("PrimaryArmHingeAxisX_%d", asIdx))

        local primaryArmHingeAxisY = getAttribute(node, "PrimaryArmHingeAxisY", asVal)
        Stream:writeFloat(primaryArmHingeAxisY, string.format("PrimaryArmHingeAxisY_%d", asIdx))

        local primaryArmHingeAxisZ = getAttribute(node, "PrimaryArmHingeAxisZ", asVal)
        Stream:writeFloat(primaryArmHingeAxisZ, string.format("PrimaryArmHingeAxisZ_%d", asIdx))

        local flipPrimaryHinge = getAttribute(node, "FlipPrimaryHinge", asVal)
        Stream:writeBool(flipPrimaryHinge, string.format("FlipPrimaryHinge_%d", asIdx))

        local useSecondaryArm = getAttribute(node, "UseSecondaryArm", asVal)
        Stream:writeBool(useSecondaryArm, string.format("UseSecondaryArm_%d", asIdx))

        if useSecondaryArm then
          local secondaryWristJointName = getAttribute(node, "SecondaryWristJointName", asVal)
          local secondaryWristJointIndex = anim.getRigChannelIndex(secondaryWristJointName, asVal)
          Stream:writeUInt(secondaryWristJointIndex, string.format("SecondaryWristJointIndex_%d", asIdx))

          local secondaryArmHingeAxisX = getAttribute(node, "SecondaryArmHingeAxisX", asVal)
          Stream:writeFloat(secondaryArmHingeAxisX, string.format("SecondaryArmHingeAxisX_%d", asIdx))

          local secondaryArmHingeAxisY = getAttribute(node, "SecondaryArmHingeAxisY", asVal)
          Stream:writeFloat(secondaryArmHingeAxisY, string.format("SecondaryArmHingeAxisY_%d", asIdx))

          local secondaryArmHingeAxisZ = getAttribute(node, "SecondaryArmHingeAxisZ", asVal)
          Stream:writeFloat(secondaryArmHingeAxisZ, string.format("SecondaryArmHingeAxisZ_%d", asIdx))

          local flipSecondaryHinge = getAttribute(node, "FlipSecondaryHinge", asVal)
          Stream:writeBool(flipSecondaryHinge, string.format("FlipSecondaryHinge_%d", asIdx))
        end

        local spinebias = getAttribute(node, "SpineBias", asVal)
        Stream:writeFloat(spinebias, string.format("SpineBias_%d", asIdx))

        local updateTargetByDeltas = getAttribute(node, "UpdateTargetByDeltas", asVal)
        Stream:writeBool(updateTargetByDeltas, string.format("UpdateTargetByDeltas_%d", asIdx))

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
      --  First, add channels from the brace joint to root joint chain
      local endJointName = getAttribute(node, "GunBindJointName", set)
      local endJointIndex = anim.getRigChannelIndex(endJointName, set)
      local rootJointName = getAttribute(node, "SpineRootJointName", set)
      local rootJointIndex = anim.getRigChannelIndex(rootJointName, set)
      local j = endJointIndex
      repeat
        reverseMap[j] = j
        j = anim.getParentBoneIndex(j, set)
      until j < rootJointIndex

      --  Second, add channels from the primary arm
      endJointName = getAttribute(node, "PrimaryWristJointName", set)
      endJointIndex = anim.getRigChannelIndex(endJointName, set)
      local midJointIndex = anim.getParentBoneIndex(endJointIndex, set)
      rootJointIndex = anim.getParentBoneIndex(midJointIndex, set)
      reverseMap[endJointIndex] = endJointIndex
      reverseMap[midJointIndex] = midJointIndex
      reverseMap[rootJointIndex] = rootJointIndex

      --  Finally, add channels from the secondary arm
      if getAttribute(node, "UseSecondaryArm", set) then
        endJointName = getAttribute(node, "SecondaryWristJointName", set)
        endJointIndex = anim.getRigChannelIndex(endJointName, set)
        midJointIndex = anim.getParentBoneIndex(endJointIndex, set)
        rootJointIndex = anim.getParentBoneIndex(midJointIndex, set)
        reverseMap[endJointIndex] = endJointIndex
        reverseMap[midJointIndex] = midJointIndex
        reverseMap[rootJointIndex] = rootJointIndex
      end

      -- Re-reverse to get output channels.  Sort to keep neat numerical order.
      local outputChannels = { }
      for jointIndex, val in reverseMap do
        outputChannels[jointIndex] = true
      end

      return outputChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      local attrs = { }
      if version < 4 then
        attrs["LeftWristJointName"] = "SecondaryWristJointName"
        attrs["LeftArmHingeAxisX"] = "SecondaryArmHingeAxisX"
        attrs["LeftArmHingeAxisY"] = "SecondaryArmHingeAxisY"
        attrs["LeftArmHingeAxisZ"] = "SecondaryArmHingeAxisZ"
        attrs["FlipLeftHinge"] = "FlipSecondaryHinge"
        attrs["RightWristJointName"] = "PrimaryWristJointName"
        attrs["RightArmHingeAxisX"] = "PrimaryArmHingeAxisX"
        attrs["RightArmHingeAxisY"] = "PrimaryArmHingeAxisY"
        attrs["RightArmHingeAxisZ"] = "PrimaryArmHingeAxisZ"
        attrs["FlipRightHinge"] = "FlipPrimaryHinge"
      end

      upgradeRenamedAttributes(node, attrs)

      if version < 3 then
        local sourcePath = string.format("%s.Source", node)
        setPinPassThrough(sourcePath, true)

        local resultPath = string.format("%s.Result", node)
        setPinPassThrough(resultPath, true)
      elseif version < 5 then
        local oldSourcePath = string.format("%s.In", node)
        local newSourcePath = string.format("%s.Source", node)
        pinLookupTable[oldSourcePath] = newSourcePath

        local oldResultPath = string.format("%s.Out", node)
        local newResultPath = string.format("%s.Result", node)
        pinLookupTable[oldResultPath] = newResultPath
      end

      if version < 6 then
        -- The following attributes were added in version 6
        --  KeepUpright
        --  ApplyJointLimits
        --  WorldSpaceTarget
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- GunAimIK custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "GunAimIK",
    {
      {
        title = "Gun",
        usedAttributes = {
          "GunJointName",
          "GunBindJointName",
          "GunPivotOffsetX",
          "GunPivotOffsetY",
          "GunPivotOffsetZ",
          "GunBarrelOffsetX",
          "GunBarrelOffsetY",
          "GunBarrelOffsetZ",
          "GunPointingVectorX",
          "GunPointingVectorY",
          "GunPointingVectorZ",
        },
        displayFunc = function(...) safefunc(attributeEditor.gunAimIkGunDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Body",
        usedAttributes = { "SpineRootJointName", "SpineBias" },
        displayFunc = function(...) safefunc(attributeEditor.gunAimIkBodyDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Primary Arm",
        usedAttributes = {
          "PrimaryWristJointName",
          "PrimaryArmHingeAxisX",
          "PrimaryArmHingeAxisY",
          "PrimaryArmHingeAxisZ",
          "FlipPrimaryHinge" },
        displayFunc = function(...) safefunc(attributeEditor.gunAimIkArmDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Secondary Arm",
        usedAttributes = {
          "UseSecondaryArm",
          "SecondaryWristJointName",
          "SecondaryArmHingeAxisX",
          "SecondaryArmHingeAxisY",
          "SecondaryArmHingeAxisZ",
          "FlipSecondaryHinge" },
        displayFunc = function(...) safefunc(attributeEditor.gunAimIkArmDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = { "UpdateTargetByDeltas" },
        displayFunc = function(...) safefunc(attributeEditor.gunAimIkPropertiesDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "General",
        usedAttributes = { "KeepUpright", "ApplyJointLimits", "WorldSpaceTarget" },
        displayFunc = function(...) safefunc(attributeEditor.gunAimIkGeneralDisplayInfoSection, unpack(arg)) end
      },
    }
  )

  previewScript.debugData.registerProvider(
    "GunAimIK",
    function(self)
      if getAttribute(self, "DebugDraw") then
        local animSets = listAnimSets()

        -- we scale the offset into the runtime unit scale
        local runtimeAssetScaleFactor = 1 / preferences.get("RuntimeAssetScaleFactor")
        
        for asIdx, asVal in ipairs(animSets) do
          local hier = anim.getRigHierarchy(asVal)
          local gunJointName = getAttribute(self, "GunJointName", asVal)
          local gunJointIndex = anim.getRigChannelIndex(gunJointName, asVal)

          -- stock pos
          do
            local offset = { }
            offset.x = getAttribute(self, "GunPivotOffsetX")
            offset.y = getAttribute(self, "GunPivotOffsetY")
            offset.z = getAttribute(self, "GunPivotOffsetZ")
            local colour= { r = 0, g = 255, b = 128 }
            local scale = 0.1
            previewScript.debugData.addPoint(self, asVal, gunJointIndex, offset, scale, colour)
          end

          -- brace bone
          do
            local braceJointName = getAttribute(self, "GunBindJointName", asVal)
            local braceJointIndex = anim.getRigChannelIndex(braceJointName, asVal)
            local offset = { x = 0, y = 0, z = 0 }
            local colour= { r = 0, g = 255, b = 0 }
            local scale = 0.05
            previewScript.debugData.addPoint(self, asVal, braceJointIndex, offset, scale, colour)
          end

          -- barrel pos + vector
          do
            local offset = { }
            offset.x = getAttribute(self, "GunBarrelOffsetX")
            offset.y = getAttribute(self, "GunBarrelOffsetY")
            offset.z = getAttribute(self, "GunBarrelOffsetZ")
            local colour= { r = 255, g = 255, b = 0 }
            local scale = 0.1
            previewScript.debugData.addPoint(self, asVal, gunJointIndex, offset, scale, colour)
          end

          do
            local offset = { }
            local vector = { }
            offset.x = getAttribute(self, "GunBarrelOffsetX")
            offset.y = getAttribute(self, "GunBarrelOffsetY")
            offset.z = getAttribute(self, "GunBarrelOffsetZ")
            
            vector.x = getAttribute(self, "GunPointingVectorX")
            vector.y = getAttribute(self, "GunPointingVectorY")
            vector.z = getAttribute(self, "GunPointingVectorZ")
            
            -- make the vector about 1m in runtime asset units (so 100 cm if the runtime uses centimeters)
            local length = math.sqrt( vector.x * vector.x + vector.y * vector.y + vector.z * vector.z )
            vector.x = (vector.x / length) * runtimeAssetScaleFactor;
            vector.y = (vector.y / length) * runtimeAssetScaleFactor;
            vector.z = (vector.z / length) * runtimeAssetScaleFactor;
          
            local colour= { r = 255, g = 255, b = 0 }
            previewScript.debugData.addLine(self, asVal, gunJointIndex, offset, vector, colour)
          end

          -- Primary Arm hinge axis
          do
            local wristName = getAttribute(self, "PrimaryWristJointName", asVal)
            local wristIndex = anim.getRigChannelIndex(wristName, asVal)
            local hingeIndex = -1
            local shoulderIndex = -1

            -- find the hinge, the parent of the primary arm's wrist
            for _, v in ipairs(hier) do
              if v.index == wristIndex then
                hingeIndex = v.parentIndex
                break
              end
            end

            -- We have a valid parent joint for primary hand
            if hingeIndex >= 0 then

              -- find the shoulder, the parent of the primary arm's hinge
              for _, v in ipairs(hier) do
                if v.index == hingeIndex then
                  shoulderIndex = v.parentIndex
                  break
                end
              end

              -- We have a valid shoulder joint for primary arm
              if shoulderIndex >= 0 then
                local vector = { }
                vector.x = getAttribute(self, "PrimaryArmHingeAxisX")
                vector.y = getAttribute(self, "PrimaryArmHingeAxisY")
                vector.z = getAttribute(self, "PrimaryArmHingeAxisZ")
                
                -- make the vector about 1m in runtime asset units (so 100 cm if the runtime uses centimeters)
                local length = math.sqrt( vector.x * vector.x + vector.y * vector.y + vector.z * vector.z )
                vector.x = (vector.x / length) * runtimeAssetScaleFactor;
                vector.y = (vector.y / length) * runtimeAssetScaleFactor;
                vector.z = (vector.z / length) * runtimeAssetScaleFactor;
            
                local colour= { r = 128, g = 0, b = 0 }
                previewScript.debugData.addLine(self, asVal, shoulderIndex, hingeIndex, vector, colour)
              end

            end
          end

          -- Secondary Arm hinge axis
          if getAttribute(self, "UseSecondaryArm", asVal) then
            local wristName = getAttribute(self, "SecondaryWristJointName", asVal)
            local wristIndex = anim.getRigChannelIndex(wristName, asVal)
            local hingeIndex = -1
            local shoulderIndex = -1

            -- find the hinge, the parent of the secondary arm's wrist
            for _, v in ipairs(hier) do
              if v.index == wristIndex then
                hingeIndex = v.parentIndex
                break
              end
            end

            -- We have a valid parent joint for secondary hand
            if hingeIndex >= 0 then

              -- find the shoulder, the parent of the secondary arm's hinge
              for _, v in ipairs(hier) do
                if v.index == hingeIndex then
                  shoulderIndex = v.parentIndex
                  break
                end
              end

              -- We have a valid shoulder joint for secondary arm
              if shoulderIndex >= 0 then
                local vector = { }
                vector.x = getAttribute(self, "SecondaryArmHingeAxisX")
                vector.y = getAttribute(self, "SecondaryArmHingeAxisY")
                vector.z = getAttribute(self, "SecondaryArmHingeAxisZ")
                
                -- make the vector about 1m in runtime asset units (so 100 cm if the runtime uses centimeters)
                local length = 4 * math.sqrt( vector.x * vector.x + vector.y * vector.y + vector.z * vector.z )
                vector.x = (vector.x / length) * runtimeAssetScaleFactor;
                vector.y = (vector.y / length) * runtimeAssetScaleFactor;
                vector.z = (vector.z / length) * runtimeAssetScaleFactor;
                
                local colour= { r = 128, g = 0, b = 0 }
                previewScript.debugData.addLine(self, asVal, shoulderIndex, hingeIndex, vector, colour)
              end

            end
          end

        end -- Anim Set loop
      end -- If DebugDraw is true
    end -- Debug Draw function
  )
end

