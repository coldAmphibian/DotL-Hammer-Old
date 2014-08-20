--[[
AD Example
deal 200 ( +0.8 AD ) damage

Replace 
"Damage"
{
	....	
}
with
"RunScript"
{
	"ScriptFile"		"scripts/vscripts/APnADnItemCore.lua"
	"Target"			.... or 
	{
		....
	}
	"Function"			"DealDamage"
	"DamageBase"		"200"
	"BonusMultiple"		"0.8"
	"Type"				"AD"
}

In AD Items +20 AD
"Modifiers"
{
	"my_modifier_name"
	{
		"Passive"		"1"
		"IsHidden"		"1"
		"OnCreated"
		{
			"RunScript"
			{
				"ScriptFile"		"scripts/vscripts/APnADnItemCore.lua"
				"Target"			"CASTER"
				"Function"			"ModifyAD"
				"Value"				"20"
			}
		}
		"OnDestroy"
		{
			"RunScript"
			{
				"ScriptFile"		"scripts/vscripts/APnADnItemCore.lua"
				"Target"			"CASTER"
				"Function"			"ModifyAD"
				"Value"				"-20"
			}
		}
	}
}



]]


APADDamage = {}
local APADDamage = APADDamage

setmetatable(APADDamage, APADDamage)


APADDamage.damage_meta = {
	__index = {
		attacker 			= nil,					-- damage source
		victim 				= nil,					-- damage target
		damage				= 0,					-- 
		damage_flags 		= 1, 					-- default
		DamageBase			= 0,					-- base ability damage
		damage_type			= nil,					-- AD =  DAMAGE_TYPE_PHYSICAL AP = DAMAGE_TYPE_MAGICAL
		BonusMultiple   	= 0.0,					-- 
		bonus_level      	= 0						-- get ap level from items
	},
}


function DealDamage(damage)
	
	-- tell out the number of enemies to deal ap damage
	print('deal damage called for '..#damage.target_entities)

	local targets	= damage.target_entities or {damage.victim}
	if not targets then
		print(debug.traceback 'deal damage called with no target')
	end

	setmetatable(damage, APADDamage.damage_meta)
		
	damage.attacker	= damage.attacker or damage.caster or EntIndexToHScript(damage.caster_entindex)
	damage.bonus_level	= GetADAPAttribute(damage.attacker,damage.Type)
	damage.damage = damage.DamageBase + damage.bonus_level * damage.BonusMultiple

	if damage.Type == "AD" then
		damage.damage_type = DAMAGE_TYPE_PHYSICAL
	else -- default is magical damage
		damage.damage_type = DAMAGE_TYPE_MAGICAL
	end

	for i, victim in ipairs(targets) do
	    damage.victim = victim
		ApplyDamage(damage)
	end

end
	
function GetADAPAttribute(hero,type)
	if type == "AD" then
		return tonumber(hero:GetContext("ability_damage") or "0")
	end
	if type == "AP" then
		return tonumber(hero:GetContext("ability_power") or "0")
	end
	return 0
end
	
function ModifyAP(keys)
	local owner = keys.caster
	local value_old = tonumber(owner:GetContext("ability_power") or "0")
	value_new = value_old + tonumber(keys.Value)
	owner:SetContext("ability_power",value_new,0)
end

function ModifyAD(keys)
	local owner = keys.caster
	local value_old = tonumber(owner:GetContext("ability_damage") or "0")
	value_new = value_old + tonumber(keys.Value)
	owner:SetContext("ability_damage",value_new,0)
end
