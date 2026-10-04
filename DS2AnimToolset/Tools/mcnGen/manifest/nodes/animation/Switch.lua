------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/SourceWeightDisplayInfo.lua"
require "luaAPI/RebuildSourceWeights.lua"

local isSettingSourceWeights = false

------------------------------------------------------------------------------------------------------------------------
-- Switch node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("Switch",
  {
    displayName = "Switch",
    helptext = "Handles switching between n source nodes",
    group = "Utilities",
    image = "Switch.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 131),
    version = 5,

    functionPins =
    {
      ["Source0"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source1"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source2"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source3"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source4"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source5"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source6"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source7"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source8"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source9"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Source10"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { },
          optional = { },
        },
      },
    },

    dataPins =
    {
      ["Weight"] = {
        input = true,
        array = false,
        type = "float",
      }
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
        displayName = "Source Weight Distribution",
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
        name = "InputSelectionMethod",
        type = "int",
        value = 0, -- [0] = "Closest", [1] = "Floor", [2] = "Ceiling"
        min = 0,
        max = 2,
        affectsPins = false,
        helptext = "If true the blend weight is rounded to the closest source blend weight. By default the blend weight value is floored."
      },
      {
        name = "EvaluationMethod",
        type = "int",
        value = 2, -- [0] = "EvalWhenActivating", [1] = "EvalEveryFrame", [2] = "EvalOnLoop"
        min = 0,
        max = 2,
        helptext = "If true the value of the weight pin is evaluated every frame potentially switching which input is used every frame. If false the value of the weight pin is only evaluated when the current input animation has finished so the animation only changes at the end of playback."
      }
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      -- validate the weight pin connection
      local weightPin = string.format("%s.Weight", node)
      if isConnected{ SourcePin = weightPin, ResolveReferences = true } then
        local connections = listConnections{ Object = weightPin, ResolveReferences = true }
        local weightNode = connections[1]
        if not isValid(weightNode) then
          return nil, string.format("Switch node %s requires a valid input to Weight, node %s is not valid", node, weightNode)
        end
      else
        return nil, string.format("Switch node %s is missing a required connection to Weight", node)
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
              return nil, string.format("Switch node %s has sparsely connected pins, ensure that there are not gaps between connected pins", node)
            else
              return nil, string.format("Switch node %s has unconnected pins before pin Source%d, ensure that the connected pins start at Source0 and there are no gaps between connected pins", node, index)
            end
          end

          local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
          local sourceNode = connections[1]
          if not isValid(sourceNode) then
            return nil, string.format("Switch node %s requires a valid input to Source%d, node %s is not valid", node, index, sourceNode)
          end
        end
      end

      if connectedPinCount < 2 then
        return nil, string.format("Switch node %s requires at least two connections to pins Source0 and Source1", node)
      end

      local sourceWeightCount = connectedPinCount

      -- Validate source weight values.
      local wrapWeights = getAttribute(node, "WrapWeights")
      if wrapWeights then
        sourceWeightCount = sourceWeightCount + 1
      end

      local sourceWeights = getAttribute(node, "SourceWeights")
      if table.getn(sourceWeights) < sourceWeightCount then
        return nil, string.format("Switch node %s has %d connected pins but only %d source weights.", node, connectedPinCount, sourceWeights)
      end
      
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      -- first write out all the parameter data
      local evalEveryFrame = getAttribute(node, "EvaluationMethod")

      stream:writeUInt(evalEveryFrame, "EvaluationMethod")
      local InputSelectionMethod = getAttribute(node, "InputSelectionMethod")
      stream:writeUInt(InputSelectionMethod,"InputSelectionMethod")

      -- next write out the id of the node connected to the weight parameter
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
        local evalAtEndOfAnim = getAttribute(node, "deprecated_EvaluateAtEndOfAnimation")
        if evalAtEndOfAnim ~= nil then
          local evalEveryFrame = not evalAtEndOfAnim
          setAttribute(string.format("%s.EvalEveryFrame", node), evalEveryFrame)
          removeAttribute(node, "deprecated_EvaluateAtEndOfAnimation")
        end
      end
      if version < 3 then
        removeAttribute(node, "deprecated_Loop")
      end
      if version < 4 then
        local resultPath = string.format("%s.Result", node)
        setPinPassThrough(resultPath, true)
        for i = 0, 10 do
          local sourcePath = string.format("%s.Source%d", node, i)
          setPinPassThrough(sourcePath, true)
        end
      end
      if version < 5 then
        -- Default to Linear and force a weight update
        local distTypeName = string.format("%s.SourceWeightDistribution", node)
        setAttribute(distTypeName, 1)
        local blendWeights = getAttribute(node, "SourceWeights")
        blendWeights[countContiguousInputs(node)] = 1.0
        setAttribute(node .. ".SourceWeights", blendWeights)
        rebuildSourceWeights(node,false)
      end      
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local results = { }
      for currentPin = 0, 10 do
        local sourcePin = string.format("%s.Source%s", node, currentPin)
        if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then
          local connections = listConnections{ Object = sourcePin, ResolveReferences = true }
          local sourceNode = connections[1]
          local channels = anim.getTransformChannels(sourceNode, set)
          if currentPin == 0 then
            results = channels
          else
            results = setUnion(results, channels)
          end
        else
          break
        end
      end

      return results
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "Switch",
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
        title = "Properties",
        usedAttributes = {
          "EvalEveryFrame", 
          "InputSelectionMethod",
        },
        displayFunc = function(...) safefunc(attributeEditor.switchDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end