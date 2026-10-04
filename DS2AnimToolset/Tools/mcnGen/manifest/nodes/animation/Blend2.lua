------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- Blend2 node definition.
------------------------------------------------------------------------------------------------------------------------
require "ui/AttributeEditor/BlendDurationEventDisplayInfo.lua"
require "ui/AttributeEditor/BlendPassThroughDisplayInfo.lua"
require "ui/AttributeEditor/BlendTimeStretchDisplayInfo.lua"
require "ui/AttributeEditor/BlendModeDisplayInfo.lua"
require "ui/AttributeEditor/BlendFlagsDisplayInfo.lua"
require "ui/AttributeEditor/BlendEventBlendModeDisplayInfo.lua"

registerNode("Blend2",
  {
    displayName = "Blend 2",
    helptext = "Blends two animation streams together",
    group = "Blends",
    image = "Blend2.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 107),
    version = 6,

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

    dataPins =
    {
      ["Weight"] = {
        input = true,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source0", "Source1", "Result", "Weight", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "BlendWeights",
        type = "floatArray",
        value = { 0, 1 },
        helptext = "Specify the weights values used to determine which source nodes are blended."
      },
      {
        name = "TimeStretchMode",
        type = "int",
        value = 1, -- [0] = "None", [1] = "Match Events"
        min = 0,
        max = 1,
        affectsPins = true,
        set = function(node, value)
          -- if TimeStretchMode is None then make sure the always blend events flag is false.
          alwaysBlendName = string.format("%s.AlwaysCombineSampledEvents", node)
          if value == 0 then
            setAttribute(alwaysBlendName, false)
          end
        end,
        helptext = "Changes the time stretching functionality of this node."
      },
      {
        name = "PassThroughMode",
        type = "int",
        value = 2, -- [0] = "Source 0", [1] = "Source 1", [2] = "None"
        min = 0,
        max = 2,
        affectsPins = true,
        set = function(node, value)
          -- if the pass through mode is set to "Source 0" or "Source 1" then make sure always blend flags are true.
          if value == 0 or value == 1 then
            local alwaysBlendName = string.format("%s.AlwaysBlendTrajectoryAndTransforms", node)
            setAttribute(alwaysBlendName, true)
            alwaysBlendName = string.format("%s.AlwaysCombineSampledEvents", node)
            timeStretchMode = getAttribute(node, "TimeStretchMode")
            if (timeStretchMode == 0) then
              setAttribute(alwaysBlendName, false)
            else
              setAttribute(alwaysBlendName, true)
            end
          end
        end,
        helptext = "Changes the interface pass through functionality of this node."
      },
      {
        name = "AlwaysBlendTrajectoryAndTransforms",
        type = "bool",
        value = false,
        helptext =
          "Always blend evaluates the trajectory delta and transform value for Source0 and Source1 even if it is "..
          "completely weighted to one input. This option will improve performance if the weight usually falls somewhere "..
          "between 0 and 1 but is not precisely 0 or 1. When this option is disabled it  may improve performance if the "..
          "weight is usually 0 or 1."
      },
      {
        name = "AlwaysCombineSampledEvents",
        type = "bool",
        value = false,
        helptext =
          "Always blend evaluates the event value for Source0 and Source1 even if it is completely weighted to "..
          "one input. This option will improve performance if the weight usually falls somewhere between 0 and 1 but "..
          "is not precisely 0 or 1. When this option is disabled it  may improve performance if the weight is usually "..
          "0 or 1."
      },
      {
        name = "PositionBlendMode",
        type = "int",
        value = 0, -- [0] = "Interpolative", [1] = "Additive"
        min = 0,
        max = 1,
        affectsPins = true,
        helptext = "Changes the position blend mode functionality of this node's transforms between interpolation and addition."
      },
      {
        name = "RotationBlendMode",
        type = "int",
        value = 0, -- [0] = "Interpolative", [1] = "Additive"
        min = 0,
        max = 1,
        affectsPins = true,
        helptext = "Changes the rotation blend mode functionality of this node's transforms between interpolation and addition."
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
      if isConnected{ SourcePin  = source0Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source0Pin, ResolveReferences = true }
        local source0Node = connections[1]
        if not isValid(source0Node) then
          return nil, string.format("Blend2 node %s requires a valid input to Source0, node %s is not valid", node, source0Node)
        end
      else
        return nil, string.format("Blend2 node %s is missing a required connection to Source0", node)
      end

      local source1Pin = string.format("%s.Source1", node)
      if isConnected{ SourcePin  = source1Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source1Pin, ResolveReferences = true }
        local source1Node = connections[1]
        if not isValid(source1Node) then
          return nil, string.format("Blend2 node %s requires a valid input to Source1, node %s is not valid", node, source1Node)
        end
      else
        return nil, string.format("Blend2 node %s is missing a required connection to Source1", node)
      end

      local weightPin = string.format("%s.Weight", node)
      if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightPin, ResolveReferences = true }
        local weightNode = connections[1]
        if isValid(weightNode) ~= true then
          return nil, string.format("Blend2 node %s requires a valid input to Weight, node %s is not valid", node, weightNode)
        end
      else
        return nil, string.format("Blend2 node %s is missing a required connection to Weight", node)
      end

      -- Validate blend weight count, as there are only two weights we don't need to check the values.
      local blendWeights = getAttribute(node, "BlendWeights")
      local blendWeightCount = table.getn(blendWeights)
      if blendWeightCount < 2 then
        return false, string.format("Blend2 node %s has %d BlendWeights, 2 BlendWeights are required", node, blendWeightCount)
      end
      
      -- Validate blend weights are increasing
      if blendWeights[1] > blendWeights[2] then
        return false, string.format("Blend2 node %s has decreasing BlendWeights, increasing BlendWeights are required", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local source0NodeID = getConnectedNodeID(node, "Source0")
      stream:writeNetworkNodeId(source0NodeID, "Source0NodeID")
      local source1NodeID = getConnectedNodeID(node, "Source1")
      stream:writeNetworkNodeId(source1NodeID, "Source1NodeID")
      local weightNodeInfo = getConnectedNodeInfo(node, "Weight")
      stream:writeNetworkNodeId(weightNodeInfo.id, "Weight", weightNodeInfo.pinIndex)

      local blendWeights = getAttribute(node, "BlendWeights")
      stream:writeFloat(blendWeights[1], ("BlendWeight_0"))
      stream:writeFloat(blendWeights[2], ("BlendWeight_1"))

      local timeStretchMode = getAttribute(node, "TimeStretchMode")
      stream:writeInt(timeStretchMode, "TimeStretchMode")
      local passThroughMode = getAttribute(node, "PassThroughMode")
      stream:writeInt(passThroughMode, "PassThroughMode")
      local eventsBlendMode = getAttribute(node, "EventsBlendMode")
      stream:writeInt(eventsBlendMode, "EventsBlendMode")

      -- always blend flags
      --local alwaysBlendTransforms = getAttribute(node, "AlwaysBlendTrajectoryAndTransforms")
      --stream:writeBool(alwaysBlendTransforms, "AlwaysBlendTrajectoryAndTransforms")
      local alwaysBlendEvents = getAttribute(node, "AlwaysCombineSampledEvents")
      stream:writeBool(alwaysBlendEvents, "AlwaysCombineSampledEvents")
      
      --[[  
      kInterpQuatInterpPos = 0,
      kInterpQuatAddPos = 1,
      kAddQuatInterpPos = 2,
      kAddQuatAddPos = 3,
      --]]
  
      local rotBlendMode = getAttribute(node, "RotationBlendMode")
      local posBlendMode = getAttribute(node, "PositionBlendMode")
      local blendMode = 0
      if (rotBlendMode == 0) and (posBlendMode == 0) then
        blendMode = 0
      elseif (rotBlendMode == 0) and (posBlendMode == 1)then
        blendMode = 1
      elseif (rotBlendMode == 1) and (posBlendMode == 0) then
        blendMode = 2
      elseif (rotBlendMode == 1) and (posBlendMode == 1)then
        blendMode = 3
      end
      stream:writeInt(blendMode, "BlendMode")

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
      local source0Channels = { }
      local source0Pin = string.format("%s.Source0", node)
      if isConnected{ SourcePin = source0Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source0Pin, ResolveReferences = true }
        local source0Node = connections[1]
        source0Channels = anim.getTransformChannels(source0Node, set)
      end

      local source1Channels = { }
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
      if version < 2 then
        -- loop was removed as this was a Blend2 node, when Blend2 and Blend2MatchEvents were
        -- merged in version 3 the loop attribute was re-added so should no longer be removed here.

        -- old Blend2MatchEvents upgrade code
        if attributeExists(node, "deprecated_DurationEventBlendInSequence") then
          local value = getAttribute(node, "deprecated_DurationEventBlendInSequence")
          setAttribute(string.format("%s.DurationEventBlendIgnoreEventOrder", node), not value)
          removeAttribute(node, "deprecated_DurationEventBlendInSequence")
        end
      end

      if version < 3 then
        local timeStretchModeName = string.format("%s.TimeStretchMode", node)
        setAttribute(timeStretchModeName, 0)

        local passThroughModeName = string.format("%s.PassThroughMode", node)
        setAttribute(passThroughModeName, 2)
      end

      if version < 5 then
      
        local additiveBlendAttitude  = false
        if attributeExists(node, "deprecated_AdditiveBlendAttitude") then
          additiveBlendAttitude = getAttribute(node, "deprecated_AdditiveBlendAttitude")
          removeAttribute(node, "deprecated_AdditiveBlendAttitude")
        end
        
        local additiveBlendPosition  = false
        if attributeExists(node, "deprecated_AdditiveBlendPosition") then
          additiveBlendPosition = getAttribute(node, "deprecated_AdditiveBlendPosition")
          removeAttribute(node, "deprecated_AdditiveBlendPosition")
        end
        
        local rotBlendModeName = string.format("%s.RotationBlendMode", node)
        local posBlendModeName = string.format("%s.PositionBlendMode", node)
        -- [0] = "Interpolative", [1] = "Additive"
        local rotBlendMode = 0
        local posBlendMode = 0
        
        if (additiveBlendAttitude) and (additiveBlendPosition) then
          rotBlendMode = 1
          posBlendMode = 1
        elseif (additiveBlendAttitude) then
          rotBlendMode = 1
          posBlendMode = 0
        elseif (additiveBlendPosition) then
          rotBlendMode = 0
          posBlendMode = 1
        end

        setAttribute(rotBlendModeName, rotBlendMode)
        setAttribute(posBlendModeName, posBlendMode)

        -- AlwaysCombineSampledEvents was added in version 5
        -- Make sure this flag is ON for older nodes so we don't modify the behaviour of existent networks
        local timeStretchMode = getAttribute(node, "TimeStretchMode")
        local alwaysBlendEventsName = string.format("%s.AlwaysCombineSampledEvents", node)
        if timeStretchMode == 0 then
          setAttribute(alwaysBlendEventsName, false)
        else
          setAttribute(alwaysBlendEventsName, true)
        end
      end
      
      if version < 6 then
        -- AlwaysBlendTrajectories and AlwaysBlendTransforms were replaced by AlwaysBlendTrajectoryAndTransforms in 
        -- version 6. If either of these are set to true then always blend trajectory and transforms.
        
        -- Make sure these flags are ON for older nodes so we don't modify the behaviour of existent networks
        local alwaysBlendValue = true
        
        if version ~= 5 then
          -- Came from version 4 or lower
          -- Make sure this flag is ON for older nodes so we don't modify the behaviour of existent networks
          alwaysBlendValue = true
          if attributeExists(node, "deprecated_AlwaysBlend") then
            alwaysBlendValue = getAttribute(node, "deprecated_AlwaysBlend")
            removeAttribute(node, "deprecated_AlwaysBlend")
          end
        else
          -- Version from == 5
          -- If either deprecated_AlwaysBlendTransforms or deprecated_AlwaysBlendTrajectories is true then 
          -- AlwaysBlendTrajectoryAndTransforms is true
          alwaysBlendValue = false
          if attributeExists(node, "deprecated_AlwaysBlendTransforms") then
            alwaysBlendValue = getAttribute(node, "deprecated_AlwaysBlendTransforms")
            removeAttribute(node, "deprecated_AlwaysBlendTransforms")
          end
          
          if attributeExists(node, "deprecated_AlwaysBlendTrajectories") then
            -- Only update the always blend if transforms was didn't always blend
            if alwaysBlendValue == false then
              alwaysBlendValue = getAttribute(node, "deprecated_AlwaysBlendTrajectories")
            end
            removeAttribute(node, "deprecated_AlwaysBlendTrajectories")
          end
        end
        
        -- Update the value of AlwaysBlendTrajectoryAndTransforms
        local alwaysBlendTrajectoryAndTransformsName = string.format("%s.AlwaysBlendTrajectoryAndTransforms", node)
        setAttribute(alwaysBlendTrajectoryAndTransformsName, alwaysBlendValue)
      end--if version < 6
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "Blend2",
    {
      {
        title = "Blend Weights",
        usedAttributes = { "BlendWeights" },
        displayFunc = function(...) safefunc(attributeEditor.blend2WeightDisplayInfoSection, unpack(arg)) end
      },
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
        title = "Blending",
        usedAttributes = { "PositionBlendMode", "RotationBlendMode" },
        displayFunc = function(...) safefunc(attributeEditor.blendModeDisplayInfoSection, unpack(arg)) end,
      },
      {
        title = "Events Blend Mode",
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
        title = "Blend Optimisation",
        usedAttributes = {
          "AlwaysBlendTrajectoryAndTransforms",
          "AlwaysCombineSampledEvents",
        },
        displayFunc = function(...) safefunc(attributeEditor.blendFlagsDisplayInfoSection, unpack(arg)) end
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
-- End of Blend2 node definition.
------------------------------------------------------------------------------------------------------------------------

