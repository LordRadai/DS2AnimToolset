------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- SetNonPhysicsTransforms node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("SetNonPhysicsTransforms",
  {
    displayName = "Set Non Physics Transforms",
    helptext = "Sets the non-physical transforms using the second animation source",
    group = "Physics",
    image = "SetNonPhysicsTransforms.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 127),
    version = 2,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["PhysicsSource"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
      ["NonPhysicsSource"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", },
          optional = { },
        },
      },
    },

    pinOrder = { "PhysicsSource", "NonPhysicsSource", "Result", },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      if isConnected{ SourcePin  = (node .. ".PhysicsSource"), ResolveReferences = true } and isConnected{ SourcePin  = (node .. ".NonPhysicsSource"), ResolveReferences = true } then
        local nodesConnectedTo0 = listConnections{ Object = (node .. ".PhysicsSource"), ResolveReferences = true }
        local nodesConnectedTo1 = listConnections{ Object = (node .. ".NonPhysicsSource"), ResolveReferences = true }
        local node0 = nodesConnectedTo0[1]
        local node1 = nodesConnectedTo1[1]
        if isValid(node0) ~= true or isValid(node1) ~= true then
          return nil, "Set Non Physics Transforms requires two valid input nodes"
        end

      else
        return nil, ("Set Non Physics Transforms node " .. node .. " is missing a required connection to PhysicsSource and/or NonPhysicsSource")
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, stream)
      local node0 = -1
      local node1 = -1

      if isConnected{ SourcePin  =(node .. ".PhysicsSource") , ResolveReferences = true } then
        node0 = getConnectedNodeID(node, "PhysicsSource")
      end
      if isConnected{ SourcePin  = (node .. ".NonPhysicsSource"), ResolveReferences = true } then
        node1 = getConnectedNodeID(node, "NonPhysicsSource")
      end

      stream:writeNetworkNodeId(node0, "PhysicsSourceConnectedNodeID")
      stream:writeNetworkNodeId(node1, "NonPhysicsSourceConnectedNodeID")
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local node0Channels = { }
      local node1Channels = { }
      if isConnected{ SourcePin  = (node .. ".PhysicsSource"), ResolveReferences = true } then
        local Source0Table = listConnections{ Object = (node .. ".PhysicsSource") , ResolveReferences = true }
        local NodeConnectedTo0 = Source0Table[1]
        node0Channels = anim.getTransformChannels(NodeConnectedTo0, set)
      end

      if isConnected{ SourcePin  = (node .. ".NonPhysicsSource"), ResolveReferences = true } then
        local Source1Table = listConnections{ Object = (node .. ".NonPhysicsSource") , ResolveReferences = true }
        local NodeConnectedTo1 = Source1Table[1]
        node1Channels = anim.getTransformChannels(NodeConnectedTo1, set)
      end

      local resultChannels = setUnion(node0Channels, node1Channels)
      return resultChannels
    end,
  }
)

