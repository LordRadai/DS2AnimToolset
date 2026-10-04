------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "luaAPI/ManifestUtils.lua"

------------------------------------------------------------------------------------------------------------------------
-- MirrorTransforms node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("MirrorTransforms",
  {
    displayName = "Mirror Transforms",
    helptext = "mirrors animation along a preferred axis",
    group = "Utilities",
    image = "Mirror.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 135),
    version = 3,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", },
          optional = { "Events", },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", },
          optional = { "Events", },
        },
      },
    },

    pinOrder = { "Source", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "EventOffset",
        type = "int",
        value = true,
      },
      {
        name = "EventPassThrough",
        type = "bool",
        value = true,
        set = function(node, value)
          -- as it is only this attribute that affects the pins then it is quicker to
          -- use an attribute setter to control the pin interfaces rather than updatePins
          -- although both methods are valid.
          local sourcePin = string.format("%s.Source", node)
          local resultPin = string.format("%s.Result", node)

          if value then
            removePinInterfaces(sourcePin, { "Events", })
            removePinInterfaces(resultPin, { "Events", })
          else
            addPinInterfaces(sourcePin, { "Events", })
            addPinInterfaces(resultPin, { "Events", })
          end
        end,
      },
      {
        name = "MirrorChannels",
        type = "boolArray",
        perAnimSet = true,
        syncWithRigChannels = true,
        helptext = "Choose the channels that will be mirrored.",
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local nodesConnectedTo = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = nodesConnectedTo[1]

        if isValid(sourceNode) ~= true then
          return nil, "Mirror Transforms node requires a valid input node"
        end
      else
        return nil, ("Mirror Transforms node" .. node .. " is missing a required connection to Source")
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local sourcePin = string.format("%s.Source", node)
      local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
      local sourceNode = connections[1]

      stream:writeNetworkNodeId(getRuntimeID(sourceNode), "InputNodeID")

      local animSets = listAnimSets()
      local numAnimSets = table.getn(animSets)

      stream:writeUInt(numAnimSets, "NumAnimSets")

      -- for every anim set serialize the channels that are being mirrored
      for index, set in animSets do
        -- get the channels output from the source node.
        local sourceChannels = anim.getTransformChannels(sourceNode, set)

        -- build the list of channels this node does not mirror
        local mirrorChannels = getAttribute(node, "MirrorChannels", set)
        local nonMirroredChannels = { }
        for i, v in ipairs(mirrorChannels) do
          if not v then
            nonMirroredChannels[i - 1] = true
          end
        end

        -- now take the intersection of the source channels and the non-mirrored channels
        -- to generate the channels that can be mirrored at runtime
        nonMirroredChannels = setIntersection(nonMirroredChannels, sourceChannels)

        local sortedNonMirroredChannels = { }
        for i,_ in pairs(nonMirroredChannels) do
          table.insert(sortedNonMirroredChannels, i)
        end
        table.sort(sortedNonMirroredChannels)
        
        -- serialize the non-mirrored channels for this set.
        stream:writeUInt(table.getn(sortedNonMirroredChannels), string.format("NonMirroredIdCount_%d", index))
        for i, v in ipairs(sortedNonMirroredChannels) do
          stream:writeUInt(v, string.format("Id_%d_%d", index, i))
        end
      end

      local eventOffset = getAttribute(node, "EventOffset")
      stream:writeInt(eventOffset, "EventOffset")

      local eventPassThrough = getAttribute(node, "EventPassThrough")
      stream:writeBool(eventPassThrough, "EventPassThrough")
     end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local transformChannels = { }

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = connections[1]
        transformChannels = anim.getTransformChannels(sourceNode, set)
      end

      return transformChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    onAttributeInherited = function(nodeName, attributeName, setName)
      if attributeName == "MirrorChannels" then
        -- init mirror channels with default anim set rig size, set all outputs 'true' by default.
        local numChannels = anim.getRigSize(setName)
        local channelIsMirroredTable = { }
        for i = 1, numChannels do
          table.insert(channelIsMirroredTable, true)
        end
        setAttribute(nodeName .. ".MirrorChannels", channelIsMirroredTable, setName)
      end
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
      local attrs = { }

      if version < 2 then
        local oldSourcePath = string.format("%s.In", node)
        local newSourcePath = string.format("%s.Source", node)
        pinLookupTable[oldSourcePath] = newSourcePath

        local oldResultPath = string.format("%s.Out", node)
        local newResultPath = string.format("%s.Result", node)
        pinLookupTable[oldResultPath] = newResultPath
      end

      if version < 3 then
        attrs["eventOffset"] = "EventOffset"
        attrs["eventPassThrough"] = "EventPassThrough"

        local sourcePin = string.format("%s.Source", node)
        local resultPin = string.format("%s.Result", node)

        local eventPassThrough = getAttribute(node, "deprecated_eventPassThrough")
        if eventPassThrough then
          removePinInterfaces(sourcePin, { "Events", })
          removePinInterfaces(resultPin, { "Events", })
        else
          addPinInterfaces(sourcePin, { "Events", })
          addPinInterfaces(resultPin, { "Events", })
        end
      end

      upgradeRenamedAttributes(node, attrs)
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- MirrorTransforms custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
attributeEditor.registerDisplayInfo(
    "MirrorTransforms",
    {
      {
        title = "Mirrored Channels",
        usedAttributes = { "MirrorChannels" },
        displayFunc = function(...) safefunc(attributeEditor.channelNameDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Events",
        usedAttributes = { "EventPassThrough", "EventOffset" },
        displayFunc = function(...) safefunc(attributeEditor.mirrorEventsDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end
------------------------------------------------------------------------------------------------------------------------
-- End of MirrorTransforms node definition.
------------------------------------------------------------------------------------------------------------------------