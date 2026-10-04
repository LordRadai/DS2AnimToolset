------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorArithmetic node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorArithmetic",
  {
    displayName = "Arithmetic",
    helptext = "Simple mathematical operation on two float inputs",
    group = "Float Operators",
    image = "OperatorArithmetic.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 111),
    version = 2,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input0"] = {
        input = true,
        array = false,
        type = "float"
      },
      ["Input1"] = {
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

    pinOrder = { "Input0", "Input1", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      { name = "Operation",
        type = "string",
        value = "*",
        helptext = "Change the mathematical operation performed by the node.  For instance, if '/' is selected the node performs the operation Result = Input0  /  Input1. "
      },
    },
    
    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local nodesConnectedTo0 = listConnections{ Object = (node .. ".Input0"), ResolveReferences = true }
      local nodesConnectedTo1 = listConnections{ Object = (node .. ".Input1"), ResolveReferences = true }
      local node0 = nodesConnectedTo0[1]
      local node1 = nodesConnectedTo1[1]
      if (node0 == nil) or (node1 == nil) then
        return nil, "Missing connection to Operator Arithmetic"
      end
      if not isValid(node0) or not isValid(node1) then
        return nil, "Operator Arithmetic requires two valid inputs"
      end
      local operation = getAttribute(node, "Operation")
      if (operation ~= "*") and
         (operation ~= "+") and
         (operation ~= "/") and
         (operation ~= "-") and
         (operation ~= "min") and
         (operation ~= "max") then
        return nil, "Unrecognised operation \'" .. operation .. "\'.  Must be one of *, +, /, -, min, max, emult."
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local input0Info = getConnectedNodeInfo((node .. ".Input0"))
      local input1Info = getConnectedNodeInfo((node .. ".Input1"))
      local operation = getAttribute(node, "Operation")
      local operationCode = -1;
      if operation == "*" then operationCode = 0 end
      if operation == "+" then operationCode = 1 end
      if operation == "/" then operationCode = 2 end
      if operation == "-" then operationCode = 3 end
      if operation == "min" then operationCode = 4 end
      if operation == "max" then operationCode = 5 end

      Stream:writeNetworkNodeId(input0Info.id, "Input0", input0Info.pinIndex)
      Stream:writeNetworkNodeId(input1Info.id, "Input1", input1Info.pinIndex)
      --Stream:writeBool(true, "IsScalar")
      Stream:writeInt(operationCode, "OperationCode")
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- Operator2Float was renamed to OperatorArithmetic
------------------------------------------------------------------------------------------------------------------------
local onManifestChanged = function(deprecatedItems, pinLookupTable)
  for _, node in deprecatedItems.blendNodes do
    if node.type == "Operator2Float" then
      upgradeDeprecatedNodeToNewNode("OperatorArithmetic", node, pinLookupTable)
    end
  end
end
registerEventHandler("mcManifestChange", onManifestChanged)

------------------------------------------------------------------------------------------------------------------------
-- OperatorArithmetic custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorArithmetic",
    {
      {
        title = "Properties",
        usedAttributes = { "Operation" },
        displayFunc = function(...) safefunc(attributeEditor.operatorArithmeticDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorArithmetic node definition.
------------------------------------------------------------------------------------------------------------------------
