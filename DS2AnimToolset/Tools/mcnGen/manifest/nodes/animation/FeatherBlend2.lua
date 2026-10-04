------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- FeatherBlend2 node definition.
------------------------------------------------------------------------------------------------------------------------
require "ui/AttributeEditor/BlendDurationEventDisplayInfo.lua"
require "ui/AttributeEditor/BlendPassThroughDisplayInfo.lua"
require "ui/AttributeEditor/BlendTimeStretchDisplayInfo.lua"
require "ui/AttributeEditor/BlendFlagsDisplayInfo.lua"

registerNode("FeatherBlend2",
  {
    displayName = "Feather Blend 2",
    helptext = "Blends two animation streams together with per bone weighting",
    group = "Blends",
    image = "FeatherBlend2.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 114),
    version = 5,

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
          -- if the pass through mode is set to "Source 0" or "Source 1" then make sure always blend is true.
          if value == 0 or value == 1 then
            local alwaysBlendName = string.format("%s.AlwaysBlendTrajectoryAndTransforms", node)
            setAttribute(alwaysBlendName, true)
            alwaysBlendName = string.format("%s.AlwaysCombineSampledEvents", node)
            setAttribute(alwaysBlendName, true)
          end
        end,
        helptext = "Changes the interface pass through functionality of this node."
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
        name = "AlwaysBlendTrajectoryAndTransforms",
        type = "bool",
        value = false,
        set = function(node, value)
          if not value then
            -- if the pass through mode is set to Source0 or Source1 or either additive blend attribute
            -- is enabled then make sure always blend transforms is true.
            local passThroughMode = getAttribute(node, "PassThroughMode")
            if passThroughMode == 0 or
               passThroughMode == 1 or
               getAttribute(node, "AdditiveBlendAttitude") == true or
               getAttribute(node, "AdditiveBlendPosition") == true then
              local alwaysBlendName = string.format("%s.AlwaysBlendTrajectoryAndTransforms", node)
              setAttribute(alwaysBlendName, true)
            end
          end
        end,
        helptext =
          "Always blend evaluates the trajectory delta and transform value for Source0 and Source1 even if it is "..
          "completely weighted to one input. This option will improve performance if the weight usually falls "..
          "somewhere between 0 and 1 but is not precisely 0 or 1. When this option is disabled it  may improve "..
          "performance if the weight is usually 0 or 1."
      },
      {
        name = "AlwaysCombineSampledEvents",
        type = "bool",
        value = false,
        set = function(node, value)
          if not value then
            -- if the pass through mode is set to Source0 or Source1 or either additive blend attribute
            -- is enabled then make sure always blend is true.
            local passThroughMode = getAttribute(node, "PassThroughMode")
            if passThroughMode == 0 or
               passThroughMode == 1 or
               getAttribute(node, "AdditiveBlendAttitude") == true or
               getAttribute(node, "AdditiveBlendPosition") == true then
              local alwaysBlendName = string.format("%s.AlwaysCombineSampledEvents", node)
              setAttribute(alwaysBlendName, true)
            end
          end
        end,
        helptext =
          "Always blend evaluates the event value for Source0 and Source1 even if it is completely weighted to "..
          "one input. This option will improve performance if the weight usually falls somewhere between 0 and 1 but "..
          "is not precisely 0 or 1. When this option is disabled it  may improve performance if the weight is usually "..
          "0 or 1."
      },
      {
        name = "ChannelAlphas",
        type = "floatArray",
        min = 0,
        max = 1,
        perAnimSet = true,
        syncWithRigChannels = true,
        helptext = "Weighting factors that influence the importance of each channel during the blending process."
      },
      {
        name = "AdditiveBlendAttitude",
        type = "bool",
        value = false,
        set = function(node, value)
          -- if additive blend attitude is true make sure always blend is true.
          if value then
            local alwaysBlendName = string.format("%s.AlwaysBlendTrajectoryAndTransforms", node)
            setAttribute(alwaysBlendName, true)
            alwaysBlendName = string.format("%s.AlwaysCombineSampledEvents", node)
            setAttribute(alwaysBlendName, true)
          end
        end,
        displayName = "Additive Rotation",
        helptext ="Add the rotation of Source1 to Source0 instead of blending with spherical interpolation."
      },
      {
        name = "AdditiveBlendPosition",
        type = "bool",
        value = false,
        set = function(node, value)
          -- if additive blend position is true make sure always blend is true.
          if value then
            local alwaysBlendName = string.format("%s.AlwaysBlendTrajectoryAndTransforms", node)
            setAttribute(alwaysBlendName, true)
            alwaysBlendName = string.format("%s.AlwaysCombineSampledEvents", node)
            setAttribute(alwaysBlendName, true)
          end
        end,
        displayName = "Additive Translation",
        helptext = "Add the translation of Source1 to Source0 instead of blending with linear interpolation."
      },
      {
        name = "SphericallyInterpolateTrajectoryPosition",
        type = "bool",
        value = false,
        displayName = "Slerp Trajectory",
        helptext = "Spherically interpolate between the two trajectory translation inputs. The trajectory orientation is always spherically interpolated."
      },
      {
        name = "StartEventIndex", type = "int", value = 0, min = 0,
        helptext = "The sync event tracks from the inputs are combined here. This is the index of the event on the new synctrack from which to start playback."
      },
      {
        name = "DurationEventBlendPassThrough", type = "bool", value = false,
        helptext = "When set, all events are copied straight to output."
      },
      {
        name = "DurationEventBlendIgnoreEventOrder", type = "bool", value = false,
        helptext = "Ignoring the order of events when blending means that events in the source tracks will be combined even if they have duration events in different orders."
      },
      {
        name = "DurationEventBlendSameUserData", type = "bool", value = false,
        helptext = "Duration events will only be blended together when they share the same user data."
      },
      {
        name = "DurationEventBlendOnOverlap", type = "bool", value = false,
        helptext = "When set, if not overlapping simply copy into the result."
      },
      {
        name = "DurationEventBlendWithinRange", type = "bool", value = false,
        helptext = "When set, blend or reject only events that are within a specified event range of each other, otherwise put straight into the result."
      },
      {
        name = "Loop", type = "bool", value = true,
        helptext = "Sets whether the result of this node loops when it reaches the end of it's sequence."
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
          return nil, string.format("FeatherBlend2 node %s requires a valid input to Source0, node %s is not valid", node, source0Node)
        end
      else
        return nil, string.format("FeatherBlend2 node %s is missing a required connection to Source0", node)
      end

      local source1Pin = string.format("%s.Source1", node)
      if isConnected{ SourcePin  = source1Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = source1Pin, ResolveReferences = true }
        local source1Node = connections[1]
        if not isValid(source1Node) then
          return nil, string.format("FeatherBlend2 node %s requires a valid input to Source1, node %s is not valid", node, source1Node)
        end
      else
        return nil, string.format("FeatherBlend2 node %s is missing a required connection to Source1", node)
      end

      -- Validate blend weight count, as there are only two weights we don't need to check the values.
      local blendWeights = getAttribute(node, "BlendWeights")
      local blendWeightCount = table.getn(blendWeights)
      if blendWeightCount < 2 then
        return false, string.format("FeatherBlend2 node %s has %d BlendWeights, 2 BlendWeights are required", node, blendWeightCount)
      end
      
      -- Validate blend weights are increasing
      if blendWeights[1] > blendWeights[2] then
        return false, string.format("Blend2 node %s has decreasing BlendWeights, increasing BlendWeights are required", node)
      end

      if not getAttribute(node, "AlwaysBlendTrajectoryAndTransforms") then
        if getAttribute(node, "AdditiveBlendAttitude") then
          return false, string.format("FeatherBlend2 node %s has additive rotation checked with Always blend trajectory and transforms disabled", node)
        end

        if getAttribute(node, "AdditiveBlendPosition") then
          return false, string.format("FeatherBlend2 node %s has additive translation checked with Always blend trajectory and transforms disabled", node)
        end

        local passThroughMode = getAttribute(node, "PassThroughMode")
        if passThroughMode == 0 then
          return false, string.format("FeatherBlend2 node %s has PassThroughMode set to Source 0 with Always blend trajectory and transforms disabled", node)
        elseif passThroughMode == 1 then
          return false, string.format("FeatherBlend2 node %s has PassThroughMode set to Source 1 with Always blend trajectory and transforms disabled", node)
        end
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

      -- always blend flags
      --local alwaysBlendTrajectoryAndTransforms = getAttribute(node, "AlwaysBlendTrajectoryAndTransforms")
      --stream:writeBool(alwaysBlendTrajectoryAndTransforms, "AlwaysBlendTrajectoryAndTransforms")
      local alwaysBlendEvents = getAttribute(node, "AlwaysCombineSampledEvents")
      stream:writeBool(alwaysBlendEvents, "AlwaysCombineSampledEvents")
      local eventsBlendMode = getAttribute(node, "EventsBlendMode")
      stream:writeInt(eventsBlendMode, "EventsBlendMode")

      local animSets = listAnimSets()
      local numAnimSets = table.getn(animSets)

      stream:writeUInt(numAnimSets, "NumAnimSets")

      for index, set in animSets do
        local AlphaValues = getAttribute(node, "ChannelAlphas", set)
        local numAlphaValues = table.getn(AlphaValues)

        stream:writeUInt(numAlphaValues, string.format("ChannelAlphasSet%dCount", index - 1))
        for i, v in ipairs(AlphaValues) do
          stream:writeFloat(v, string.format("ChannelAlphasSet%d_Value%d", index - 1, i - 1))
        end
      end

      local additiveBlendAttitude = getAttribute(node, "AdditiveBlendAttitude")
      stream:writeBool(additiveBlendAttitude, "AdditiveBlendAttitude")
      local additiveBlendPosition = getAttribute(node, "AdditiveBlendPosition")
      stream:writeBool(additiveBlendPosition, "AdditiveBlendPosition")
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
      local node0Channels = { }
      local node1Channels = { }
      if isConnected{ SourcePin  = (node .. ".Source0"), ResolveReferences = true } then
        local Source0Table = listConnections{ Object = (node .. ".Source0") , ResolveReferences = true }
        local NodeConnectedTo0 = Source0Table[1]
        node0Channels = anim.getTransformChannels(NodeConnectedTo0, set)
      end

      if isConnected{ SourcePin  = (node .. ".Source1"), ResolveReferences = true } then
        local Source1Table = listConnections{ Object = (node .. ".Source1") , ResolveReferences = true }
        local NodeConnectedTo1 = Source1Table[1]
        node1Channels = anim.getTransformChannels(NodeConnectedTo1, set)
      end

      local resultChannels = setUnion(node0Channels, node1Channels)
      return resultChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    onAttributeInherited = function(nodeName, attributeName, setName)
      if attributeName == "ChannelAlphas" then
        -- init filter channels with default anim set rig size, set all outputs 'true' by default.
        local numChannels = anim.getRigSize(setName)
        local channelIsOutputTable = { }
        for i = 1, numChannels do
          table.insert(channelIsOutputTable, 1)
        end
        setAttribute(nodeName .. ".ChannelAlphas", channelIsOutputTable, setName)
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        -- loop was removed as this was a Blend2 node, when Blend2 and Blend2MatchEvents were
        -- merged in version 3 the loop attribute was re-added so should no longer be removed here.

        -- old FeatherBlend2 upgrade code
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
      
      if version < 4 then
        -- AlwaysBlendTrajectories and AlwaysCombineSampledEvents was added in version 4
        -- Make sure these flags are ON for older nodes so we don't modify the behaviour of existent networks
        local alwaysBlendValue = true
        if attributeExists(node, "deprecated_AlwaysBlend") then
          local passThroughMode = getAttribute(node, "PassThroughMode")
          local additiveBlendAttitude = getAttribute(node, "AdditiveBlendAttitude")
          local additiveBlendPosition = getAttribute(node, "AdditiveBlendPosition")
          if (additiveBlendAttitude  == false) and (additiveBlendPosition  == false) and (passThroughMode == 2) then
            alwaysBlendValue = getAttribute(node, "deprecated_AlwaysBlend")
          end
          removeAttribute(node, "deprecated_AlwaysBlend")
        end

        local alwaysBlendEventsName = string.format("%s.AlwaysCombineSampledEvents", node)
        setAttribute(alwaysBlendEventsName, true)
      end
      
      if version < 5 then
        -- AlwaysBlendTrajectories and AlwaysBlendTransforms were replaced by AlwaysBlendTrajectoryAndTransforms in 
        -- version 6. If either of these are set to true then always blend trajectory and transforms.
        
        -- Make sure these flags are ON for older nodes so we don't modify the behaviour of existent networks
        local alwaysBlendValue = true
        
        if version ~= 4 then
          -- Came from version 3 or lower
          -- Make sure this flag is ON for older nodes so we don't modify the behaviour of existent networks
          alwaysBlendValue = true
          if attributeExists(node, "deprecated_AlwaysBlend") then
            local passThroughMode = getAttribute(node, "PassThroughMode")
            local additiveBlendAttitude = getAttribute(node, "AdditiveBlendAttitude")
            local additiveBlendPosition = getAttribute(node, "AdditiveBlendPosition")
            if (additiveBlendAttitude  == false) and (additiveBlendPosition  == false) and (passThroughMode == 2) then
              alwaysBlendValue = getAttribute(node, "deprecated_AlwaysBlend")
            end
            removeAttribute(node, "deprecated_AlwaysBlend")
          end
        else
          -- Version from == 4
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
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- FeatherBlend2 custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "FeatherBlend2",
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
        title = "Channel Alphas",
        usedAttributes = { "ChannelAlphas" },
        displayFunc = function(...) safefunc(attributeEditor.channelNameDisplayInfoSection, unpack(arg)) end
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
          "AdditiveBlendAttitude",
          "AdditiveBlendPosition",
          "SphericallyInterpolateTrajectoryPosition",
        },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end
------------------------------------------------------------------------------------------------------------------------
-- End of FeatherBlend2 node definition.
------------------------------------------------------------------------------------------------------------------------
