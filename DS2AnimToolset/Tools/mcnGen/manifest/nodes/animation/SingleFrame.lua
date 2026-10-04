------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- SingleFrame node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("SingleFrame",
  {
    displayName = "Single Frame",
    helptext = "Plays a single frame of the input based on the value of the control parameter",
    group = "Utilities",
    image = "SingleFrame.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 109),
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
      ["Control"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source", "Result", "Control", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = sourcePin, ResolveReferences = true }
        local inputNode = nodesConnected[1]

        if isValid(inputNode) ~= true then
          return false, "SingleFrame requires a valid input node"
        end
      else
        return false, ("SingleFrame node " .. node .. " is missing a required connection to Source")
      end

      local controlPin = string.format("%s.Control", node)
      if isConnected{ SourcePin = controlPin, ResolveReferences = true } then
        local nodesConnectedToWeight = listConnections{ Object = controlPin, ResolveReferences = true }
        local nodeWeight = nodesConnectedToWeight[1]
        if isValid(nodeWeight) ~= true then
          return false, (node .. " has no valid input to control pin")
        end
      else
        return false, (node .. " has no input to control pin")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputNode = -1
      local controlNode = nil

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        inputNode = getConnectedNodeID(sourcePin)
      end
      Stream:writeNetworkNodeId(inputNode, "InputNodeID")

      local controlPin = string.format("%s.Control", node)
      if isConnected{ SourcePin = controlPin, ResolveReferences = true } then
        controlNode = getConnectedNodeInfo(controlPin)
        Stream:writeNetworkNodeId(controlNode.id, "Control", controlNode.pinIndex)
      else
        Stream:writeNetworkNodeId(-1, "Control")
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local inputNodeChannels = { }

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local sourceTable = listConnections{ Object = sourcePin, ResolveReferences = true }
        local nodeConnected = sourceTable[1]
        inputNodeChannels = anim.getTransformChannels(nodeConnected, set)
      end

      return inputNodeChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version == 1 then
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
------------------------------------------------------------------------------------------------------------------------
-- End of SingleFrame node definition.
------------------------------------------------------------------------------------------------------------------------