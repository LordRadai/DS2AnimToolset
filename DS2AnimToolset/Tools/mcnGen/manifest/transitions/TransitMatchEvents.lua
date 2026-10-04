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
require "ui/AttributeEditor/TransitMatchEventsDisplayInfo.lua"
require "ui/AttributeEditor/TransitPropertiesDisplayInfo.lua"
require "ui/AttributeEditor/BlendDurationEventDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- TransitMatchEvents transition definition.
------------------------------------------------------------------------------------------------------------------------
registerTransition("TransitMatchEvents",
  {
    helptext = "Performs a transition blend between two states over a given period of time. Blends and outputs Events.",
    interfaces = { "Transforms", "Time", "Events" },
    supportsTransitToSelf = false,
    animId = generateNamespacedId(idNamespaces.NaturalMotion, 400),
    physicsId = generateNamespacedId(idNamespaces.NaturalMotion, 401),
    version = 5,

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      -- It would be ideal if this information was not stored here but rather that we get it from the firing of transition conditions.
      -- This would also allow us to specify sets of either Fractions or EventPositions. We cant currently do this because we have no way of knowing
      -- which one of the set fired the transition.

      {
        name = "DurationInEvents", type = "float", value = 0.5, min = 0.0,  -- How long the transition will last expressed as a number of Events (whole and fractional).
        helptext = "This controls how long the transition will last expressed in whole and fractional events."
      },

      -- These 2 are mutually exclusive.
      {
        name = "DestEventSequenceOffset", type = "int", value = 0, min = 0, -- Dest starting index is dependent on the starting index of the source.
        helptext = "Offset the destination synchronisation event track in relation to the source synchronisation event track."
      },
      {
        name = "DestStartEventIndex", type = "int", value = 0, min = 0,     -- Dest will always start in this Event range no matter what the Event index in the source.
        helptext = "Force a particular event from the destination synchronisation event track to be the destination start event (irrespective of the source start event)."
      },

      {
        name = "UsingDestStartEventIndex", type = "bool", value = false,    -- If false, then using DestEventSequenceOffset instead.
        helptext = "Method to determine the start event within the destination."
      },
      {
        name = "DurationEventBlendPassThrough", type = "bool", value = false,
        helptext = "Merge the event tracks together into one resulting output track without blending."
      },
      {
        name = "DurationEventBlendIgnoreEventOrder", type = "bool", value = false,
        helptext = "Ignore the order of events when blending means that events in the source tracks will be combined even if they have duration events in different orders."
      },
      {
        name = "DurationEventBlendSameUserData", type = "bool", value = false,
        helptext = "Duration events will only be blended together when they share the same user data."
      },
      {
        name = "DurationEventBlendOnOverlap", type = "bool", value = false,
        helptext = "Blend duration events when their durations overlap."
      },
      {
        name = "DurationEventBlendWithinRange", type = "bool", value = false,
        helptext = "Blend duration events when they occur within a specified range of one another."
      },
      {
        name = "ReversibleTransit", type = "bool", value = false,
        helptext = "Make transition reversible."
      },
      {
        name = "ReverseControlParameter", type = "controlParameter",
        helptext = "Control parameter for transition reversing."
      },
      {
        name = "DeltaTrajSource", type = "int", value = 3,
        helptext = "Specifies which child/children we will use to source our delta trajectory."
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
        name = "DeadblendBreakoutToSource", type = "bool", value = false, displayName = "Dead Blend Source on Breakout",
        helptext = "If turned on, a breakout transition to this transition's source will replace the source with a dead-reckoned animation."
      },
      {
        name = "DestinationSubState", type = "ref", kind = "transitionSubState",
        helptext = "An optional override for transitioning into sub-states other than the defaults."
      },
      {
        name = "BreakoutTransit", type = "bool", value = false,
        helptext = "Allows this transition to execute while a transition to its source state is still in progress."
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

          if CP_Type ~= "bool" then
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

      local durationInEvents = getAttribute(transition, "DurationInEvents")
      local destEventSequenceOffset = getAttribute(transition, "DestEventSequenceOffset")
      local destStartEventIndex = getAttribute(transition, "DestStartEventIndex")
      local usingDestStartEventIndex = getAttribute(transition, "UsingDestStartEventIndex")
      local deltaTrajSource = getAttribute(transition, "DeltaTrajSource")

      local additiveAtt = getAttribute(transition, "AdditiveBlendAttitude")
      local additivePos = getAttribute(transition, "AdditiveBlendPosition")
      local sphericalTrajPos = getAttribute(transition, "SphericallyInterpolateTrajectoryPosition")
      local deadblendBreakoutToSource = getAttribute(transition, "DeadblendBreakoutToSource")

   if sourceNodeRuntimeID ~= nil then
        -- a nil value indicates that the source node will be determined by the runtime
        Stream:writeNetworkNodeId(sourceNodeRuntimeID, "SourceNodeID")
      end
      if destNodeRuntimeID ~= nil then
        -- a nil value indicates that the destination node will be determined by the runtime
        Stream:writeNetworkNodeId(destNodeRuntimeID, "DestNodeID")
      end

      Stream:writeFloat(durationInEvents, "DurationInEvents")
      Stream:writeUInt(destEventSequenceOffset, "DestEventSequenceOffset")
      Stream:writeUInt(destStartEventIndex, "DestStartEventIndex")
      Stream:writeBool(usingDestStartEventIndex, "UsingDestStartEventIndex")
      Stream:writeUInt(deltaTrajSource, "DeltaTrajSource")

      Stream:writeBool(additiveAtt, "AdditiveBlendAttitude")
      Stream:writeBool(additivePos, "AdditiveBlendPosition")
      Stream:writeBool(sphericalTrajPos, "SphericallyInterpolateTrajectoryPosition")
      Stream:writeBool(deadblendBreakoutToSource, "DeadblendBreakoutToSource")

      -- Duration event blending flags --
      local durationEventBlendPassThrough = getAttribute(transition, "DurationEventBlendPassThrough")
      local durationEventBlendIgnoreEventOrder = getAttribute(transition, "DurationEventBlendIgnoreEventOrder")
      local durationEventBlendSameUserData = getAttribute(transition, "DurationEventBlendSameUserData")
      local durationEventBlendOnOverlap = getAttribute(transition, "DurationEventBlendOnOverlap")
      local durationEventBlendWithinRange = getAttribute(transition, "DurationEventBlendWithinRange")
      Stream:writeBool(durationEventBlendPassThrough, "DurationEventBlendPassThrough")
      Stream:writeBool(not durationEventBlendIgnoreEventOrder, "DurationEventBlendInSequence")
      Stream:writeBool(durationEventBlendSameUserData, "DurationEventBlendSameUserData")
      Stream:writeBool(durationEventBlendOnOverlap, "DurationEventBlendOnOverlap")
      Stream:writeBool(durationEventBlendWithinRange, "DurationEventBlendWithinRange")

      local breakoutTransit = getAttribute(transition, "BreakoutTransit")
      Stream:writeBool(breakoutTransit, "BreakoutTransit")

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
      if version < 2 then
        -- nothing to do here
      end

      if version < 3 then
        local value = getAttribute(transition, "deprecated_DurationEventBlendInSequence")
        setAttribute(string.format("%s.DurationEventBlendIgnoreEventOrder", transition), not value)
        removeAttribute(transition, "deprecated_DurationEventBlendInSequence")
      end

      if version < 4 then
        -- deprecated_Duration may or may not exist on version 4 files due to
        -- a previously missing upgrade function when the attribute was first renamed.
        local attributes = listAttributes(transition)

        local deprecatedAttributes = { }
        for i, attribute in ipairs(attributes) do
          if attribute == "deprecated_Duration" then
            local deprecated = string.format("%s.%s", transition, attribute)

            local value = getAttribute(deprecated)
            setAttribute(string.format("%s.DurationInTime", transition), value)

            removeAttribute(transition, attribute)
          end
        end
      end

      if version < 5 then
        -- there was an incorrect upgrade function at version 3 so this attribute may be hanging around
        -- and should be removed
        if attributeExists(transition, "deprecated_DurationEventBlendInSequence") then
          removeAttribute(transition, "deprecated_DurationEventBlendInSequence")
        end
      end
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "TransitMatchEvents",
    {
      {
        title = "Duration",
        usedAttributes = {
          "DurationInEvents"
        },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Destination Start Event",
        usedAttributes = {
          "DestEventSequenceOffset",
          "DestStartEventIndex",
          "UsingDestStartEventIndex"
        },
        displayFunc = function(...) safefunc(attributeEditor.transitMatchEventsDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Duration Event Blending",
        usedAttributes = {
          "DurationEventBlendPassThrough",
          "DurationEventBlendIgnoreEventOrder",
          "DurationEventBlendSameUserData",
          "DurationEventBlendOnOverlap",
          "DurationEventBlendWithinRange",
        },
        displayFunc = function(...) safefunc(attributeEditor.blendDurationEventDisplayInfo, unpack(arg)) end
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
-- End of TransitMatchEvents node definition.
------------------------------------------------------------------------------------------------------------------------