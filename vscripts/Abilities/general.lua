function BlinkToPoint(keys)
	local location=keys.target_points[1]
	local unit=keys.caster
	unit:MoveToPosition(location)
	unit:Damage()
end