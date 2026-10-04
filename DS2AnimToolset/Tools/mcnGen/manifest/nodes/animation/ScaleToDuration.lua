------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- ScaleToDuration node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("ScaleToDuration",
  {
    displayName = "Scale To Duration",
    helptext = "Scales playback speed to conform to a specific interval.",
    group = "Utilities",
    image = "ScaleToDuration.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 151),
    version = 3,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
     ["Duration"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source", "Result", "Duration", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = connections[1]
        if isValid(sourceNode) ~= true then
          return nil, "Scale To Duration node " .. node .. " requires a valid input node"
        end
      else
        return nil, ("Scale To Duration node " .. node .. " is missing a required connection to Source")
      end

      local durationPin = string.format("%s.Duration", node)
      if isConnected{ SourcePin = durationPin, ResolveReferences = true } then
        local connections = listConnections{ Object = durationPin, ResolveReferences = true }
        local durationNode = connections[1]
        if isValid(durationNode) ~= true then
          return false, (node .. " has no valid input to Duration")
        end
      else
        return false, (node .. " has no input to Duration")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputNode = -1
      local durationNodeInfo = nil

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        inputNode = getConnectedNodeID(sourcePin)
      end
      Stream:writeNetworkNodeId(inputNode, "InputNodeID")

      local durationPin = string.format("%s.Duration", node)
      if isConnected{ SourcePin = durationPin, ResolveReferences = true } then
        durationNodeInfo = getConnectedNodeInfo(durationPin)
        Stream:writeNetworkNodeId(durationNodeInfo.id, "Duration", durationNodeInfo.pinIndex)
      else
        Stream:writeNetworkNodeId(-1, "Duration")
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local inputNodeChannels = { }

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = connections[1]
        inputNodeChannels = anim.getTransformChannels(sourceNode, set)
      end

      return inputNodeChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 3 then
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
------------------------------------------------------------------------------------------------------------------------
-- End of ScaleToDuration node definition.
------------------------------------------------------------------------------------------------------------------------