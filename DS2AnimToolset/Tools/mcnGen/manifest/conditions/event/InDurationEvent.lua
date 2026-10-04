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

registerCondition("InDurationEvent",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 618),
    version = 1,
    helptext = "Gets triggered when the source is, or is not, inside the requested event.",

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
        value = true,
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

    ["OnNotSet"] =
      {
        type = "bool", value = false,
        helptext = "This request condition resolves as true when it is not set.",
        displayName = "True when not set"
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
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

      local onNotSet = getAttribute(condition, "OnNotSet")
      Stream:writeBool(onNotSet, "OnNotSet")
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "InDurationEvent",
    {
      {
        title = "Thing",
        usedAttributes = { "UseEventTrackUserTypeID", "EventTrackUserTypeID", "UseEventUserTypeID", "EventUserTypeID", "OnNotSet", },
        displayFunc = function(...) safefunc(attributeEditor.InDurationEventDisplayInfo, unpack(arg)) end
      }
    }
  )
end
