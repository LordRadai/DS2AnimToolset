------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorFloatsToVector3 node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("OperatorVector3ToFloats",
  {
    displayName = "Vec3 To Floats",
    helptext = "Simple Node that takes a vector3 and outputs the specified float value",
    group = "Vector3 Operators",
    image = "OperatorVector3ToFloat.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 154),
    version = 3,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input"] = {
        input = true,
        array = false,
        type = "vector3",
      },
      ["FloatX"] = {
        input = false,
        array = false,
        type = "float",
      },
      ["FloatY"] = {
        input = false,
        array = false,
        type = "float",
      },
      ["FloatZ"] = {
        input = false,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Input", "FloatX", "FloatY", "FloatZ" },
   
    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local inVector3Pin = string.format("%s.Input", node)
      if isConnected{ SourcePin  = inVector3Pin, ResolveReferences = true } then
        local connections = listConnections{ Object = inVector3Pin, ResolveReferences = true }
        local inVector3Node = connections[1]
        if not isValid(inVector3Node) then
          return nil, string.format("OperatorVector3ToFloats node %s requires a valid input to Input, node %s is not valid", node, inVector3Node)
        end
      else
        return nil, string.format("OperatorVector3ToFloats node %s is missing a required connection to Source0", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      --Export Connections
      local inVector3NodeInfo = getConnectedNodeInfo(node, "Input")
      stream:writeNetworkNodeId(inVector3NodeInfo.id, "Input", inVector3NodeInfo.pinIndex)
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
      if attributeExists(node, "deprecated_ExtractXValue") then
        -- ExtractXValue, ExtractYValue and ExtractZValue boolean attributes were removed and
        -- replaced with one int value OutputValue

        local extractXValue = getAttribute(node, "deprecated_ExtractXValue")
        local extractYValue = getAttribute(node, "deprecated_ExtractYValue")
        local extractZValue = getAttribute(node, "deprecated_ExtractZValue")

        local oldResult = string.format("%s.Result", node)
        if extractYValue then
          pinLookupTable[oldResult] = string.format("%s.FloatY", node)
          
        elseif extractZValue then
          pinLookupTable[oldResult] = string.format("%s.FloatZ", node)
          
        else
          -- connect to X by default.
          pinLookupTable[oldResult] = string.format("%s.FloatX", node)
        end
        
        removeAttribute(node, "deprecated_ExtractXValue")
        removeAttribute(node, "deprecated_ExtractYValue")
        removeAttribute(node, "deprecated_ExtractZValue")
      end
      
      -- upgrade OperatorVector3ToFloat to OperatorVector3ToFloats
      if attributeExists(node, "deprecated_OutputValue") then
        local value = getAttribute(node, "deprecated_OutputValue")
        removeAttribute(node, "deprecated_OutputValue")
        
        local oldResult = string.format("%s.Result", node)
        if value == 1 then
          pinLookupTable[oldResult] = string.format("%s.FloatX", node)
        elseif value == 2 then
          pinLookupTable[oldResult] = string.format("%s.FloatY", node)
        elseif value == 3 then
          pinLookupTable[oldResult] = string.format("%s.FloatZ", node)
        end
      end
      end
      if version < 3 then
        local oldSourcePath = string.format("%s.InVector3", node)
        local newSourcePath = string.format("%s.Input", node)
        pinLookupTable[oldSourcePath] = newSourcePath
      end

    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorVector3ToFloats",
    {
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorFloatsToVector3 node definition.
------------------------------------------------------------------------------------------------------------------------
