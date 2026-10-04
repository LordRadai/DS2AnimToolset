------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorSmoothFloat node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorSmoothFloat",
{
    displayName = "Smooth Float",
    helptext = "Smoothly approach a value over time",
    group = "Float Operators",
    image = "OperatorSmoothFloat.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 142),
    version = 5,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Result"] = {
        input = false,
        array = false,
        type = "float"
      },
    },

    pinOrder = { "Input", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "SmoothTime", type = "float", value = 0.1,
        helptext = "A time constant representing the smoothness. A value of 0.1 means the output will take about 1 second (or 10 times the SmoothTime) to change after a step change in the input."
      },
      {
        name = "SmoothVelocity", 
        type = "bool", 
        value = 1, 
        helptext = "True has a smoothly changing velocity, it acts as a critically damped spring. False is only smooth in position, an exponential decay towards the input"
      },
      {
        name = "InitValueX", type = "float", value = 0.0,
        helptext = "Float initial value."
      },
      {
        name = "UseInitValueOnInit", type = "bool", value = 0,
        helptext = "Set True to use the InitValue to set the operator's value on initialisation.  False will make the operator use the input control parameter value on initialisation."
      },
    },


    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local nodeInConnected = listConnections{ Object = (node .. ".Input"), ResolveReferences = true }
      local inputNode = nodeInConnected[1]
              
      if (inputNode == nil) then
        return nil, "Operator SmoothFloat node: missing input connection"
      end
      
      if not isValid(inputNode) then
        return nil, "Operator SmoothFloat not valid"
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
      
      Stream:writeBool(true, "IsScalar")
      
      local SmoothTime = getAttribute(node, "SmoothTime")
      Stream:writeFloat(SmoothTime, "SmoothTime")
      local SmoothVelocity = getAttribute(node, "SmoothVelocity")
      Stream:writeBool(SmoothVelocity, "SmoothVelocity")
      local InitValueX = getAttribute(node, "InitValueX")
      Stream:writeFloat(InitValueX, "InitValueX")
      Stream:writeFloat(0, "InitValueY")
      Stream:writeFloat(0, "InitValueZ")
      local UseInitValueOnInit = getAttribute(node, "UseInitValueOnInit")
      Stream:writeBool(UseInitValueOnInit, "UseInitValueOnInit")

    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version > 1 and version < 4 then
        -- upgrade is handled by the onManifestChanged function
      end
    end,
    
  }
)

------------------------------------------------------------------------------------------------------------------------
-- The vector part of OperatorSmoothFloat was moved to OperatorSmoothVector3
------------------------------------------------------------------------------------------------------------------------
local onManifestChanged = function(deprecatedItemsTable, pinLookupTable, orphanedItemsTable, upgradedItemsTable)
  for _, node in upgradedItemsTable.blendNodes do
    if node.type == "OperatorSmoothFloat" then
      if node.version > 1 and node.version < 4 then
        local fullPath = string.format("%s|%s", node.path, node.name)
        if attributeExists(fullPath, "deprecated_DataType") then
          local dataType = getAttribute(fullPath, "deprecated_DataType")
          removeAttribute(fullPath, "deprecated_DataType")
          if dataType == 2 then
            replaceNodeWithNewNode(fullPath, "OperatorSmoothVector3", pinLookupTable)
          else
            if attributeExists(fullPath, "deprecated_InitValueY") then
              removeAttribute(fullPath, "deprecated_InitValueY")
            end
            if attributeExists(fullPath, "deprecated_InitValueZ") then
              removeAttribute(fullPath, "deprecated_InitValueZ")
            end
          end
        end
      end
    end
  end
end
registerEventHandler("mcManifestChange", onManifestChanged)

------------------------------------------------------------------------------------------------------------------------
-- OperatorSmoothFloat custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorSmoothFloat",
    {
      {
        title = "Properties",
        usedAttributes = { "SmoothTime", "SmoothVelocity", "InitValueX", "UseInitValueOnInit"},
        displayFunc = function(...) safefunc(attributeEditor.operatorSmoothFloatDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorSmoothFloat node definition.
------------------------------------------------------------------------------------------------------------------------
