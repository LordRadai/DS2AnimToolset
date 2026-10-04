------------------------------------------------------------------------------------------------------------------------
-- HipsIK node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("HipsIK",
  {
    helptext = "Move the hips procedurally while keeping the same foot placement",
    group = "IK",
    image = "HipsIK.png",
    displayName = "Hips IK",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 129),
    version = 2,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        passThrough = false,
        interfaces =
        {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        passThrough = false,
        interfaces =
        {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    updatePins = function(node)
      local dataType = getAttribute(node, "InputRotationType")
      local rotationDelta = string.format("%s.RotationDelta", node)

      if dataType == 1 then
        setDataPinType(rotationDelta, "vector3")
      elseif dataType == 2 then
        setDataPinType(rotationDelta, "vector4")
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    dataPins = 
    {
      ["PositionDelta"] = {
        input = true, 
        type = "vector3", 
      },
      ["RotationDelta"] = {
        input = true,
        array = false,
        type = {
          default = "vector3",
          allowed = { "vector3", "vector4" },
        },
      },
      ["FootTurnWeight"] = {
        input = true, 
        type = "float", 
      },
      ["Weight"] = {
        input = true, 
        type = "float", 
      },
    },

    pinOrder =
    {
      "Source",
      "Result",
      "PositionDelta",
      "RotationDelta",
      "FootTurnWeight",
      "Weight",
    },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "InputRotationType",
        type = "int",
        value = 1, -- [1] = "Euler Angle", [2] = "Quaternion"
        affectsPins = true,
        helptext = "Define the input Rotation delta as either a Euler angle or as a Quaternion."    
      },    
      {
        name = "HipsName",
        displayName = "Hips",
        type = "rigChannelName",
        value = "",
        perAnimSet = true,
        helptext = "The joint controlled by this node. Will default to the hips joint marked up in your rig.",
      },
      {
        name = "LeftAnkleName",
        displayName = "LeftAnkle",
        type = "rigChannelName",
        value = "",
        perAnimSet = true,
        helptext = "The name of the left ankle joint.",
      },
      {
        name = "LeftBallName",
        displayName = "LeftBall",
        type = "rigChannelName",
        value = "",
        perAnimSet = true,
        helptext = "The name of the left ball joint.",
      },
      {
        name = "LeftKneeRotationAxisX",
        type = "float",
        perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent.",
      },
      {
        name = "LeftKneeRotationAxisY",
        type = "float",
        perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent.",
      },
      {
        name = "LeftKneeRotationAxisZ",
        type = "float",
        perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent.",
      },
      {
        name = "RightAnkleName",
        displayName = "RightAnkle",
        type = "rigChannelName",
        value = "",
        perAnimSet = true,
        helptext = "The name of the right ankle joint.",
      },
      {
        name = "RightBallName",
        displayName = "RightBall",
        type = "rigChannelName",
        value = "",
        perAnimSet = true,
        helptext = "The name of the right ball joint.",
      },
      {
        name = "RightKneeRotationAxisX",
        type = "float",
        perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent.",
      },
      {
        name = "RightKneeRotationAxisY",
        type = "float",
        perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent.",
      },
      {
        name = "RightKneeRotationAxisZ",
        type = "float",
        perAnimSet = true,
        helptext = "The axis to rotate the knee joint around, in the coordinate frame of its parent.",
      },
      {
        name = "UseBallJoints",
        type = "bool",
        value = true,
        perAnimSet = true,
        helptext = "If your character has an articulated toe it is usually the ball-of-foot joints that you will want to remain fixed as the hips move, rather than the ankle joints.  If this is the case set this to true and identify the ball joint for each foot, which must be a child of the ankle.",
      },
      {
        name = "FootTurnWeight",
        displayName = "Default Foot Turn Weight",
        type = "float",
        value = 1.0,
        min = 0,
        max = 1.0,
        perAnimSet = false,
        helptext = "The amount the feet are allowed to turn to the left or right with the hips, as a weight between 0 and 1.  This is overridden by the value at the Foot turn weight control parameter input pin, if connected.",
      },
      {
        name = "KeepWorldFootOrientation",
        type = "bool",
        value = true,
        perAnimSet = false,
        helptext = "If false the local orientation of the feet will not be modified, which means they will turn with the hips. If true, the feet will keep their original orientation in character space, except they will be allowed to turn around the vertical by an amount controlled by the FootTurnWeight.",
      },
      {
        name = "KneeSwivelWeight",
        type = "float",
        value = 0.5,
        min = 0,
        max = 1.0,
        perAnimSet = false,
        helptext = "The amount the legs can swivel (knees swung in vs knees swung out) to achieve the desired orientation of the foot.",
      },
      {
        name = "LocalReferenceFrame",
        type = "bool",
        value = false,
        perAnimSet = false,
        helptext = "Specifies whether the input transform is an offset from the current Hips transform in a local frame, or an offset to it in the Character frame.",
      }
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local SourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = SourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = SourcePin, ResolveReferences = true }
        local SourceNode = connections[1]
        if isValid(SourceNode) ~= true then
          return false, string.format("HipsIK node %s has no valid input to Source, node %s is not valid", node, SourceNode)
        end
      else
        return false, string.format("HipsIK node %s is missing a required connection to Source", node)
      end

      local PositionDeltaPin = string.format("%s.PositionDelta", node)
      local positionDeltaConnected = false
      if isConnected{ SourcePin = PositionDeltaPin, ResolveReferences = true } then
        positionDeltaConnected = true
        local connections = listConnections{ Object = PositionDeltaPin, ResolveReferences = true }
        local PositionDeltaNode = connections[1]
        if isValid(PositionDeltaNode) ~= true then
          return false, string.format("HipsIK node %s has no valid input to PositionDelta, node %s is not valid", node, PositionDeltaNode)
        end
      end

      local RotationDeltaQuatPin = string.format("%s.RotationDelta", node)
      local rotationDeltaConnected = false
      if isConnected{ SourcePin = RotationDeltaQuatPin, ResolveReferences = true } then
        rotationDeltaConnected = true
        local connections = listConnections{ Object = RotationDeltaQuatPin, ResolveReferences = true }
        local rotationDeltaNode = connections[1]
        if isValid(rotationDeltaNode) ~= true then
          return false, string.format("HipsIK node %s has no valid input to the RotationDelta", node)
        end
      end

      local FootTurnWeightPin = string.format("%s.FootTurnWeight", node)
      if isConnected{ SourcePin = FootTurnWeightPin, ResolveReferences = true } then
        local connections = listConnections{ Object = FootTurnWeightPin, ResolveReferences = true }
        local FootTurnWeightNode = connections[1]
        if isValid(FootTurnWeightNode) ~= true then
          return false, string.format("HipsIK node %s has no valid input to FootTurnWeight, node %s is not valid", node, FootTurnWeightNode)
        end
      end

      local WeightPin = string.format("%s.Weight", node)
      if isConnected{ SourcePin = WeightPin, ResolveReferences = true } then
        local connections = listConnections{ Object = WeightPin, ResolveReferences = true }
        local WeightNode = connections[1]
        if isValid(WeightNode) ~= true then
          return false, string.format("HipsIK node %s has no valid input to Weight, node %s is not valid", node, WeightNode)
        end
      end
      
      --------------------------------------------------------------------------------------
      -- Validate hierarchies
      
      local animSets = listAnimSets()
      local validateIKChain = function(leftOrRight)
      
        for asIdx, asVal in animSets do
          local useBallJoints = getAttribute(node, "UseBallJoints", asVal)
      
          local ankleName = getAttribute(node, leftOrRight.."AnkleName", asVal)
          local ankleIndex = nil
          if ankleName ~= nil then
            ankleIndex = anim.getRigChannelIndex(ankleName, asVal)
          end

          local rigSize = anim.getRigSize(asVal)
          
          local hipsName = getAttribute(node, "HipsName", asVal)
          local hipsIndex = nil
          if hipsName ~= nil then
            hipsIndex = anim.getRigChannelIndex(hipsName, asVal)
          end
          if hipsIndex == nil or hipsIndex <= 0 or hipsIndex >= rigSize then
            return false, ("HipsIK node "..node.." (animset "..asVal..") requires a valid Hips joint")
          end

          if not useBallJoints then

            if ankleIndex == nil or ankleIndex <= 0 or ankleIndex >= rigSize then
              return false, ("HipsIK node " .. node .. " (animset " .. asVal .. ") requires a valid "..leftOrRight.."AnkleName")
            end

          else

            local ballName = getAttribute(node, leftOrRight.."BallName", asVal)
            local ballIndex = nil
            if ballName ~= nil then
              ballIndex = anim.getRigChannelIndex(ballName, asVal)
            end

            if ballIndex == nil or ballIndex <= 0 or ballIndex >= rigSize then
              return false, ("HipsIK node " .. node .. " (animset " .. asVal .. ") requires a valid "..leftOrRight.."BallName")
            end

            local ankleIndexFromHierarchy = anim.getParentBoneIndex(ballIndex, asVal)
            if ankleIndexFromHierarchy ~= ankleIndex then
              return false, ("HipsIK node " .. node .. " (animset " .. asVal .. ") requires that the "..leftOrRight.." ankle joint is the parent of the "..leftOrRight.." ball joint")
            end
            ankleIndex = ankleIndexFromHierarchy
          end
          local kneeIndex = anim.getParentBoneIndex(ankleIndex, asVal)
          local hipIndex = anim.getParentBoneIndex(kneeIndex, asVal)
          if kneeIndex == nil or kneeIndex <= 0 or kneeIndex >= rigSize or
             hipIndex == nil or hipIndex <= 0 or hipIndex >= rigSize then
            return false, ("HipsIK node " .. node .. " (animset " .. asVal .. "): no valid trace through the hierarchy to the "..leftOrRight.." hip joint. Check your joint names.")
          end
          
          -- Check the hip is parented to the Hips
          local j = anim.getParentBoneIndex(hipIndex, asVal)
          while j > 0 do
            if j == hipsIndex then
              break
            end
            j = anim.getParentBoneIndex(j, asVal)
          end
          if j ~= hipsIndex then
            return false, ("HipsIK node " .. node .. " (animset " .. asVal .. "): the " .. leftOrRight .. " hip (determined by traversing the hierarchy from the ankle joint) is not parented to the Hips joint.")
          end
        end
        
        return true
      end
      
      local result, message = validateIKChain("Left")
      if not result then
        return result, message
      end
      local result, message = validateIKChain("Right")
      if not result then
        return result, message
      end

      --------------------------------------------------------------------------------------
      -- Validate the knee axes
      for asIdx, asVal in animSets do
        -- Left knee
        local x = getAttribute(node, "LeftKneeRotationAxisX", asVal)
        local y = getAttribute(node, "LeftKneeRotationAxisY", asVal)
        local z = getAttribute(node, "LeftKneeRotationAxisZ", asVal)
        if x == 0 and y == 0 and z == 0 then
          return false, ("HipsIK node " .. node .. " (animset " .. asVal .. ") must have a non-zero Left Knee axis")
        end

        -- Right knee
        local x = getAttribute(node, "RightKneeRotationAxisX", asVal)
        local y = getAttribute(node, "RightKneeRotationAxisY", asVal)
        local z = getAttribute(node, "RightKneeRotationAxisZ", asVal)
        if x == 0 and y == 0 and z == 0 then
          return false, ("HipsIK node " .. node .. " (animset " .. asVal .. ") must have a non-zero Right Knee axis")
        end
      end
      
      --------------------------------------------------------------------------------------
      -- Double-check the world up preference since this node relies on its behaviour
      local worldUpAxis = preferences.get("WorldUpAxis")
      if not (worldUpAxis == "X Axis" or worldUpAxis == "Y Axis" or worldUpAxis == "Z Axis") then
        return false, ("GunAimIK node " .. node  .. " failed because the world up axis preference returned an unexpected value")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local SourcePin = string.format("%s.Source", node)
      local SourceNodeID = getConnectedNodeID(SourcePin)
      stream:writeNetworkNodeId(SourceNodeID, "SourceNodeID")

      local PositionDeltaPin = string.format("%s.PositionDelta", node)
      if isConnected{ SourcePin = PositionDeltaPin, ResolveReferences = true } then
        local nodeInfo = getConnectedNodeInfo(PositionDeltaPin)
        stream:writeNetworkNodeId(nodeInfo.id, "PositionDelta", nodeInfo.pinIndex)
      end

      local rotationDeltaPin = string.format("%s.RotationDelta", node)
      if isConnected{ SourcePin = rotationDeltaPin, ResolveReferences = true } then
        local nodeInfo = getConnectedNodeInfo(rotationDeltaPin)
        local rotationType = getAttribute(node, "InputRotationType")
        if rotationType == 1 then
          stream:writeNetworkNodeId(nodeInfo.id, "RotationDeltaEulerNodeID", nodeInfo.pinIndex)
        else
          stream:writeNetworkNodeId(nodeInfo.id, "RotationDeltaQuatNodeID", nodeInfo.pinIndex)
        end
      end

      local FootTurnWeightPin = string.format("%s.FootTurnWeight", node)
      if isConnected{ SourcePin = FootTurnWeightPin, ResolveReferences = true } then
        local nodeInfo = getConnectedNodeInfo(FootTurnWeightPin)
        stream:writeNetworkNodeId(nodeInfo.id, "FootTurnWeight", nodeInfo.pinIndex)
      end

      local WeightPin = string.format("%s.Weight", node)
      if isConnected{ SourcePin = WeightPin, ResolveReferences = true } then
        local nodeInfo = getConnectedNodeInfo(WeightPin)
        stream:writeNetworkNodeId(nodeInfo.id, "Weight", nodeInfo.pinIndex)
      end

      local footTurnWeight = getAttribute(node, "FootTurnWeight")
      stream:writeFloat(footTurnWeight, "FootTurnWeight")

      local keepWorldFootOrientation = getAttribute(node, "KeepWorldFootOrientation")
      stream:writeBool(keepWorldFootOrientation, "KeepWorldFootOrientation")

      local kneeSwivelWeight = getAttribute(node, "KneeSwivelWeight")
      stream:writeFloat(kneeSwivelWeight, "KneeSwivelWeight")
      
      local localReferenceFrame = getAttribute(node, "LocalReferenceFrame")
      stream:writeBool(localReferenceFrame, "LocalReferenceFrame")

      -- Serialise world up axis as an index into a Cartesian 3-vector
      local worldUpAxis = preferences.get("WorldUpAxis")
      local upAxisIndex = 0
      if worldUpAxis == "Y Axis" then
        upAxisIndex = 1
      elseif worldUpAxis == "Z Axis" then
        upAxisIndex = 2
      end
      stream:writeUInt(upAxisIndex, "UpAxisIndex")

      local animSets = listAnimSets()
      for asIdx, asVal in animSets do

        local attrHipsName = getAttribute(node, "HipsName", asVal)
        local hipsIndex = anim.getRigChannelIndex(attrHipsName, asVal)
        if hipsIndex ~= nil then
          stream:writeInt(hipsIndex, "HipsIndex_" .. asIdx)
        end

        local attrLeftAnkleName = getAttribute(node, "LeftAnkleName", asVal)
        local leftAnkleIndex = anim.getRigChannelIndex(attrLeftAnkleName, asVal)
        if leftAnkleIndex ~= nil then
          stream:writeInt(leftAnkleIndex, "LeftAnkleIndex_" .. asIdx)
        end

        local attrLeftBallName = getAttribute(node, "LeftBallName", asVal)
        local leftBallIndex = anim.getRigChannelIndex(attrLeftBallName, asVal)
        if leftBallIndex ~= nil then
          stream:writeInt(leftBallIndex, "LeftBallIndex_" .. asIdx)
        end

        local attrLeftKneeRotationAxisX = getAttribute(node, "LeftKneeRotationAxisX", asVal)
        stream:writeFloat(attrLeftKneeRotationAxisX, "LeftKneeRotationAxisX_" .. asIdx)

        local attrLeftKneeRotationAxisY = getAttribute(node, "LeftKneeRotationAxisY", asVal)
        stream:writeFloat(attrLeftKneeRotationAxisY, "LeftKneeRotationAxisY_" .. asIdx)

        local attrLeftKneeRotationAxisZ = getAttribute(node, "LeftKneeRotationAxisZ", asVal)
        stream:writeFloat(attrLeftKneeRotationAxisZ, "LeftKneeRotationAxisZ_" .. asIdx)

        local attrRightAnkleName = getAttribute(node, "RightAnkleName", asVal)
        local rightAnkleIndex = anim.getRigChannelIndex(attrRightAnkleName, asVal)
        if rightAnkleIndex ~= nil then
          stream:writeInt(rightAnkleIndex, "RightAnkleIndex_" .. asIdx)
        end

        local attrRightBallName = getAttribute(node, "RightBallName", asVal)
        local rightBallIndex = anim.getRigChannelIndex(attrRightBallName, asVal)
        if rightBallIndex ~= nil then
          stream:writeInt(rightBallIndex, "RightBallIndex_" .. asIdx)
        end

        local attrRightKneeRotationAxisX = getAttribute(node, "RightKneeRotationAxisX", asVal)
        stream:writeFloat(attrRightKneeRotationAxisX, "RightKneeRotationAxisX_" .. asIdx)

        local attrRightKneeRotationAxisY = getAttribute(node, "RightKneeRotationAxisY", asVal)
        stream:writeFloat(attrRightKneeRotationAxisY, "RightKneeRotationAxisY_" .. asIdx)

        local attrRightKneeRotationAxisZ = getAttribute(node, "RightKneeRotationAxisZ", asVal)
        stream:writeFloat(attrRightKneeRotationAxisZ, "RightKneeRotationAxisZ_" .. asIdx)

        local useBallJoints = getAttribute(node, "UseBallJoints", asVal)
        stream:writeBool(useBallJoints, "UseBallJoints_" .. asIdx)
      end

    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local SourceChannels = { }
      local SourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = SourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = SourcePin , ResolveReferences = true }
        local SourceNode = connections[1]
        SourceChannels = anim.getTransformChannels(SourceNode, set)
      end

      return SourceChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        local oldSourcePath = string.format("%s.RotationDeltaEuler", node)
        local newSourcePath = string.format("%s.RotationDelta", node)
        pinLookupTable[oldSourcePath] = newSourcePath
      end
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "HipsIK",
    {
      {
        title = "Joints",
        usedAttributes = { "HipsName", "LeftAnkleName", "LeftBallName", "RightAnkleName", "RightBallName", "UseBallJoints" },
        displayFunc = function(...) safefunc(attributeEditor.HipsIKJointsDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Knee Axes",
        usedAttributes = {
          "LeftKneeRotationAxisX",
          "LeftKneeRotationAxisY",
          "LeftKneeRotationAxisZ",
          "RightKneeRotationAxisX",
          "RightKneeRotationAxisY",
          "RightKneeRotationAxisZ"
        },
        displayFunc = function(...) safefunc(attributeEditor.HipsIKKneeAxisDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Foot Orientation",
        usedAttributes = {
          "KeepWorldFootOrientation",
          "FootTurnWeight",
          "KneeSwivelWeight",
        },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "General",
        usedAttributes = {
          "LocalReferenceFrame",
          "InputRotationType"
        },
        displayFunc = function(...) safefunc(attributeEditor.hipsIKGeneralDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end
