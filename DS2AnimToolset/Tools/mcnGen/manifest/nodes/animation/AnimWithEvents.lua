------------------------------------------------------------------------------------------------------------------------
-- Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
-- Not to be copied, adapted, modified, used, distributed, sold, licensed or commercially exploited in any manner
-- without the written consent of NaturalMotion.
--
-- All non public elements of this software are the confidential information of NaturalMotion and may not be disclosed
-- to any person nor used for any purpose not expressly approved by NaturalMotion in writing.
------------------------------------------------------------------------------------------------------------------------

require "luaAPI/CoreUtils.lua"
require "ui/AttributeEditor/AnimationTakeDisplayInfo.lua"
require "ui/AttributeEditor/DefaultDisplayInfo.lua"

------------------------------------------------------------------------------------------------------------------------
-- used to stop animationTake attributes set function be a recursive call.
------------------------------------------------------------------------------------------------------------------------
local isSettingAnimationTake = false

------------------------------------------------------------------------------------------------------------------------
-- AnimWithEvents node definition.
------------------------------------------------------------------------------------------------------------------------
registerNode("AnimWithEvents",
  {
    displayName = "Anim With Events",
    helptext = "Supplies clip of animation data from an entry in the animation set",
    group = "Utilities",
    image = "AnimWithEvents.png",
    id = generateNamespacedId(idNamespaces.NaturalMotion, 104),
    version = 9,

    --------------------------------------------------------------------------------------------------------------------
    functionPins =
    {
      ["Result"] = {
        input = false,
        array = false,
        interfaces = {
          required = { "Transforms", "Time", "Events", },
          optional = { },
        }
      },
    },

    attributes =
    {
      -- Attributes with the 'perAnimSet' flag set as true will have a widget per animation set,
      -- will generate data for each animation set.
      -- Note that the 'AnimationTake' type always has perAnimSet = true
      {
        name = "AnimationTake",
        type = "animationTake",
        value = { filename = "", takename = "" },
        set = function(node, value, set)
          -- this guard stops this function recursing indefinitely
          --
          if isSettingAnimationTake then
            return
          end

          isSettingAnimationTake = true

          local options = animfmt.parseOptions(value.options)
          options = animfmt.removeInvalidOptions(value.format, options)
          value.options = animfmt.compileOptions(options)

          local attributePath = string.format("%s.AnimationTake", node)
          setAttribute(attributePath, value, set)

          isSettingAnimationTake = false
        end,
      },
      {
        name = "Loop", type = "bool", value = true,
        helptext = "Sets whether the result of this node loops when it reaches the end of it's sequence."
      },
      {
        name = "PlayBackwards", type = "bool", value = false,
        helptext = "If set then the animation will play backwards."
      },
      {
        name = "GenerateAnimationDeltas", type = "bool", value = false, displayName = "Additive",
        helptext = "Make this animation suitable for additive blending. Subtracts the first frame from each subsequent frame."
      },
      {
        name = "DefaultClip", type = "bool", value = true, perAnimSet = true,
        helptext = "Specify the start and end positions of the clip are different from those that are defined for this animation."
      },
      {
        name = "ClipStartFraction", type = "float", units = "fraction", min = 0.0, max = 1.0, value = 0.0, perAnimSet = true, displayName = "Start",
        helptext = "Specify the start position to begin playback sometime after the authored start of the clip."
      },
      {
        name = "ClipEndFraction", type = "float", units = "fraction", min = 0.0, max = 1.0, value = 1.0, perAnimSet = true, displayName = "End",
        helptext = "Specify the end position to finish playback sometime before the authored end of the clip."
      },
      {
        name = "StartEventIndex", type = "int", value = 0, min = 0, perAnimSet = true,
        helptext = "The index of the event on the synctrack from which to start playback. If the majority of your animations start on the right foot, then the first event (index 0) will be when the left foot is planted. If you have an animation that starts on the left foot then the first event will be the right foot being planted, and this should have an index of 1 to match the events in the other animations."
      },
      {
        name = "PreComputeSyncEventTracks", type = "bool", value = false,
        helptext = "If true sync event tracks are generated in the asset compiler - This is fast at runtime but takes more memory. If false sync event tracks are generated at runtime - This is slower but takes less memory."
      },
    },

    --------------------------------------------------------------------------------------------------------------------
    validate = function(node)
      -- for now just a simple check to ensure that the file exists
      -- In the future validate against file types the runtime will support
      local valid = true
      local missingTakesInfo = { }
      local errorMessage = ""

      local animSets = listAnimSets()
      local setsWithData = 0

      local attrPath = string.format("%s.AnimationTake", node)
      for _, set in animSets do
        local shouldCheckForData = hasAnimSetData(attrPath, set)

        if shouldCheckForData then
          local animationTake = getAttribute(node, "AnimationTake", set)
          --  anim.takeExists will use the current best practice to determine if the take is present on disk.
          --  The implementation might change in future version of connect to improve performance.
          local validTake, reason = anim.takeExists(animationTake)

          setsWithData = setsWithData + 1

          if not validTake then
            valid = false

            local missingInfo = {
              filename = animationTake and animationTake.filename or "",
              takename = animationTake and animationTake.takename or "",
              set = set,
              reason = reason or "",
            }

            table.insert(missingTakesInfo, missingInfo)
          else
            local syncTrackName = animationTake.synctrack
            if syncTrackName == nil or syncTrackName == "" then
              errorMessage = string.format("The sync track for node %s is not set", node)
            else
              local eventData = anim.getTakeMarkupData(animationTake)
              local syncTrack = eventData[syncTrackName]
              if syncTrack == nil then
                local missingInfo = {
                  filename = animationTake and animationTake.filename or "",
                  syncTrackName = syncTrackName,
                  set = set,
                  reason = "MissingSyncTrack",
                }
--                errorMessage = string.format("The sync track %s for node %s is missing", syncTrackName, node)
                table.insert(missingTakesInfo, missingInfo)
                valid = false
              else
                local looped = getAttribute(node, "Loop")
                if not looped then
                  -- check there are events in the syncevent track
                  if table.getn(syncTrack) > 0 then
                    local foundEventAt0 = false
                    for k, v in syncTrack do
                      if v['position'] == 0 then
                        foundEventAt0 = true
                      end
                    end
                    if not foundEventAt0 then
                      errorMessage = string.format("Non-looped animation (%s) without an event at time 0.0", node)
                    end
                  end
                end
              end
            end
          end
        end
      end

      if valid then
        if string.len(errorMessage) == 0 then
          return true
        else
          return true, errorMessage
        end
      else
        -- the formatting of the error message depends if multiple sets are used.  If data is only in one set
        -- do not mention sets in the error message.
        if setsWithData == 1 then
          if missingTakesInfo[1].reason == "TakeNotFound" then
            errorMessage = string.format(
              "The animation take referenced by %s [\"%s|%s\"] does not exist",
              node,
              missingTakesInfo[1].filename,
              missingTakesInfo[1].takename)
          elseif missingTakesInfo[1].reason == "MissingSyncTrack" then
            errorMessage = string.format("The sync track %s does not exist", missingTakesInfo[1].syncTrackName)
          elseif missingTakesInfo[1].reason == "NotInCache" then
            errorMessage = string.format("Details of the file %q was not yet cached", missingTakesInfo[1].filename)
          elseif missingTakesInfo[1].reason == "FileNotFound" then
            errorMessage = string.format("The animation file referenced by %s [%q] does not exist", node, missingTakesInfo[1].filename)
          elseif missingTakesInfo[1].reason == "LocationNotFound" then
            errorMessage = string.format("The animation location for file %q referenced by %s does not exist", missingTakesInfo[1].filename, node)
          else
            errorMessage = string.format("The animation file referenced by %s [%q] is invalid.", missingTakesInfo[1].filename, node)
          end
        else
          local missingTakesStr = ""
          local missingSyncTracksStr = ""
          local missingInfosStr = ""
          local missingFilesStr = ""
          local missingLocationsStr = ""
          local unknownErrorsStr = ""

          for _, info in ipairs(missingTakesInfo) do
            if info.reason == "TakeNotFound" then
              missingTakesStr = string.format("%s%s - \"%s|%s\"; ", missingTakesStr, info.set, info.filename, info.takename)
            elseif info.reason == "MissingSyncTrack" then
              missingSyncTracksStr = string.format("%s%s - \"%s|%s\"; ", missingSyncTracksStr, info.set, info.filename, info.syncTrackName)
            elseif info.reason == "NotInCache" then
              missingInfosStr = string.format("%s%s - %q; ", missingInfosStr, info.set, info.filename)
            elseif info.reason == "FileNotFound" then
              missingFilesStr = string.format("%s%s - %q; ", missingFilesStr, info.set, info.filename)
            elseif info.reason == "LocationNotFound" then
              missingLocationsStr = string.format("%s%s - %q; ", missingLocationsStr, info.set, info.filename)
            else
              unknownErrorsStr = string.format("%s%s - %q; ", unknownErrorsStr, info.set, info.filename)
            end
          end

          if string.len(missingTakesStr) > 0 then
            errorMessage = string.format("The animation takes referenced by %s [%s] do not exist. ", node, missingTakesStr)
          end

          if string.len(missingSyncTracksStr) > 0 then
            errorMessage = string.format("The sync tracks referenced by %s [%s] do not exist. ", node, missingSyncTracksStr)
          end

          if string.len(missingInfosStr) > 0 then
            errorMessage = string.format("%sFile details %s [%s] were not yet cached. ", errorMessage, node, missingInfosStr)
          end

          if string.len(missingFilesStr) > 0 then
            errorMessage = string.format("%sThe animation files referenced by %s [%s] do not exist. ", errorMessage, node, missingFilesStr)
          end

          if string.len(missingLocationsStr) > 0 then
            errorMessage = string.format("%sThe animation locations for files [%s] referenced by %s do not exist. ", errorMessage, node, missingLocationsStr)
          end

          if string.len(unknownErrorsStr) > 0 then
            errorMessage = string.format("%sThe animation files referenced by %s [%s] are invalid.", errorMessage, node, unknownErrorsStr)
          end
        end
        return nil, errorMessage
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    serialize = function(node, Stream)
      local animationTake = getAttribute(node, "AnimationTake")
      local animationIndex = animationTake.index
      local generateAnimationDeltas = getAttribute(node, "GenerateAnimationDeltas")

      local loop = getAttribute(node, "Loop")
      local playBackwards = getAttribute(node, "PlayBackwards")
      local preComputeSyncEventTracks = getAttribute(node, "PreComputeSyncEventTracks")

      Stream:writeAnimationId(animationIndex, "AnimIndex")
      Stream:writeBool(generateAnimationDeltas, "GenerateAnimationDeltas")
      Stream:writeBool(loop, "Loop")
      Stream:writeBool(playBackwards, "PlayBackwards")
      Stream:writeBool(preComputeSyncEventTracks, "PreComputeSyncEventTracks")

      local animSets = listAnimSets()
      local numAnimSets = table.getn(animSets)

      for asIdx, asVal in ipairs(animSets) do
        local defaultClip = getAttribute(node, "DefaultClip")
        local clipStartFraction = getAttribute(node, "ClipStartFraction", asVal)
        local clipEndFraction = getAttribute(node, "ClipEndFraction", asVal)

         -- Make sure the clip end time is greater or equal to the clip start time.
        if clipEndFraction < clipStartFraction then
          clipEndFraction = clipStartFraction
        end

        Stream:writeBool(defaultClip, string.format("DefaultClip_%d", asIdx))
        Stream:writeFloat(clipStartFraction, string.format("ClipStartFraction_%d", asIdx))
        Stream:writeFloat(clipEndFraction, string.format("ClipEndFraction_%d", asIdx))

        local startEventIndex = getAttribute(node, "StartEventIndex", asVal)
        Stream:writeInt(startEventIndex, string.format("StartEventIndex_%d", asIdx))
      end
    end,

    --------------------------------------------------------------------------------------------------------------------
    getTransformChannels = function(node, set)
      local animationTake = string.format("%s.AnimationTake", node)
      local transformChannels = anim.getTransformChannels(animationTake, set)
      return transformChannels
    end,

    --------------------------------------------------------------------------------------------------------------------
    getEvents = function(node)
      local animationTake = getAttribute(node .. ".AnimationTake")
      if animationTake then
        local tracks = anim.getTakeMarkupData(animationTake)
        if tracks then
          local _min = nil
          local _max = nil

          for i, track in pairs(tracks) do
            local numevents = table.getn(track)
            if _min == nil or numevents < _min then
              _min = numevents
            end
            if _max == nil or numevents > _max then
              _max = numevents
            end
          end

          return { min = _min or 0, max = _max or 0 }
        end
      end

      return { min = 0, max = 0 }
    end,

    --------------------------------------------------------------------------------------------------------------------
    upgrade = function(node, version, pinLookupTable)
      if version < 2 then
        local sets = listAnimSets()
        for _, set in ipairs(sets) do
          local attr = string.format("%s.AnimationTake", node)
          if hasAnimSetData(attr, set) then
            local value = getAttribute(node, "AnimationTake", set)
            if value.format == "jba" then
              value.format = "nsa"
              setAttribute(attr, value, set)
            end
          end
        end
      end

      -- For existing networks just turn off the default
      --
      -- there was a bug in the version 5 migrator, so here we either correct the bug
      -- or we upgrade normally if it hasn't already been upgraded.
      if version == 5 then
        -- the 3.0 version 5 upgrade added one false to the default animation set.
        -- detect if there is only one value...
        local dataCount = 0
        local attr = string.format("%s.DefaultClip", node)
        local sets = listAnimSets()
        for _, set in ipairs(sets) do
          if hasAnimSetData(attr, set) then
            dataCount = dataCount + 1
          end
        end
        
        -- if so, then re-upgrade the data
        if dataCount == 1 then
          local copyOrderFromAttr = string.format("%s.ClipStartFraction", node)
          for _, set in ipairs(sets) do
            -- remove any old data
            if hasAnimSetData(attr, set) then
              destroyAnimSetData(attr, set)
            end
            
            -- store new data
            if hasAnimSetData(copyOrderFromAttr, set) then
              setAttribute(attr, false, set)
            end
          end
          local srcOrder = listAnimSetOrder(copyOrderFromAttr)
          changeAnimSetOrder(attr, srcOrder)
        end
      elseif version < 6 then
        local sets = listAnimSets()
        local copyOrderFromAttr = string.format("%s.ClipStartFraction", node)
        local attr = string.format("%s.DefaultClip", node)
        for _, set in ipairs(sets) do
          if hasAnimSetData(copyOrderFromAttr, set) then
            setAttribute(attr, false, set)
          end
        end
        local srcOrder = listAnimSetOrder(copyOrderFromAttr)
        changeAnimSetOrder(attr, srcOrder)
      end
      
      -- Remove DWA animation format and replace with NSA
      if version < 7 then
        local sets = listAnimSets()
        for _, set in ipairs(sets) do
          local attr = string.format("%s.AnimationTake", node)
          if hasAnimSetData(attr, set) then
            local value = getAttribute(node, "AnimationTake", set)
            if value.format == "dwa" then
              value.format = "nsa"
              setAttribute(attr, value, set)
            end
          end
        end
      end
      
      -- Remove any mention to the option nsub in nsa formats
      if version < 9 then
        local sets = listAnimSets()
        for _, set in ipairs(sets) do
          local attr = string.format("%s.AnimationTake", node)
          if hasAnimSetData(attr, set) then
            local value = getAttribute(node, "AnimationTake", set)
            if value.format == "nsa" then
              local optionsTable = animfmt.parseOptions(value.options)
              animfmt.removeOption(optionsTable, "nsub")
              value.options = animfmt.compileOptions(optionsTable)
              local attributePath = string.format("%s.AnimationTake", node)
              setAttribute(attributePath, value, set)
            end
          end
        end
      end
    end
  }
)

------------------------------------------------------------------------------------------------------------------------
-- AnimWithEvents custom editor
------------------------------------------------------------------------------------------------------------------------
if not mcn.inCommandLineMode() then
  attributeEditor.registerDisplayInfo(
    "AnimWithEvents",
    {
      {
        title = "Animation Take",
        usedAttributes = { "AnimationTake" },
        displayFunc = function(...) safefunc(attributeEditor.animationTakeDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Clip Range",
        usedAttributes = { "ClipStartFraction", "ClipEndFraction", "DefaultClip" },
        displayFunc = function(...) safefunc(attributeEditor.animationClipRangeDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Start Event Index",
        usedAttributes = { "StartEventIndex" },
        displayFunc = function(...) safefunc(attributeEditor.animSetDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Playback Options",
        usedAttributes = { "Loop", "PlayBackwards", "GenerateAnimationDeltas" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      },
      {
        title = "Performance Options",
        usedAttributes = { "PreComputeSyncEventTracks" },
        displayFunc = function(...) safefunc(attributeEditor.standardDisplayInfoSection, unpack(arg)) end
      }
    }
  )
end