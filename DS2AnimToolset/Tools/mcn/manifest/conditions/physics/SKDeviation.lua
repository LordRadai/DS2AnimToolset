------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- SKDeviation condition definition.
------------------------------------------------------------------------------------------------------------------------
registerCondition("SKDeviation",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 620),
    version = 1,
    helptext = "Condition on the deviation of SK parts from their target",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["DeviationTime"] =
      {
        type = "float", value = 0.0,
        helptext = "The minimum time that needs to elapse when the deviation is exceeded before the condition becomes true."
      },
      ["DeviationTriggerAmount"] =
      {
        type = "float", value = 0.0,
        helptext = "The amount of (position) deviation used as a trigger."
      },
      ["TriggerWhenDeviationIsExceeded"] =
      {
        type = "bool", value = true,
        helptext = "If true (default) the condition is triggered when the deviation is exceeded"
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local DeviationTime = getAttribute(condition, "DeviationTime")
      local DeviationTriggerAmount = getAttribute(condition, "DeviationTriggerAmount")
      local TriggerWhenDeviationIsExceeded = getAttribute(condition, "TriggerWhenDeviationIsExceeded")
      Stream:writeFloat(DeviationTime, "DeviationTime")
      Stream:writeFloat(DeviationTriggerAmount, "DeviationTriggerAmount")
      Stream:writeBool(TriggerWhenDeviationIsExceeded, "TriggerWhenDeviationIsExceeded")
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of SKDeviation condition definition.
------------------------------------------------------------------------------------------------------------------------

