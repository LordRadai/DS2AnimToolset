------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorOneInputArithmetic node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorOneInputArithmetic",
  {
    displayName = "One Input Arithmetic",
    helptext = "Simple mathematical operation using a single float input and a constant",
    group = "Float Operators",
    image = "OperatorOneInputArithmetic.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 112),
    version = 3,

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
      { name = "Operation",
        type = "string",
        value = "*",
        helptext = "Change the mathematical operation performed by the node.  For instance, select '/' to perform the operation Result = Input / Constant."
      },
      { name = "ConstantValue",
        type = "float",
        value = 0.0,
        helptext = "Constant numerical value used in the operation Result = Input operation Constant.  For instance, select '/' to perform the operation Result = Input / Constant. "
      },
    },
    
    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local nodesConnected = listConnections{ Object = (node .. ".Input"), ResolveReferences = true }
      local inputNode = nodesConnected[1]
      local constant = getAttribute(node, "ConstantValue");
      local operation = getAttribute(node, "Operation")

      if (inputNode == nil) then
        return nil, "Missing connection to Operator One Input Arithemtic node"
      end
      if not isValid(inputNode) then
        return nil, "Operator One Input Arithemtic requires a valid input"
      end

      if (constant == nil) then
         return nil, "Invalid constant value for Operator One Input Arithemtic.\n";
      end
      if (operation ~= "*") and
         (operation ~= "+") and
         (operation ~= "/") and
         (operation ~= "-") and
         (operation ~= "min") and
         (operation ~= "max") and
         (operation ~= "emult") then
        return nil, "Unrecognised operation \'" .. operation .. "\'.  Must be one of *, +, /, -, min, max."
      end
      if (operation == "/") and (constant == 0) then
        return nil, "Operator Operator One Input Arithemtic is dividing by zero. Const must be non zero if performing a divide operation"
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version == 1 then
        if attributeExists(node .. ".deprecated_Constant") then
          local deprecatedConstant = getAttribute(node .. ".deprecated_Constant")
          setAttribute(string.format("%s.ConstantValue", node), deprecatedConstant)
          removeAttribute(node, "deprecated_Constant")
        end
      elseif version < 3 then
        local dataType = 0
        if attributeExists(node .. ".DataType") then
         dataType = getAttribute(node .. ".DataType")
        end
        
        if attributeExists(node .. ".deprecated_Constant") and dataType == 1 then
          local deprecatedConstant = getAttribute(node .. ".deprecated_Constant")
          removeAttribute(node, "deprecated_Constant")
          setAttribute(string.format("%s.VectorConstantX", node), deprecatedConstant)
          if attributeExists(node, "deprecated_VectorConstantX") then
            removeAttribute(node, "deprecated_VectorConstantX")
          end
          if attributeExists(node, "deprecated_VectorConstantY") then
            removeAttribute(node, "deprecated_VectorConstantY")
          end
          if attributeExists(node, "deprecated_VectorConstantZ") then
            removeAttribute(node, "deprecated_VectorConstantZ")
          end
        else
          deprecatedConstant = getAttribute(node .. ".deprecated_VectorConstantX")
          if deprecatedConstant then
            setAttribute(string.format("%s.ConstantValueX", node), deprecatedConstant)
            removeAttribute(node, "deprecated_VectorConstantX")
          end
          deprecatedConstant = getAttribute(node .. ".deprecated_VectorConstantY")
          if deprecatedConstant then
            setAttribute(string.format("%s.ConstantValueY", node), deprecatedConstant)
            removeAttribute(node, "deprecated_VectorConstantY")
          end
          deprecatedConstant = getAttribute(node .. ".deprecated_VectorConstantZ")
          if deprecatedConstant then
            setAttribute(string.format("%s.ConstantValueZ", node), deprecatedConstant)
            removeAttribute(node, "deprecated_VectorConstantZ")
          end
        end
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputInfo = getConnectedNodeInfo(node .. ".Input")
      local operation = getAttribute(node, "Operation")
      local operationCode = -1
      -- Expand this list as necessary.  Must be consistent with enum in AttribDataArithmeticOperation
      if operation == "*" then operationCode = 0 end
      if operation == "+" then operationCode = 1 end
      if operation == "/" then operationCode = 2 end
      if operation == "-" then operationCode = 3 end
      if operation == "min" then operationCode = 4 end
      if operation == "max" then operationCode = 5 end
      if operation == "emult" then operationCode = 6 end

      if (inputInfo == nil) then
        Stream:writeNetworkNodeId(-1, "Input")
      else
        Stream:writeNetworkNodeId(inputInfo.id, "Input", inputInfo.pinIndex)
      end
      --Stream:writeBool(true, "IsScalar")
      Stream:writeInt(operationCode, "OperationCode")
      Stream:writeFloat(getAttribute(node, "ConstantValue"),  "ConstantValue")
      Stream:writeFloat(0, "ConstantValueX")
      Stream:writeFloat(0, "ConstantValueY")
      Stream:writeFloat(0, "ConstantValueZ")
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- OperatorConst was renamed to OperatorOneInputArithmetic
------------------------------------------------------------------------------------------------------------------------
local onManifestChanged = function(deprecatedItems, pinLookupTable)
  for _, node in deprecatedItems.blendNodes do
    if node.type == "OperatorConst" then
      upgradeDeprecatedNodeToNewNode("OperatorOneInputArithmetic", node, pinLookupTable)
    end
  end
end
registerEventHandler("mcManifestChange", onManifestChanged)

------------------------------------------------------------------------------------------------------------------------
-- OperatorOneInputArithmetic custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorOneInputArithmetic",
    {
      {
        title = "Properties",
        usedAttributes = { "Operation", "ConstantValue" },
        displayFunc = function(...) safefunc(attributeEditor.operatorOneInputArithmeticDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorOneInputArithmetic node definition.
------------------------------------------------------------------------------------------------------------------------
