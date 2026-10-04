------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- PhysicsInUse condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("PhysicsInUse",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 613),
    version = 1,
    helptext = "Condition is true if a physical body is assigned to the network.",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["OnPhysicsInUse"] =
      {
        type = "bool", value = true,
        helptext = "If not checked, the condition becomes true when a physical body is not assigned to the network."
     },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local OnPhysicsInUse = getAttribute(condition, "OnPhysicsInUse")
      Stream:writeBool(OnPhysicsInUse, "OnPhysicsInUse")
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of stuff
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "PhysicsInUse",
    {
      {
        title = "On Physics In Use",
        usedAttributes = { "OnPhysicsInUse" },
        displayFunc = function(...) safefunc(attributeEditor.physicsInUseSection, unpack(arg)) end
      },

    }
  )
end

