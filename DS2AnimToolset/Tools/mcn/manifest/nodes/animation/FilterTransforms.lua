------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- FilterTransforms node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("FilterTransforms",
  {
    displayName = "Filter Transforms",
    helptext = "Filter out the specified channels from an input buffer. Pass through all other Node communication.",
    group = "Utilities",
    image = "FilterTransforms.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 105),
    version = 2,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", },
          optional = { },
        },
      },
    },

    pinOrder = { "Source", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "ChannelIsOutput", type = "boolArray", perAnimSet = true, syncWithRigChannels = true,
        helptext = "The channels that are output by the filter."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
        local nodesConnectedTo = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = nodesConnectedTo[1]
        if isValid(sourceNode) ~= true then
          return nil, "Filter Transforms requires a valid input node"
        end
      else
        return nil, ("Filter Transforms node " .. node .. " is missing a required connection to Source")
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local sourceID = -1
      local nodesConnectedTo
      local sourceNode = nil

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
        sourceID = getConnectedNodeID(sourcePin)
        nodesConnectedTo = listConnections{ Object = sourcePin, ResolveReferences = true }
        sourceNode = nodesConnectedTo[1]
      end

      local animSets = listAnimSets()
      local numAnimSets = table.getn(animSets)

      Stream:writeNetworkNodeId(sourceID, "NodeConnectedTo")
      Stream:writeUInt(numAnimSets, "NumAnimSets")

      for asIdx, asVal in animSets do

        local connectedChannels = { }
        if sourceNode then
          connectedChannels = anim.getTransformChannels(sourceNode, asVal)
        end

        local channelIsOutput = getAttribute(node, "ChannelIsOutput", asVal)

        local notExportedChannelsList = { }
        for i, v in ipairs(channelIsOutput) do
          if v == false then
            notExportedChannelsList[i - 1] = true
          end
        end

        -- The goal is to write out those channels stripped by this node.
        -- These are the channels that are available (return by anim.getTransformChannels) but
        -- are false in ChannelIsOutput (they were checked off)
        notExportedChannelsList = setIntersection(notExportedChannelsList, connectedChannels)

        local sortedNotExportedChannelsList = { }
        for i,_ in pairs(notExportedChannelsList) do
          table.insert(sortedNotExportedChannelsList, i)
        end
        table.sort(sortedNotExportedChannelsList)

        Stream:writeUInt(table.getn(sortedNotExportedChannelsList), ("FilterIdCount_" .. asIdx))
        for i,v in ipairs(sortedNotExportedChannelsList) do
          Stream:writeUInt(v, ("Id_" .. asIdx .. "_" .. i))
        end

      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local nodesConnectedTo = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = nodesConnectedTo[1]
        local connectedChannels = anim.getTransformChannels(sourceNode, set)
        local exportedChannelsList = { }
        local arrayAttrValues = getAttribute(node, "ChannelIsOutput")
        for i, v in ipairs(arrayAttrValues) do
          if v == true then
            exportedChannelsList[i - 1] = true
          end
        end
        local resultChannels = setIntersection(exportedChannelsList, connectedChannels)
        return resultChannels
      else
        return { }
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getEvents = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local nodesConnectedTo = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = nodesConnectedTo[1]
        local sourceEvents = anim.getEvents(sourceNode)
        return sourceEvents
      end

      return { min = 0, max = 0 }
    end,

    --------------------------------------------------------------------------------------------------------------------
    onAttributeInherited = function(nodeName, attributeName, setName)
      if attributeName == "ChannelIsOutput" then
        -- init filter channels with default anim set rig size, set all outputs 'true' by default.
        local numChannels = anim.getRigSize(setName)
        local channelIsOutputTable = { }
        for i = 1, numChannels do
          table.insert(channelIsOutputTable, true)
        end
        setAttribute(nodeName .. ".ChannelIsOutput", channelIsOutputTable, setName)
      end
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
-- FilterTransforms custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "FilterTransforms",
    {
      {
        title = "Output Channel",
        usedAttributes = { "ChannelIsOutput" },
        displayFunc = function(...) safefunc(attributeEditor.channelNameDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end