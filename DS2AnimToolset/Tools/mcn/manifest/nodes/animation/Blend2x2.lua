------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/BlendDurationEventDisplayInfo.lua"
require "ui/AttributeEditor/BlendTimeStretchDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- Blend2x2 node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("Blend2x2",
  {
    displayName = "Blend 2x2",
    helptext = "Blends four animation streams together",
    group = "Blends",
    image = "Blend2x2.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 149),
    version = 2,

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
      ["Source2"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source3"] = {
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

    dataPins =
    {
      ["WeightX"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["WeightY"] = {
        input = true,
        array = false,
        type = "float"
      },
    },

    pinOrder = { "Source0", "Source1", "Source2", "Source3", "WeightX", "WeightY", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
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
      local source2Pin = string.format("%s.Source2", node)
      local source3Pin = string.format("%s.Source3", node)
      local resultPin = string.format("%s.Result", node)

      local timeStretchMode = getAttribute(node, "TimeStretchMode")
      if timeStretchMode == 0 then
        -- no time stretching
        removePinInterfaces(source0Pin, { "Events", })
        removePinInterfaces(source1Pin, { "Events", })
        removePinInterfaces(source2Pin, { "Events", })
        removePinInterfaces(source3Pin, { "Events", })
        removePinInterfaces(resultPin, { "Events", })
      else
        addPinInterfaces(source0Pin, { "Events", })
        addPinInterfaces(source1Pin, { "Events", })
        addPinInterfaces(source2Pin, { "Events", })
        addPinInterfaces(source3Pin, { "Events", })
        addPinInterfaces(resultPin, { "Events", })
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local connectedPinCount = 0
      local pinsAllowed = true
      local errorMessage = ""

      for index = 0, 3 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
          local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
          local sourceNode = connections[1]
          if not isValid(sourceNode) then
            return nil, string.format("Blend2x2 node %s requires a valid input to Source%d, node %s is not valid", node, index, sourceNode)
          end
        else
          return nil, string.format("Blend2x2 node %s is missing a required connection to Source%d", node, index)
        end
      end

      local weightXPin = string.format("%s.WeightX", node)
      if isConnected{ SourcePin = weightXPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightXPin, ResolveReferences = true }
        local weightXNode = connections[1]
        if isValid(weightXNode) ~= true then
          return nil, string.format("Blend2x2 node %s requires a valid input to WeightX, node %s is not valid", node, weightXNode)
        end
      else
        return nil, string.format("Blend2x2 node %s is missing a required connection to WeightX", node)
      end

      local weightYPin = string.format("%s.WeightY", node)
      if isConnected{ SourcePin = weightYPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightYPin, ResolveReferences = true }
        local weightYNode = connections[1]
        if isValid(weightYNode) ~= true then
          return nil, string.format("Blend2x2 node %s requires a valid input to WeightY, node %s is not valid", node, weightYNode)
        end
      else
        return nil, string.format("Blend2x2 node %s is missing a required connection to WeightY", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      -- next write out the id of the node connected to the two weight parameters
      local wInfo = getConnectedNodeInfo((node .. ".WeightX"))
      stream:writeNetworkNodeId(wInfo.id, "WeightX", wInfo.pinIndex)
      local yInfo = getConnectedNodeInfo((node .. ".WeightY"))
      stream:writeNetworkNodeId(yInfo.id, "WeightY", yInfo.pinIndex)

      -- Write runtime id of connected pins
      local source0NodeID = getConnectedNodeID(node, "Source0")
      stream:writeNetworkNodeId(source0NodeID, "Source0NodeID")

      local source1NodeID = getConnectedNodeID(node, "Source1")
      stream:writeNetworkNodeId(source1NodeID, "Source1NodeID")

      local source2NodeID = getConnectedNodeID(node, "Source2")
      stream:writeNetworkNodeId(source2NodeID, "Source2NodeID")

      local source3NodeID = getConnectedNodeID(node, "Source3")
      stream:writeNetworkNodeId(source3NodeID, "Source3NodeID")

      local timeStretchMode = getAttribute(node, "TimeStretchMode")
      stream:writeInt(timeStretchMode, "TimeStretchMode")

      local slerpTrajectory = getAttribute(node, "SphericallyInterpolateTrajectoryPosition")
      stream:writeBool(slerpTrajectory, "SphericallyInterpolateTrajectoryPosition")

      local startEventIndex = getAttribute(node, "StartEventIndex")
      stream:writeInt(startEventIndex, "StartEventIndex")

      stream:writeFloat(0, "BlendWeightX_0");
      stream:writeFloat(0, "BlendWeightX_1");
      stream:writeFloat(0, "BlendWeightY_0");
      stream:writeFloat(0, "BlendWeightY_1");

      stream:writeBool(true, "AlwaysCombineSampledEvents")

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

      local loop = getAttribute(node, "Loop")
      stream:writeBool(loop, "Loop")
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local unionOfResults = { }

      local source0Pin = string.format("%s.Source0", node)
      if isConnected{ SourcePin = source0Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source0Pin, ResolveReferences = true }
        local source0Node = connections[1]
        unionOfResults = anim.getTransformChannels(source0Node, set)
      end

      for index = 1, 3 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
          local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
          local sourceNode = connections[1]
          local channels = anim.getTransformChannels(sourceNode, set)
          unionOfResults = setUnion(unionOfResults, channels)
        else
          break
        end
      end

      return unionOfResults
    end,
    
    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        local timeStretchModeName = string.format("%s.TimeStretchMode", node)
        setAttribute(timeStretchModeName, 0)
      end
    end
  }
)

------------------------------------------------------------------------------------------------------------------------
-- Blend2x2 custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "Blend2x2",
    {
      {
        title = "Time Stretching",
        usedAttributes = { "TimeStretchMode", },
        displayFunc = function(...) safefunc(attributeEditor.blendTimeStretchDisplayInfoSection, unpack(arg)) end,
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
    }
  )
end