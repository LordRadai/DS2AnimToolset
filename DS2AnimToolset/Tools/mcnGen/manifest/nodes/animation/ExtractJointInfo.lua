------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- ExtractJointInfo node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("ExtractJointInfo",
  {
    displayName = "Extract Joint Info",
    helptext = "Outputs the current local space position vector and angles of a selected joint. Passes through all other Node communication.",
    group = "Utilities",
    image = "ExtractJointInfo.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 152),
    version = 3,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        passThrough = true,
        interfaces = {
          required = { "Transforms", },
          optional = { },
        },
      },
    },

    dataPins =
    {
      ["Position"] = {
        input = false,
        array = false,
        type = "vector3",
      },
      ["Angle"] = {
        input = false,
        array = false,
        type = "float",
      },
    },

    pinOrder = { "Source", "Result", "Position", "Angle" },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "JointName", type = "rigChannelName", value = "", perAnimSet = true,
        helptext = "Selects the joint that will output its position vector and angles."
      },
      { 
        name = "OutputSpace",
        type = "bool",
        value = "Object",
        helptext = "Change the space within which the outputs of this node are defined.",
      },
      { 
        name = "AngleType",
        type = "int",
        value = "Total",
        helptext = "Change the angle you want to output: total, eulerX, eulerY or eulerZ",
      },
      { 
        name = "MeasureUnit",
        type = "bool",
        value = "Radian",
        helptext = "Change the measure unit, radian or degree, of the ouput angle.",
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = sourcePin, ResolveReferences = true } then

        -- Source pin.
        local nodesConnectedTo = listConnections{ Object = sourcePin, ResolveReferences = true }
        local sourceNode = nodesConnectedTo[1]
        if (sourceNode == nil) then
          return nil, "Missing input connection to ExtractJointInfo"
        end
        if not isValid(sourceNode) then
          return nil, ("ExtractJointInfo requires a valid input node")
        end

      else
        return nil, ("ExtractJointInfo node " .. node .. " is missing a required connection to Source")
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local inputNodeChannels = { }

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then
        local SourceTable = listConnections{ Object = sourcePin, ResolveReferences = true }
        local NodeConnected = SourceTable[1]
        inputNodeChannels = anim.getTransformChannels(NodeConnected, set)
      end

      return inputNodeChannels
    end,
    
    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      --
      local sourceNodeID = -1
      local pin = string.format("%s.Source", node)
      if isConnected{ SourcePin = pin, ResolveReferences = true } then
        sourceNodeID = getConnectedNodeID(pin)
      end
      Stream:writeNetworkNodeId(sourceNodeID, "SourceNodeID")
      
      -- Num animation sets.
      local animSets = listAnimSets()
      local numAnimSets = table.getn(animSets)
      Stream:writeUInt(numAnimSets, "NumAnimSets")

      -- Indexes of joints to sample from.
      for asIdx, asVal in animSets do
        local jointName = getAttribute(node, "JointName", asVal)
        local jointIndex = anim.getRigChannelIndex(jointName, asVal)
        if jointIndex == nil then
          jointIndex = 0
        end
        Stream:writeUInt(jointIndex, "JointIndex_"..asIdx)
      end
      
      -- Output space { [true] = "object", [false] = "local" }
      local outputSpace = getAttribute(node, "OutputSpace")
      Stream:writeBool(outputSpace, "OutputSpace")
      
      -- AngleType { [1] = "Total", [2] = "EulerX", [3] = "EulerY", [4] = "EulerZ" }
      local angleType = getAttribute(node, "AngleType")
      Stream:writeInt(angleType, "AngleType")
      
      -- Measure unit { [true] = "radian", [false] = "degree" }
      local unit = getAttribute(node, "MeasureUnit")
      Stream:writeBool(unit, "MeasureUnit")
      
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- ExtractJointInfo custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "ExtractJointInfo",
    {
      {
        title = "Properties",
        usedAttributes = { "JointName", "OutputSpace", "AngleType", "MeasureUnit" },
        displayFunc = function(...) safefunc(attributeEditor.extractJointInfoDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end