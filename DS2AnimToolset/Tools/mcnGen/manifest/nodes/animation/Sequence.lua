------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- Sequence node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("Sequence",
  {
    displayName = "Sequence",
    helptext = "Sequentially switches between n source nodes",
    group = "Utilities",
    image = "Sequence.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 133),
    version = 3,

    functionPins =
    {
      ["Source0"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source1"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source2"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source3"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source4"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source5"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source6"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source7"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source8"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source9"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Source10"] = {
        input = true,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
      ["Result"] = {
        input = false,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        },
      },
    },

    pinOrder =
    {
      "Source0",
      "Source1",
      "Source2",
      "Source3",
      "Source4",
      "Source5",
      "Source6",
      "Source7",
      "Source8",
      "Source9",
      "Source10",
      "Result",
    },

    --------------------------------------------------------------------------------------------------------------------
    attributes =
    {
      {
        name = "Loop", type = "bool", value = true,
        helptext = "Sets whether the result of this node loops when it reaches the end of it's sequence."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local connectedPinCount = 0
      local previousPinConnected = false
      local pinsAllowed = true
      local error = false
      local warn = false
      local errorMessage = ""
      local warnMessage = ""

      for currentPin = 0, 10 do
        local pinName = (node .. ".Source" .. currentPin)
        if isConnected{ SourcePin  = pinName, ResolveReferences = true } then
          if pinsAllowed then
            previousPinConnected = true
            connectedPinCount = connectedPinCount + 1
          else
            error = true
            if previousPinConnected == true then
              errorMessage = "The pins on sequence node " .. node .. " are not connected consecutively. Ensure that there is not a gap between connections."
            else
              errorMessage = "The pins before Source" .. currentPin .. " on sequence node " .. node .. " are not connected. Ensure that the connected pins start at Source0 and there are no gaps between connections."
            end
          end

          local nodesConnected = listConnections{ Object = pinName, ResolveReferences = true }
          if isValid(nodesConnected[1]) ~= true then
            error = true
            errorMessage = "The pins on sequence node " .. node .. " are not connected to valid nodes"
          end
        else
          pinsAllowed = false
        end
      end

      if not(error or warn) then
        if connectedPinCount < 2 then
          error = true
          errorMessage = "The sequence node " .. node .. " requires at least two connections"

        end
      end

      -- verify that the node is correctly setup
      if error then
        return nil, errorMessage
      end
      if warn then
        return true, warnMessage
      end
      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local connectedNodeCount = 0
      local connectedPinCount = 0
      local connectedIds = { }

      for currentPin = 0, 10 do
        local pinName = (node .. ".Source" .. currentPin)
        if isConnected{ SourcePin  =  pinName, ResolveReferences = true } then
          connectedPinCount = connectedPinCount + 1
          connectedIds[currentPin] = getConnectedNodeID(pinName)
        else
          break
        end
      end

      -- now write out the number of connected pins
      Stream:writeInt(connectedPinCount, "ConnectedPinCount")

    -- Get and write the attribute data
    local loop = getAttribute(node, "Loop")
      Stream:writeBool(loop, "Loop")

      -- finally write connectedPinCount of runtime id's
      for currentPin = 0, (connectedPinCount - 1) do
        Stream:writeNetworkNodeId(connectedIds[currentPin], "ConnectedNodeID_" .. currentPin)
      end

    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local unionOfResults = { }
      for currentPin = 0, 10 do
        local pinName = (node .. ".Source" .. currentPin)
        if isConnected{ SourcePin  = pinName, ResolveReferences = true } then
          local sourceTable = listConnections{ Object =  pinName, ResolveReferences = true }
          local sourceNode = sourceTable[1]
          local curChannels = anim.getTransformChannels(sourceNode, set)
          if firstPin then
            unionOfResults = curChannels
          else
            unionOfResults = setUnion(unionOfResults, curChannels)
          end
        else
          break
        end
      end
      return unionOfResults
    end,
  }
)

if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "sequence",
    {
      {
        title = "Playback Options",
        usedAttributes = { "Loop" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

