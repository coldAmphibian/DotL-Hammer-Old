function makeForwardProjectile(keys)
  --PrintTable(keys)
  local caster = keys.caster
  local ability = keys.ability
  local origin = caster:GetOrigin()
  origin.z = origin.z + 60
  
  local info = {
    EffectName = keys.EffectName,
    Ability = ability,
    vSpawnOrigin = caster:GetAbsOrigin(),
    fDistance = tonumber(keys.FixedDistance),
    fStartRadius = tonumber(keys.StartRadius),
    fEndRadius = tonumber(keys.EndRadius),
    Source = caster,
    bHasFrontalCone = false,
    iMoveSpeed = tonumber(keys.MoveSpeed),
    bReplaceExisting = true,
    iUnitTargetTeam = DOTA_UNIT_TARGET_TEAM_ENEMY,
    iUnitTargetFlags = DOTA_UNIT_TARGET_FLAG_NONE,
    iUnitTargetType = DOTA_UNIT_TARGET_HERO + DOTA_UNIT_TARGET_BASIC,
    --iUnitTargetTeam = DOTA_UNIT_TARGET_TEAM_FRIENDLY,
    --iUnitTargetFlags = DOTA_UNIT_TARGET_FLAG_NONE,
    --iUnitTargetType = DOTA_UNIT_TARGET_HERO + DOTA_UNIT_TARGET_BASIC + DOTA_UNIT_TARGET_OTHER,
    --fMaxSpeed = 5200,
    fExpireTime = GameRules:GetGameTime() + 8.0,
  }
  
  --print ('0-------------0')
  --PrintTable(info)
  --print ('0--------------0')
  local speed = keys.MoveSpeed
  
  info.vVelocity = caster:GetForwardVector() * speed
  
  ProjectileManager:CreateLinearProjectile( info )
end        