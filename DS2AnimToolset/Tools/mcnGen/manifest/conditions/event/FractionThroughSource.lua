------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- FractionThroughSource condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("FractionThroughSource",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 603),
    version = 1,
    helptext = "Requires that the source state is specific fraction through an animation before the transition is triggered.  This condition becomes true the first frame that the specified fraction is reached.  For instance, you might specify that that the transition should occur 70% (0.7) through the animation. The condition is true the first frame where the sample point is greater than 70% and the previous sample point was less than 70%.",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["TriggerPercent"] =
      {
        type = "float", value = 1.00, min = 0.0, max = 1.0,
        helptext = "The fraction through the duration of the source to monitor. When this position fraction is crossed this condition is satisfied.",
        displayName = "Trigger fraction"
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      -- the min and max condition on the TriggerPosition ensures this is valid
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local triggerPos = getAttribute(condition, "TriggerPercent")
      Stream:writeFloat(triggerPos, "TestFraction")
    end,
  }
)

