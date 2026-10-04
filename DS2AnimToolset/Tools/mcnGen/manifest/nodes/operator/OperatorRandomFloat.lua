------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorRandomFloat node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorRandomFloat",
  {
    displayName = "Random Float",
    helptext = "Node used to generate a random number in a specified range, that changes over time",
    group = "Float Operators",
    image = "OperatorRandomFloat.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 146),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Result"] = {
                     input = false,
                     array = false,
                     type = "float",
                   },
    },

    pinOrder = { "Result" },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "Min", type = "float", value = 0.0, displayName = "Min",
        helptext = "The minimum number that can be output."
      },
      {
        name = "Max", type = "float", value = 1.0, displayName = "Max",
        helptext = "The maximum number that can be output."
      },
      {
        name = "DurationMode", type = "string", value = "Every Update", displayName = "Mode",
        helptext = "If set to Every Update, a new random number will be generated every network update. If set to Specify, a random number will be generated every specified number of seconds."
      },
      {
        name = "Interval", type = "float", value = 0.0, min = 0.0, displayName = "Interval",
        helptext = "The number of seconds to wait before generating a new random number."
      },
      {
        name = "Seed", type = "int", value = 1, min = 1, displayName = "Seed",
        helptext = "Used to initialize the random number generator."
      },
      {
        name = "GenerateSeed", type = "string", value = "User Specified", displayName = "Generate Seed",
        helptext = "If set to Generated, the runtime target will generate a seed instead of using the specified seed."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)

      local minAttrib = getAttribute(node, "Min")
      local maxAttrib = getAttribute(node, "Max")

      if minAttrib > maxAttrib then
        return nil, node .. " has a minimum output higher than the maximum output."
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local seed = 0

      if getAttribute(node, "GenerateSeed") == "User Specified" then
        seed = getAttribute(node, "Seed")
      end

      local interval = 0.0

      if getAttribute(node, "DurationMode") == "Specify" then
        interval = getAttribute(node, "Interval")
      end

      Stream:writeUInt(seed, "Seed")
      Stream:writeFloat(getAttribute(node, "Min"), "Min")
      Stream:writeFloat(getAttribute(node, "Max"), "Max")
      Stream:writeFloat(interval, "Interval")

    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- OperatorRandomFloat custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorRandomFloat",
    {
      {
        title = "Output Range",
        usedAttributes = { "Min", "Max" },
        displayFunc = function(...) safefunc(attributeEditor.operatorRandomFloatRangeDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Seed",
        usedAttributes = { "GenerateSeed", "Seed" },
        displayFunc = function(...) safefunc(attributeEditor.operatorRandomFloatSeedDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Update Frequency",
        usedAttributes = { "DurationMode", "Interval" },
        displayFunc = function(...) safefunc(attributeEditor.operatorRandomFloatDurationDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorRandomFloat node definition.
------------------------------------------------------------------------------------------------------------------------
