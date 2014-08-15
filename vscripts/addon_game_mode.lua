--[[
Dota PvP game mode
]]

require("GameData")
require("GameDataInfo")

print( "Dota PvP game mode loaded." )

if DotaPvP == nil then
	DotaPvP = class({})
end

--------------------------------------------------------------------------------
-- ACTIVATE
--------------------------------------------------------------------------------
function Activate()
    GameRules.DotaPvP = DotaPvP()
    GameRules.DotaPvP:InitGameMode()
end

--------------------------------------------------------------------------------
-- INIT
--------------------------------------------------------------------------------
function DotaPvP:InitGameMode()
	local GameMode = GameRules:GetGameModeEntity()

	-- Enable the standard Dota PvP game rules
	GameRules:GetGameModeEntity():SetTowerBackdoorProtectionEnabled( true )

	-- Register Think
	GameMode:SetContextThink( "DotaPvP:GameThink", function() return self:GameThink() end, 0.25 )

	-- Register Game Events
end

--------------------------------------------------------------------------------
function DotaPvP:GameThink()
	return 0.25
end
