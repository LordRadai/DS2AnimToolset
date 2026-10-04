------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorVector3RateOfChange node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorVector3RateOfChange",
{
    displayName = "Vec3 Rate of Change",
    helptext = "Outputs the rate of change of the input vector3",
    group = "Vector3 Operators",
    image = "OperatorVector3RateOfChange.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 173),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input"] = {
        input = true,
        array = false,
        type = "vector3"
      },
      ["Result"] = {
        input = false,
        array = false,
        type = "vector3"
      },
    },

    pinOrder = { "Input", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)

      -- Validate input pin
      local inputPin = string.format("%s.Input", node)
      if isConnected{ SourcePin = inputPin, ResolveReferences = true } then
        local nodesConnected = listConnections{ Object = inputPin, ResolveReferences = true }
        local inputNode = nodesConnected[1]
        if not isValid(inputNode) then
          return nil, string.format("OperatorRateOfChange node %s requires a valid input to Input, node %s is not valid", node, inputNode)
        end
      else
        return nil, string.format("OperatorRateOfChange node %s is missing a required connection to Input", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
    
      local inNodeInfo  = nil 
      if isConnected{ SourcePin = (node .. ".Input"), ResolveReferences = true } then
        inNodeInfo = getConnectedNodeInfo((node .. ".Input"))
      end
      if inNodeInfo then
        Stream:writeNetworkNodeId(inNodeInfo.id, "Input", inNodeInfo.pinIndex)
      else
        Stream:writeNetworkNodeId(-1, "Input")
      end
      
      Stream:writeBool(false, "IsScalar")
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
    end,
    
  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorVector3RateOfChange node definition.
------------------------------------------------------------------------------------------------------------------------
