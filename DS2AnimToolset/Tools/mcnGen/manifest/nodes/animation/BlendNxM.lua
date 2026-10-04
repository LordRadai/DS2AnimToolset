------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/BlendWeightNxMDisplayInfo.lua"
require "ui/AttributeEditor/BlendDurationEventDisplayInfo.lua"
require "ui/AttributeEditor/BlendFlagsDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- BlendNxM node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("BlendNxM",
  {
    displayName = "Blend NxM",
    helptext = "Blends NxM animation streams together (up to 4x4, 5x3 or 3x5)",
    group = "Blends",
    image = "BlendNxM.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 147),
    version = 4,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source0"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source1"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source2"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source3"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source4"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source5"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source6"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source7"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source8"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source9"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source10"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source11"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source12"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source13"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source14"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Source15"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
      },
      ["Result"] = {
        input = false,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { "Events", },
        }
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
      "Source11",
      "Source12",
      "Source13",
      "Source14",
      "Source15",
      "WeightX",
      "WeightY",
      "Result",
    },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "BlendWeightsX",
        type = "floatArray",
        value = { 0, 1, 0, 0 },
        helptext = "Describes the mapping between the weightX and the animation sources"
      },
      {
        name = "BlendWeightsY",
        type = "floatArray",
        value = { 0, 1, 0, 0 },
        helptext = "Describes the mapping between the weightY and the animation sources"
      },
      {
        name = "XValue",
        type = "int",
        value = 2,
        min = 2,
        max = 5,
        set = function(node, value)
          -- if the number of rows is set to 2 or 4 then make sure the number of columns is not 5.
          if (value == 4) then
            local yValue = getAttribute(node, "YValue")
            if (yValue == 5) then
              local yValueName = string.format("%s.YValue", node)
              setAttribute(yValueName, 4)
            end
          end
          -- if the number of rows is set to 5 then make sure the number of columns is  3.
          if (value == 5) then
            local yValue = getAttribute(node, "YValue")
            if (yValue ~= 3) and yValue ~= 2 then
              local yValueName = string.format("%s.YValue", node)
              setAttribute(yValueName, 3)
            end
          end
        end,
        helptext = "X value (rows) of the NxM matrix"
      },
      {
        name = "YValue",
        type = "int",
        value = 2,
        min = 2,
        max = 5,
        set = function(node, value)
          -- if the number of columns is set to 2 or 4 then make sure the number of rows is not 5.
          if (value == 4) then
            local xValue = getAttribute(node, "XValue")
            if (xValue == 5) then
              local xValueName = string.format("%s.XValue", node)
              setAttribute(xValueName, 4)
            end
          end
          -- if the number of columns is set to 5 then make sure the number of rows is 3.
          if (value == 5) then
            local xValue = getAttribute(node, "XValue")
            if (xValue ~= 3) and (xValue ~= 2) then
              local xValueName = string.format("%s.XValue", node)
              setAttribute(xValueName, 3)
            end
          end
        end,
        helptext = "Y value (columns) of the NxM matrix"
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
        helptext = "Spherically interpolate between the two trajectory translation inputs. The trajectory orientation is always spherically interpolated."
      },
      {
        name = "AlwaysBlendTrajectoryAndTransforms",
        type = "bool",
        value = false,
        helptext =
          "Always blend evaluates the trajectory delta and transform value for Source0 and Source1 even if it is "..
          "completely weighted to one input. This option will improve performance if the weight usually falls "..
          "somewhere between 0 and 1 but is not precisely 0 or 1. When this option is disabled it  may improve "..
          "performance if the weight is usually 0 or 1."
      },
      {
        name = "AlwaysBlendEvents",
        type = "bool",
        value = false,
        helptext =
          "Always blend evaluates the event value for Source0 and Source1 even if it is completely weighted to "..
          "one input. This option will improve performance if the weight usually falls somewhere between 0 and 1 but "..
          "is not precisely 0 or 1. When this option is disabled it  may improve performance if the weight is usually "..
          "0 or 1."
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
      {
        name = "Loop",
        type = "bool",
        value = true,
        helptext = "Sets whether the result of this node loops when it reaches the end of it's sequence."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    updatePins = function(node)
      local timeStretchMode = getAttribute(node, "TimeStretchMode")

      for index = 0, 15 do
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
      -- validate the weight pin connections
      local weightXPin = string.format("%s.WeightX", node)
      if isConnected{ SourcePin = weightXPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightXPin, ResolveReferences = true }
        local weightXNode = connections[1]
        if not isValid(weightXNode) then
          return nil, string.format("BlendN node %s requires a valid input to WeightX, node %s is not valid", node, weightXNode)
        end
      else
        return nil, string.format("BlendNxM node %s is missing a required connection to WeightX", node)
      end

      local weightYPin = string.format("%s.WeightY", node)
      if isConnected{ SourcePin = weightYPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightYPin, ResolveReferences = true }
        local weightYNode = connections[1]
        if not isValid(weightYNode) then
          return nil, string.format("BlendN node %s requires a valid input to WeightY, node %s is not valid", node, weightYNode)
        end
      else
        return nil, string.format("BlendNxM node %s is missing a required connection to WeightY", node)
      end

      -- validate the source pins connections
      local connectedPinCount = 0
      for index = 0, 15 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
          -- if the connectedPinCount is not equal to index then there were some unconnected pins before this point
          if connectedPinCount == index then
            connectedPinCount = connectedPinCount + 1
          else
            error = true
            if connectedPinCount > 0 then
              return nil, string.format("BlendNxM node %s has sparsely connected pins, ensure that there are not gaps between connected pins", node)
            else
              return nil, string.format("BlendNxM node %s has unconnected pins before pin Source%d, ensure that the connected pins start at Source0 and there are no gaps between connected pins", node, index)
            end
          end

          local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
          local sourceNode = connections[1]
          if not isValid(sourceNode) then
            return nil, string.format("BlendNxM node %s requires a valid input to Source%d, node %s is not valid", node, index, sourceNode)
          end
        end
      end

      if connectedPinCount < 4 then
        return nil, string.format("BlendNxM node %s requires at least four connections", node)
      end

      -- Validate X & Y values depending on the number of pins connected
      local nodeCountX = getAttribute(node, "XValue")
      local nodeCountY = getAttribute(node, "YValue")
      if (nodeCountX * nodeCountY) > 16 then
        return nil, string.format("BlendNxM node only supports 16 connected pins. Please modified the X and Y values.", node, connectedPinCount, nodeCountX * nodeCountY)
      end
      if (nodeCountX * nodeCountY) ~= connectedPinCount then
        return nil, string.format("BlendNxM node %s number of connected pins %d does not match the expected number %d", node, connectedPinCount, nodeCountX * nodeCountY)
      end

      -- Validate blend weight X values.
      local blendWeightsX = getAttribute(node, "BlendWeightsX")
      local previousWeightX = blendWeightsX[1]
      for i = 2, nodeCountX do
        local thisWeight = blendWeightsX[i]

        if thisWeight < previousWeightX then
          return nil, string.format("BlendNxM node %s requires BlendWeightsX with increasing values.", node)
        end

        previousWeightX = thisWeight
      end

      -- Validate blend weight Y values.
      local blendWeightsY = getAttribute(node, "BlendWeightsY")
      local previousWeightY = blendWeightsY[1]
      for i = 2, nodeCountY do
        local thisWeight = blendWeightsY[i]

        if thisWeight < previousWeightY then
          return nil, string.format("BlendNxM node %s requires BlendWeightsY with increasing values.", node)
        end

        previousWeightY = thisWeight
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      -- write out the id of the node connected to the weight parameters
      local weightXNodeID = getConnectedNodeInfo(node, "WeightX")
      stream:writeNetworkNodeId(weightXNodeID.id, "WeightX", weightXNodeID.pinIndex)
      
      local weightYNodeID = getConnectedNodeInfo(node, "WeightY")
      stream:writeNetworkNodeId(weightYNodeID.id, "WeightY", weightYNodeID.pinIndex)

      for index = 0, 15 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
          local sourceNodeID = getConnectedNodeID(sourcePin)
          stream:writeNetworkNodeId(sourceNodeID, string.format("Source%dNodeID", index))
        else
          break
        end
      end

      local nodeCountX = getAttribute(node, "XValue")
      stream:writeUInt(nodeCountX, "NodeCountX")

      local nodeCountY = getAttribute(node, "YValue")
      stream:writeUInt(nodeCountY, "NodeCountY")

      -- Write out blend weights
      local blendWeightsX = getAttribute(node, "BlendWeightsX")
      local blendWeightsY = getAttribute(node, "BlendWeightsY")
      local index = 0
      for x = 1, nodeCountX do
       for y = 1, nodeCountY do
          stream:writeFloat(blendWeightsX[x], string.format("BlendWeightX_%d", index))
          stream:writeFloat(blendWeightsY[y], string.format("BlendWeightY_%d", index))
          index = index + 1
        end
      end

      local timeStretchMode = getAttribute(node, "TimeStretchMode")
      stream:writeInt(timeStretchMode, "TimeStretchMode")

      -- Write out Spherical Interpolation attribute
      local sphericalTrajPos = getAttribute(node, "SphericallyInterpolateTrajectoryPosition")
      stream:writeBool(sphericalTrajPos, "SphericallyInterpolateTrajectoryPosition")

      -- always blend flags
      --local alwaysBlendTrajectoryAndTransforms = getAttribute(node, "AlwaysBlendTrajectoryAndTransforms")
      --stream:writeBool(alwaysBlendTrajectoryAndTransforms, "AlwaysBlendTrajectoryAndTransforms")
      local alwaysBlendEvents = getAttribute(node, "AlwaysBlendEvents")
      stream:writeBool(alwaysBlendEvents, "AlwaysCombineSampledEvents")

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

      for index = 1, 15 do
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
      
      if version < 3 then
        -- AlwaysBlendEvents was added in version 3
        -- Make sure this flag is ON for older nodes so we don't modify the behaviour of existent networks
        local alwaysBlendEventsName = string.format("%s.AlwaysBlendEvents", node)
        setAttribute(alwaysBlendEventsName, true)
      end
      
      if version < 4 then
        -- AlwaysBlendTrajectories and AlwaysBlendTransforms were replaced by AlwaysBlendTrajectoryAndTransforms in 
        -- version 6. If either of these are set to true then always blend trajectory and transforms.
        
        -- Make sure these flags are ON for older nodes so we don't modify the behaviour of existent networks
        local alwaysBlendValue = true
        
        if version ~= 3 then
          -- Came from version 3 or lower
          -- Make sure this flag is ON for older nodes so we don't modify the behaviour of existent networks
          alwaysBlendValue = true
          if attributeExists(node, "deprecated_AlwaysBlend") then
            alwaysBlendValue = getAttribute(node, "deprecated_AlwaysBlend")
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
    end
  }
)

------------------------------------------------------------------------------------------------------------------------
-- BlendNxM custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "BlendNxM",
    {
      {
        title = "Blend Weights",
        usedAttributes = { "BlendWeightsX",  "BlendWeightsY",  "XValue", "YValue" },
        displayFunc = function(...) safefunc(attributeEditor.blendWeightNxMDisplayInfoSection, unpack(arg)) end
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
          "AlwaysBlendEvents",
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

