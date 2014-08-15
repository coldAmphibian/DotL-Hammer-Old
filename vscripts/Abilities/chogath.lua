function AddStack(keys)
	local caster=keys.caster
	local playerowner = caster.GetPlayerOwner(caster)
	local DataCounter = GameData:For("DataCounter",playerowner)

	if DataCounter.ChoGathStacks~=6 then
		DataCounter.ChoGathStacks=DataCounter.ChoGathStacks+1
	end
	
	local ability = keys.ability
	local level = ability:GetLevel()
	
	local dummyab = "greenterror_HP" .. tostring(level)
	
	if DataCounter.ChoGathStackAbility~=dummyab then
	caster:RemoveAbility(DataCounter.ChoGathStackAbility)
	caster:RemoveModifierByName("modifier_feast_bonus")
	caster:AddAbility(dummyab)	
	DataCounter.ChoGathStackAbility=dummyab	
	end
	
	local abilite=caster:FindAbilityByName(dummyab)
	abilite:SetLevel(DataCounter.ChoGathStacks)		
end

function RemoveStack(keys)
	local caster=keys.caster
	local playerowner = caster.GetPlayerOwner(caster)
	local DataCounter = GameData:For("DataCounter",playerowner)
	local ability = keys.ability
	
	if DataCounter.ChoGathStacks==2 then
		DataCounter.ChoGathStacks=1
	end
	if DataCounter.ChoGathStacks==3 then
		DataCounter.ChoGathStacks=2
	end
	if DataCounter.ChoGathStacks==4 then
		DataCounter.ChoGathStacks=2
	end
	if DataCounter.ChoGathStacks==5 then
		DataCounter.ChoGathStacks=3
	end
	if DataCounter.ChoGathStacks==6 then
		DataCounter.ChoGathStacks=3
	end
	
	local ability = keys.ability
	local level = ability:GetLevel()
	
	local dummyab = "greenterror_HP" .. tostring(level)
	
	if DataCounter.ChoGathStackAbility~=dummyab then
	caster:RemoveAbility(DataCounter.ChoGathStackAbility)
	caster:RemoveModifierByName("modifier_feast_bonus")
	caster:AddAbility(dummyab)	
	DataCounter.ChoGathStackAbility=dummyab	
	end
	
	local abilite=caster:FindAbilityByName(dummyab)
	abilite:SetLevel(DataCounter.ChoGathStacks)		
	
end