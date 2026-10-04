------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorreRange node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorReRange",
  {
    displayName = "ReRange",
    helptext = "Simple mathematical operation where an input range is converted to an output range",
    group = "Float Operators",
    image = "OperatorReRange.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 112),
    version = 3,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input"] = {
        input = true,
        array = false,
        type = "float",
      },
      ["Output"] = {
        input = false,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Input", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      { name = "InputRange1",
        type = "float",
        value = 0.0,
        helptext = "First range value which maps to OutputRange1"
      },
      { name = "InputRange2",
        type = "float",
        value = 1.0,
        helptext = "Second range value which maps to OutputRange2"
      },
      { name = "OutputRange1",
        type = "float",
        value = 0.0,
        helptext = "First output, mapped from InputRange1"
      },
      { name = "OutputRange2",
        type = "float",
        value = 1.0,
        helptext = "Second output, mapped from InputRange2"
      },
    },
    
    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local inputPin = node .. ".Input"
      if isConnected{ SourcePin = inputPin, ResolveReferences = true } then
		    local nodesConnected = listConnections{ Object = inputPin, ResolveReferences = true }
		    local inputNode = nodesConnected[1]
        if not isValid(inputNode) then
          return nil, "Operator OperatorReRange not valid"
        end
		    if (getAttribute(node, "InputRange1") == getAttribute(node, "InputRange2")) then
			    return nil, "The input range values must be different from each other in order to re-range"
		    end
	      return true
      else
        return nil, ("OperatorReRange node " .. node .. " is missing a required connection to Input")
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputInfo = getConnectedNodeInfo(node .. ".Input")
      local operationCode = 7

      if (inputInfo == nil) then
        Stream:writeNetworkNodeId(-1, "Input")
      else
        Stream:writeNetworkNodeId(inputInfo.id, "Input", inputInfo.pinIndex)
      end
      local pinType = getDataPinType(node, "Input")
      --Stream:writeBool(true, "IsScalar")
      Stream:writeInt(6, "OperationCode")
      local d1 = getAttribute(node, "InputRange1")
      local d2 = getAttribute(node, "InputRange2")
      local r1 = getAttribute(node, "OutputRange1")
      local r2 = getAttribute(node, "OutputRange2")
      local times = (r2-r1)/(d2-d1)
      local add = r1 - (d1*times)
      Stream:writeFloat(times,  "ConstantValue")
      Stream:writeFloat(add, "ConstantValueX")
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorReRange",
    {
      {
        title = "Map Range",
        usedAttributes = {
          "InputRange1", "InputRange2",
          "OutputRange1", "OutputRange2",
        },
        displayFunc = function(...) attributeEditor.reRangeDisplayInfoSection(unpack(arg)) end
      },
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorReRange node definition.
------------------------------------------------------------------------------------------------------------------------
