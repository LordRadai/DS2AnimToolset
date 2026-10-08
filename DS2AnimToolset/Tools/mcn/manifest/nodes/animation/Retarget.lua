------------------------------------------------------------------------------------------------------------------------
-- Retarget node definition.

require "ui/AttributeEditor/RetargetDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
registerNode("Retarget",
  {
    helptext = "Retargets to required animation set from given input set",
    group = "Retargeting",
    image = "Retarget.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 161),
    version = 4,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        passThrough = true,
        interfaces =
        {
          required = { "Transforms", },
          optional = { "Time", "Events" },
        },
      },
      ["Result"] = {
        input = false,
        passThrough = true,
        interfaces =
        {
          required = { "Transforms", },
          optional = { "Time", "Events" },
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
      {
        name = "InputAnimSet",
        type = "ref", kind = "AnimationSet",
        perAnimSet = true,
        helptext = "Specifies the anim set of the input sub network.",
        set = function(node, value, animSet)
          if value ~= "" then
            local _, newSet = splitStringAtLastOccurence(value, "|")
            if animSet == newSet then
              local attrPath = string.format("%s.InputAnimSet", node)
              setAttribute(attrPath, "", animSet)
            end
          end
        end,
      },
      {
        name = "exportAssetManagerMessageId",
        type = "bool",
        value = false,
        helptext = "Special value for use in the retargeting preview mode in the asset manager",
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local InPin = string.format("%s.Source", node)
      if isConnected{ SourcePin = InPin, ResolveReferences = true } then
        local connections = listConnections{ Object = InPin, ResolveReferences = true }
        local InNode = connections[1]
        if isValid(InNode) ~= true then
          return false, string.format("Retarget node %s has no valid input to In, node %s is not valid", node, InNode)
        end
      else
        return false, string.format("Retarget node %s is missing a required connection to In", node)
      end     

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local InPin = string.format("%s.Source", node)
      local InNodeID = getConnectedNodeID(InPin)	  
      stream:writeNetworkNodeId(InNodeID, "SourceNodeID")
      
      -- build a lookup table from animation set to index
      local animSets = listAnimSets();
      local animationSetLookup = { }
      for i, set in ipairs(animSets) do
        animationSetLookup[set] = i
      end
      
      for asIdx, asVal in ipairs(animSets) do
        local animSetIndex = asIdx
        local animSetValue = getAttribute(node, "InputAnimSet", asVal)
        if animSetValue ~= "" then
          local _
          _, animSetValue = splitStringAtLastOccurence(animSetValue, "|")
          animSetIndex = animationSetLookup[animSetValue];
        end
        
        animSetIndex = animSetIndex - 1
        stream:writeUInt(animSetIndex, string.format("InputAnimSetIndex_%d", asIdx))
      end
      
      -- This code is used by the special asset manager network and is required to ensure that offset editing in 
      -- connect will work correctly.  
      local exportSpecialMessage = getAttribute(node, "exportAssetManagerMessageId")
      stream:writeBool(exportSpecialMessage, "AssetManagerExport")
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local InChannels = { }
      local InPin = string.format("%s.Source", node)
      if isConnected{ SourcePin  = InPin, ResolveReferences = true } then
        local connections = listConnections{ Object = InPin , ResolveReferences = true }
        local InNode = connections[1]
        InChannels = anim.getTransformChannels(InNode, set)
      end

      return InChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        local animationSets = listAnimSets();
        local index = getAttribute(node, "deprecated_InputAnimSetIndex")
        local animationSet = animationSets[index]
        if animationSet then
          setAttribute(string.format("%s.InputAnimSet", node), 
                       string.format("AnimationSets|%s", animationSet))
        end

        --remove deprecated value
        removeAttribute(node, "deprecated_InputAnimSetIndex")
      end
      
      -- Make sure that all animation sets were fully populated and that no animationset
      -- attempts to retarget to its self
      if version < 4 then
        local animationSets = listAnimSets();
        local attrPath = string.format("%s.InputAnimSet", node)
        for _, set in animationSets do
          if hasAnimSetData(attrPath, set) then
            local value = getAttribute(node, "InputAnimSet", set)
            local _, newSet = splitStringAtLastOccurence(value, "|")
            if newSet == set then
              setAttribute(attrPath, "", set)
            end
          else
            setAttribute(attrPath, "", set)
          end
        end
      end
    end,
    
    --------------------------------------------------------------------------------------------------------------------
    onNodeCreated = function(node)
      local animationSets = listAnimSets();
      local attrPath = string.format("%s.InputAnimSet", node)
      for _, set in animationSets do
        setAttribute(attrPath, "", set)
      end
    end,
    
    --------------------------------------------------------------------------------------------------------------------
    onAnimationSetCreated = function(node, set)
      local attrPath = string.format("%s.InputAnimSet", node)
      setAttribute(attrPath, "", set)
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "Retarget",
    {
      {
        title = "Properties",
        usedAttributes = { "InputAnimSet", "exportAssetManagerMessageId" },
        displayFunc = function(...) safefunc(attributeEditor.retargetDisplayInfoSection, unpack(arg)) end
      },
    }
  )
end
