------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "manifest/transitions/TransitBase.lua"

require "ui/AttributeEditor/TransitDisplayInfo.lua"
require "ui/AttributeEditor/TransitAtEventDisplayInfo.lua"
require "ui/AttributeEditor/TransitPropertiesDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- Transit transition definition.
------------------------------------------------------------------------------------------------------------------------

registerTransition("Transit",
  {
    helptext = "Performs a transition blend between two states over a given period of time.",
    interfaces = { "Transforms", "Time" },
    supportsTransitToSelf = true,
    animId = generateNamespacedId(idNamespaces.NaturalMotion, 402),
    physicsId = generateNamespacedId(idNamespaces.NaturalMotion, 403),
    euphoriaId = generateNamespacedId(idNamespaces.NaturalMotion, 403),
    version = 4,

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "DurationInTime", type = "float", value = 0.5, min = 0.0,      -- How long the transition will last in time
        helptext = "Controls how long the transition will take in seconds."   -- This is the dt that if fed from the parent
                                                                              -- Node so it may well be warped time.
      },
      {
        name = "AdditiveBlendAttitude", type = "bool", value = false, displayName = "Additive Rotation",
        helptext = "When set, the rotations of Destination are added to Source."
      },
      {
        name = "AdditiveBlendPosition", type = "bool", value = false, displayName = "Additive Translation",
        helptext = "When set, the rotations and positions of Destination are added to Source."
      },
      {
        name = "SphericallyInterpolateTrajectoryPosition", type = "bool", value = false, displayName = "Slerp Trajectory",
        helptext = "Spherically interpolate between the two trajectory positions inputs. The trajectory orientation is always spherically interpolated."
      },
      {
        name = "DeadblendBreakoutToSource", type = "bool", value = false, displayName = "Dead blend source on breakout",
        helptext = "If turned on, a breakout transition to this transition's source will replace the source with a dead-reckoned animation."
      },
      {
        name = "DestinationStartFraction", type = "float", value = 0.0, min = 0.0, max = 1.0,
        helptext = ""
      },
      {
        name = "DestinationStartSyncEventIndex", type = "float", value = 0.0, min = 0.0,
        helptext = ""
      },
      {
        name = "DestinationStartSyncEventFraction", type = "float", value = 0.0, min = 0.0, max = 1.0,
        helptext = ""
      },
      {
        name = "UseDestinationStartFraction", type = "bool", value = false,
        helptext = ""
      },
      {
        name = "UseDestinationStartSyncEventIndex", type = "bool", value = false,
        helptext = ""
      },
      {
        name = "UseDestinationStartSyncEventFraction", type = "bool", value = false,
        helptext = ""
      },
      {
        name = "ReversibleTransit", type = "bool", value = false,
        helptext = "Allow transition to be reversible."
      },
      {
        name = "ReverseControlParameter", type = "controlParameter",
        helptext = "Control parameter used to control transition reversing."
      },
      {
        name = "DeltaTrajSource", type = "int", value = 3,
        helptext = "Specifies which child/children we will use to source our delta trajectory."
      },
      {
        name = "DestinationSubState", type = "ref", kind = "transitionSubState",
        helptext = "An optional override for transitioning into sub-states other than the defaults."
      },
      {
        name = "BreakoutTransit", type = "bool", value = false,
        helptext = "Allows this transition to execute while a transition to its source state is still in progress."
      },
      {
        name = "FreezeSource", type = "bool", value = false,
        helptext = "Stop the source state until transit is complete"
      },
      {
        name = "FreezeDest", type = "bool", value = false,
        helptext = "Stop the destination state until transit is complete"
      },
      
      -- these attributes were on the DeadBlend in <= 3.0.1
      {
        name = "UseDeadReckoningWhenDeadBlending", type = "bool", value = true,
        helptext = "If turned on, then the transition source is replaced with a dead-reckoned animation source. If turned off, then the last output of the transition source is maintained during the transition.",
        displayName = "Use dead reckoning when dead blending"
      },
      {
        name = "BlendToDestinationPhysicsBones", type = "bool", value = false,
        helptext = "If turned on, then a normal blend between source and destination is performed on the transforms affected by the physics rig. If turned off, then the destination (physics) transforms are used.",
        displayName = "Blend to destination physics bones"
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      -- make sure we have a valid control parameter
      if getAttribute(node, "ReversibleTransit") then
        local controlParam = getAttribute(node, "ReverseControlParameter")
        if controlParam == "" then
          -- No Control Param correctly connected
          return nil, ("The transition " .. node .." is reversible but does not have a control parameter specified.")
        else
          -- Control param connected check for correct type
          local CP_Type

          if getType(controlParam) == "PassDownPin" then
            -- Passdown pin so resolve references
            local connections = listConnections{
              Object = controlParam,
              Upstream = true,
              Downstream = false,
              ResolveReferences = true
            }

            if table.getn(connections) == 1 then
              _, CP_Type = getType(connections[1])
            end
          else
            -- normal control param so get type
            _, CP_Type = getType(controlParam)
          end

          if (CP_Type ~= "bool") then
            return nil, ("The transition " .. node .." is reversible but does not have a boolean control parameter specified.")
          end

        end

      end
      return validateDestinationSubState(node)
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(transition, Stream)
      local sourceNodesTable = listConnections{ Object= transition, Upstream = true, Downstream = false, ResolveReferences = true }
      local sourceNodeName = sourceNodesTable[1]
      local sourceNodeRuntimeID = getRuntimeID(sourceNodeName)
      local destNodesTable = listConnections{ Object= transition, Upstream = false, Downstream = true, ResolveReferences = true }
      local destNodeName = destNodesTable[1]
      local destNodeRuntimeID = getRuntimeID(destNodeName)

      local durationInTime = 0.0
      if getTransitionCategory(transition) ~= "euphoria" then
        durationInTime = getAttribute(transition, "DurationInTime")
      end

      local additiveAtt = getAttribute(transition, "AdditiveBlendAttitude")
      local additivePos = getAttribute(transition, "AdditiveBlendPosition")
      local sphericalTrajPos = getAttribute(transition, "SphericallyInterpolateTrajectoryPosition")
      local deadblendBreakoutToSource = getAttribute(transition, "DeadblendBreakoutToSource")
      local destinationStartFraction = getAttribute(transition, "DestinationStartFraction")
      local destinationStartSyncEventIndex = getAttribute(transition, "DestinationStartSyncEventIndex")
      local destinationStartSyncEventFraction = getAttribute(transition, "DestinationStartSyncEventFraction")
      local useDestinationStartFraction = getAttribute(transition, "UseDestinationStartFraction")
      local useDestinationStartSyncEventIndex = getAttribute(transition, "UseDestinationStartSyncEventIndex")
      local useDestinationStartSyncEventFraction = getAttribute(transition, "UseDestinationStartSyncEventFraction")
      local deltaTrajSource = getAttribute(transition, "DeltaTrajSource")
      local breakoutTransit = getAttribute(transition, "BreakoutTransit")

      local freezeSource = getAttribute(transition, "FreezeSource")
      local freezeDest = getAttribute(transition, "FreezeDest")

      Stream:writeBool(freezeSource, "FreezeSource")
      Stream:writeBool(freezeDest, "FreezeDest")

      local destinationStartSyncEvent = 0.0
      if useDestinationStartSyncEventIndex then
        destinationStartSyncEvent = destinationStartSyncEvent + math.floor(destinationStartSyncEventIndex)
      end
      if useDestinationStartSyncEventFraction then
        destinationStartSyncEvent = destinationStartSyncEvent + destinationStartSyncEventFraction
      end

      if sourceNodeRuntimeID ~= nil then
        -- a nil value indicates that the source node will be determined by the runtime
        Stream:writeNetworkNodeId(sourceNodeRuntimeID, "SourceNodeID")
      end
      if destNodeRuntimeID ~= nil then
        -- a nil value indicates that the destination node will be determined by the runtime
        Stream:writeNetworkNodeId(destNodeRuntimeID, "DestNodeID")
      end
      Stream:writeFloat(durationInTime, "DurationInTime")
      Stream:writeBool(additiveAtt, "AdditiveBlendAttitude")
      Stream:writeBool(additivePos, "AdditiveBlendPosition")
      Stream:writeBool(sphericalTrajPos, "SphericallyInterpolateTrajectoryPosition")
      Stream:writeBool(deadblendBreakoutToSource, "DeadblendBreakoutToSource")
      Stream:writeFloat(destinationStartFraction, "DestinationStartFraction")
      Stream:writeFloat(destinationStartSyncEvent, "DestinationStartSyncEvent")
      Stream:writeBool(useDestinationStartFraction, "UseDestinationStartFraction")
      Stream:writeBool(useDestinationStartSyncEventIndex, "UseDestinationStartSyncEventIndex")
      Stream:writeBool(useDestinationStartSyncEventFraction, "UseDestinationStartSyncEventFraction")

      Stream:writeUInt(deltaTrajSource, "DeltaTrajSource")
      Stream:writeBool(breakoutTransit, "BreakoutTransit")

      Stream:writeBool(freezeSource, "FreezeSource")
      Stream:writeBool(freezeDest, "FreezeDest")

      -- Reversible transition --
      local isReversibleTransit = getAttribute(transition, "ReversibleTransit")
      Stream:writeBool(isReversibleTransit, "isReversibleTransit")
      if (isReversibleTransit) then
        local controlParamOrPassDownPin = getAttribute(transition, "ReverseControlParameter")

        local runtimeID = nil
        if getType(controlParamOrPassDownPin) == "PassDownPin" then
          local connections = listConnections{
            Object = controlParamOrPassDownPin,
            Upstream = true,
            Downstream = false,
            ResolveReferences = true
          }

          if table.getn(connections) == 1 then
            runtimeID = getRuntimeID(connections[1])
          end
        else
          runtimeID = getRuntimeID(controlParamOrPassDownPin)
        end

        Stream:writeNetworkNodeId(runtimeID, "RuntimeNodeID")
      end

      serializeDestinationSubState(transition, Stream)
      serializeDeadblendProperties(transition, Stream)
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(transition, version, pinLookupTable)
      if version < 4 and version > 0 then
        -- as the version number wasn't always bumped when attributes were added we must check which attributes exist
        -- the following attributes were changed somewhere between version 1 and version 3
        --   UseDestinationStartSyncEventIndexAndFraction and UseDestinationStartSyncEvent
        -- the following attributes were changed at version 4
        --   DestinationStartSyncEvent

        local attributes = listAttributes(transition)

        local deprecatedAttributes = { }
        for i, attribute in ipairs(attributes) do

          -- this statement is from TransitAtEvent which is now merged with Transit
          if attribute == "deprecated_Duration" then
            local deprecated = string.format("%s.%s", transition, attribute)

            local value = getAttribute(deprecated)
            setAttribute(string.format("%s.DurationInTime", transition), value)

            table.insert(deprecatedAttributes, attribute)
          end
        
          if attribute == "deprecated_UseDestinationStartSyncEvent" then
            local deprecated = string.format("%s.%s", transition, attribute)

            if getAttribute(deprecated) == true then
              setAttribute(string.format("%s.UseDestinationStartSyncEventIndex", transition), true)
            end

            table.insert(deprecatedAttributes, attribute)

          elseif attribute == "deprecated_UseDestinationStartSyncEventIndexAndFraction" then
            local deprecated = string.format("%s.%s", transition, attribute)

            if getAttribute(deprecated) == true then
              setAttribute(string.format("%s.UseDestinationStartSyncEventIndex", transition), true)
              setAttribute(string.format("%s.UseDestinationStartSyncEventFraction", transition), true)
            end

            table.insert(deprecatedAttributes, attribute)

          elseif attribute == "deprecated_DestinationStartSyncEvent" then
            local deprecated = string.format("%s.%s", transition, attribute)

            local deprecatedValue = getAttribute(deprecated)
            setAttribute(string.format("%s.DestinationStartSyncEventIndex", transition), math.floor(deprecatedValue))
            setAttribute(string.format("%s.DestinationStartSyncEventFraction", transition), deprecatedValue - math.floor(deprecatedValue))

            table.insert(deprecatedAttributes, attribute)
          end
        end

        for i, attribute in ipairs(deprecatedAttributes) do
          removeAttribute(transition, attribute)
        end
      end

      if version < 4 then
        local startFraction = string.format("%s.UseDestinationStartFraction", transition)
        local startSyncEventIndex = string.format("%s.UseDestinationStartSyncEventIndex", transition)
        local startSyncEventFraction = string.format("%s.UseDestinationStartSyncEventFraction", transition)

        if getAttribute(startFraction) and getAttribute(startSyncEventIndex) then
          setAttribute(startSyncEventFraction, true)
          setAttribute(startFraction, false)
        end

      end
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "Transit",
    {
      {
        title = "Duration",
        usedAttributes = {
          "DurationInTime"
        },
        displayFunc = function(...) safefunc(attributeEditor.transitDurationInTimeSection, unpack(arg)) end
      },
      {
        title = "Destination Start Point",
        usedAttributes = {
          "DestinationStartFraction",
          "DestinationStartSyncEventIndex",
          "DestinationStartSyncEventFraction",
          "UseDestinationStartFraction",
          "UseDestinationStartSyncEventIndex",
          "UseDestinationStartSyncEventFraction"
        },
        displayFunc = function(...) safefunc(attributeEditor.transitDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Reversible Transit",
        usedAttributes = {
          "ReversibleTransit",
          "ReverseControlParameter"
        },
        displayFunc = function(...) safefunc(attributeEditor.reversibleTransitDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Delta Traj Source",
        usedAttributes = {
          "DeltaTrajSource"
        },
        displayFunc = function(...) safefunc(attributeEditor.DeltaTrajSourceDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Destination",
        usedAttributes = {
          "DestinationSubState"
        },
        displayFunc = function(...) safefunc(attributeEditor.destSubStateDisplayInfo, unpack(arg)) end
      },
      {
        title = "Properties",
        usedAttributes = {
          "AdditiveBlendAttitude",
          "AdditiveBlendPosition",
          "BreakoutTransit",
          "DeadblendBreakoutToSource",
          "FreezeDest",
          "FreezeSource",
          "SphericallyInterpolateTrajectoryPosition",
        },
        displayFunc = function(...) safefunc(attributeEditor.transitPropertiesDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Blend Properties",
        usedAttributes = {
          "UseDeadReckoningWhenDeadBlending",
          "BlendToDestinationPhysicsBones",
        },
        displayFunc = function(...) safefunc(attributeEditor.blendPropertiesDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of Transit node definition.
------------------------------------------------------------------------------------------------------------------------