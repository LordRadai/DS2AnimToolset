------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- AtEvent condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("AtEvent",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 604),
    version = 1,
    helptext = "Requires that the source state crosses a specific event number in the sync track before the transition is started. The event number can be a floating point value.",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["EventPosition"] =
      {
        type = "float", value = 0.0, min = 0.0,
        helptext = "Synchronisation event index to check for. A floating point value can be used to specify a point part way between events."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      -- the min condition on the attribute ensures this is valid
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local evtPosition = getAttribute(condition, "EventPosition")
      Stream:writeFloat(evtPosition, "EventPosition")
    end,
  }
)