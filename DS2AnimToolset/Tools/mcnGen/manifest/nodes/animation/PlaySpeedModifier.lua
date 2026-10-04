------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

registerNode("PlaySpeedModifier",
  {
    displayName = "Play Speed Modifier",
    helptext = "Scales playback speed based on a connected control parameter.",
    group = "Utilities",
    image = "PlaySpeedModifier.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 125),
    version = 3,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Weight"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source", "Result", "Weight", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local nodesConnectedTo = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = nodesConnectedTo[1]
        if isValid(sourceNode) ~= true then
          return nil, "Play Speed Modifier node " .. node .. " requires a valid input node"
        end
      else
        return nil, ("Play Speed Modifier node " .. node .. " is missing a required connection to Source")
      end

      local weightPin = string.format("%s.Weight", node)
      if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
        local nodesConnectedToWeight = listConnections{ Object = weightPin, ResolveReferences = true }
        local weightNode = nodesConnectedToWeight[1]
        if isValid(weightNode) ~= true then
          return false, (node .. " has no valid input to weight pin")
        end
      else
        return false, (node .. " has no input to weight pin")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputNode = -1
      local weightNodeInfo = nil

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        inputNode = getConnectedNodeID(sourcePin)
      end
      Stream:writeNetworkNodeId(inputNode, "InputNodeID")

      local weightPin = string.format("%s.Weight", node)
      if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
        weightNodeInfo = getConnectedNodeInfo(weightPin)
        Stream:writeNetworkNodeId(weightNodeInfo.id, "Weight", weightNodeInfo.pinIndex)
      else
        Stream:writeNetworkNodeId(-1, "Weight")
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
        local sourcePath = string.format("%s.Source", node)
        setPinPassThrough(sourcePath, true)

        local resultPath = string.format("%s.Result", node)
        setPinPassThrough(resultPath, true)
      elseif version == 2 then
        local oldSourcePath = string.format("%s.In", node)
        local newSourcePath = string.format("%s.Source", node)
        pinLookupTable[oldSourcePath] = newSourcePath

        local oldResultPath = string.format("%s.Out", node)
        local newResultPath = string.format("%s.Result", node)
        pinLookupTable[oldResultPath] = newResultPath
      end
    end,
  }
)