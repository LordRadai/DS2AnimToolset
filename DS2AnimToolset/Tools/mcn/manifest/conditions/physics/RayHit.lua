------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "luaAPI/debugDataAPI.lua"
require "previewScripts/NetworkNodeDebugDraw.lua"

------------------------------------------------------------------------------------------------------------------------
-- ControlParamFloatGreaterThan condition definition.
------------------------------------------------------------------------------------------------------------------------
registerCondition("RayHit",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 615),
    version = 3,
    helptext = "Condition is true if a raytest returns a hit/miss",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["HitMode"] =
      {
        type = "int", value = 1,
        helptext = "If 'On Hit' the condition will evaluate to true on contact with any object. If 'On Hit Moving' it will be true if the hit object is moving. if 'On Hit Stationary' it will be true if the hit object is stationary. Otherwise the condition is true when nothing was hit."
      },
      ["UseLocalOrientation"] =
      {
        type = "bool", value = true,
        helptext = "If checked, the ray direction is rotated accordingly to the character root transform."
      },
      ["RayStartX"] =
      {
        type = "float",
        helptext = "Represents the starting point of the ray."
      },
      ["RayStartY"] =
      {
        type = "float",
        helptext = "Represents the starting point of the ray."
      },
      ["RayStartZ"] =
      {
        type = "float",
        helptext = "Represents the starting point of the ray."
      },
      ["RayDeltaX"] =
      {
        type = "float",
        helptext = "Represents the direction of the ray."
      },
      ["RayDeltaY"] =
      {
        type = "float",
        helptext = "Represents the direction of the ray."
      },
      ["RayDeltaZ"] =
      {
        type = "float",
        helptext = "Represents the direction of the ray."
      },
      ["DebugDraw"] =
      {
        type = "bool", value = false,
        helptext = "If set, then debug lines will be drawn in the viewport.\n\nYellow line indicates the ray.\n\nIn the Preview Script an update handler will need to call viewport.debugDraw.update() to see the debug information."
      },

    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local HitMode = getAttribute(condition, "HitMode")
      Stream:writeInt(HitMode, "HitMode")
      local UseLocalOrientation = getAttribute(condition, "UseLocalOrientation")
      Stream:writeBool(UseLocalOrientation, "UseLocalOrientation")

      local RayStartX = getAttribute(condition, "RayStartX")
      Stream:writeFloat(RayStartX, "RayStartX")
      local RayStartY = getAttribute(condition, "RayStartY")
      Stream:writeFloat(RayStartY, "RayStartY")
      local RayStartZ = getAttribute(condition, "RayStartZ")
      Stream:writeFloat(RayStartZ, "RayStartZ")
      local RayDeltaX = getAttribute(condition, "RayDeltaX")
      Stream:writeFloat(RayDeltaX, "RayDeltaX")
      local RayDeltaY = getAttribute(condition, "RayDeltaY")
      Stream:writeFloat(RayDeltaY, "RayDeltaY")
      local RayDeltaZ = getAttribute(condition, "RayDeltaZ")
      Stream:writeFloat(RayDeltaZ, "RayDeltaZ")
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        local value = getAttribute(node, "UseLocalOrientation")
        value = not value
        local attr = string.format("%s.UseLocalOrientation", node)
        local updated = setAttribute(attr, value)
        if not updated then
          print("error versioning " .. node)
        end
      end
      if version < 3 then
        local trueIfHit = getAttribute(node, "deprecated_TrueIfHit")
        local hitMode = 1
        if not trueIfHit then
          hitMode = 0
        end
        local updated = setAttribute(node .. ".HitMode", hitMode)
        if not updated then
          print("error versioning " .. node)
        else
          removeAttribute(node, "deprecated_TrueIfHit")
        end
      end
    end

  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "RayHit",
    {
      {
        title = "Ray Cast",
        usedAttributes = {
          "RayStartX",
          "RayStartY",
          "RayStartZ",
          "RayDeltaX",
          "RayDeltaY",
          "RayDeltaZ",
        },
        displayFunc = function(...) attributeEditor.rayCastDisplayInfo(unpack(arg)) end
      },
      {
        title = "Space",
        usedAttributes = {
          "UseLocalOrientation",
        },
        displayFunc = function(...) attributeEditor.spaceDisplayInfo(unpack(arg)) end
      },
      {
        title = "Hit Mode",
        usedAttributes = {
          "HitMode",
        },
        displayFunc = function(...) attributeEditor.HitModeDisplayInfo(unpack(arg)) end
      }
    }
  )

  previewScript.debugData.registerProvider(
    "RayHit",
    function(node)
      if getAttribute(node, "DebugDraw") then
        -- The 'owner' of this condition is the source state of the transition that has this condition
        local transitName, _ = splitNodePath(node)
        local connections = listConnections{ Object = transitName, Upstream = true, Downstream = false }
        if table.getn(connections) == 1 then
          local sourceState = connection[1]

          -- ray is relative to the character root so use the root joint
          local jointIndex = 0

          -- the user sets these settings explicitly, so they should already be in runtime units
          local rayOffset = {
            x = getAttribute(node, "RayStartX"),
            y = getAttribute(node, "RayStartY"),
            z = getAttribute(node, "RayStartZ"),
          }

          local rayVector = {
            x = getAttribute(node, "RayDeltaX"),
            y = getAttribute(node, "RayDeltaY"),
            z = getAttribute(node, "RayDeltaZ"),
          }

          local colour = {
            r = 255,
            g = 255,
            b = 0,
          }

          local sets = listAnimSets()
          for _, set in ipairs(sets) do
            previewScript.debugData.addLine(sourceState, set, jointIndex, rayOffset, rayVector, colour)
          end
        end -- Anim Set loop
      end -- If DebugDraw is true
    end -- Debug Draw function
  )
end

