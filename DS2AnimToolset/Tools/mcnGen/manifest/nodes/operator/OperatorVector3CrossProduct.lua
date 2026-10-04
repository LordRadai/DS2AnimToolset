------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorVector3CrossProduct node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("OperatorVector3CrossProduct",
  {
    displayName = "Cross Product",
    helptext = "Takes one vector and outputs cross product vector, which is at right angles to both inputs.",
    group = "Vector3 Operators",
    image = "OperatorVector3CrossProduct.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 172),
    version = 2,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input0"] = {
        input = true,
        array = false,
        type = "vector3",
      },
      ["Input1"] = {
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

    pinOrder = { "Input0", "Input1", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
    
      -- check first input
      local inVector3A = string.format("%s.Input0", node)
      if isConnected{ SourcePin  = inVector3A, ResolveReferences = true } then
        local connections = listConnections{ Object = inVector3A, ResolveReferences = true }
        local inVector3Node = connections[1]
        if not isValid(inVector3Node) then
          return nil, string.format("OperatorVector3CrossProduct node %s requires a valid input to InVector3, node %s is not valid", node, inVector3Node)
        end
      else
        return nil, string.format("OperatorVector3CrossProduct node %s is missing a required connection to Source0", node)
      end
      
      local inVector3B = string.format("%s.Input1", node)
      if isConnected{ SourcePin  = inVector3B, ResolveReferences = true } then
        local connections = listConnections{ Object = inVector3B, ResolveReferences = true }
        local inVector3Node = connections[1]
        if not isValid(inVector3Node) then
          return nil, string.format("OperatorVector3CrossProduct node %s requires a valid input to InVector3, node %s is not valid", node, inVector3Node)
        end
      else
        return nil, string.format("OperatorVector3CrossProduct node %s is missing a required connection to Source0", node)
      end
      
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      --Export Connections
      
      -- export first input
      local inVector3NodeInfoA = getConnectedNodeInfo(node, "Input0")
      stream:writeNetworkNodeId(inVector3NodeInfoA.id, "Input0", inVector3NodeInfoA.pinIndex)
      -- export second input
      local inVector3NodeInfoB = getConnectedNodeInfo(node, "Input1")
      stream:writeNetworkNodeId(inVector3NodeInfoB.id, "Input1", inVector3NodeInfoB.pinIndex)
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        local oldSourcePath = string.format("%s.InVector3A", node)
        local newSourcePath = string.format("%s.Input0", node)
        pinLookupTable[oldSourcePath] = newSourcePath
        
        local oldSourcePath = string.format("%s.InVector3B", node)
        local newSourcePath = string.format("%s.Input1", node)
        pinLookupTable[oldSourcePath] = newSourcePath
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorVector3CrossProduct",
    {
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorCrossProduct node definition.
------------------------------------------------------------------------------------------------------------------------