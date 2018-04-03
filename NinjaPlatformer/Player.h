#pragma once
#include <GameEngineOpenGL\SpriteBatch.h>
#include <GameEngineOpenGL\TileSheet.h>
#include <GameEngineOpenGL\InputManager.h>
#include <GameEngineOpenGL\DebugRenderer.h>
#include "Box.h"
#include "Capsule.h"

class Player
{
public:
	Player();
	~Player();

	void init(b2World* world, glm::vec2 pos, glm::vec2 drawDims,
		glm::vec2 collitionDims, ge::ColorRGBA8 color, bool fixedRotation);

	void draw(ge::SpriteBatch& spriteBatch);
	void drawDebug(ge::DebugRenderer& debugRenderer);

	void update(ge::InputManager inputManager);

	//const Box& getBox() const { return m_collisionBox; }
	const Capsule& getCapsule() const { return m_capsule; }
	
private:
	//Box m_collisionBox;
	glm::vec2 m_drawDims;
	Capsule m_capsule;
	ge::TileSheet m_tileSheet;
	ge::ColorRGBA8 m_color;

private:
	// True if player's bottom has contact point
	bool CanJump();

};
