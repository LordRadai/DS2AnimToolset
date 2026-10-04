------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorSmoothVector3 node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorSmoothVector3",
{
    displayName = "Vec3 Smooth",
    helptext = "Smoothly approach a value over time",
    group = "Vector3 Operators",
    image = "OperatorSmoothVector3.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 142),
    version = 3,

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
      {
        name = "SmoothTime", type = "float", value = 0.0,
        helptext = "Time required to approach the input specified in game units (usually seconds). Because the system is implemented as a damped spring the SmoothTime is the expected lag of the result behind a constantly changing input."
      },
      {
        name = "SmoothVelocity", 
        type = "bool", 
        value = 1, 
        helptext = "True has a smoothly changing velocity, it acts as a critically damped spring. False is only smooth in position, an exponential decay towards the input"
      },
      {
        name = "InitValueX", type = "float", value = 0.0,
        helptext = "Vector3 (x,y,z) initial value."
      },
      {
        name = "InitValueY", type = "float", value = 0.0,
        helptext = "Vector3 (x,y,z) initial value."
      },
      {
        name = "InitValueZ", type = "float", value = 0.0,
        helptext = "Vector3 (x,y,z) initial value."
      },
      {
        name = "UseInitValueOnInit", type = "bool", value = 0,
        helptext = "Set True to use the InitValue to set the operator's value on initialisation.  False will make the operator use the input control parameter value on initialisation."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local nodeInConnected = listConnections{ Object = (node .. ".Input"), ResolveReferences = true }
      local inNode = nodeInConnected[1]
              
      if (inNode == nil) then
        return nil, "Operator SmoothVector3 node: missing input connection"
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
      
      local SmoothTime = getAttribute(node, "SmoothTime")
      Stream:writeFloat(SmoothTime, "SmoothTime")
      local SmoothVelocity = getAttribute(node, "SmoothVelocity")
      Stream:writeBool(SmoothVelocity, "SmoothVelocity")
      local InitValueX = getAttribute(node, "InitValueX")
      Stream:writeFloat(InitValueX, "InitValueX")
      local InitValueY = getAttribute(node, "InitValueY")
      Stream:writeFloat(InitValueY, "InitValueY")
      local InitValueZ = getAttribute(node, "InitValueZ")
      Stream:writeFloat(InitValueZ, "InitValueZ")
      local UseInitValueOnInit = getAttribute(node, "UseInitValueOnInit")
      Stream:writeBool(UseInitValueOnInit, "UseInitValueOnInit")

    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
    end,
    
  }
)

------------------------------------------------------------------------------------------------------------------------
-- OperatorSmoothVector3 custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorSmoothVector3",
    {
      {
        title = "Properties",
        usedAttributes = { "SmoothTime", "SmoothVelocity", "InitValueX", "InitValueY", "InitValueZ", "UseInitValueOnInit" },
        displayFunc = function(...) safefunc(attributeEditor.operatorSmoothVector3DisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorSmoothVector3 node definition.
------------------------------------------------------------------------------------------------------------------------
