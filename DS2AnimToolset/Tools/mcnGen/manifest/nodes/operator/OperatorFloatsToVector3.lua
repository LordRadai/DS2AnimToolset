------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorFloatsToVector3 node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorFloatsToVector3",
  {
    displayName = "Floats To Vec3",
    helptext = "Simple Node that takes 3 floats and combines them into a Vec3",
    group = "Float Operators",
    image = "OperatorFloatsToVector3.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 144),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["FloatX"] = {
        input = true,
        array = false,
        type = "float",
      },
      ["FloatY"] = {
        input = true,
        array = false,
        type = "float",
      },
      ["FloatZ"] = {
        input = true,
        array = false,
        type = "float",
      },
      ["Result"] = {
        input = false,
        array = false,
        type = "vector3",
      },
    },

    pinOrder = { "FloatX", "FloatY", "FloatZ", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
    
      local floatXPin = string.format("%s.FloatX", node)
      if isConnected{ SourcePin  = floatXPin, ResolveReferences = true } then
        local connections = listConnections{ Object = floatXPin, ResolveReferences = true }
        local floatXNode = connections[1]
        if not isValid(floatXNode) then
          return nil, string.format("OperatorFloatsToVector3 node %s requires a valid input to FloatX", node)
        end
      else
        return nil, string.format("OperatorFloatsToVector3 node %s is missing a required connection to FloatX", node)
      end
      
      local floatYPin = string.format("%s.FloatY", node)
      if isConnected{ SourcePin  = floatYPin, ResolveReferences = true } then
        local connections = listConnections{ Object = floatYPin, ResolveReferences = true }
        local floatYNode = connections[1]
        if not isValid(floatYNode) then
          return nil, string.format("OperatorFloatsToVector3 node %s requires a valid input to FloatY", node)
        end
      else
        return nil, string.format("OperatorFloatsToVector3 node %s is missing a required connection to FloatY", node)
      end
      
      local floatZPin = string.format("%s.FloatZ", node)
      if isConnected{ SourcePin  = floatZPin, ResolveReferences = true } then
        local connections = listConnections{ Object = floatZPin, ResolveReferences = true }
        local floatZNode = connections[1]
        if not isValid(floatZNode) then
          return nil, string.format("OperatorFloatsToVector3 node %s requires a valid input to FloatZ", node)
        end
      else
        return nil, string.format("OperatorFloatsToVector3 node %s is missing a required connection to FloatZ", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local input0Info = getConnectedNodeInfo((node .. ".FloatX"))
      local input1Info = getConnectedNodeInfo((node .. ".FloatY"))
      local input2Info = getConnectedNodeInfo((node .. ".FloatZ"))
      Stream:writeNetworkNodeId(input0Info.id, "FloatX", input0Info.pinIndex)
      Stream:writeNetworkNodeId(input1Info.id, "FloatY", input1Info.pinIndex)
      Stream:writeNetworkNodeId(input2Info.id, "FloatZ", input2Info.pinIndex)
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorFloatsToVector3 node definition.
------------------------------------------------------------------------------------------------------------------------
