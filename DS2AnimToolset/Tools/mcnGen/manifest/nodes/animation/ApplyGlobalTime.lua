------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- ApplyGlobalTime node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("ApplyGlobalTime",
  {
    displayName = "Apply Global Time",
    helptext = "Overrides source time with the global network time",
    group = "Utilities",
    image = "ApplyGlobalTime.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 119),
    version = 2,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Time", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Time", },
          optional = { },
        },
      },
    },

    pinOrder = { "Source", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
        local nodesConnectedTo = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = nodesConnectedTo[1]
        if isValid(sourceNode) ~= true then
          return nil, "Apply Global Time node requires a valid input node"
        end
      else
        return nil, ("Apply Global Time node " .. node .. " is missing a required connection to Source")
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local sourceID = -1

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
        sourceID = getConnectedNodeID(sourcePin)
      end

      Stream:writeNetworkNodeId(sourceID, "NodeConnectedTo")
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

      return sourceNodeChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    getEvents = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = connections[1]
        local sourceEvents = anim.getEvents(sourceNode)
        return sourceEvents
      end

      return { min = 0, max = 0 }
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
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
-- End of ApplyGlobalTime node definition.
------------------------------------------------------------------------------------------------------------------------