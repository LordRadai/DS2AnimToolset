------------------------------------------------------------------------------------------------------------------------
-- include some useful utilities
-- RayCastToolPreview.lua is required for the default ray cast tool functionality
------------------------------------------------------------------------------------------------------------------------
require "previewScripts/DebugDraw.lua"
require "previewScripts/RayCastToolPreview.lua"
require "previewScripts/VectorMath.lua"
------------------------------------------------------------------------------------------------------------------------

-- Place init logic here --


local updateFunction = function()
  -- Update any debug drawing that may be specified
  if (type(viewport.debugDraw.update) == "function") then
    --viewport.debugDraw.update()
  end

  -- Update any trigger volumes that may exist
  if (type(triggerVolumes.update) == "function") then
    triggerVolumes.update()
  end

  -- Place per-tick control script logic here
end

registerUpdateHandler(updateFunction)
