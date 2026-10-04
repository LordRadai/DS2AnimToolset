------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/BlendDurationEventDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- BlendAll node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("BlendAll",
  {
	displayName = "Blend All",
    helptext = "Blends all the input animations together",
    group = "Blends",
    image = "BlendAll.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 169),
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
      ["Weight0"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight1"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight2"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight3"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight4"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight5"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight6"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight7"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight8"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight9"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight10"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight11"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight12"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight13"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight14"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Weight15"] = {
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
      "Weight0",
      "Weight1",
      "Weight2",
      "Weight3",
      "Weight4",
      "Weight5",
      "Weight6",
      "Weight7",
      "Weight8",
      "Weight9",
      "Weight10",
      "Weight11",
      "Weight12",
      "Weight13",
      "Weight14",
      "Weight15",
      "Result",
    },

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
              return nil, string.format("BlendAll node %s has sparsely connected pins, ensure that there are not gaps between connected pins", node)
            else
              return nil, string.format("BlendAll node %s has unconnected pins before pin Source%d, ensure that the connected pins start at Source0 and there are no gaps between connected pins", node, index)
            end
          end

          local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
          local sourceNode = connections[1]
          if not isValid(sourceNode) then
            return nil, string.format("BlendAll node %s requires a valid input to Source%d, node %s is not valid", node, index, sourceNode)
          end
        end
      end

      if connectedPinCount < 2 then
        return nil, string.format("BlendAll node %s requires at least two connections", node)
      end
      
      -- validate the weight pins connections
      local weightPinCount = 0
      for index = 0, 15 do
        local weightPin = string.format("%s.Weight%d", node, index)
        if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
            weightPinCount = weightPinCount + 1
        end
      end
      if weightPinCount ~= connectedPinCount then
        return nil, string.format("BlendAll node %s requires a weight parameter per source connected", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      
      -- Source inputs
      local count = 0
      for index = 0, 15 do
        local sourcePin = string.format("%s.Source%d", node, index)
        if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
          local sourceNodeID = getConnectedNodeID(sourcePin)
          stream:writeNetworkNodeId(sourceNodeID, string.format("Source%dNodeID", index))
          count = count + 1
        else
          break
        end
      end

      -- Write out the number of connected pins
      stream:writeInt(count, "SourceNodeCount")
      
      -- Write out the id of the node connected to the weight parameters
      for index = 0, 15 do
        local weightPin = string.format("%s.Weight%d", node, index)
        if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
          local weightNodeID = getConnectedNodeInfo(node, string.format("Weight%d", index))
		  stream:writeNetworkNodeId(weightNodeID.id, string.format("Weight%d", index), weightNodeID.pinIndex)
        else
          break
        end
      end

      local timeStretchMode = getAttribute(node, "TimeStretchMode")
      stream:writeInt(timeStretchMode, "TimeStretchMode")

      -- Write out Spherical Interpolation attribute
      local sphericalTrajPos = getAttribute(node, "SphericallyInterpolateTrajectoryPosition")
      stream:writeBool(sphericalTrajPos, "SphericallyInterpolateTrajectoryPosition")

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
    end
  }
)

------------------------------------------------------------------------------------------------------------------------
-- BlendAll custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "BlendAll",
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

