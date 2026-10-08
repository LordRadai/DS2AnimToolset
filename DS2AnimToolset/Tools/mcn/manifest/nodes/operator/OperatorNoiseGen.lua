------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- OperatorNoiseGen node definition.
------------------------------------------------------------------------------------------------------------------------

registerNode("OperatorNoiseGen",
{
    displayName = "Noise Gen",
    helptext = "Signal generation. Noise, Saw Wave, Ease-in/Ease-out",
    group = "Float Operators",
    image = "OperatorNoiseGen.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 113),
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
      {
        name = "Noise", type = "bool", value = true,
        helptext = "Output a noise curve."
      },
      {
        name = "SawWave", type = "bool", value = false,
        helptext = "Output a saw tooth curve."
      },
      {
        name = "EaseInEaseOut", type = "bool", value = false,
        helptext = "Output a sin curve."
      },
      {
        name = "NoiseSawFrequency", type = "float", value = 1.0,
        helptext = "Sets the frequency of the noise and saw functions."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      local nodesConnected = listConnections{ Object = (node .. ".Input"), ResolveReferences = true }
      local inputNode = nodesConnected[1]

      if (inputNode == nil) then
        return nil, "Missing connection to Operator NoiseGen node"
      end

      if not isValid(inputNode) then
        return nil, "Operator NoiseGen not valid"
      end

      local noise              = getAttribute(node, "Noise")
      local sawWave            = getAttribute(node, "SawWave")
      local easeInEaseOut      = getAttribute(node, "EaseInEaseOut")
      local noiseSawFrequency  = getAttribute(node, "NoiseSawFrequency")

      if (noise ~= true) and (noise ~= false) then
         return nil, "Bad Noise value on Operator NoiseGen node."
      end

      if (sawWave ~= true) and (sawWave ~= false) then
         return nil, "Bad SawWave value on Operator NoiseGen node."
      end

      if (easeInEaseOut ~= true) and (easeInEaseOut ~= false) then
         return nil, "Bad EaseInEaseOut value on Operator NoiseGen node."
      end

      if (noiseSawFrequency == nil) then
         return nil, "Bad NoiseSawFrequency value on Operator NoiseGen node."
      end

      return true
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local inputInfo = getConnectedNodeInfo((node .. ".Input"))

      -- make local copies of the attribute values
      local noise              = getAttribute(node, "Noise")
      local sawWave            = getAttribute(node, "SawWave")
      local easeInEaseOut      = getAttribute(node, "EaseInEaseOut")
      local noiseSawFrequency  = getAttribute(node, "NoiseSawFrequency")

      -- and write them out
      Stream:writeNetworkNodeId(inputInfo.id, "Input", inputInfo.pinIndex)

      Stream:writeFloat(noiseSawFrequency, "NoiseSawFrequency")

      local flags = 0
      if noise then flags = flags + 1 end
      if sawWave then flags = flags + 2 end
      if easeInEaseOut then flags = flags + 8 end --rather than 4 so to be consistent with morpheme 1.3

      Stream:writeInt(flags, "NoiseFlags")
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
    end,
  }
)

------------------------------------------------------------------------------------------------------------------------
-- OperatorNoiseGen custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "OperatorNoiseGen",
    {
      {
        title = "Properties",
        usedAttributes = { "Noise", "SawWave", "EaseInEaseOut", "NoiseSawFrequency" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end

------------------------------------------------------------------------------------------------------------------------
-- End of OperatorNoiseGen node definition.
------------------------------------------------------------------------------------------------------------------------
