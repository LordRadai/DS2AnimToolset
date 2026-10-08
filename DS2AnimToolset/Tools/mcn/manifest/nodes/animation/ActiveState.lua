------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

registerStateMachineNode("ActiveState",
{
    displayName = "Active State",
    version = 1,
    id = generateNamespacedId(idNamespaces.NaturalMotion, 137),
    helptext = "Reference the Active State",
    group = "States",
    image = "ActiveState.png",
    interfaces = { },
    functionPins = { },
    pinOrder = { },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "AllStates", type = "bool", value = true,
        helptext = "Specifies whether this node represents all possible states or a subset of states."
      },
      {
        name = "States", type = "refArray",
        helptext = "The subset of states that this node represents."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      -- Intentionally left blank
    end
  }
)

------------------------------------------------------------------------------------------------------------------------
-- ActiveState custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "ActiveState",
    {
      {
        title = "Source",
        usedAttributes = { "AllStates", "States" },
        displayFunc = function(...) safefunc(attributeEditor.activeStateDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end
