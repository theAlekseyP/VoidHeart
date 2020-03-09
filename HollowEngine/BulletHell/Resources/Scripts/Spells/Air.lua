function AirDash()
    -----------------------------------------
    -- playtesting vars
	local dashSpeed = player:GetStats().dashSpeed
    -----------------------------------------

	local body = gameObject:GetBody()
	-- Damp Overall Velocity and Rotation
	local transform = gameObject:GetTransform()
    local rot = transform.rotation
    local angle = rot.y
    angle = angle * math.pi / 180
    local xDir = math.sin(angle)
    local zDir = math.cos(angle)

	local direction = vec3.new(xDir, 0, zDir)--body.velocity
	local magnitude = math.sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z)
	if magnitude > 0.001 then
		direction.x = direction.x / magnitude
		direction.y = direction.y / magnitude
		direction.z = direction.z / magnitude
		local impulse = vec3.new(0.0, 0.0, 0.0)
		impulse = impulse + direction * dashSpeed
		impulse.y = 0.0
		PlaySFX("Resources/Audio/SFX/PlayerDash.wav")

		local particle = gameObject:GetParticleEmitter()
		if particle then
			particle.active = true
			particle.direction = VecNormalize(vec3.new(-xDir,0,-zDir));
			particle.count = 0
		end

		ApplyLinearImpulse(gameObject, impulse)
	end
end

function CheckValidAttack()
	local attack = gameObject:GetAttack()
	if attack.shouldAttack then
		AirDash()
		attack.currentAttackTime = 0.0
		attack.shouldAttack = false
	end
	if attack.currentAttackTime >= 0.5 then
		local particle = gameObject:GetParticleEmitter()
		if particle then
			particle.active = false
		end
	end
end

CheckValidAttack()