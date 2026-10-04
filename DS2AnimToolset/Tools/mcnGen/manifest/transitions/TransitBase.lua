------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

local function buildGetDestinationState()
  local getDestinationState = function(transition)
    -- get the destination state ignoring all transition from transitions, the state
    -- will be the only connected downstream object of type "StateMachine"
    local connections = listConnections{ Object = transition, Upstream = false, Downstream = true }
    local destinationState = nil
    for i, connection in connections do
      local type, objectType = getType(connection)
      if objectType == "StateMachine" or objectType == "PhysicsStateMachine" then
        return connection
      end
    end
    -- something has gone very wrong if we get here
    return nil
  end
  return getDestinationState
end
getDestinationState = buildGetDestinationState()

-- used to validate the destination sub state of a transition
local function buildValidateDestinationSubState()
  local validateDestinationSubState = function(transition)
    local destinationSubState = getAttribute(transition, "DestinationSubState")
    
    -- No destination sub-state is specified so it must be valid
    if not destinationSubState or string.len(destinationSubState) == 0 then
      return true
    end
    
    -- validation is quite minimal as specifying 'kind = "transitionSubState"' should guarantee that the node is valid
    -- if the node that it references is valid
    if not isValid(destinationSubState) then
      return false, string.format("The transition %s is not valid as the destination sub-state %s is not valid", transition, destinationSubState)
    end
    
    return true
  end

  return validateDestinationSubState
end

-- A set of manifest types indicating that the graph is a state machine
local kStateMachineManifestTypes = {
  ["PhysicsStateMachine"] = true,
  ["BehaviourStateMachine"] = true,
  ["StateMachine"] = true
}

-- A set of blend tree types
local kBlendTreeTypes = {
  ["BlendTree"] = true,
  ["PhysicsBlendTree"] = true,
}

validateDestinationSubState = buildValidateDestinationSubState()
-- serialize the destination sub state of a transition
local function buildSerializeDestinationSubState()
  local serializeDestinationSubState = function(transition, stream)
    -- State machines nested in blend trees add no pair of their own.
    local parents, states = { }, { }

    local destNodesTable = listConnections{ Object= transition, Upstream = false, Downstream = true, ResolveReferences = true }
    local destNodeName = destNodesTable[1]

    local destinationSubState = getAttribute(transition, "DestinationSubState")
    if type(destinationSubState) == "string" and string.len(destinationSubState) > 0 then
      while destinationSubState ~= destNodeName do
        local parent = getParent(destinationSubState)
        local parentType, parentManifestType = getType(parent)
        local parentIsBlendTree = kBlendTreeTypes[parentType] == true
        local parentGraphIsStateMachine = parentIsBlendTree and (kStateMachineManifestTypes[parentManifestType] == true)
        if (not parentIsBlendTree) or parentGraphIsStateMachine then
          table.insert(parents, 1, getRuntimeID(parent))
          table.insert(states, 1, getRuntimeID(destinationSubState))
        end
        destinationSubState = parent
      end
    end

    local count = table.getn(states)
    stream:writeInt(count, "DestinationSubStateCount")
    for i = 1, count do
      -- a sub state in a referenced network without connected weight pins has no runtime ids
      if parents[i] and states[i] then
        stream:writeNetworkNodeId(parents[i], string.format("DestinationSubStateParentID_%d", i - 1))
        stream:writeNetworkNodeId(states[i], string.format("DestinationSubStateID_%d", i - 1))
      end
    end
  end
  return serializeDestinationSubState
end
serializeDestinationSubState = buildSerializeDestinationSubState()

-- serialize the common dead blend properties of the transition
local function buildSerializeDeadblendProperties()
  local serializeDeadblendProperties = function(transition, stream)
    local useDeadReckoningWhenDeadBlending = getAttribute(transition, "UseDeadReckoningWhenDeadBlending")
    local blendToDestinationPhysicsBones = getAttribute(transition, "BlendToDestinationPhysicsBones")

    stream:writeBool(useDeadReckoningWhenDeadBlending, "UseDeadReckoningWhenDeadBlending")
    stream:writeBool(blendToDestinationPhysicsBones, "BlendToDestinationPhysicsBones")
  end
  return serializeDeadblendProperties
end
serializeDeadblendProperties = buildSerializeDeadblendProperties()

local environment = app.getLuaEnvironment("ValidateSerialize")

app.registerToEnvironment(buildGetDestinationState(), "getDestinationState", environment)
app.registerToEnvironment(buildValidateDestinationSubState(), "validateDestinationSubState", environment)
app.registerToEnvironment(buildSerializeDestinationSubState(), "serializeDestinationSubState", environment)
app.registerToEnvironment(buildSerializeDeadblendProperties(), "serializeDeadblendProperties", environment)

