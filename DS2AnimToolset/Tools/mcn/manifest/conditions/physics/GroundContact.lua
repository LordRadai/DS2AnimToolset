------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- GroundContact condition definition.
------------------------------------------------------------------------------------------------------------------------
registerCondition("GroundContact",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 614),
    version = 1,
    helptext = "Condition depending on character contact with the ground (or not) greater than a certain time",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["TriggerTime"] =
      {
        type = "float", value = 0.0,
        helptext = "The minimum ground contact time that needs to elapse before the condition becomes true."
      },
      ["OnGround"] =
      {
        type = "bool", value = true,
        --helptext = "If not checked the condition triggers when the character is not in contact with the ground for the time specified by the TriggerTime attribute."
        helptext = "Change whether the character must be in the air or on the ground to trigger this condition"
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local TriggerTime = getAttribute(condition, "TriggerTime")
      local OnGround = getAttribute(condition, "OnGround")
      Stream:writeFloat(TriggerTime, "TriggerTime")
      Stream:writeBool(OnGround, "OnGround")
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of Ground Contact condition definition.
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "GroundContact",
    {
      {
        title = "GroundContactSubSection",
        usedAttributes = {
          "TriggerTime",
          "OnGround"
        },
        displayFunc = function(...) safefunc(attributeEditor.OnGroundContactInfoSection, unpack(arg)) end
      },
    }
  )
end

