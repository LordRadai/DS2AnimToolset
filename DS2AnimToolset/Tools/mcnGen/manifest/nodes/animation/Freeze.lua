------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/AnimationTakeDisplayInfo.lua"
require "ui/AttributeEditor/DefaultDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- AnimWithEvents node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("Freeze",
  {
    displayName = "Freeze",
    helptext = "Freezes the last transformation state of the system",
    group = "Utilities",
    image = "Freeze.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 126),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Result"] = {
        input = false,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events" },
          optional = { },
        },
      },
    },

    attributes =
    {
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      return true;
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      Stream:writeBool(true, "passThroughTransformsOnce")
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local transformChannels = anim.getTransformChannels((node .. ".AnimationTake"), set)
      return transformChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    getEvents = function(node)

      return { min = 0, max = 0 }
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)

    end
  }
)

------------------------------------------------------------------------------------------------------------------------
-- AnimWithEvents custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "Freeze",
    {

    }
  )
end