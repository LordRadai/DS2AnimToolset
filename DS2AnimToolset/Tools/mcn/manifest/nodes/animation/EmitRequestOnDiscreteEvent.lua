------------------------------------------------------------------------------------------------------------------------
-- EmitRequestFromUserDataTrack node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("EmitRequestOnDiscreteEvent",
  {
    displayName = "Emit Request",
    group = "Utilities",
    image = "EmitRequestOnDiscreteEvent.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 153),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        passThrough = true,
        interfaces =
        {
          required = { "Time", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        passThrough = true,
        interfaces =
        {
          required = { "Time", },
          optional = { },
        },
      },
    },

    pinOrder =
    {
      "Source",
      "Result",
    },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {      
      -- Request 0
      { name = "EventUserData0", type = "int" },
      { name = "EmittedRequest0", type = "request" },
      { name = "Action0", type = "string", value = "Set" },
      { name = "Target0", type = "ref", kind = "allStateMachines", weak = true },

      -- Request 1
      { name = "EventUserData1", type = "int" },
      { name = "EmittedRequest1", type = "request" },
      { name = "Action1", type = "string", value = "Set" },
      { name = "Target1", type = "ref", kind = "allStateMachines", weak = true },
      
      -- Request 2
      { name = "EventUserData2", type = "int" },
      { name = "EmittedRequest2", type = "request" },
      { name = "Action2", type = "string", value = "Set" },
      { name = "Target2", type = "ref", kind = "allStateMachines", weak = true },
      
      -- Request 3
      { name = "EventUserData3", type = "int" },
      { name = "EmittedRequest3", type = "request" },
      { name = "Action3", type = "string", value = "Set" },
      { name = "Target3", type = "ref", kind = "allStateMachines", weak = true },
      
      -- Request 4
      { name = "EventUserData4", type = "int" },
      { name = "EmittedRequest4", type = "request" },
      { name = "Action4", type = "string", value = "Set" },
      { name = "Target4", type = "ref", kind = "allStateMachines", weak = true },
      
      -- Request 5
      { name = "EventUserData5", type = "int" },
      { name = "EmittedRequest5", type = "request" },
      { name = "Action5", type = "string", value = "Set" },
      { name = "Target5", type = "ref", kind = "allStateMachines", weak = true },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local SourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = SourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = SourcePin, ResolveReferences = true }
        local SourceNode = connections[1]
        if isValid(SourceNode) ~= true then
          return false, string.format("EmitRequestOnDiscreteEvent node %s has no valid input to Source, node %s is not valid", node, SourceNode)
        end
      else
        return false, string.format("EmitRequestOnDiscreteEvent node %s is missing a required connection to Source", node)
      end
     
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
    
      local SourceNodeID = -1
      local TargetNodeID = -1

      local sourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = sourcePin, ResolveReferences = true } then 
        SourceNodeID = getConnectedNodeID(sourcePin)
      end

      stream:writeNetworkNodeId(SourceNodeID, "SourceNodeID")

      -- An array of emitted requests (maximum of 6).
      local requestIndex = 0
      local numRequests = 6
      local numOutputRequests = 0
      for i = 0, (numRequests-1) do        
        if (serializeRequest(node, stream, tostring(i), tostring(numOutputRequests), false)) then       
          local attrEventUserData = getAttribute(node, ("EventUserData"..i))
          stream:writeInt(attrEventUserData, ("EventUserData_"..numOutputRequests))
          numOutputRequests = numOutputRequests + 1
        end
      end
      stream:writeUInt(numOutputRequests, "NumMessageSlots")
                     
      stream:writeBool(true, "NodeEmitsMessages");

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
      -- for future use
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "EmitRequestOnDiscreteEvent",
    {
      {
        title = "Emitted Requests",
        usedAttributes = { "EventUserData0", "EmittedRequest0", "Action0", "Target0",
                           "EventUserData1", "EmittedRequest1", "Action1", "Target1",
                           "EventUserData2", "EmittedRequest2", "Action2", "Target2",
                           "EventUserData3", "EmittedRequest3", "Action3", "Target3",
                           "EventUserData4", "EmittedRequest4", "Action4", "Target4",
                           "EventUserData5", "EmittedRequest5", "Action5", "Target5" },
        displayFunc = 
          function(...) 
            local spec = {
              first = 0, last = 5,
              event   = { name = "EventUserData", text = "EventUserData" },
              request = { name = "EmittedRequest", text = "Request" },
              action  = { name = "Action", text = "Action" },
              target  = { name = "Target", text = "Target" },
            }
            safefunc(attributeEditor.emitRequestOnDiscreteEventSection, spec, unpack(arg)) 
          end
      },
    }
  )
end

