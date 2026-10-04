------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- InSubState condition definition.
------------------------------------------------------------------------------------------------------------------------

registerCondition("InSubState",
  {
    id = generateNamespacedId(idNamespaces.NaturalMotion, 617),
    version = 1,
    helptext = "Requires that the source state machine has a specific sub state node active.",

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      ["Node"] =
      {
       type = "ref", kind = "conditionSubState",
       helptext = "The full path of the sub state node."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(condition)
      local node = getAttribute(condition, "Node")

      if not node or string.len(node) == 0 then
        return false, string.format("The condition %s is not valid as no substate has been specified", condition)
      end
      
      -- validation is quite minimal as specifying 'kind = "conditionSubState"' should gurantee that the node is valid
      -- if the node that it references is valid
      if not isValid(node) then
        return false, string.format("The condition %s is not valid as the sub-state node %s is not valid", condition, node)
      end
    
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(condition, Stream)
      local node = getAttribute(condition, "Node")
      Stream:writeNetworkNodeId(getRuntimeID(node), "NodeID")
    end,

  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "InSubState",
    {
      {
        title = "Node",
        usedAttributes = {
          "Node"
        },
        displayFunc = function(...) safefunc(attributeEditor.conditionSubStateDisplayInfo, unpack(arg)) end
      }
    }
  )
end