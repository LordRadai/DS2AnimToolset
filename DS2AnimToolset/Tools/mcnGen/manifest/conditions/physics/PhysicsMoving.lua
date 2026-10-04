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

registerCondition("PhysicsMoving",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 619),
    version = 1,
    helptext = "Condition is true if a physical body's parts velocities are below the threshold for a given time",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["VelocityThreshold"] =
      {
        type = "float",
        value = 0.1,
        helptext = "Minimum average velocity per physics part to dip below to fulfil the condition.",
        displayName = "min. velocity"
      },

      ["AngularVelocityThreshold"] =
      {
        type = "float",
        value = 1.0,
        helptext = "Minimum average angular velocity per physics part to dip below to fulfil the condition.",
        displayName = "min. angular velocity"
      },

       ["MinFramesBelow"] =
      {
        type = "int",
        value = 1,
        helptext = "Number of frames the velocities must be below thresholds to trigger the condition.",
        displayName = "min. frames below velocities"
      },

      ["OnNotSet"] =
      {
        type = "bool",
        value = true,
        helptext = "Invert the checking logic (which means that the condition is checking for 'not moving' instead of 'moving'.",
        displayName = "true when not moving"
      },

    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local velThreshold = getAttribute(condition, "VelocityThreshold")
      Stream:writeFloat(velThreshold, "VelocityThreshold")

      local angVelThreshold = getAttribute(condition, "AngularVelocityThreshold")
      Stream:writeFloat(angVelThreshold, "AngularVelocityThreshold")

      local minFrames = getAttribute(condition, "MinFramesBelow")
      Stream:writeInt(minFrames, "MinFramesBelow")

      local onNotSet = getAttribute(condition, "OnNotSet")
      Stream:writeBool(onNotSet, "OnNotSet")
    end,
  }
)
--[[
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
]]--