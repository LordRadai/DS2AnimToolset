------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorVector3Angle node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorVector3Angle",
{
    displayName = "Vec3 Angle",
    helptext = "Output the angle between two vectors",
    group = "Vector3 Operators",
    image = "OperatorVector3Angle.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 174),
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
      ["Degrees"] = {
        input = false,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Input0", "Input1", "Degrees", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "Axis",
        type = "int",
        value = 0, -- 0: none or 1,2,3 for the primary axes
        helptext = "Select which axis to get angle around."        
      },
    },
    
    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      -- check first input
      local inVector3PinA = string.format("%s.Input0", node)
      if isConnected{ SourcePin  = inVector3PinA, ResolveReferences = true } then
        local connections = listConnections{ Object = inVector3PinA, ResolveReferences = true }
        local inVector3Node = connections[1]
        if not isValid(inVector3Node) then
          return nil, string.format("OperatorVector3Angle node %s requires a valid input to Input0, node %s is not valid", node, inVector3Node)
        end
      else
        return nil, string.format("OperatorVector3Angle node %s is missing a required connection to Source0", node)
      end
      
      -- check second input
      local inVector3PinB = string.format("%s.Input1", node)
      if isConnected{ SourcePin  = inVector3PinB, ResolveReferences = true } then
        local connections = listConnections{ Object = inVector3PinB, ResolveReferences = true }
        local inVector3Node = connections[1]
        if not isValid(inVector3Node) then
          return nil, string.format("OperatorVector3Angle node %s requires a valid input to Input1, node %s is not valid", node, inVector3Node)
        end
      else
        return nil, string.format("OperatorVector3Angle node %s is missing a required connection to Source1", node)
      end
      
      return true
    end,

    serialize = function(node, stream)
      --Export Connections
      
      -- export first input
      local inVector3NodeInfoA = getConnectedNodeInfo(node, "Input0")
      stream:writeNetworkNodeId(inVector3NodeInfoA.id, "Input0", inVector3NodeInfoA.pinIndex)
      -- export second input
      local inVector3NodeInfoB = getConnectedNodeInfo(node, "Input1")
      stream:writeNetworkNodeId(inVector3NodeInfoB.id, "Input1", inVector3NodeInfoB.pinIndex)
      
      local Axis = getAttribute(node, "Axis")
      stream:writeInt(Axis, "Axis")
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
-- OperatorVector3Angle custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorVector3Angle",
    {
      {
        title = "Properties",
        usedAttributes = { "Axis" },
        displayFunc = function(...) safefunc(attributeEditor.operatorVector3AngleDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorVector3Angle node definition.
------------------------------------------------------------------------------------------------------------------------
