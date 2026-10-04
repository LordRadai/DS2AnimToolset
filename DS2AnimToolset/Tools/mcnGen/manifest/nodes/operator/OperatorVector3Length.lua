------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorVector3Distance node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("OperatorVector3Length",
  {
    displayName = "Vec3 Length",
    helptext = "Returns the length, or magnitude of a vector",
    group = "Vector3 Operators",
    image = "OperatorVector3Length.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 171),
    version = 2,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input"] = {
        input = true,
        array = false,
        type = "vector3",
      },
      ["Length"] = {
        input = false,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Input", "Length", },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
    
      -- check first input
      local inVector3 = string.format("%s.Input", node)
      if isConnected{ SourcePin  = inVector3, ResolveReferences = true } then
        local connections = listConnections{ Object = inVector3, ResolveReferences = true }
        local inVector3Node = connections[1]
        if not isValid(inVector3Node) then
          return nil, string.format("OperatorVector3Length node %s requires a valid input to Input, node %s is not valid", node, inVector3Node)
        end
      else
        return nil, string.format("OperatorVector3Length node %s is missing a required connection to Input", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      --Export Connections
      
      -- export first input
      local inVector3NodeInfo = getConnectedNodeInfo(node, "Input")
      stream:writeNetworkNodeId(inVector3NodeInfo.id, "Input", inVector3NodeInfo.pinIndex)
    end,

     --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        local oldSourcePath = string.format("%s.InVector3", node)
        local newSourcePath = string.format("%s.Input", node)
        pinLookupTable[oldSourcePath] = newSourcePath
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorVector3Distance node definition.
------------------------------------------------------------------------------------------------------------------------