GameData = {}
GameData.Info = {}

function GameData:AddSchema(name, data)
	self.Info[name] = {
		Default = data,
		Entries = {}
	}
end


function GameData:For(name, ent)
	local ret = self.Info[name].Entries[ent] or self:Assign(name, ent)
	return ret
end

function GameData:Assign(name, entobj)
	local data = self.Info[name].Default
	local bind = TableCopy(data)
	bind.Ent = entobj
	self.Info[name].Entries[entobj] = bind
	return self.Info[name].Entries[entobj] --better way?
end

function TableCopy(table)
	local ret = {}
	for k,v in pairs(table) do 
		ret[k] = v
	end
	return ret
end

PlayerDataCounter = {
	ChoGathStacks = 0,	
	ChoGathStackAbility = nil
}

GameData:AddSchema("DataCounter", PlayerDataCounter)