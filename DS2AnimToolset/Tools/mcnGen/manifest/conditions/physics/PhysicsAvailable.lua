------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- PhysicsAvailable condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("PhysicsAvailable",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 612),
    version = 1,
    helptext = "Condition is true if a physical body is available to be assigned to the network.",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["OnPhysicsAvailable"] =
      {
        type = "bool", value = true,
        helptext = "If not checked, the condition becomes true when a physical body is not available."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local OnPhysicsAvailable = getAttribute(condition, "OnPhysicsAvailable")
      Stream:writeBool(OnPhysicsAvailable, "OnPhysicsAvailable")
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "PhysicsAvailable",
    {
      {
        title = "On Physics Available",
        usedAttributes = { "OnPhysicsAvailable" },
        displayFunc = function(...) safefunc(attributeEditor.physicsAvailableSection, unpack(arg)) end
      },

    }
  )
end

