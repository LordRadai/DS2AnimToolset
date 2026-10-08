------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- DuringEvent condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("DuringEvent",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 605),
    version = 1,
    helptext = "Requires that the source state is at the given event index in the sync track. This condition does not care about the previous event index on the sync track, and is true everywhere between the specified event and the event that follows.  For instance, if the event index 3 is specified the condition is true anywhere in the region between the third and forth event index (or from the third event until the end of the cycle if there is not a forth event).",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["EventIndex"] =
      {
        type = "int", value = 0, min = 0,
        helptext = "Index of synchronisation event to monitor for."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      -- the min and max condition on the TriggerPosition ensures this is valid
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local index = getAttribute(condition, "EventIndex")
      Stream:writeUInt(index, "EventIndex")
    end,

  }
)