------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- InEventRange condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("InEventRange",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 611),
    version = 1,
    helptext = "Requires that the source state is between two specified event numbers in the sync track before the transition is started. The event numbers can be floating point values. For instance, require that the source is between the events 2.0 and 4.2 on the sync track. ",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["EventRangeStart"] =
      {
        type = "float", value = 0.0, min = 0.0,
        helptext = "The start point for the range, specified as an event value. A floating point value can be used to specify a point part way between events.",
        displayName = "Start event"
      },
      ["EventRangeEnd"] =
      {
        type = "float", value = 0.0, min = 0.0,
        helptext = "The end point for the range, specified as an event value. A floating point value can be used to specify a point part way between events.",
        displayName = "End event"
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local evtRangeStart = getAttribute(condition, "EventRangeStart")
      local evtRangeEnd = getAttribute(condition, "EventRangeEnd")
      Stream:writeFloat(evtRangeStart, "EventRangeStart")
      Stream:writeFloat(evtRangeEnd, "EventRangeEnd")
    end,
  }
)
