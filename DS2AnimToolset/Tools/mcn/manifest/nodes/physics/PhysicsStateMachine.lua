------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

registerPhysicsStateMachine("PhysicsStateMachine",
{
    displayName = "Physics State Machine",
    helptext = "Create a Physics State Machine",
    group = "Containers",
    image = "PhysicsStateMachine.png",
    id = 10, -- ID of old StateMachine
    version = 1,
    interfaces = { },
    functionPins = { },
    pinOrder = { "Result" },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      -- Intentionally left blank
    },

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      -- Please note that connect automatically serializes state machines to contain details of the 
      -- contained transitions and nodes.  Any values written to the stream will follow the default data
      local defaultState = getDefaultState(node);
      -- if there is no default state we grab the first of the children and make it the default
      local smChildren = listChildren(node)
      local smIndex = 1
      local childCount =  table.getn(smChildren)
      if (defaultState == nil) then
        defaultState = smChildren[smIndex];
      end

      while not isValid(defaultState) and smIndex <= childCount do
        local childType =  getType(smChildren[smIndex])
        if childType == "PhysicsBlendTree" or childType == "BlendTree" or childType == "StateMachine" or childType == "PhysicsStateMachine" then
          if (isValid(smChildren[smIndex])) then
            defaultState = smChildren[smIndex]
            break
          end
          end
        smIndex = smIndex + 1
      end

      -- write the id of the default state
      local runtimeID = getRuntimeID(defaultState)
      stream:writeNetworkNodeId(runtimeID, "DefaultNodeID")   
    end,
    
    --------------------------------------------------------------------------------------------------------------------
    validate = function(node, Stream)
       -- Although returning false from this function will stop serialization of the state machine, returing true will 
       -- not force the state machine to a valid state if it does not contain a valid state or transition
       return true
    end

  }
)

------------------------------------------------------------------------------------------------------------------------
-- PhysicsStateMachine custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "PhysicsStateMachine",
    {
      -- Intentionally left blank
    }
  )
end

