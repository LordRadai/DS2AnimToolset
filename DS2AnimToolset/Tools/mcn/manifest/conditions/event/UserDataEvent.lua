------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- UserDataEvent condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("UserDataEvent",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 602),
    version = 1,
    helptext = "This condition is true on any frame where an event with the given user data value is played",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["EventUserTypeID"] =
      {
        type = "int", value = 0,
        displayName = "Event User Data",
        helptext = "The User Data ID of the event to monitor for."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      -- the min condition on the attribute ensures this is valid
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local evtUserTypeID = getAttribute(condition, "EventUserTypeID")
      Stream:writeInt(evtUserTypeID, "EventUserTypeID")
    end,
  }
)

