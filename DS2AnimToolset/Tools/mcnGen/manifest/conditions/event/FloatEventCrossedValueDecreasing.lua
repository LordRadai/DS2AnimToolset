------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- FloatEventCrossedValueDecreasing condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("FractionThroughDurationEvent",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 610),
    version = 1,
    helptext = "Requires that the value on a specified float track dips below a certain value before the transition is started. For instance, require that the float track tagged with user data '8' has its value dip below 0.5. In this example the condition is only true if the float value started at a value above 0.5 originally.",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["UseEventTrackUserTypeID"] =
      {
        type = "bool",
        value = true,
        helptext = "When set the condition will only monitor events from tracks with the specified ID.",
        displayName = "Use track user ID"
      },

      ["EventTrackUserTypeID"] =
      {
        type = "int",
        value = 0,
        helptext = "The user ID of the duration event track to monitor.",
        displayName = "Track user ID"
      },

      ["UseEventUserTypeID"] =
      {
        type = "bool",
        value = false,
        helptext = "When set the condition will only monitor events with the specified ID.",
        displayName = "Use event user ID"
      },

      ["EventUserTypeID"] =
      {
        type = "int",
        value = 0,
        helptext = "The user ID of the duration event to monitor.",
        displayName = "Event user ID"
      },

      ["TestValue"] =
      {
        type = "float", value = 0.0,
        helptext = "The fraction through the user duration event at which the condition becomes true.",
        displayName = "Value"
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      -- the min condition on the attribute ensures this is valid
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local useEventTrackUserTypeID = getAttribute(condition, "UseEventTrackUserTypeID")
      Stream:writeBool(useEventTrackUserTypeID, "UseEventTrackUserTypeID")

      local eventTrackUserTypeID = getAttribute(condition, "EventTrackUserTypeID")
      Stream:writeInt(eventTrackUserTypeID, "EventTrackUserTypeID")

      local useEventUserTypeID = getAttribute(condition, "UseEventUserTypeID")
      Stream:writeBool(useEventUserTypeID, "UseEventUserTypeID")

      local eventUserTypeID = getAttribute(condition, "EventUserTypeID")
      Stream:writeInt(eventUserTypeID, "EventUserTypeID")

      local testValue = getAttribute(condition, "TestValue")
      Stream:writeFloat(testValue, "TestValue")
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "FractionThroughDurationEvent",
    {
      {
        title = "Thing",
        usedAttributes = { "TestValue", "UseEventTrackUserTypeID", "EventTrackUserTypeID", "UseEventUserTypeID", "EventUserTypeID" },
        displayFunc = function(...) safefunc(attributeEditor.FractionThroughDurationEventInfoSection, unpack(arg)) end
      }
    }
  )
end
