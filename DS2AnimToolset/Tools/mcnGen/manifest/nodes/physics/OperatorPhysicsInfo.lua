------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorPhysicsInfo node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorPhysicsInfo",
{
    displayName = "Physics Info",
    helptext = "Outputs information about the physical state of the character",
    group = "Physics",
    image = "OperatorPhysicsInfo.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 148),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Position"] = {
        input = false,
        array = false,
        type = "vector3",
      },
      ["Velocity"] = {
        input = false,
        array = false,
        type = "vector3",
      },
    },

    pinOrder = { "Position", "Velocity" },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "PartIndex", type = "int", value = 0,
        helptext = "Physics rig part index."
      },
      {
        name = "OutputInWorldSpace", type = "bool", value = true,
        helptext = "When not output in world space, the returned data will be in the space of the character controller."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      -- Make sure that at least 1 output cp pin is connected
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local PartIndex = getAttribute(node, "PartIndex")
      local OutputInWorldSpace = getAttribute(node, "OutputInWorldSpace")
      stream:writeInt(PartIndex, "PartIndex")
      stream:writeBool(OutputInWorldSpace, "OutputInWorldSpace")
    end,

  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorPhysicsInfo node definition.
------------------------------------------------------------------------------------------------------------------------

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorPhysicsInfo",
    {
      {
        title = "Part",
        usedAttributes = { "PartIndex" },
        displayFunc = function(...) safefunc(attributeEditor.physicsPartIndexSection, unpack(arg)) end
      },
      {
        title = "Output",
        usedAttributes = { "OutputInWorldSpace" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end
