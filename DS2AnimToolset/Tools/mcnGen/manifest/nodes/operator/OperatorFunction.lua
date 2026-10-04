------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorFunction node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorFunction",
  {
    displayName = "Function",
    helptext = "Simple mathmatical operation on one float parameter",
    group = "Float Operators",
    image = "OperatorFunction.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 110),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    dataPins =
    {
      ["Input"] = {
        input = true,
        array = false,
        type = "float",
      },
      ["Result"] = {
       input = false,
       array = false,
       type = "float",
      },
    },

    pinOrder = { "Input", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      { name = "Operation",
        type = "string",
        value = "sin",
        helptext = "Change the mathematical operation performed on the input",
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local nodesConnected = listConnections{ Object =(node .. ".Input"), ResolveReferences = true }
      local inputNode = nodesConnected[1]
      local operation = getAttribute(node, "Operation")
      if (inputNode == nil) then
        return nil, "Missing connection to Operator Function"
      end
      if not isValid(inputNode) then
        return nil, "Operator Function requires a valid input"
      end
      if (operation ~= "sin") and
         (operation ~= "cos") and
         (operation ~= "tan") and
         (operation ~= "exp") and
         (operation ~= "log") and
         (operation ~= "sqrt") and
         (operation ~= "abs") and
         (operation ~= "asin") and
         (operation ~= "acos") then
        return nil, "Unrecognised operation \'" .. operation .. "\'.  Must be one of sin, cos, tan, exp, log, sqrt, abs."
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputInfo = getConnectedNodeInfo((node .. ".Input"))
      local operation = getAttribute(node, "Operation")
      local operationCode = -1
      -- Expand this list as necessary.  Must be consistent with enum in
      --  mrNodeControlParamOp.h
      if operation == "sin" then operationCode = 0 end
      if operation == "cos" then operationCode = 1 end
      if operation == "tan" then operationCode = 2 end
      if operation == "exp" then operationCode = 3 end
      if operation == "log" then operationCode = 4 end
      if operation == "sqrt" then operationCode = 5 end
      if operation == "abs" then operationCode = 6 end
      if operation == "asin" then operationCode = 7 end
      if operation == "acos" then operationCode = 8 end

      Stream:writeNetworkNodeId(inputInfo.id, "Input", inputInfo.pinIndex)
      Stream:writeInt(operationCode, "OperationCode")
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- OperatorFloat was renamed to OperatorFunction
------------------------------------------------------------------------------------------------------------------------
local onManifestChanged = function(deprecatedItems, pinLookupTable)
  for _, node in deprecatedItems.blendNodes do
    if node.type == "OperatorFloat" then
      upgradeDeprecatedNodeToNewNode("OperatorFunction", node, pinLookupTable)
    end
  end
end
registerEventHandler("mcManifestChange", onManifestChanged)

------------------------------------------------------------------------------------------------------------------------
-- OperatorFunction custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorFunction",
    {
      {
        title = "Properties",
        usedAttributes = { "Operation" },
        displayFunc = function(...) safefunc(attributeEditor.operatorFunctionDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorFunction node definition.
------------------------------------------------------------------------------------------------------------------------
