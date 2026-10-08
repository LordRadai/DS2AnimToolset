------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- ControlParamFloatGreaterThan condition definition.
------------------------------------------------------------------------------------------------------------------------
registerCondition("ControlParamTest",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 616),
    version = 1,
    helptext = "Compare a control parameter to a given value before the transition is started using one of '>', '<', '>=' and '<='. For instance, require that the 'speed' control parameter is greater than 0.5.",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["ControlParameter"] =
      {
        type = "controlParameter",
        helptext = "The control parameter that is compared every frame"
      },
      ["TriggerValue"] =
      {
        type = "float", value = 0.5,
        helptext = "The value that the control parameter is compared against"
      },
      ["Comparison"] =
      {
        type = "string", value = ">",
        helptext = "Change the type of comparison performed."
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

        if table.getn(pinsConnectedToPassDownPin) > 1 then
          return nil, string.format("%s.ControlParameter requires a valid reference control parameter, the reference control parameter %s has no input.", condition, controlParamOrPassDownPin)
        end

        if getType(pinConnectedToPassDownPin) ~= "DataPin" then
          return nil, string.format("%s.ControlParameter requires a valid reference control parameter, the reference control parameter %s must be connected to a DataPin.", condition, controlParamOrPassDownPin)
        end

        local nodeConnectedToPassDownPin, pinName = splitPinPath(pinConnectedToPassDownPin)
        local typeConnectedToPDP, subtype = getType(nodeConnectedToPassDownPin)
        if typeConnectedToPDP ~= "ControlParameter" then
          return nil, string.format("%s.ControlParameter requires a valid reference control parameter, the reference control parameter %s has no valid input.", condition, controlParamOrPassDownPin)
        end
      end

      local operation = getAttribute(condition, "Comparison")
      if (operation ~= ">") and
         (operation ~= ">=") and
         (operation ~= "<") and
         (operation ~= "<=") then
        return nil, "Unrecognised operation \'" .. operation .. "\'.  Must be one of >, <, >=, <=."
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local trigger = getAttribute(condition, "TriggerValue")
      local controlParamOrPassDownPin = getAttribute(condition, "ControlParameter")
      local operation = getAttribute(condition, "Comparison")
      local lessThanOp = false
      local orEqual = false
      if operation == ">" then
         lessThanOp = false
         orEqual = false
      elseif operation == ">=" then
         lessThanOp = false
         orEqual = true
      elseif operation == "<" then
         lessThanOp = true
         orEqual = false
      elseif operation == "<=" then
         lessThanOp = true
         orEqual = true
      end

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
        Stream:writeUInt(trigger, "TestValue")
      elseif subtype == "int" then
        Stream:writeInt(trigger, "TestValue")
      else
        Stream:writeFloat(trigger, "TestValue")
      end

      Stream:writeNetworkNodeId(runtimeID, "RuntimeNodeID")
      Stream:writeBool(orEqual, "OrEqual")
      Stream:writeBool(lessThanOp, "LessThanOperation")
      Stream:writeString(subtype, "DataType")
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "ControlParamTest",
    {
      {
        title = "Control Parameter Test",
        usedAttributes = {
          "ControlParameter",
          "TriggerValue",
          "Comparison"
        },
        displayFunc = function(...) attributeEditor.controlParameterConditionInfoSection(unpack(arg)) end
      }
    }
  )
end

