------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/ChannelNameDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- SmoothTransforms node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("SmoothTransforms",
  {
    displayName = "Smooth Transforms",
    helptext = "Smooth channels from an input buffer. Pass through all other Node communication.",
    group = "Utilities",
    image = "SmoothTransforms.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 500),
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

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Multiplier"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source", "Result", "Multiplier", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "ChannelSmoothingStrengths", type = "floatArray", min = 0.0, max = 1.0,
        perAnimSet = true, syncWithRigChannels = true,
        helptext = "Weighting factors that influence the smoothing of each channel."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = connections[1]
        if isValid(sourceNode) ~= true then
          return nil, string.format("SmoothTransforms node %s requires a valid input to Source, node %s is not valid", node, sourceNode)
        end
      else
        return nil, string.format("SmoothTransforms node %s is missing a required connection to Source", node)
      end

      local multiplierPin = string.format("%s.Multiplier", node)
      if isConnected{ SourcePin = multiplierPin, ResolveReferences = true } then
        local connections = listConnections{ Object = multiplierPin, ResolveReferences = true }
        local multiplierNode = connections[1]
        if isValid(multiplierNode) ~= true then
          return nil, string.format("SmoothTransforms node %s requires a valid input to Multiplier, node %s is not valid", node, multiplierNode)
        end
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local sourceNodeID = getConnectedNodeID(node, "Source")
      stream:writeNetworkNodeId(sourceNodeID, "SourceNodeID")

      -- there can be no connection to the multiplier  in which case getConnectedNodeID will return nil
      local multiplierNodeInfo = getConnectedNodeInfo(node, "Multiplier")
      if multiplierNodeInfo then
        stream:writeNetworkNodeId(multiplierNodeInfo.id, "Multiplier", multiplierNodeInfo.pinIndex)
      else
        stream:writeNetworkNodeId(-1, "Multiplier")
      end

      local animSets = listAnimSets()
      for asIdx, asVal in animSets do
        local smoothingStrengths = getAttribute(node, "ChannelSmoothingStrengths", asVal)
        local numSmoothingStrengths = table.getn(smoothingStrengths)

        stream:writeUInt(numSmoothingStrengths, "numSmoothingStrengthsSet_" .. asIdx)
        for i, v in ipairs(smoothingStrengths) do
          stream:writeFloat(v, "SmoothingStrengths_" .. i .. "_Set_" .. asIdx)
        end
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local nodeChannels = { }

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = connections[1]
        nodeChannels = anim.getTransformChannels(sourceNode, set)
      end

      return nodeChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    onAttributeInherited = function(node, attribute, set)
      if attribute == "ChannelSmoothingStrengths" then
        local rigMarkupData = anim.getRigMarkupData(set)

        -- +1 because hipIndex is an index into a c++ array not a lua table.
        local hipIndex = rigMarkupData.hipIndex + 1

        -- Init channels with default anim set rig size and give them a default value.
        -- NOTE: when observing the data before filtering we notice that the hips are quite noisy
        -- while the other joints look better. Because of that, the user should smooth the
        -- joints before (and including) the root more than the ones after.
        local numChannels = anim.getRigSize(set)
        local channelIsOutputTable = { }
        for i = 1, numChannels do
          if i <= hipIndex + 1 then
            table.insert(channelIsOutputTable, 0.5)
              else
            table.insert(channelIsOutputTable, 0.25)
          end
        end

        local attributePath = string.format("%s.ChannelSmoothingStrengths", node)
        setAttribute(attributePath, channelIsOutputTable, set)
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        -- this node was changed from a regular node to a filter node without
        -- the version number being bumped so we must take into account two possible pin name changes.
        local newSourcePath = string.format("%s.Source", node)

        -- when it first was a BlendTreeNode its input pin was called Source0
        local oldSource0Path = string.format("%s.Source0", node)
        pinLookupTable[oldSource0Path] = newSourcePath

        -- when it was changed to a FilterNode its input pin was called In
        local oldInPath = string.format("%s.In", node)
        pinLookupTable[oldInPath] = newSourcePath

        -- when it first was a BlendTreeNode its output pin was called Result so no change necessary
        -- when it was changed to a FilterNode its output pin was called Out
        local oldOutPath = string.format("%s.Out", node)
        local newResultPath = string.format("%s.Result", node)
        pinLookupTable[oldOutPath] = newResultPath
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- SmoothTransforms custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "SmoothTransforms",
    {
      {
        title = "Channel Smoothing Strengths",
        usedAttributes = { "ChannelSmoothingStrengths" },
        displayFunc = function(...) safefunc(attributeEditor.channelNameDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end
