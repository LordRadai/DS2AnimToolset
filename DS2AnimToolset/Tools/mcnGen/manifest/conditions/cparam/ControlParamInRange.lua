------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- ControlParamFloatInRange condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("ControlParamInRange",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 609),
    version = 1,
    helptext = "Ensure that a control parameter is between a lower and upper test value before the transition is started. For instance, require that the 'speed' control parameter is between 0.2 and 0.6",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["ControlParameter"] =
      {
        type = "controlParameter",
        helptext = "The name of the control parameter to monitor."
      },
      ["LowerTestValue"] =
      {
        type = "float", value = 0.5,
        helptext = "Lower value of the test to check against."
      },
      ["UpperTestValue"] =
      {
        type = "float", value = 0.5,
        helptext = "Upper value of the test to check against."
      },
      ["NotInRange"] =
      {
        type = "bool", value = false,
        helptext = "Flag to negate the condition. If checked, the condition becomes true when the control parameter is not within the specified range."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      -- make sure we have a valid control parameter
      local controlParamOrPassDownPin = getAttribute(condition, "ControlParameter")
      if controlParamOrPassDownPin == "" then
        return nil, string.format("%s.ControlParameter requires a valid control parameter, no control parameter is specified.", condition)
      end

      local type, subtype = getType(controlParamOrPassDownPin)
      if type == "ControlParameter" then
        if subtype ~= "float" and subtype ~= "int" and subtype ~= "uint" then
          return nil, string.format(
            "%s.ControlParameter requires a valid control parameter, the control parameter %s is of type '%s' required type is 'Float'.",
            condition,
            controlParamOrPassDownPin,
            subtype)
        end
      elseif type == "PassDownPin" then
        local pinsConnectedToPassDownPin = listConnections{
          Object = controlParamOrPassDownPin,
          Pins = true,
          Upstream = true,
          Downstream = false,
          ResolveReferences = true
        }

        local pinConnectedToPassDownPin = pinsConnectedToPassDownPin[1]
        if table.getn(pinsConnectedToPassDownPin) < 1 then
          return nil, string.format("%s.ControlParameter requires a valid reference control parameter, the reference control parameter %s has no input.", condition, controlParamOrPassDownPin)
        end

        if getType(pinConnectedToPassDownPin) ~= "DataPin" then
          return nil, string.format("%s.ControlParameter requires a valid reference control parameter, the reference control parameter %s must be connected to a DataPin.", condition, controlParamOrPassDownPin)
        end

        local nodeConnectedToPassDownPin, pinName = splitPinPath(pinConnectedToPassDownPin)
        local typeConnectedToPDP, subtype = getType(nodeConnectedToPassDownPin)
        if typeConnectedToPDP ~= "ControlParameter" or subtype ~= "float" then
          return nil, string.format("%s.ControlParameter requires a valid reference control parameter, the reference control parameter %s has no valid input.", condition, controlParamOrPassDownPin)
        end
      end

      -- make sure our range values are sensible.
      local lowerValue = getAttribute(condition, "LowerTestValue")
      local upperValue = getAttribute(condition, "UpperTestValue")
      if lowerValue > upperValue then
        return nil, ("ControlParamFloatInRange condition " .. condition .. " has invalid range values set.")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local lowerValue = getAttribute(condition, "LowerTestValue")
      local upperValue = getAttribute(condition, "UpperTestValue")
      local controlParamOrPassDownPin = getAttribute(condition, "ControlParameter")
      local notInRange = getAttribute(condition, "NotInRange")

      local runtimeID = nil
      local type, subtype = getType(controlParamOrPassDownPin)
      if type == "PassDownPin" then
        local connections = listConnections{
          Object = controlParamOrPassDownPin,
          Upstream = true,
          Downstream = false,
          ResolveReferences = true
        }

        runtimeID = getRuntimeID(connections[1])
        type, subtype = getType(connections[1])
      else
        runtimeID = getRuntimeID(controlParamOrPassDownPin)
      end

      if subtype == "uint" then
        Stream:writeUInt(lowerValue, "LowerTestValue")
        Stream:writeUInt(upperValue, "UpperTestValue")
      elseif subtype == "int" then
        Stream:writeInt(lowerValue, "LowerTestValue")
        Stream:writeInt(upperValue, "UpperTestValue")
      else
        Stream:writeFloat(lowerValue, "LowerTestValue")
        Stream:writeFloat(upperValue, "UpperTestValue")
      end
            
      Stream:writeNetworkNodeId(runtimeID, "RuntimeNodeID")
      Stream:writeBool(notInRange, "NotInRange")
      Stream:writeString(subtype, "DataType")
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "ControlParamInRange",
    {
      {
        title = "Control Parameter Range",
        usedAttributes = {
          "ControlParameter",
          "LowerTestValue",
          "UpperTestValue",
          "NotInRange"
        },
        displayFunc = function(...) attributeEditor.controlParameterConditionInRangeInfoSection(unpack(arg)) end
      }
    }
  )
end

