------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorRayCast node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorRayCast",
{
    displayName = "Ray Cast",
    helptext = "Ray cast against the environment, outputting one of the hit distance, position, normal and angles (in degrees)",
    group = "Vector3 Operators",
    image = "OperatorRayCast.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 141),
    version = 2,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["hitDistance"] = {
        input = false,
        array = false,
        type = "float",
      },
      ["hitPosition"] = {
        input = false,
        array = false,
        type = "vector3",
      },
      ["hitNormal"] = {
        input = false,
        array = false,
        type = "vector3",
      },
      ["hitPitchDownAngle"] = {
        input = false,
        array = false,
        type = "float",
      },
      ["hitRollRightAngle"] = {
        input = false,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "hitDistance", "hitPosition", "hitNormal", "hitPitchDownAngle", "hitRollRightAngle" },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "UseLocalOrientation", type = "bool", value = true,
        helptext = "If turned on, RayDelta will use the character's local coordinate frame. If turned off, RayDelta will use the world space coordinate frame."
      },
      {
        name = "RayStartX", type = "float",
        helptext = "Ray start as an offset from the character controller's position, in character space."
      },
      {
        name = "RayStartY", type = "float",
        helptext = "Ray start as an offset from the character controller's position, in character space."
      },
      {
        name = "RayStartZ", type = "float",
        helptext = "Ray start as an offset from the character controller's position, in character space."
      },
      {
        name = "RayDeltaX", type = "float",
        helptext = "Vector defining the direction of the ray from RayStart."
      },
      {
        name = "RayDeltaY", type = "float",
        helptext = "Vector defining the direction of the ray from RayStart."
      },
      {
        name = "RayDeltaZ", type = "float",
        helptext = "Vector defining the direction of the ray from RayStart."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local UseLocalOrientation = getAttribute(node, "UseLocalOrientation")
      Stream:writeBool(UseLocalOrientation, "UseLocalOrientation")

      local RayStartX = getAttribute(node, "RayStartX")
      local RayStartY = getAttribute(node, "RayStartY")
      local RayStartZ = getAttribute(node, "RayStartZ")
      local RayDeltaX = getAttribute(node, "RayDeltaX")
      local RayDeltaY = getAttribute(node, "RayDeltaY")
      local RayDeltaZ = getAttribute(node, "RayDeltaZ")
      Stream:writeFloat(RayStartX, "RayStartX")
      Stream:writeFloat(RayStartY, "RayStartY")
      Stream:writeFloat(RayStartZ, "RayStartZ")
      Stream:writeFloat(RayDeltaX, "RayDeltaX")
      Stream:writeFloat(RayDeltaY, "RayDeltaY")
      Stream:writeFloat(RayDeltaZ, "RayDeltaZ")
      
      -- Serialise world up axis as an index into a Cartesian 3-vector
      local worldUpAxis = preferences.get("WorldUpAxis")
      local upAxisIndex = 0
      if worldUpAxis == "Y Axis" then
        upAxisIndex = 1
      elseif worldUpAxis == "Z Axis" then
        upAxisIndex = 2
      end
      Stream:writeUInt(upAxisIndex, "UpAxisIndex")
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        -- In version 1 you had to specify a limited set of control param output data to be used (due to an architectural limitation).
        -- This is no longer the case all output control param data is generated and available.
        removeAttribute(node, "deprecated_OutputDistance")     
        removeAttribute(node, "deprecated_OutputPosition")
        removeAttribute(node, "deprecated_OutputNormal")
        removeAttribute(node, "deprecated_OutputPitchDownAngle")
        removeAttribute(node, "deprecated_OutputHitRollRightAngle")
      end
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorRayCast",
    {
      {
        title = "Ray Cast",
        usedAttributes = {
          "RayStartX",
          "RayStartY",
          "RayStartZ",
          "RayDeltaX",
          "RayDeltaY",
          "RayDeltaZ"
        },
        displayFunc = function(...) attributeEditor.rayCastDisplayInfoSection(unpack(arg)) end
      },
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorRayCast node definition.
------------------------------------------------------------------------------------------------------------------------
