------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- MessageCondition condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("MessageCondition",
{
    id = generateNamespacedId(idNamespaces.NaturalMotion, 601),
    version = 2,
    helptext = "Requires that a message with the given name is sent to the network before the transition is started.  These message are typically sent by the game directly, but they can also be sent from connect by pressing a button that will appear when the network is run.",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["Message"] =
      {
        type = "request", value = "",
        helptext = "This is a text identification of a request."
      },
      ["OnNotSet"] =
      {
        type = "bool", value = false,
        helptext = "This request condition resolves as true when it is not set."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
     -- must have a value for the request
      local requestPath = getAttribute(condition, "Message")
      if (string.len(requestPath) == 0) then
        return nil, ("A request string is required for condition: " .. condition)
      else
        local reqId = target.getRequestID(requestPath)
        if (reqId == nil) then
          return nil, ("A valid request must be set on condition:" .. condition)
        end
        return true
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local messagePath = getAttribute(condition, "Message")
      local messageId = target.getRequestID(messagePath)
      local onNotSet = getAttribute(condition, "OnNotSet")
      Stream:writeUInt(messageId, "MessageID")
      Stream:writeBool(onNotSet, "OnNotSet")
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(condition, version)
      if version < 2 then
        local value = getAttribute(condition, "deprecated_Request")
        setAttribute(string.format("%s.Message", condition), value)
        removeAttribute(condition, "deprecated_Request")
      end
    end,
  }
)