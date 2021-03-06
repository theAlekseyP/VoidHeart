function Move()
	local speed = 10.0
	if math.abs(enemyBody.velocity.x) < 0.000001 then
		enemyBody.velocity.x = speed
	end
	if math.abs(enemyBody.velocity.z) > 0.0 then
		 enemyBody.velocity.z = 0.0
	end
	-- Turn around if past a certain x point
	local reversePosition = 20.0
	if 	enemyBody.position.x > reversePosition or
		enemyBody.position.x < -reversePosition then
		enemyBody.velocity.x = -enemyBody.velocity.x
	end
end

function SemiCircle(offset)
	local gameObjectPath = "Resources/Json data/Bullet.json"
	local numObj = 10
	for i=0,numObj do
		go = CreateGameObject(gameObjectPath)
		body = go:GetBody()
		body.position = attackPosition
		local theta = (i/numObj * math.pi) + math.rad(offset)
		local attackSpeed = 24.0
		body.velocity = vec3.new(attackSpeed*math.cos(theta), 0.0, attackSpeed*math.sin(theta))
		transform = go:GetTransform()
		transform.position = body.position
	end
end

function VBulletPattern()
	gameObjectPath = "Resources/Json data/Bullet.json"
	local numObj = 10
	for i=1,numObj do
		go = CreateGameObject(gameObjectPath)
		body = go:GetBody()
		body.position = attackPosition + vec3.new(0, 0.0, 0.0)
		body.velocity = vec3.new((i - numObj/2)*2.0, 0.0, 10.0)
		transform = go:GetTransform()
		transform.position = body.position
	end
end

function FirePattern ()
	VBulletPattern()
	SemiCircle(0)
	PlaySFX("Resources/Audio/SFX/1UP.wav")
end

function Attack()
	if currentAttackTime > baseAttackTime then
		FirePattern()
		currentAttackTime = 0.0
	end
end

function TransitionMove()
	local transitionspeed = -2.0
	enemyBody.velocity.z = transitionspeed
end

function Update()
	if transitionTime < 0.0001 then
		Move()
		Attack()
	else
		TransitionMove()
	end
end

Update()