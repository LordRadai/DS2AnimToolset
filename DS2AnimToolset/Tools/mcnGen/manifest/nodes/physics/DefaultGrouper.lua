------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

registerGrouperNode(
{
    helptext = "Combines multiple physics results together",
    group = "Physics",
    version = 2,
    interfaces = {
      required = { "Transforms", "Time" },
    },

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      -- please note that Grouper nodes always have pins called "Base", "Override", and "Result"
      -- these pins have the interfaces defined in the interfaces table of the grouper manifest.
      -- Additional pins can be added by adding them here.
    },

    pinOrder = { "Base", "Override", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      -- Grouper nodes always have an attribute called "OverrideGroups".  This is a BodyGroupArray attribute. It is not stored per anim set
      {
        name = "PreferBaseTrajectory",
        type = "bool",
        value = true,
        helptext = "If base and override inputs use the same motion transfer method, this determines which one will provide the trajectory output.",
      },
      {
        name = "PassThroughMode",
        type = "int",
        value = 0, -- [0] = "Base", [1] = "Override"
        min = 0,
        max = 1,
        affectsPins = true,
        helptext = "Changes the interface pass through functionality of this node.",
      },
    },
    
    --------------------------------------------------------------------------------------------------------------------
    updatePins = function(node)
      local basePin = string.format("%s.Base", node)
      local overridePin = string.format("%s.Override", node)
      local resultPin = string.format("%s.Result", node)

      local passThroughMode = getAttribute(node, "PassThroughMode")
      if passThroughMode == 0 then
        -- pass through Base
        setPinPassThrough(basePin, true)
        setPinPassThrough(overridePin, false)
        setPinPassThrough(resultPin, true)
      elseif passThroughMode == 1 then
        -- pass through Override
        setPinPassThrough(basePin, false)
        setPinPassThrough(overridePin, true)
        setPinPassThrough(resultPin, true)
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      if isConnected{ SourcePin  = (node .. ".Base"), ResolveReferences = true }  then
        local nodesConnectedToBase = listConnections{ Object = (node .. ".Base"), ResolveReferences = true }
        local baseNode = nodesConnectedToBase[1]
        if isValid(baseNode) ~= true then
          return nil, string.format("%s '%s' requires a valid input to Base.", getType(node), node)
        end
      else
        return nil, string.format("%s '%s' is missing a required connection to Base.", getType(node), node)
      end

      if isConnected{ SourcePin  = (node .. ".Override"), ResolveReferences = true }  then
        local nodesConnectedToOverride = listConnections{ Object = (node .. ".Override"), ResolveReferences = true }
        local overNode = nodesConnectedToOverride[1]
        if isValid(overNode) ~= true then
          return nil, string.format("%s '%s' requires a valid input to Override.", getType(node), node)
        end
      else
        return nil, string.format("%s '%s' is missing a required connection to Override.", getType(node), node)
      end

      local result, warning = referencesValidBodyGroups(node, "OverrideGroups")
      if not result then
        return nil, warning
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local baseNodeId = -1
      local OverrideNodeId = -1

      local nodeBase = -1
      local nodeOverride = -1
      if isConnected{ SourcePin  =(node .. ".Base") , ResolveReferences = true } then
        nodeBase = getConnectedNodeID(node, "Base")
      end
      if isConnected{ SourcePin  = (node .. ".Override"), ResolveReferences = true } then
        nodeOverride = getConnectedNodeID(node, "Override")
      end

      local animSets = listAnimSets()
      local numAnimSets = table.getn(animSets)

      local preferBaseTrajectory = getAttribute(node, "PreferBaseTrajectory")
      stream:writeBool(preferBaseTrajectory, "PreferBaseTrajectory")

      stream:writeUInt(numAnimSets, "NumAnimSets")

      for asIdx, asVal in animSets do
        -- get the animation channels
        local ChannelIndices = getBodyGroupAttributeChannels(node, "OverrideGroups", asVal)
        local numChannelIndices = table.getn(ChannelIndices)
        stream:writeUInt(numChannelIndices, "NumChannelIndicesSet_"..asIdx)
        for i, v in ipairs(ChannelIndices) do
          if v == true then
            stream:writeUInt(1, "ChannelIndex_"..i .. "_Set_"..asIdx)
          else
            stream:writeUInt(0, "ChannelIndex_"..i .. "_Set_"..asIdx)
          end
        end
      end

      stream:writeNetworkNodeId(nodeBase, "NodeBaseConnectedNodeID")
      stream:writeNetworkNodeId(nodeOverride, "NodeOverrideConnectedNodeID")
      local passThroughMode = getAttribute(node, "PassThroughMode")
      stream:writeInt(passThroughMode, "PassThroughMode")
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local unionOfResults = { }
      local basePin = string.format("%s.Base", node)
      if isConnected{ SourcePin  = basePin, ResolveReferences = true } then
        local baseTable = listConnections{ Object = basePin, ResolveReferences = true }
        local baseNode = baseTable[1]
        local baseChannels = anim.getTransformChannels(baseNode, set)
        unionOfResults = baseChannels
      end

      local overPin = string.format("%s.Override", node)
      if isConnected{ SourcePin  = overPin, ResolveReferences = true } then
        local overTable = listConnections{ Object = overPin, ResolveReferences = true }
        local overNode = overTable[1]
        local overChannels = anim.getTransformChannels(overNode, set)
        unionOfResults = setUnion(unionOfResults, overChannels)
      end

      return unionOfResults
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        -- pass through functionality was added to the Grouper

        -- setting this attribute ensures that the pins are also set correctly to pass through
        local passThroughModeName = string.format("%s.PassThroughMode", node)
        local passThroughMode = getAttribute(passThroughModeName)
        
        local basePin = string.format("%s.Base", node)
        local overridePin = string.format("%s.Override", node)
        local resultPin = string.format("%s.Result", node)

        -- pass through Base
        if passThroughMode == 0 then
          setPinPassThrough(basePin, true)
          setPinPassThrough(overridePin, false)
        -- pass through Override
        elseif passThroughMode == 1 then
          setPinPassThrough(basePin, false)
          setPinPassThrough(overridePin, true)
        end

        setPinPassThrough(resultPin, true)
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- HeadLook custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "Grouper",
    {
      {
        title = "Overrides",
        usedAttributes = { "OverrideGroups" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Pass Through",
        usedAttributes = { "PassThroughMode", },
        displayFunc = function(...) safefunc(attributeEditor.grouperPassThroughDisplayInfoSection, unpack(arg)) end,
      }
    }
  )
end

