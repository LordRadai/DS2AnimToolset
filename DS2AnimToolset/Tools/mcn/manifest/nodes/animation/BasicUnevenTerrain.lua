------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/BasicUnevenTerrainDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- BasicUnevenTerrain node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("BasicUnevenTerrain",
  {
    displayName = "Basic Uneven Terrain",
    helptext = "Basic Uneven Terrain: Lifts and aligns the feet of the character with the environment.",
    group = "IK",
    image = "BasicUnevenTerrain.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 136),
    version = 5,

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
        ["IkHipsWeight"] = {
          input = true,
          array = false,
          type = "float",
        },
        ["IkFkBlendWeight"] = {
          input = true,
          array = false,
          type = "float",
        }
    },

    pinOrder = { "Source", "Result", "IkHipsWeight", "IkFkBlendWeight" },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ---------------------------
      -- HIPS ATTRIBUTES
      {
        name = "HipsName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The root-most joint that is allowed to translate that is used for character height control. Height control increases the range of motion of the leg IK, allowing it to adapt better to uneven surfaces."
      },

      {
        name = "HipsHeightControlEnable", type = "bool", value = true, perAnimSet = true,
        helptext = "Enables character height control."
      },
      {
        name = "HipsPosVelLimitEnable", type = "bool", value = true, perAnimSet = true,
        helptext = "Enables velocity clamping of the vertical character height control."
      },
      {
        name = "HipsPosVelLimit", type = "float", value = 0.6, min = 0.0, perAnimSet = true,
        helptext = "The velocity limit of the character height control (in leg lengths per second)."
      },

      {
        name = "HipsPosAccelLimitEnable", type = "bool", value = true, perAnimSet = true,
        helptext = "Enables acceleration clamping of the vertical character height control motion."
      },
      {
        name = "HipsPosAccelLimit", type = "float", value = 40.0, min = 0.0, perAnimSet = true,
        helptext = "The acceleration limit of the character height control (in leg lengths per second^2)."
      },

      ---------------------------
      -- SHARED LEG ATTRIBUTES
      {
        name = "BallJointEnable", type = "bool", value = false, perAnimSet = true,
        helptext = "Enables the use of a specified ball joint. The footbase pivot is determined from the centroid of the foot joints. This point is lifted onto the terrain surface."
      },
      {
        name = "ToeJointEnable", type = "bool", value = false, perAnimSet = true,
        helptext = "Enables the use of a specified ball joint. The footbase pivot is determined from the centroid of the foot joints. This point is lifted onto the terrain surface."
      },
      {
        name = "StraightestLegFactor", type = "float", value = 0.98, perAnimSet = true,
        helptext = "How straight the leg is allowed to become, as a fraction of the fully straightened (hip, knee and ankle all lined up). When the foot lifting target is out of reach, the leg will go straight and this can look unnatural."
      },

      ---------------------------
      -- LEFT LEG ATTRIBUTES

      {
        name = "LeftAnkleName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The name of the left ankle joint."
      },
      {
        name = "LeftBallName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "An optional joint to specify the ball of the foot. The footbase pivot is determined from the centroid of the foot joints. This point is lifted onto the terrain surface."
      },
      {
        name = "LeftToeName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "An optional joint to specify the toe of the foot. The footbase pivot is determined from the centroid of the foot joints. This point is lifted onto the terrain surface."
      },
      {
        name = "LeftKneeRotationAxisX", type = "float", value = 1, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "LeftKneeRotationAxisY", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "LeftKneeRotationAxisZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "LeftFlipKneeRotationDirection", type = "bool", value = false, perAnimSet = true,
        helptext = "Change the sign of the knee axis, which will flip its bending direction."
      },

      ---------------------------
      -- RIGHT LEG ATTRIBUTES

      {
        name = "RightAnkleName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "The name of the right ankle joint."
      },
      {
        name = "RightBallName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "An optional joint to specify the ball of the foot. The footbase pivot is determined from the centroid of the foot joints. This point is lifted onto the terrain surface."
      },
      {
        name = "RightToeName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "An optional joint to specify the toe of the foot. The footbase pivot is determined from the centroid of the foot joints. This point is lifted onto the terrain surface."
      },
      {
        name = "RightKneeRotationAxisX", type = "float", value = 1, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "RightKneeRotationAxisY", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "RightKneeRotationAxisZ", type = "float", value = 0, perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent."
      },
      {
        name = "RightFlipKneeRotationDirection", type = "bool", value = false, perAnimSet = true,
        helptext = "Change the sign of the knee axis, which will flip its bending direction."
      },

      ---------------------------
      -- LEGS LIMITS
      {
        name = "AnklePosVelLimitEnable", type = "bool", value = false, perAnimSet = true,
        helptext = "Enables velocity clamping for the vertical foot lifting of the ankle joint."
      },
      {
        name = "AnklePosVelLimit", type = "float", value = 1.0, min = 0.0, perAnimSet = true,
        helptext = "The velocity limit of the ankle joint foot lifting (in leg lengths per second)."
      },
      {
        name = "AnklePosAccelLimitEnable", type = "bool", value = true, perAnimSet = true,
        helptext = "Enables acceleration clamping for the vertical foot lifting of the ankle joint."
      },
      {
        name = "AnklePosAccelLimit", type = "float", value = 80.0, min = 0.0, perAnimSet = true,
        helptext = "The acceleration limit of the ankle joint foot lifting (in leg lengths per second^2)."
      },
      {
        name = "AnkleAngVelLimitEnable", type = "bool", value = false, perAnimSet = true,
        helptext = "Enables velocity clamping for the ankle joint surface orientation alignment."
      },
      {
        name = "AnkleAngVelLimit", type = "float", value = 1.0, min = 0.0, perAnimSet = true,
        helptext = "The velocity limit of the ankle joint surface orientation alignment (revolutions per second)."
      },
      {
        name = "AnkleAngAccelLimitEnable", type = "bool", value = true, perAnimSet = true,
        helptext = "Enables acceleration clamping for the ankle joint surface orientation alignment."
      },
      {
        name = "AnkleAngAccelLimit", type = "float", value = 50.0, min = 0.0, perAnimSet = true,
        helptext = "The acceleration limit of the ankle joint surface orientation alignment (revolutions per second^2)."
      },

      -- Foot alignment with the terrain surface options
      {
        name = "UseGroundPenetrationFixup", type = "bool", value = true, perAnimSet = true,
        helptext = "If turned on prevents the foot penetrating onto the surface if the foot is moving downward from above with large velocity."
      },
      {
        name = "UseTrajectorySlopeAlignment", type = "bool", value = true, perAnimSet = true,
        helptext = "Consider vertical trajectory motion of the input source when computing foot lifting and surface alignment."
      },
      {
        name = "FootAlignToSurfaceAngleLimit", type = "float", value = 30.0, min = 0.0, max = 90.0, perAnimSet = true,
        helptext = "The maximum terrain surface angle limit (in degrees) that the foot can be aligned with. Foot alignment is clamped to this limit."
      },
      {
        name = "FootAlignToSurfaceMaxSlopeAngle", type = "float", value = 75.0, min = 0.0, max = 90.0, perAnimSet = true,
        helptext = "The maximum terrain slope gradient limit (in degrees) that is considered for foot alignment. The foot is aligned back to the ground plane if beyond this limit."
      },

      -- Foot lifting options
      {
        name = "FootLiftingHeightLimit", type = "float", value = 0.6, min = 0.0, max = 1.0, perAnimSet = true,
        helptext = "The maximum height from the character root to the ankle joint that the foot can be lifted to (in leg lengths)."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)

      ----------------------
      validateChain = function(node, inputNode, animSetName, jointNameAttr, rootNameAttr)

        -- Find the current anim set
        local animSets = listAnimSets()
        local animSetIndex = nil;
        for asIdx, asVal in animSets do
          if asVal == animSetName then
            animSetIndex = asIdx
            break
          end
        end
        assert(animSetIndex, string.format("Unable to find animation set %s", animSetName))

        local rigSize = anim.getRigSize(animSetName)

        -- Is the root joint channel valid
        local rootJointName = getAttribute(node, rootNameAttr, animSetName)
        local rootJointIndex = nil
        if rootJointName ~= nil then
          rootJointIndex = anim.getRigChannelIndex(rootJointName, animSetName)
        end
        if rootJointIndex == nil or rootJointIndex <= 0 or rootJointIndex >= rigSize then
          return nil, ("BasicUnevenTerrain node " .. node .. " (animset " .. animSetName .. ") requires a valid " .. rootNameAttr)
        end

        -- Is the joint channel valid
        local jointName = getAttribute(node, jointNameAttr, animSetName)
        local jointIndex = nil
        if jointName ~= nil then
          jointIndex = anim.getRigChannelIndex(jointName, animSetName)
        end
        if jointIndex == nil or jointIndex <= 0 or jointIndex >= rigSize then
          return nil, ("BasicUnevenTerrain node " .. node .. " (animset " .. animSetName .. ") requires a valid " .. jointNameAttr)
        end

        -- Validate the channels between the joints in the IK chain
        while jointIndex ~= rootJointIndex do

          -- Is valid channel
          if jointIndex == nil or jointIndex <= 0 or jointIndex >= rigSize then
            return nil, ("BasicUnevenTerrain node " .. node .. " (animset " .. animSetName .. "): the root joint '" .. rootJointName .. "' is not an ancestor of the '" .. jointName .. "' joint")
          end

          -- Update
          jointIndex = anim.getParentBoneIndex(jointIndex, animSetName)
        end

        return true
      end

      ----------------------
      local inputNode = nil;

      -- Validate connections
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = sourcePin, ResolveReferences = true }
        inputNode = nodesConnected[1]
        if not isValid(inputNode) then
          return nil, string.format("BasicUnevenTerrain node %s requires a valid input node", node)
        end
      else
        return nil, string.format("BasicUnevenTerrain node %s is missing a required connection to Source", node)
      end

      ---------------------------
      -- Validate Rig indices
      -- When this node supports non-sequential IK chains, this needs to be expanded
      local animSets = listAnimSets()
      for asIdx, asVal in animSets do

        local rigSize = anim.getRigSize(asVal)

        -- Foot IK model
        local ballJointEnable = getAttribute(node, "BallJointEnable", asVal)
        local toeJointEnable = getAttribute(node, "ToeJointEnable", asVal)

        ---------------------------
        -- HIPS
        local hipsName = getAttribute(node, "HipsName", asVal)

        -- Is the hips channel valid
        local hipsIndex = nil
        if hipsName ~= nil then
          hipsIndex = anim.getRigChannelIndex(hipsName, asVal)
        end
        if hipsIndex == nil or hipsIndex <= 0 or hipsIndex >= rigSize then
          return nil, ("BasicUnevenTerrain node " .. node .. " (animset " .. asVal .. ") requires a valid HipsName")
        end

        -- predeclare these for use below
        local result, msg

        ---------------------------
        -- LEFT LEG
        result, msg = validateChain(node, inputNode, asVal, "LeftAnkleName", "HipsName")
        if result == nil then
          return result, msg
        end

        if ballJointEnable then
          result, msg = validateChain(node, inputNode, asVal, "LeftBallName", "LeftAnkleName")
          if result == nil then
            return result, msg
          end
        end

        if toeJointEnable then
          result, msg = validateChain(node, inputNode, asVal, "LeftToeName", "LeftAnkleName")
          if result == nil then
            return result, msg
          end
        end

        ---------------------------
        -- RIGHT LEG
        result, msg = validateChain(node, inputNode, asVal, "RightAnkleName", "HipsName")
        if result == nil then
          return result, msg
        end

        if ballJointEnable then
          result, msg = validateChain(node, inputNode, asVal, "RightBallName", "RightAnkleName")
          if result == nil then
            return result, msg
          end
        end

        if toeJointEnable then
          result, msg = validateChain(node, inputNode, asVal, "RightToeName", "RightAnkleName")
          if result == nil then
            return result, msg
          end
        end

      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)

      ----------------------
      serializeLeg = function(node, Stream, legType)

        local animSets = listAnimSets()
        for asIdx, asVal in animSets do

          ----------------------
          -- IK Chain options

          -- ANKLE
          local ankleName = getAttribute(node, string.format("%sAnkleName", legType), asVal)
          local ankleIndex = anim.getRigChannelIndex(ankleName, asVal)
          if ankleIndex == nil then
            ankleIndex = -1
          end
          Stream:writeUInt(ankleIndex, string.format("%sAnkleIndex_", legType) .. asIdx)

          -- calculate knee and hips as parent and grandparent of ankle joint
          local hierarchy = anim.getRigHierarchy(asVal)

          -- KNEE
          -- find the knee, the parent of the primary ankle
          local kneeIndex = -1
          for _, v in ipairs(hierarchy) do
            if v.index == ankleIndex then
              kneeIndex = v.parentIndex
              break
            end
          end
          Stream:writeUInt(kneeIndex, string.format("%sKneeIndex_", legType) .. asIdx)

          -- HIP
          -- find the leg hip joint as the parent of the knee joint
          local hipIndex = -1
          for _, v in ipairs(hierarchy) do
            if v.index == kneeIndex then
              hipIndex = v.parentIndex
              break
            end
          end
          Stream:writeUInt(hipIndex, string.format("%sHipIndex_", legType) .. asIdx)

          -- BALL
          local ballIndex = -1
          local ballJointEnable = getAttribute(node, "BallJointEnable", asVal)
          if ballJointEnable then
            local ballName = getAttribute(node, string.format("%sBallName", legType), asVal)
            ballIndex = anim.getRigChannelIndex(ballName, asVal)
            if ballIndex == nil then
              ballIndex = -1
            end
          end
          Stream:writeUInt(ballIndex, string.format("%sBallIndex_", legType) .. asIdx)

          -- TOE
          local toeIndex = -1
          local toeJointEnable = getAttribute(node, "ToeJointEnable", asVal)
          if toeJointEnable then
            local toeName = getAttribute(node, string.format("%sToeName", legType), asVal)
            toeIndex = anim.getRigChannelIndex(toeName, asVal)
            if toeIndex == nil then
              toeIndex = -1
            end
          end
          Stream:writeUInt(toeIndex, string.format("%sToeIndex_", legType) .. asIdx)

          -- KNEE ROTATION AXIS
          local kneeRotationAxisX = getAttribute(node, string.format("%sKneeRotationAxisX", legType), asVal)
          Stream:writeFloat(kneeRotationAxisX, string.format("%sKneeRotationAxisX_", legType) .. asIdx)

          local kneeRotationAxisY = getAttribute(node, string.format("%sKneeRotationAxisY", legType), asVal)
          Stream:writeFloat(kneeRotationAxisY, string.format("%sKneeRotationAxisY_", legType) .. asIdx)

          local kneeRotationAxisZ = getAttribute(node, string.format("%sKneeRotationAxisZ", legType), asVal)
          Stream:writeFloat(kneeRotationAxisZ, string.format("%sKneeRotationAxisZ_", legType) .. asIdx)

          local flipKneeRotationDirection = getAttribute(node, string.format("%sFlipKneeRotationDirection", legType), asVal)
          Stream:writeBool(flipKneeRotationDirection, string.format("%sFlipKneeRotationDirection_", legType) .. asIdx)

        end
      end

      ----------------------
      local sourceID = -1      
      local iKHipsWeightNodeInfo = nil
      local iKFkBlendWeightNodeInfo = nil

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
        sourceID = getConnectedNodeID(sourcePin)
      end     

      if isConnected{ SourcePin = (node .. ".IkHipsWeight"), ResolveReferences = true } then
        iKHipsWeightNodeInfo = getConnectedNodeInfo(node, "IkHipsWeight")
      end

      if isConnected{ SourcePin = (node .. ".IkFkBlendWeight"), ResolveReferences = true } then
        iKFkBlendWeightNodeInfo = getConnectedNodeInfo(node, "IkFkBlendWeight")
      end

      -- PIN CONNECTIONS
      Stream:writeNetworkNodeId(sourceID, "InputNodeID")

      if iKHipsWeightNodeInfo then
        Stream:writeNetworkNodeId(iKHipsWeightNodeInfo.id, "IkHipsWeight", iKHipsWeightNodeInfo.pinIndex)
      else
        Stream:writeNetworkNodeId(-1, "IkHipsWeight")
      end

      if iKFkBlendWeightNodeInfo then
        Stream:writeNetworkNodeId(iKFkBlendWeightNodeInfo.id, "IkFkBlendWeight", iKFkBlendWeightNodeInfo.pinIndex)
      else
        Stream:writeNetworkNodeId(-1, "IkFkBlendWeight")
      end

      -- Serialise world up axis as an index
      local worldUpAxis = preferences.get("WorldUpAxis")
      local upAxisIndex = 1
      if (worldUpAxis == "X Axis") then
        upAxisIndex = 0
      elseif (worldUpAxis == "Y Axis") then
        upAxisIndex = 1
      elseif (worldUpAxis == "Z Axis") then
        upAxisIndex = 2
      end
      Stream:writeUInt(upAxisIndex, "UpAxisIndex")

      ----------------------
      local animSets = listAnimSets()
      for asIdx, asVal in animSets do

        ----------------------
        -- HIP ATTRIBUTES

        -- HipsName
        local hipsName = getAttribute(node, "HipsName", asVal)
        local hipsIndex = anim.getRigChannelIndex(hipsName, asVal)
        if hipsIndex == nil then
          hipsIndex = -1
        end
        Stream:writeUInt(hipsIndex, "HipsIndex_"..asIdx)

        -- HipsHeightControlEnable
        local hipsHeightControlEnable = getAttribute(node, "HipsHeightControlEnable", asVal)
        Stream:writeBool(hipsHeightControlEnable, "HipsHeightControlEnable_" .. asIdx)

        -- HipsPosVelLimitEnable
        local hipsPosVelLimitEnable = getAttribute(node, "HipsPosVelLimitEnable", asVal)
        Stream:writeBool(hipsPosVelLimitEnable, "HipsPosVelLimitEnable_" .. asIdx)

        -- HipsPosVelLimit
        local hipsPosVelLimit = getAttribute(node, "HipsPosVelLimit", asVal)
        Stream:writeFloat(hipsPosVelLimit, "HipsPosVelLimit_" .. asIdx)

        -- HipsPosAccelLimitEnable
        local hipsPosAccelLimitEnable = getAttribute(node, "HipsPosAccelLimitEnable", asVal)
        Stream:writeBool(hipsPosAccelLimitEnable, "HipsPosAccelLimitEnable_" .. asIdx)

        -- HipsPosAccelLimit
        local hipsPosAccelLimit = getAttribute(node, "HipsPosAccelLimit", asVal)
        Stream:writeFloat(hipsPosAccelLimit, "HipsPosAccelLimit_" .. asIdx)

        ----------------------
        -- Leg Limits

        -- StraightestLegFactor
        local straightestLegFactor = getAttribute(node, "StraightestLegFactor", asVal)
        Stream:writeFloat(straightestLegFactor, "StraightestLegFactor_" .. asIdx)

        -- AnklePosVelLimitEnable
        local anklePosVelLimitEnable = getAttribute(node, "AnklePosVelLimitEnable", asVal)
        Stream:writeBool(anklePosVelLimitEnable, "AnklePosVelLimitEnable_" .. asIdx)

        -- AnklePosVelLimit
        local anklePosVelLimit = getAttribute(node, "AnklePosVelLimit", asVal)
        Stream:writeFloat(anklePosVelLimit, "AnklePosVelLimit_" .. asIdx)

        -- AnklePosAccelLimitEnable
        local anklePosAccelLimitEnable = getAttribute(node, "AnklePosAccelLimitEnable", asVal)
        Stream:writeBool(anklePosAccelLimitEnable, "AnklePosAccelLimitEnable_" .. asIdx)

        -- AnklePosAccelLimit
        local anklePosAccelLimit = getAttribute(node, "AnklePosAccelLimit", asVal)
        Stream:writeFloat(anklePosAccelLimit, "AnklePosAccelLimit_" .. asIdx)

        -- AnkleAngVelLimitEnable
        local ankleAngVelLimitEnable = getAttribute(node, "AnkleAngVelLimitEnable", asVal)
        Stream:writeBool(ankleAngVelLimitEnable, "AnkleAngVelLimitEnable_" .. asIdx)

        -- AnkleAngVelLimit
        local ankleAngVelLimit = getAttribute(node, "AnkleAngVelLimit", asVal)
        Stream:writeFloat(ankleAngVelLimit, "AnkleAngVelLimit_" .. asIdx)

        -- AnkleAngAccelLimitEnable
        local ankleAngAccelLimitEnable = getAttribute(node, "AnkleAngAccelLimitEnable", asVal)
        Stream:writeBool(ankleAngAccelLimitEnable, "AnkleAngAccelLimitEnable_" .. asIdx)

        -- AnkleAngAccelLimit
        local ankleAngAccelLimit = getAttribute(node, "AnkleAngAccelLimit", asVal)
        Stream:writeFloat(ankleAngAccelLimit, "AnkleAngAccelLimit_" .. asIdx)

        ----------------------
        -- Foot alignment with the terrain surface options

        -- UseGroundPenetrationFixup
        local useGroundPenetrationFixup = getAttribute(node, "UseGroundPenetrationFixup", asVal)
        Stream:writeBool(useGroundPenetrationFixup, "UseGroundPenetrationFixup_" .. asIdx)

        -- UseTrajectorySlopeAlignment
        local useTrajectorySlopeAlignment = getAttribute(node, "UseTrajectorySlopeAlignment", asVal)
        Stream:writeBool(useTrajectorySlopeAlignment, "UseTrajectorySlopeAlignment_" .. asIdx)

        -- FootAlignToSurfaceAngleLimit
        local footAlignToSurfaceAngleLimit = getAttribute(node, "FootAlignToSurfaceAngleLimit", asVal)
        Stream:writeFloat(footAlignToSurfaceAngleLimit, "FootAlignToSurfaceAngleLimit_" .. asIdx)

        -- FootAlignToSurfaceMaxSlopeAngle
        local footAlignToSurfaceMaxSlopeAngle = getAttribute(node, "FootAlignToSurfaceMaxSlopeAngle", asVal)
        Stream:writeFloat(footAlignToSurfaceMaxSlopeAngle, "FootAlignToSurfaceMaxSlopeAngle_" .. asIdx)

        -- FootLiftingHeightLimit
        local footLiftingHeightLimit = getAttribute(node, "FootLiftingHeightLimit", asVal)
        Stream:writeFloat(footLiftingHeightLimit, "FootLiftingHeightLimit_" .. asIdx)
      end

      ----------------------
      -- LEG ATTRIBUTES
      serializeLeg(node, Stream, "Left")
      serializeLeg(node, Stream, "Right")
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
      for i, jointIndex in inputNodeChannels do
        reverseMap[jointIndex] = jointIndex
      end

      -- Add any channels output by this node that are not provided by the input
      --  First, add the hips, if present
      local hipsName = getAttribute(node, "HipsName", set)
      local hipsIndex = anim.getRigChannelIndex(hipsName, set)
      reverseMap[hipsIndex] = hipsIndex

      --  Second, add channels from left leg
      local endJointName = getAttribute(node, "LeftAnkleName", set)
      local endJointIndex = anim.getRigChannelIndex(endJointName, set)
      local midJointIndex = anim.getParentBoneIndex(endJointIndex, set)
      local rootJointIndex = anim.getParentBoneIndex(midJointIndex, set)
      reverseMap[endJointIndex] = endJointIndex
      reverseMap[midJointIndex] = midJointIndex
      reverseMap[rootJointIndex] = rootJointIndex

      --  Finally, add channels from the right leg
      endJointName = getAttribute(node, "RightAnkleName", set)
      endJointIndex = anim.getRigChannelIndex(endJointName, set)
      midJointIndex = anim.getParentBoneIndex(endJointIndex, set)
      rootJointIndex = anim.getParentBoneIndex(midJointIndex, set)
      reverseMap[endJointIndex] = endJointIndex
      reverseMap[midJointIndex] = midJointIndex
      reverseMap[rootJointIndex] = rootJointIndex

      -- Re-reverse to get output channels.  Sort to keep neat numerical order.
      local outputChannels = { }
      for jointIndex,_ in reverseMap do
        outputChannels[jointIndex] = true
      end

      return outputChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
    
      ---------------------
      if version < 2 then
        local oldSourcePath = string.format("%s.In", node)
        local newSourcePath = string.format("%s.Source", node)
        pinLookupTable[oldSourcePath] = newSourcePath

        local oldResultPath = string.format("%s.Out", node)
        local newResultPath = string.format("%s.Result", node)
        pinLookupTable[oldResultPath] = newResultPath
      end
      
      ---------------------
      if version < 4 then
       
       -- The following attributes were hidden in version 4
        --  UpAxisX, UpAxisY, UpAxisZ - this is now set automatically from the project preferences
        local oldAxis = {x = 0, y = 0, z = 0}
        local upAxisX = getAttribute(node .. ".deprecated_UpAxisX")
        if (upAxisX ~= nil) then
          oldAxis.x = upAxisX
        end
        local upAxisY = getAttribute(node .. ".deprecated_UpAxisY")
        if (upAxisY ~= nil) then
          oldAxis.y = upAxisY
        end
        local upAxisZ = getAttribute(node .. ".deprecated_UpAxisZ")
        if (upAxisZ ~= nil) then
          oldAxis.z = upAxisZ
        end

        local worldUpAxis = preferences.get("WorldUpAxis")
        local newAxis = {x = 0, y = 0, z = 0}
        if (worldUpAxis == "X Axis") then
          newAxis.x = 1
        elseif (worldUpAxis == "Y Axis") then
          newAxis.y = 1
        elseif (worldUpAxis == "Z Axis") then
          newAxis.z = 1
        end

        local err = math.abs(newAxis.x - oldAxis.x) + math.abs(newAxis.y - oldAxis.y) + math.abs(newAxis.z - oldAxis.z)
        if (err > 1e-4) then
          app.warning("While upgrading BasicUnevenTerrain node " .. node .. ", the UpAxis attribute was changed to match your preferences.")
        else
          --remove deprecated values
          removeAttribute(node, "deprecated_UpAxisX")
          removeAttribute(node, "deprecated_UpAxisY")
          removeAttribute(node, "deprecated_UpAxisZ")
        end
      end

    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- BasicUnevenTerrain custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "BasicUnevenTerrain",
    {
      {
        title = "Hips",
        usedAttributes = {
                "HipsName",
                "HipsHeightControlEnable"
                },
        displayFunc = function(...) safefunc(attributeEditor.basicUnevenTerrainHipsDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Legs",
        usedAttributes = {
                "LeftAnkleName",
                "LeftBallName",
                "LeftToeName",
                "LeftKneeRotationAxisX",
                "LeftKneeRotationAxisY",
                "LeftKneeRotationAxisZ",
                "LeftFlipKneeRotationDirection",
                "RightAnkleName",
                "RightBallName",
                "RightToeName",
                "RightKneeRotationAxisX",
                "RightKneeRotationAxisY",
                "RightKneeRotationAxisZ",
                "RightFlipKneeRotationDirection",
                "StraightestLegFactor",
                "BallJointEnable",
                "ToeJointEnable"
                },
        displayFunc = function(...) safefunc(attributeEditor.basicUnevenTerrainLegDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Limits",
        usedAttributes = {
                "HipsPosVelLimitEnable",
                "HipsPosVelLimit",
                "HipsPosAccelLimitEnable",
                "HipsPosAccelLimit",
                "AnklePosVelLimitEnable",
                "AnklePosVelLimit",
                "AnklePosAccelLimitEnable",
                "AnklePosAccelLimit",
                "AnkleAngVelLimitEnable",
                "AnkleAngVelLimit",
                "AnkleAngAccelLimitEnable",
                "AnkleAngAccelLimit",
                "UseGroundPenetrationFixup",
                "UseTrajectorySlopeAlignment",
                "FootAlignToSurfaceAngleLimit",
                "FootAlignToSurfaceMaxSlopeAngle",
                "FootLiftingHeightLimit",
                },
        displayFunc = function(...) safefunc(attributeEditor.basicUnevenTerrainLimitsDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end
