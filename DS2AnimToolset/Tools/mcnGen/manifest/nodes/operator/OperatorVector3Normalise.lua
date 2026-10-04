------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorVector3Normalise node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("OperatorVector3Normalise",
  {
    displayName = "Normalise",
    helptext = "Takes one vector and outputs the unit length, normalised vector.",
    group = "Vector3 Operators",
    image = "OperatorVector3Normalise.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 175),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input"] = {
        input = true,
        array = false,
        type = "vector3",
      },
      ["Result"] = {
        input = false,
        array = false,
        type = "vector3",
      },
    },

    pinOrder = { "Input", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
    
      -- check first input
      local input = string.format("%s.Input", node)
      if isConnected{ SourcePin  = input, ResolveReferences = true } then
        local connections = listConnections{ Object = input, ResolveReferences = true }
        local inputNode = connections[1]
        if not isValid(inputNode) then
          return nil, string.format("OperatorVector3Normalise node %s requires a valid input to Input, node %s is not valid", node, inputNode)
        end
      else
        return nil, string.format("OperatorVector3Normalise node %s is missing a required connection to Input", node)
      end
      
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      --Export Connections
      
      -- export first input
      local inputNodeInfo = getConnectedNodeInfo(node, "Input")
      stream:writeNetworkNodeId(inputNodeInfo.id, "Input", inputNodeInfo.pinIndex)
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorVector3Normalise",
    {
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorNormalise node definition.
------------------------------------------------------------------------------------------------------------------------