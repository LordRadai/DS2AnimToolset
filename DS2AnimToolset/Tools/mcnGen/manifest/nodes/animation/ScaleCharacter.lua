------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "ui/AttributeEditor/ScaleCharacterDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- ScaleCharacter node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("ScaleCharacter",
  {
    helptext = "Scales bone lengths to reshape character (usually for a different mesh)",
    group = "Utilities",
    image = "ScaleCharacter.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 162),
    version = 1,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Source"] = {
        input = true,
        passThrough = true,
        interfaces =
        {
          required = { "Transforms", },
          optional = { "Time", "Events", },
        },
      },
      ["Result"] = {
        input = false,
        passThrough = true,
        interfaces =
        {
          required = { "Transforms", },
          optional = { "Time", "Events", },
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
        name = "ScaleRigMessage",
        type = "request", value = "",
        supportedType = "ScaleRig",
        helptext = "The message that this node responds to for changing scale values."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      -- Check if the necessary pins are connected.
      local SourcePin = string.format("%s.Source", node)
      if isConnected{ SourcePin = SourcePin, ResolveReferences = true } then
        local connections = listConnections{ Object = SourcePin, ResolveReferences = true }
        local SourceNode = connections[1]
        if isValid(SourceNode) ~= true then
          return false, string.format("ScaleCharacter node %s has no valid input to Source, node %s is not valid.", node, SourceNode)
        end
      else
        return false, string.format("ScaleCharacter node %s is missing a required connection to Source.", node)
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      -- serialize the connected pins
      local SourcePin = string.format("%s.Source", node)
      local SourceNodeID = getConnectedNodeID(SourcePin)
      stream:writeNetworkNodeId(SourceNodeID, "SourceNodeID")

      local scaleMessageID = -1
      local scaleMessagePath = getAttribute(node, "ScaleRigMessage")
      if scaleMessagePath ~= "" then
        scaleMessageID = target.getRequestID(scaleMessagePath)
      end

      stream:writeUInt(scaleMessageID, "ScaleMessageID")
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
    "ScaleCharacter",
    {
      -- No custom UI as yet
    }
  )
end
