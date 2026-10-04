------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/BlendDurationEventDisplayInfo.lua"
require "ui/AttributeEditor/SourceWeightDisplayInfo.lua"
require "ui/AttributeEditor/BlendFlagsDisplayInfo.lua"

local isSettingSourceWeights = false

------------------------------------------------------------------------------------------------------------------------
-- BlendN node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("BlendN",
  {
    displayName = "Blend N",
    helptext = "Blends N animation streams together",
    group = "Blends",
    image = "BlendN.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 108),
    version = 10,

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
      ["Source4"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source5"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source6"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source7"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source8"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source9"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        },
      },
      ["Source10"] = {
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

    pinOrder =
    {
      "Source0",
      "Source1",
      "Source2",
      "Source3",
      "Source4",
      "Source5",
      "Source6",
      "Source7",
      "Source8",
      "Source9",
      "Source10",
      "Result",
      "Weight",
    },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "SourceWeights",
        type = "floatArray",
        value = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        set = function(node, value)
          if isSettingSourceWeights then return end
          isSettingSourceWeights = true
          
          rebuildSourceWeights(node, false)          
          
          isSettingSourceWeights = false
        end,
        helptext = "Describes the mapping between the weight and the animation sources"
      },
      {
        -- This attribute is internal data and should never be displayed to end-users
        name = "SourceWeightDistributionRange",
        type = "floatArray",
        value = { 0, 1 },
        helptext = "This is used to keep the same range for linear distribution of weights"
      },      
      {
        name = "SourceWeightDistribution",
        type = "int",
        value = 1, -- [0] = "Custom", [1] = "Linear", [2] = "Integer"
        set = function(node, value)
          rebuildSourceWeights(node, true)
        end,
        displayName = "Distribution",
        helptext = "Defines how the input to the weight pin blends between the input nodes"
      },
      {
        name = "WrapWeights",
        type = "bool",
        value = false,
        set = function(node, value)
          rebuildSourceWeights(node, true)
        end,
        displayName = "Wrap Weights",
        helptext = "Interpolate across the last animation back to the first one"
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
        name = "SphericallyInterpolateTrajectoryPosition",
        type = "bool",
        value = false,
        displayName = "Slerp Trajectory",
        helptext = "Spherically interpolate between the two trajectory translation inputs. The trajectory orientation is always spherically interpolated.",
      },
      {
        name = "AlwaysBlendTrajectoryAndTransforms",
        type = "bool",
        value = false,
        helptext =
          "Always blend evaluates the trajectory delta and transform buffers for the two active sources even if it is "..
          "completely weighted to one source. This option may improve performance if the weight usually falls somewhere "..
          "between source weights but does not precisely fall on a source weight. When this option is disabled it may "..
          "improve performance if the weight usually falls precisely on a source weight."
      },
      {
        name = "AlwaysCombineSampledEvents",
        type = "bool",
        value = false,
        helptext =
          "Always blend evaluate the events buffers for the two active sources even if it is completely weighted to "..
          "one source. This option may improve performance if the weight usually falls somewhere between source "..
          "weights but does not precisely fall on a source weight. When this option is disabled it may improve "..
          "performance if the weight usually falls precisely on a source weight."
      },
      {
        name = "Loop",
        type = "bool",
        value = true,
        helptext = "Sets whether the result of this node loops when it reaches the end of it's sequence."
      },
      {
        name = "StartEventIndex",
        type = "int",
        value = 0,
        min = 0,
        helptext = "The sync event tracks from the inputs are combined here. This is the index of the event on the new synctrack from which to start playback."
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
    },

    --------------------------------------------------------------------------------------------------------------------
    updatePins = function(node)
      local timeStretchMode = getAttribute(node, "TimeStretchMode")

      for index = 0, 10 do
        local sourcePin = string.format("%s.Source%d", node, index)

        if timeStretchMode == 0 then
          -- no time stretching
          removePinInterfaces(sourcePin, { "Events", })
        else
          addPinInterfaces(sourcePin, { "Events", })
        end
      end

      local resultPin = string.format("%s.Result", node)
      if timeStretchMode == 0 then
        -- no time stretching
        removePinInterfaces(resultPin, { "Events", })
      else
        addPinInterfaces(resultPin, { "Events", })
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      -- validate the weight pin connection
      local weightPin = string.format("%s.Weight", node)
      if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightPin, ResolveReferences = true }
        local weightNode = connections[1]
        if not isValid(weightNode) then
          return nil, string.format("BlendN node %s requires a valid input to Weight, node %s is not valid", node, weightNode)
        end
      else
        return nil, string.format("BlendN node %s is missing a required connection to Weight", node)
      end

      -- validate the source pins connections
      local connectedPinCount = 0
      for index = 0, 10 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
          -- if the connectedPinCount is not equal to index then there were some unconnected pins before this point
          if connectedPinCount == index then
            connectedPinCount = connectedPinCount + 1
          else
            error = true
            if connectedPinCount > 0 then
              return nil, string.format("BlendN node %s has sparsely connected pins, ensure that there are not gaps between connected pins", node)
            else
              return nil, string.format("BlendN node %s has unconnected pins before pin Source%d, ensure that the connected pins start at Source0 and there are no gaps between connected pins", node, index)
            end
          end

          local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
          local sourceNode = connections[1]
          if not isValid(sourceNode) then
            return nil, string.format("BlendN node %s requires a valid input to Source%d, node %s is not valid", node, index, sourceNode)
          end
        end
      end

      if connectedPinCount < 2 then
        return nil, string.format("BlendN node %s requires at least two connections to pins Source0 and Source1", node)
      end

      local sourceWeightCount = connectedPinCount

      -- Validate source weight values.
      local wrapWeights = getAttribute(node, "WrapWeights")
      if wrapWeights then
        sourceWeightCount = sourceWeightCount + 1
      end

      local sourceWeights = getAttribute(node, "SourceWeights")
      if table.getn(sourceWeights) < sourceWeightCount then
        return nil, string.format("BlendN node %s has %d connected pins but only %d source weights.", node, connectedPinCount, sourceWeights)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      -- write out the id of the node connected to the weight parameter
      local weightNodeInfo = getConnectedNodeInfo(node, "Weight")
      stream:writeNetworkNodeId(weightNodeInfo.id, "Weight", weightNodeInfo.pinIndex)

      -- write connected node runtime ids and source weights
      local sourceWeights = getAttribute(node, "SourceWeights")

      local count = 11 -- This will be left set if we never find an unconnected pin.  11 is the max number of inputs
      for index = 0, 10 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
          local sourceNodeID = getConnectedNodeID(sourcePin)
          stream:writeNetworkNodeId(sourceNodeID, string.format("Source%dNodeID", index))
          stream:writeFloat(sourceWeights[index + 1], string.format("SourceWeight_%d", index))
        else
          count = index
          break
        end
      end

      -- now write out the number of connected pins
      stream:writeInt(count, "SourceNodeCount")

      -- Write the wrap around weight if that behavior is enabled
      local wrapWeights = getAttribute(node, "WrapWeights")
      stream:writeBool(wrapWeights, "WrapWeights")
      if wrapWeights then
        stream:writeFloat(sourceWeights[count + 1], "WrapWeight")
      end

      local timeStretchMode = getAttribute(node, "TimeStretchMode")
      stream:writeInt(timeStretchMode, "TimeStretchMode")

      -- do we want this node to spherically interpolate the input delta trajectory translations.
      local sphericalTrajPos = getAttribute(node, "SphericallyInterpolateTrajectoryPosition")
      stream:writeBool(sphericalTrajPos, "SphericallyInterpolateTrajectoryPosition")

      -- Always blend flags
      --local alwaysBlendTrajectoryAndTransforms = getAttribute(node, "AlwaysBlendTrajectoryAndTransforms")
      --stream:writeBool(alwaysBlendTrajectoryAndTransforms, "AlwaysBlendTrajectoryAndTransforms")
      local alwaysBlendEvents = getAttribute(node, "AlwaysCombineSampledEvents")
      stream:writeBool(alwaysBlendEvents, "AlwaysCombineSampledEvents")

      -- Which event to start playback of this blend on.
      local startEventIndex = getAttribute(node, "StartEventIndex")
      stream:writeInt(startEventIndex, "StartEventIndex")

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

      -- do we want this nodes output to loop.
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

      for index = 1, 10 do
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
    onPinConnected = function(nodeName, pinName)
      rebuildSourceWeights(nodeName, true)
    end,
    
    onPinDisconnected = function(nodeName, pinName)
      rebuildSourceWeights(nodeName, true)
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        -- loop was removed as this was a Blend2 node, when Blend2 and Blend2MatchEvents were
        -- merged in version 3 the loop attribute was re-added so should no longer be removed here.

        -- old BlendNMatchEvents upgrade code
        local value = getAttribute(node, "deprecated_DurationEventBlendInSequence")
        setAttribute(string.format("%s.DurationEventBlendIgnoreEventOrder", node), not value)
        removeAttribute(node, "deprecated_DurationEventBlendInSequence")
      end

      if version < 5 then
        -- make sure we always have the maximum number of source weights, currently 11 + 1 for wrap
        local blendWeights = getAttribute(node, "SourceWeights")
        local numBlendWeights = table.getn(blendWeights)
        if numBlendWeights < 12 then
          upgradeBlendWeights(node)
        end
      end

      if version < 6 then
        local timeStretchModeName = string.format("%s.TimeStretchMode", node)
        setAttribute(timeStretchModeName, 0)
      end
      
      if version < 8 then
        -- TODO: Check whether we are 'linear' and use that instead of 'custom'
        local distTypeName = string.format("%s.SourceWeightDistribution", node)
        -- Default to Custom
        setAttribute(distTypeName, 0)

        local value = getAttribute(node, "deprecated_BlendWeights")
        setAttribute(string.format("%s.SourceWeights", node), value)
        removeAttribute(node, "deprecated_BlendWeights")

        -- Ensure that the attributes are all set consistently (mainly the distribution range)
        rebuildSourceWeights(node, false)
      end
      
      if version < 9 then
      
        -- AlwaysCombineSampledEvents was added in version 9
        -- Make sure this flag is ON for older nodes so we don't modify the behaviour of existent networks
        local alwaysBlendEventsName = string.format("%s.AlwaysCombineSampledEvents", node)
        setAttribute(alwaysBlendEventsName, true)
      end
      
      if version < 10 then

        if version ~= 9 then
          -- Came from version 8 or lower
          -- Make sure this flag is ON for older nodes so we don't modify the behaviour of existent networks
          alwaysBlendValue = true
          if attributeExists(node, "deprecated_AlwaysBlend") then
            alwaysBlendValue = getAttribute(node, "deprecated_AlwaysBlend")
            removeAttribute(node, "deprecated_AlwaysBlend")
          end
        else
          -- Version from == 9
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
      end
      
    end
  }
)

------------------------------------------------------------------------------------------------------------------------
-- BlendN custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "BlendN",
    {
      {
        title = "Source Weights",
        usedAttributes = { 
          "SourceWeights", 
          "SourceWeightDistributionRange",
          "SourceWeightDistribution", 
          "WrapWeights",
      },
        displayFunc = function(...) safefunc(attributeEditor.sourceWeightDisplayInfoSection, unpack(arg)) end
      },
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
