------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorRampFloat node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorRampFloat",
{
    displayName = "Ramp Float",
    helptext = "Modifies an initial value over time at a fixed rate",
    group = "Float Operators",
    image = "OperatorRampFloat.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 143),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["RateMultiplier"] = {
        input = true,
        array = false,
        type = "float",
      },
      ["Result"] = {
        input = false,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Input", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "Start", type = "float", value = 0.0,
        helptext = "Initial value of Result.",
        set = function(node, value)
          local clampEnabled = getAttribute(node, "Enable")
          if clampEnabled then
            local clampedValue = value

            local minimum = getAttribute(node, "Minimum")
            if clampedValue < minimum then
              clampedValue = minimum
            end

            local maximum = getAttribute(node, "Maximum")
            if clampedValue > maximum then
              clampedValue = maximum
            end

            if clampedValue ~= value then
              local attribute = string.format("%s.Start", node)
              setAttribute(attribute, clampedValue)
            end
          end
        end,
      },
      {
        name = "Rate", type = "float", value = 1.0,
        helptext = "Amount that Result will increase or decrease per second, the node accounts for variable time steps. If Start is set to 1.0 and the rate is -1.0 Result will be 0.0 after 1 second. This rate can be modified at runtime by connecting an optional RateMultiplier."
      },
      {
        name = "Enable", type = "bool", value = true,
        helptext = "Enable clamping of Result.",
        set = function(node, value)
          if value then
            local startName = string.format("%s.Start", node)
            local value = getAttribute(startName)
            local clampedValue = value

            local minimum = getAttribute(node, "Minimum")
            if clampedValue < minimum then
              clampedValue = minimum
            end

            local maximum = getAttribute(node, "Maximum")
            if clampedValue > maximum then
              clampedValue = maximum
            end

            if clampedValue ~= value then
              setAttribute(startName, clampedValue)
            end
          end
        end,
      },
      {
        name = "Minimum", type = "float", value = 0.0,
        helptext = "Minimum value of Result.",
        set = function(node, value)
          local maximum = getAttribute(node, "Maximum")
          if value > maximum then
            value = maximum
            local attribute = string.format("%s.Minimum", node)
            setAttribute(attribute, value)
          end

          local enable = getAttribute(node, "Enable")
          if enable then
            local startName = string.format("%s.Start", node)
            local start = getAttribute(startName)
            if start < value then
              setAttribute(startName, value)
            end
          end
        end,
      },
      {
        name = "Maximum", type = "float", value = 1.0,
        helptext = "Maximum value of Result.",
        set = function(node, value)
          local minimum = getAttribute(node, "Minimum")
          if value < minimum then
            value = minimum
            local attribute = string.format("%s.Maximum", node)
            setAttribute(attribute, value)
          end

          local enable = getAttribute(node, "Enable")
          if enable then
            local startName = string.format("%s.Start", node)
            local start = getAttribute(startName)
            if start > value then
              setAttribute(startName, value)
            end
          end
        end,
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local nodesConnected = listConnections{ Object =(node .. ".RateMultiplier"), ResolveReferences = true }
      local inputNode = nodesConnected[1]
      if (inputNode ~= nil) and not isValid(inputNode) then
        return nil, "Operator Ramp Float requires a valid input"
      end

      -- RateMultiplier is optional - will default to 1
      local EnableClamping = getAttribute(node, "Enable")
      local MinimumValue = getAttribute(node, "Minimum")
      local MaximumValue = getAttribute(node, "Maximum")
      local Start = getAttribute(node, "Start")

      if (EnableClamping == true) and (MaximumValue < MinimumValue) then
        return nil, "Clamping range is invalid"
      end

      if (EnableClamping == true) and (Start < MinimumValue) then
        return nil, "Start value must be greater than minimum value if clamping is enabled"
      end

      if (EnableClamping == true) and (Start > MaximumValue) then
        return nil, "Start value must be less than maximum value if clamping is enabled"
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputInfo = getConnectedNodeInfo((node .. ".RateMultiplier"))
      local InitialValue = getAttribute(node, "Start")
      local RateOfChange = getAttribute(node, "Rate")
      local MinimumValue = getAttribute(node, "Minimum")
      local MaximumValue = getAttribute(node, "Maximum")
      local EnableClamping = getAttribute(node, "Enable")
      -- indicate clamping is disabled by an invaliud range
      if (EnableClamping == false) then
        MinimumValue = 1
        MaximumValue = 0
      end

      if inputInfo ~= nil then
        Stream:writeNetworkNodeId(inputInfo.id, "RateMultiplier", inputInfo.pinIndex)
      end
      Stream:writeFloat(InitialValue, "InitialValue")
      Stream:writeFloat(RateOfChange, "RateOfChange")
      Stream:writeFloat(MinimumValue, "MinimumValue")
      Stream:writeFloat(MaximumValue, "MaximumValue")

    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- OperatorRampFloat custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorRampFloat",
    {
      {
        title = "Properties",
        usedAttributes = { "Start", "Rate" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Clamping",
        usedAttributes = { "Enable", "Minimum", "Maximum" },
        displayFunc = function(...) safefunc(attributeEditor.operatorRampFloatClampingDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorRampFloat node definition.
------------------------------------------------------------------------------------------------------------------------