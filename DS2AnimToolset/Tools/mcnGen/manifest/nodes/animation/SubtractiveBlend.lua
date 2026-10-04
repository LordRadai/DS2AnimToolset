------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- SubtractiveBlend node definition.
------------------------------------------------------------------------------------------------------------------------
require "ui/AttributeEditor/BlendDurationEventDisplayInfo.lua"
require "ui/AttributeEditor/BlendPassThroughDisplayInfo.lua"
require "ui/AttributeEditor/BlendTimeStretchDisplayInfo.lua"
require "ui/AttributeEditor/BlendModeDisplayInfo.lua"
require "ui/AttributeEditor/BlendFlagsDisplayInfo.lua"
require "ui/AttributeEditor/BlendEventBlendModeDisplayInfo.lua"

registerNode("SubtractiveBlend",
  {
    displayName = "Subtractive Blend",
    helptext = "Subtracts an animation's transforms from a base animation's transforms",
    group = "Blends",
    image = "SubtractiveBlend.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 170),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source0"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source1"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
    },

    dataPins = { },

    pinOrder = { "Source0", "Source1", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "PassThroughMode",
        type = "int",
        value = 2, -- [0] = "Source 0", [1] = "Source 1", [2] = "None"
        min = 0,
        max = 2,
        affectsPins = true,
        helptext = "Changes the interface pass through functionality of this node."
      },
      {
        name = "TimeStretchMode",
        type = "int",
        value = 1, -- [0] = "None", [1] = "Match Events"
        min = 0,
        max = 1,
        affectsPins = true,
        helptext = "Changes the time stretching functionality of this node."
      },
      {
        name = "EventsBlendMode",
        type = "int",
        value = 0, -- [0] = "Add", [1] = "Merge"
        min = 0,
        max = 1,
        affectsPins = true,
        helptext = "Changes events blending mode functionality between additive blending and merging."
      },
      {
        name = "SphericallyInterpolateTrajectoryPosition",
        type = "bool",
        value = false,
        displayName = "Slerp Trajectory",
        helptext = "Spherically interpolate between the two trajectory translation inputs. The trajectory orientation is always spherically interpolated."
      },
      {
        name = "Loop",
        type = "bool",
        value = true,
        helptext = "Sets whether the result of this node loops when it reaches the end of it's sequence."
      },
      {
        name = "DurationEventBlendPassThrough",
        type = "bool",
        value = false,
        helptext = "When set, all events are copied straight to output."
      },
      {
        name = "DurationEventBlendIgnoreEventOrder",
        type = "bool",
        value = false,
        helptext = "Ignoring the order of events when blending means that events in the source tracks will be combined even if they have duration events in different orders."
      },
      {
        name = "DurationEventBlendSameUserData",
        type = "bool",
        value = false,
        helptext = "Duration events will only be blended together when they share the same user data."
      },
      {
        name = "DurationEventBlendOnOverlap",
        type = "bool",
        value = false,
        helptext = "When set, if not overlapping simply copy into the result."
      },
      {
        name = "DurationEventBlendWithinRange",
        type = "bool",
        value = false,
        helptext = "When set, blend or reject only events that are within a specified event range of each other, otherwise put straight into the result."
      },
      {
        name = "StartEventIndex",
        type = "int",
        value = 0,
        min = 0,
        helptext = "The sync event tracks from the inputs are combined here. This is the index of the event on the new synctrack from which to start playback."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    updatePins = function(node)
      local source0Pin = string.format("%s.Source0", node)
      local source1Pin = string.format("%s.Source1", node)
      local resultPin = string.format("%s.Result", node)

      local timeStretchMode = getAttribute(node, "TimeStretchMode")
      if timeStretchMode == 0 then
        -- no time stretching
        removePinInterfaces(source0Pin, { "Events", })
        removePinInterfaces(source1Pin, { "Events", })
        removePinInterfaces(resultPin, { "Events", })
      else
        addPinInterfaces(source0Pin, { "Events", })
        addPinInterfaces(source1Pin, { "Events", })
        addPinInterfaces(resultPin, { "Events", })
      end

      local passThroughMode = getAttribute(node, "PassThroughMode")
      if passThroughMode == 0 then
        -- pass through Source0
        setPinPassThrough(source0Pin, true)
        setPinPassThrough(source1Pin, false)
        setPinPassThrough(resultPin, true)
      elseif passThroughMode == 1 then
        -- pass through Source1
        setPinPassThrough(source0Pin, false)
        setPinPassThrough(source1Pin, true)
        setPinPassThrough(resultPin, true)
      else
        -- no pass through
        setPinPassThrough(source0Pin, false)
        setPinPassThrough(source1Pin, false)
        setPinPassThrough(resultPin, false)
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local source0Pin = string.format("%s.Source0", node)
      if isConnected{ SourcePin = source0Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source0Pin, ResolveReferences = true }
        local baseNode = connections[1]
        if not isValid(baseNode) then
          return nil, string.format("SubtractiveBlend node %s requires a valid input to Source0, node %s is not valid", node, source0Node)
        end
      else
        return nil, string.format("SubtractiveBlend node %s is missing a required connection to Source0", node)
      end

      local source1Pin = string.format("%s.Source1", node)
      if isConnected{ SourcePin = source1Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source1Pin, ResolveReferences = true }
        local source1Node = connections[1]
        if not isValid(source1Node) then
          return nil, string.format("SubtractiveBlend node %s requires a valid input to Source1, node %s is not valid", node, source1Node)
        end
      else
        return nil, string.format("SubtractiveBlend node %s is missing a required connection to Source1", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local source0NodeID = getConnectedNodeID(node, "Source0")
      stream:writeNetworkNodeId(source0NodeID, "Source0NodeID")
      local source1NodeID = getConnectedNodeID(node, "Source1")
      stream:writeNetworkNodeId(source1NodeID, "Source1NodeID")

      local timeStretchMode = getAttribute(node, "TimeStretchMode")
      stream:writeInt(timeStretchMode, "TimeStretchMode")
      local passThroughMode = getAttribute(node, "PassThroughMode")
      stream:writeInt(passThroughMode, "PassThroughMode")
      local eventsBlendMode = getAttribute(node, "EventsBlendMode")
      stream:writeInt(eventsBlendMode, "EventsBlendMode")

      local slerpTrajectory = getAttribute(node, "SphericallyInterpolateTrajectoryPosition")
      stream:writeBool(slerpTrajectory, "SphericallyInterpolateTrajectoryPosition")

      local loop = getAttribute(node, "Loop")
      stream:writeBool(loop, "Loop")

      -- Duration event blending flags
      local durationEventBlendPassThrough = getAttribute(node, "DurationEventBlendPassThrough")
      stream:writeBool(durationEventBlendPassThrough, "DurationEventBlendPassThrough")
      local durationEventBlendIgnoreEventOrder = getAttribute(node, "DurationEventBlendIgnoreEventOrder")
      stream:writeBool(not durationEventBlendIgnoreEventOrder, "DurationEventBlendInSequence")
      local durationEventBlendSameUserData = getAttribute(node, "DurationEventBlendSameUserData")
      stream:writeBool(durationEventBlendSameUserData, "DurationEventBlendSameUserData")
      local durationEventBlendOnOverlap = getAttribute(node, "DurationEventBlendOnOverlap")
      stream:writeBool(durationEventBlendOnOverlap, "DurationEventBlendOnOverlap")
      local durationEventBlendWithinRange = getAttribute(node, "DurationEventBlendWithinRange")
      stream:writeBool(durationEventBlendWithinRange, "DurationEventBlendWithinRange")

      local startEventIndex = getAttribute(node, "StartEventIndex")
      stream:writeInt(startEventIndex, "StartEventIndex")
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local baseChannels = { }
      local source0Pin = string.format("%s.Source0", node)
      if isConnected{ SourcePin = source0Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source0Pin, ResolveReferences = true }
        local source0Node = connections[1]
        source0Channels = anim.getTransformChannels(source0Node, set)
      end

      local inputChannels = { }
      local source1Pin = string.format("%s.Source1", node)
      if isConnected{ SourcePin = source1Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source1Pin, ResolveReferences = true }
        local source1Node = connections[1]
        source1Channels = anim.getTransformChannels(source1Node, set)
      end

      local resultChannels = setUnion(source0Channels, source1Channels)
      return resultChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "SubtractiveBlend",
    {
      {
        title = "Time Stretching",
        usedAttributes = { "TimeStretchMode", },
        displayFunc = function(...) safefunc(attributeEditor.blendTimeStretchDisplayInfoSection, unpack(arg)) end,
      },
      {
        title = "Pass Through",
        usedAttributes = { "PassThroughMode", },
        displayFunc = function(...) safefunc(attributeEditor.blendPassThroughDisplayInfoSection, unpack(arg)) end,
      },
      {
        title = "Events Blend",
        usedAttributes = { "EventsBlendMode", },
        displayFunc = function(...) safefunc(attributeEditor.blendEventBlendModeDisplayInfoSection, unpack(arg)) end,
      },
      {
        title = "Duration Event Blending",
        usedAttributes = {
          "DurationEventBlendPassThrough",
          "DurationEventBlendIgnoreEventOrder",
          "DurationEventBlendSameUserData",
          "DurationEventBlendOnOverlap",
          "DurationEventBlendWithinRange",
        },
        displayFunc = function(...) safefunc(attributeEditor.blendDurationEventDisplayInfo, unpack(arg)) end
      },
      {
        title = "Match Events Properties",
        usedAttributes = {
          "StartEventIndex",
          "Loop",
        },
        displayFunc = function(...) safefunc(attributeEditor.matchEventsPropertiesDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = {
          "SphericallyInterpolateTrajectoryPosition",
        },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of SubtractiveBlend node definition.
------------------------------------------------------------------------------------------------------------------------

