------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorVector3OneInputArithmetic node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorVector3OneInputArithmetic",
  {
    displayName = "Vec3 One Input Arithmetic",
    helptext = "Simple mathematical element-wise operation using a single vector input and a vector constant",
    group = "Vector3 Operators",
    image = "OperatorVector3OneInputArithmetic.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 185),
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
      { name = "Operation",
        type = "string",
        value = "*",
        helptext = "Change the mathematical operation performed by the node.  For instance, select '/' to perform the operation Result = Input / Constant. Min, max operations act per-component on vectors."
      },
      { name = "ConstantValueX",
        type = "float",
        value = 0.0,
        helptext = "Constant numerical value used in the operation Result = Input operation Constant.  For instance, select '/' to perform the operation Result = Input / Constant. "
      },
      { name = "ConstantValueY",
        type = "float",
        value = 0.0,
        helptext = "Constant numerical value used in the operation Result = Input operation Constant.  For instance, select '/' to perform the operation Result = Input / Constant. "
      },
      { name = "ConstantValueZ",
        type = "float",
        value = 0.0,
        helptext = "Constant numerical value used in the operation Result = Input operation Constant.  For instance, select '/' to perform the operation Result = Input / Constant. "
      },
    },

    
    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local nodesConnected = listConnections{ Object = (node .. ".Input"), ResolveReferences = true }
      local inputNode = nodesConnected[1]
      local operation = getAttribute(node, "Operation")
      if (inputNode == nil) then
        return nil, "Missing connection to Operator Vector3 Const node"
      end
      if (getAttribute(node, "ConstantValueX") == nil) then
         return nil, "Invalid constant value for Operator Vector3 Const.\n";
      end
      if (operation ~= "*") and
         (operation ~= "+") and
         (operation ~= "/") and
         (operation ~= "-") and
         (operation ~= "min") and
         (operation ~= "max") then
        return nil, "Unrecognised operation \'" .. operation .. "\'.  Must be one of *, +, /, -, min, max."
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if  version > 1 and version < 3 then
        local deprecatedConstant = getAttribute(node .. ".deprecated_Constant")
        
        local dataType = 0
        if attributeExists(node .. ".DataType") then
         dataType = getAttribute(node .. ".DataType")
        end
        
        if deprecatedConstant and dataType == 1 then
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

      if (inputInfo == nil) then
        Stream:writeNetworkNodeId(-1, "Input")
      else
        Stream:writeNetworkNodeId(inputInfo.id, "Input", inputInfo.pinIndex)
      end
      --Stream:writeBool(false, "IsScalar")
      Stream:writeInt(operationCode, "OperationCode")
      Stream:writeFloat(0,  "ConstantValue")
      Stream:writeFloat(getAttribute(node, "ConstantValueX"), "ConstantValueX")
      Stream:writeFloat(getAttribute(node, "ConstantValueY"), "ConstantValueY")
      Stream:writeFloat(getAttribute(node, "ConstantValueZ"), "ConstantValueZ")
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- OperatorVector3Const was renamed to OperatorVector3OneInputArithmetic
------------------------------------------------------------------------------------------------------------------------
local onManifestChanged = function(deprecatedItems, pinLookupTable)
  for _, node in deprecatedItems.blendNodes do
    if node.type == "OperatorVector3Const" then
      upgradeDeprecatedNodeToNewNode("OperatorVector3OneInputArithmetic", node, pinLookupTable)
    end
  end
end
registerEventHandler("mcManifestChange", onManifestChanged)

------------------------------------------------------------------------------------------------------------------------
-- OperatorVector3OneInputArithmetic custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorVector3OneInputArithmetic",
    {
      {
        title = "Properties",
        usedAttributes = { "Operation", "ConstantValueX", "ConstantValueY", "ConstantValueZ" },
        displayFunc = function(...) safefunc(attributeEditor.operatorVector3OneInputArithmeticDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorVector3OneInputArithmetic node definition.
------------------------------------------------------------------------------------------------------------------------
