-- mcnExport.lua - round-trip export of a rebuilt (and packed) project, run by mcnGen.bat after packing:
--   morphemeConnect.exe -nogui -script mcnExport.lua
-- with MCNGEN_ROOT (the project folder) and MCNGEN_NAME (the character) set in the environment.
-- Writes <root>\build\roundtrip\<name>.xml and logs to <root>\build\<name>_export.log.
-- Kept apart from the stage scripts so a failing export (missing animations, ...) cannot cost the packed project.
local ROOT = os.getenv("MCNGEN_ROOT")
local NAME = os.getenv("MCNGEN_NAME")
local BUILD = ROOT .. "\\build"
local LOGF = io.open(BUILD .. "\\" .. NAME .. "_export.log", "w")
local NFAIL = 0
local LOG = function(s) LOGF:write(s .. "\n"); LOGF:flush() end
local TRY = function(desc, f)
  local ok, r = pcall(f)
  if not ok or r == nil or r == false then NFAIL = NFAIL + 1; LOG("FAIL  " .. desc .. "  :: " .. tostring(r)); return nil end
  return r
end

TRY("project.open", function() return project.open(ROOT .. "\\" .. NAME .. ".mcp") end)
TRY("mcn.open", function() return mcn.open(ROOT .. "\\" .. NAME .. ".mcn") end)
pcall(function() app.createDirectory(BUILD .. "\\roundtrip") end)
local ok, res, ids, errors, warnings = pcall(mcn.export, BUILD .. "\\roundtrip\\" .. NAME .. ".xml")
LOG("export ok=" .. tostring(ok) .. " result=" .. tostring(res))
if type(errors) == "table" then
  for i, v in ipairs(errors) do LOG("EXPORT ERROR " .. tostring(v.name) .. " : " .. tostring(v.message)) end
end
LOG("DONE failures=" .. NFAIL)
LOGF:close()
