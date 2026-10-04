------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/ClosestAnimDisplayInfo.lua"
require "ui/AttributeEditor/ChannelNameDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- ClosestAnim node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("ClosestAnim",
  {
    displayName = "Closest Anim",
    helptext = "Selects the animation source with the closest pose to the current network output",
    group = "Utilities",
    image = "ClosestAnim.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 130),
    version = 5,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source0"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source1"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source2"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source3"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source4"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source5"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source6"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source7"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source8"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source9"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
    },

    dataPins =
    {
      ["DeadBlendWeight"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder =
    {
      "Source0",
      "Source1",
      "Source2",
      "Source3",
      "Source4",
      "Source5",
      "Source6",
      "Source7",
      "Source8",
      "Source9",
      "BlendWeight",
      "Result",
    },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "PrecomputeSourcesOffline",
        type = "bool",
        value = true,
        helptext = "Determines how the connected sources are evaluated. Use Offline if the sources should be pre-computed offline and stored as definition data. Use the Offline method if fast matching is required. The Online method is more flexible but slower to compute."
      },
      {
        name = "InfluenceBetweenPositionAndOrientation",
        type = "float",
        min = 0,
        max = 1,
        value = 0.5,
        helptext = "Used to weight the pose matching process between errors in position and orientation."
      },
      {
        name = "UseVelocity",
        type = "bool",
        value = false,
        helptext = "Compare the first two frames of each input animation source to the last two frames of the network output instead of only the first frame of each input animation source with the last frame of the network output."
      },
      {
        name = "UseRootRotationBlending",
        type = "bool",
        value = true,
        helptext = "If set to 'Character Root' then an offset rotation is applied to the character root joint of the closest matching source to align the character with the network output pose. This offset rotation is blended into the trajectory channel over time so that the character remains aligned whilst driving to the target orientation. NOTE: Use a zero length transition into the state machine containing the node to avoid competing blend requirements."
      },
      {
        name = "BlendDuration",
        type = "float",
        value = 0.25,
        helptext = "The duration of the source animation to apply the root rotation blending over."
      },
      {
        name = "MatchTolerance",
        type = "float",
        value = 180,
        helptext = "The maximum root rotation angle allowed for a valid closest anim match."
      },
      {
        name = "ChannelAlphas",
        type = "floatArray",
        min = 0,
        max = 1,
        perAnimSet = true,
        syncWithRigChannels = true,
        helptext = "Weighting factors that influence the importance of each channel during the pose matching process."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)

      ---------------------
      getUpstreamAnimNode = function(pinName, validNodeTypes)

        local error = false
        local errorMessage = ""

        -- Check for a valid connected pin
        local nodesConnected = listConnections{ Object = pinName, Upstream = true, Downstream = false, ResolveReferences = true }
        local animNode = nodesConnected[1]
        if animNode == nil then
          errorMessage = string.format("No upstream animation is connected to %s", pinName)
          return nil, errorMessage
        end

        -- Iterate over the upstream child node connections until we find an animation node
        local nodeType = getType(animNode)
        while (not(nodeType == nil) and not(nodeType == "AnimWithEvents")) do

          -- Check for a valid connected node type
          local isValidNodeType = false
          for _, v in pairs(validNodeTypes) do
            if nodeType == v then
              isValidNodeType = true
              break
            end
          end

          if not(isValidNodeType) then
            error = true
            errorMessage = string.format("Node %s of type %s connected to %s is not supported", animNode, nodeType, pinName)
            break
          end

          -- Check for a single connected child node
          nodesConnected = listConnections{ Object = animNode, Upstream = true, Downstream = false, ResolveReferences = true }
          local numChildNodes = table.getn(nodesConnected)    
          if (not(numChildNodes == 1)) then
            error = true
            errorMessage = string.format("Node %s of type %s connected to %s is not supported", animNode, nodeType, pinName)
            break
          end

          -- Get the next upstream child
          animNode = nodesConnected[1]
          nodeType = getType(animNode)
        end

        if error then
          return nil, errorMessage
        end

        return animNode
      end

      ---------------------
      getNumAlphaChannelWeights = function(node, setName)

        local alphaValues = getAttribute(node, "ChannelAlphas", setName)
        local numAlphaValues = table.getn(alphaValues)
        local tol = 1e-4

        local numWeights = 0
        for i, v in ipairs(alphaValues) do
          if i > 1 and v > tol then
            numWeights = numWeights + 1
          end
        end

        return numWeights
      end

      ---------------------
      -- validate the dead blend weight pin connection
      local weightPin = string.format("%s.DeadBlendWeight", node)
      if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightPin, ResolveReferences = true }
        local weightNode = connections[1]
        if not isValid(weightNode) then
          return nil, string.format("ClosestAnim node %s requires a valid input to DeadBlendWeight, node %s is not valid", node, weightNode)
        end
      end

      -- Get the connected pin count
      local connectedPinCount = 0
      for index = 0, 9 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
          -- if the connectedPinCount is not equal to index then there were some unconnected pins before this point
          if connectedPinCount == index then
            connectedPinCount = connectedPinCount + 1
          else
            error = true
            if connectedPinCount > 0 then
              return nil, string.format("ClosestAnim node %s has sparsely connected pins, ensure that there are not gaps between connected pins", node)
            else
              return nil, string.format("ClosestAnim node %s has unconnected pins before pin Source%d, ensure that the connected pins start at Source0 and there are no gaps between connected pins", node, index)
            end
          end
        end
      end

      if connectedPinCount == 0 then
        return nil, string.format("There are no valid connections to ClosestAnim node %s", node)
      end

      -- Check for valid connections
      for index = 0, connectedPinCount - 1 do
        local sourcePin = string.format("%s.Source%d", node, index)
        local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = connections[1]
        if not isValid(sourceNode) then
          return nil, string.format("ClosestAnim node %s requires a valid input to Source%d, node %s is not valid", node, index, sourceNode)
        end
      end

      -- Validate a single connected source (Must have root rotation enabled)
      if connectedPinCount == 1 then
        local useRootRotationBlending = getAttribute(node, "UseRootRotationBlending")
        if not(useRootRotationBlending) then
          return nil, string.format("ClosestAnim node %s: The Blend Type must be set to 'Character Root' if only a single source is connected", node)
        end
      end

      -- Validate the connections for offline matching
      local precomputeSourcesOffline = getAttribute(node, "PrecomputeSourcesOffline")
      if precomputeSourcesOffline then

        local validNodeTypes = {"PassThrough", "MirrorTransforms", "FilterTransforms", "ApplyBindPose"}
        for index = 0, connectedPinCount - 1 do
          local sourcePin = string.format("%s.Source%d", node, index)
          local animNode, errorMsg = getUpstreamAnimNode(sourcePin, validNodeTypes)
          if animNode == nil then
            return nil, string.format("ClosestAnim node %s has connected nodes that are incompatible with offline matching: %s", node, errorMsg)
          end
        end
      end
      
      -- Validate the channel alpha weights
      local animSets = listAnimSets()
      for setIndex, setName in animSets do
        local numValidWeights = getNumAlphaChannelWeights(node, setName)
        if numValidWeights == 0 then
          return nil, string.format("ClosestAnim node %s does not have any joint weights for anim set %s", node, setName)
        end
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)

      -- write node attributes
      local precomputeSourcesOffline = getAttribute(node, "PrecomputeSourcesOffline")
      stream:writeBool(precomputeSourcesOffline, "PrecomputeSourcesOffline")
      local influenceBetweenPositionAndOrientation = getAttribute(node, "InfluenceBetweenPositionAndOrientation")
      stream:writeFloat(influenceBetweenPositionAndOrientation, "InfluenceBetweenPositionAndOrientation")
      local useVelocity = getAttribute(node, "UseVelocity")
      stream:writeBool(useVelocity, "UseVelocity")
      local useRootRotationBlending = getAttribute(node, "UseRootRotationBlending")
      stream:writeBool(useRootRotationBlending, "UseRootRotationBlending")
      local fractionThroughSource = getAttribute(node, "BlendDuration")
      stream:writeFloat(fractionThroughSource, "BlendDuration")
      local maxRootRotationAngle = getAttribute(node, "MatchTolerance")
      maxRootRotationAngle = maxRootRotationAngle * (3.1415926535897932384626433832795 / 180)
      stream:writeFloat(maxRootRotationAngle, "MatchTolerance")

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
      stream:writeUInt(upAxisIndex, "UpAxisIndex")

      -- There can be no connection to the event blending weight, in which case getConnectedNodeID will return nil
      local deadBlendWeightNodeInfo = getConnectedNodeInfo(node, "DeadBlendWeight")
      if deadBlendWeightNodeInfo then
        stream:writeNetworkNodeId(deadBlendWeightNodeInfo.id, "DeadBlendWeight", deadBlendWeightNodeInfo.pinIndex)
      else
        stream:writeNetworkNodeId(-1, "DeadBlendWeight")
      end

      -- write connected node runtime ids
      local count = 10 -- This will be left set if we never find an unconnected pin.  11 is the max number of inputs
      for index = 0, 9 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
          local sourceNodeID = getConnectedNodeID(sourcePin)
          stream:writeNetworkNodeId(sourceNodeID, string.format("Source%dNodeID", index))
        else
          count = index
          break
        end
      end

      -- now write out the number of connected pins
      stream:writeInt(count, "SourceNodeCount")

      -- Channel weights
      local animSets = listAnimSets()
      local numAnimSets = table.getn(animSets)

      stream:writeUInt(numAnimSets, "NumAnimSets")

      for asIdx, asVal in animSets do
        local AlphaValues = getAttribute(node, "ChannelAlphas", asVal)
        local numAlphaValues = table.getn(AlphaValues)

        stream:writeUInt(numAlphaValues, "NumAlphaValuesSet_"..asIdx)
        for i, v in ipairs(AlphaValues) do
          stream:writeFloat(v, "Alpha_" ..i .. "_Set_"..asIdx)
        end
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    onAttributeInherited = function(nodeName, attributeName, setName)

      -- ChannelAlpha Weights
      if attributeName == "ChannelAlphas" then

        -- Get the index of the character root joint (i.e. Hips)
        -- Would like to do something like:
        --     local rootIndx = anim.getCharacterRootJoint(setName)
        -- However, this function is not available at the present time
        local rootIndx = -1;
        local names = anim.getRigChannelNames(setName)
        for i, joint in ipairs(names) do
          local name = string.lower(joint)
          if string.find(name, "hips") then
            rootIndx = i
            break
          end
        end

        -- init filter channels with default anim set rig size, set all output weights to zero by default.
        local numChannels = anim.getRigSize(setName)
        local channelIsOutputTable = { }
        for i = 1, numChannels do
          if i == rootIndx then
            table.insert(channelIsOutputTable, 1)
          else
            table.insert(channelIsOutputTable, 0)
          end
        end
        setAttribute(nodeName .. ".ChannelAlphas", channelIsOutputTable, setName)
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local results = { }
      for currentPin = 0, 9 do
        local sourcePin = string.format("%s.Source%s", node, currentPin)
        if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
          local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
          local sourceNode = connections[1]
          local channels = anim.getTransformChannels(sourceNode, set)
          if currentPin == 0 then
            results = channels
          else
            results = setUnion(results, channels)
          end
        else
          break
        end
      end

      return results
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)     

      ---------------------
      if version < 3 then
        for index = 0, 9 do
          local sourcePin = string.format("%s.Source%d", node, index)
          setPinPassThrough(sourcePin, true)
        end

        local resultPin = string.format("%s.Result", node)
        setPinPassThrough(resultPin, true)

        -- the attribute may have been previously upgraded due to a versioning number error
        if attributeExists("deprecated_FractionThroughSource") then
          local oldBlendDuration = getAttribute(node, "deprecated_FractionThroughSource")
          local newBlendDuration = getAttribute(node, "BlendDuration")

          -- Remove deprecated attribute if both old and new attribute values are the same
          if (newBlendDuration == oldBlendDuration) then
            removeAttribute(node, "deprecated_FractionThroughSource")
          end
        end

        -- the attribute may have been previously upgraded due to a versioning number error
        if attributeExists("deprecated_FractionThroughSource") then
          local oldMatchTolerance = getAttribute(node, "deprecated_MaxRootRotationAngle")
          local newMatchTolerance = getAttribute(node, "MatchTolerance")

          -- Remove deprecated attribute if both old and new attribute values are the same
          if (newMatchTolerance == oldMatchTolerance) then
            removeAttribute(node, "deprecated_MaxRootRotationAngle")
          end
        end

      --[[

      Between versions 2.3.1 and 2.3.2 two Closest Anim node attributes got renamed without an upgrade function.

      The following attributes were renamed:

      1) FractionThroughSource
      2) MaxRootRotationAngle

      Which respectively became:

      1) BlendDuration
      2) MatchTolerance

      This means that networks containing closest anim nodes created in versions previous to 2.3.2 will not retain the values for
      these two now deprecated attributes. The new attributes will just be set to their defaults.

      To make sure we do not lose your previous data we are deliberately not removing these deprecated attributes. This way you are
      free to either copy their values into the new attributes before removing them just remove them (in case you have already edited those values).

      So below we provide the implementation for these 2 mutually exclusive options which you only need to uncomment.

      -- ***************************************************************************************************************************
      -- OPTION 1: Copy deprecated attribute values across to the new attributes if new attributes are set to their default values

        if ((newBlendDuration == 0.25) and (newBlendDuration ~= oldBlendDuration)) then
          setAttribute(node .. ".BlendDuration", oldBlendDuration)
        end
        removeAttribute(node, "deprecated_FractionThroughSource")

        if ((newMatchTolerance == 180) and (newMatchTolerance ~= oldMatchTolerance)) then
          setAttribute(node .. ".MatchTolerance", oldMatchTolerance)
        end
        removeAttribute(node, "deprecated_MaxRootRotationAngle")

      -- OPTION 1 - END
      -- ***************************************************************************************************************************

      -- ***************************************************************************************************************************
      -- OPTION 2: Just remove the deprecated attributes

        removeAttribute(node, "deprecated_FractionThroughSource")
        removeAttribute(node, "deprecated_MaxRootRotationAngle")

      -- OPTION 2 - END
      -- ***************************************************************************************************************************

      --]]

      end

      ---------------------
      if version < 4 then
        -- An option to compute the sources offline was added. All previous version closest
        -- anim nodes must be upgraded to use online source evaluation by default
        setAttribute(node .. ".PrecomputeSourcesOffline", false)
      end

      ---------------------
      if version < 5 then

        -- The following attributes were hidden in version 5
        --  RootRotationAxis - this is now set automatically from the project preferences
        local oldAxis = {x = 0, y = 0, z = 0}
        local upAxisX = getAttribute(node .. ".deprecated_RootRotationAxisX")
        if (upAxisX ~= nil) then
          oldAxis.x = upAxisX
        end
        local upAxisY = getAttribute(node .. ".deprecated_RootRotationAxisY")
        if (upAxisY ~= nil) then
          oldAxis.y = upAxisY
        end
        local upAxisZ = getAttribute(node .. ".deprecated_RootRotationAxisZ")
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
          app.warning("While upgrading ClosestAnim node " .. node .. ", the RootRotationAxis attribute was changed to match your preferences.")
        else
          --remove deprecated values
          removeAttribute(node, "deprecated_RootRotationAxisX")
          removeAttribute(node, "deprecated_RootRotationAxisY")
          removeAttribute(node, "deprecated_RootRotationAxisZ")
        end

      end

    end
  }
)

------------------------------------------------------------------------------------------------------------------------
-- Closest Anim custom editors
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "ClosestAnim",
    {
     {
        title = "Matching",
        usedAttributes = { "PrecomputeSourcesOffline" },
        displayFunc = function(...) safefunc(attributeEditor.closestAnimMatchingSection, unpack(arg)) end
      },
      {
        title = "Influences",
        usedAttributes = { "InfluenceBetweenPositionAndOrientation", "UseVelocity" },
        displayFunc = function(...) safefunc(attributeEditor.closestAnimInfluenceSection, unpack(arg)) end
      },
      {
        title = "Root Rotation",
        usedAttributes = { "UseRootRotationBlending", "BlendDuration", "MatchTolerance"},
        displayFunc = function(...) safefunc(attributeEditor.closestAnimRootRotationSection, unpack(arg)) end
      },
      {
        title = "Channel Alphas",
        usedAttributes = { "ChannelAlphas" },
        displayFunc = function(...) safefunc(attributeEditor.channelNameDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end