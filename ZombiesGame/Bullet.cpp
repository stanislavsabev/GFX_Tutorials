#include <GameEngineOpenGL\ResourceManager.h>
#include "Bullet.h"
#include "Agent.h"
#include "Human.h"
#include "Zombie.h"
#include "Level.h"


Bullet::Bullet(glm::vec2 position, glm::vec2 direction,
	float bulletDamage, float bulletSpeed) :
	m_pos(position),
	m_dir(direction),
	m_damage(bulletDamage),
	m_speed(bulletSpeed)
{
}

Bullet::~Bullet()
{
}

bool Bullet::update(const std::vector<std::string>& lvlData, float deltaTime)
{
	m_pos += m_dir * m_speed * deltaTime;
	return collideWithLevel(lvlData);
}

void Bullet::draw(ge::SpriteBatch& spriteBatch)
{
	static int texId = ge::ResourceManager::getTexture("Textures/agent.png").id;

	auto destRect = glm::vec4 (m_pos.x , m_pos.y ,
		BULLET_WIDTH, BULLET_WIDTH);

	const auto uvRect = glm::vec4(0.f, 0.f, 1.f, 1.f);

	auto color = ge::ColorRGBA8(75, 75, 75, 255);
	// drawing
	spriteBatch.draw(destRect, uvRect, texId, 0.f, color);
}

bool Bullet::collideWithAgent(Agent * agent)
{
	const float MIN_DISTANCE = AGENT_RADIUS + BULLET_RADIUS;
	auto thisCenterPos = this->m_pos + glm::vec2(BULLET_RADIUS);
	auto agentCenterPos = agent->getPos() + glm::vec2(AGENT_RADIUS);

	auto distVec = thisCenterPos - agentCenterPos;
	auto collisionDepth = MIN_DISTANCE - glm::length(distVec);

	// if collision occured
	if (0 < collisionDepth) {
		return true;
	}
	return false;
}

bool Bullet::collideWithLevel(const std::vector<std::string>& lvlData)
{
	// getting bullet's position in the world
	//auto thisCenterPos = this->pos + glm::vec2(BULLET_RADIUS);
	float gridX = floor(m_pos.x / TILE_WITH);
	float gridY = floor(m_pos.y / TILE_WITH);

	// if we are outside of the world, just return
	if (gridX < 0 || gridX >= lvlData[0].size() ||
		gridY < 0 || gridY >= lvlData.size()) {
		return true;
	}

	// true if is a wall
	return ('.' != lvlData[(int)gridY][(int)gridX]);
}
