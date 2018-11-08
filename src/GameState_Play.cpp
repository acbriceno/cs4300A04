#include "GameState_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "math.h"

GameState_Play::GameState_Play(GameEngine & game, const std::string & levelPath)
	: GameState(game)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}

void GameState_Play::init(const std::string & levelPath)
{
	loadLevel(levelPath);
	spawnPlayer();
	spawnSword(m_player);
}

void GameState_Play::loadLevel(const std::string & filename)
{
	m_entityManager = EntityManager();


	//loading entities
	std::ifstream file(filename);
	std::string str, animation, AI;
	size_t tilex, tiley, roomx, roomy, bm, bv, pp, pxi, pyi;
	float followSpeed, patrolSpeed;

	while (file.good())
	{
		file >> str;
		//64*tile y +half of 64, roomx * windowsx and roomy *window y
		if (str == "Tile")
		{
			float posx = (roomx * m_game.window().getSize().x) + ((tilex * 64) + 32);
			float posy = (roomy * m_game.window().getSize().y) + ((tiley * 64) + 32);

			file >> animation >> roomx >> roomy >> tilex >> tiley >> bm >> bv;

			auto entity = m_entityManager.addEntity("Tile");
			entity->addComponent<CTransform>(Vec2(posx, posy));
			entity->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
			entity->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(animation).getSize(), bm, bv);
			//entity->addComponent<CDraggable>();
		}
		else if (str == "NPC")
		{
			float posx = (roomx * m_game.window().getSize().x) + ((tilex * 64) + 32);
			float posy = (roomy * m_game.window().getSize().y) + ((tiley * 64) + 32);
			file >> animation >> roomx >> roomy >> tilex >> tiley >> bm >> bv >> AI;
			auto entity = m_entityManager.addEntity("NPC");

			if (AI == "Follow")
			{
				file >> followSpeed;
				entity->addComponent<CTransform>(Vec2(posx, posy));
				entity->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
				entity->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(animation).getSize(), bm, bv);
				//entity->addComponent<CState>()->state=("attack");
				//entity->addComponent<CFollowPlayer>()->home = Vec2(roomx,roomy);
			}
			else if (AI == "Patrol")
			{
				int num;
				file >> patrolSpeed >> pp;
				//for(int i; i<=pp;i++)

				//file>>pxi>>pyi;
				entity->addComponent<CTransform>(Vec2(posx, posy));
				entity->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
				entity->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(animation).getSize(), bm, bv);
				//entity->addComponent<CState>()->state=("attack");
				//entity->addComponent<CFollowPlayer>()->home = Vec2(roomx,roomy);
			}
		}
		else if (str == "Player")
		{
			file >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX
				>> m_playerConfig.CY >> m_playerConfig.SPEED;
		}
		else
		{
			std::cerr << "Unknown Asset Type: " << str << std::endl;
		}
	}


}

void GameState_Play::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CTransform>(Vec2(640, 384));
	m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("StandDown"), true);
	m_player->addComponent<CInput>();

	// New element to CTransform: 'facing', to keep track of where the player is facing
	m_player->getComponent<CTransform>()->facing = Vec2(0, 1);
}

void GameState_Play::spawnSword(std::shared_ptr<Entity> entity)
{
	auto eTransform = entity->getComponent<CTransform>();
	//facing*64 , facing*64

	auto sword = m_entityManager.addEntity("sword");
	sword->addComponent<CTransform>(m_player->getComponent<CTransform>()->pos + eTransform->facing * 64);
	sword->addComponent<CAnimation>(m_game.getAssets().getAnimation("SwordRight"), true);
}

void GameState_Play::update()
{
	m_entityManager.update();

	if (!m_paused)
	{
		sAI();
		sMovement();
		sLifespan();
		sCollision();
		sAnimation();
	}

	sUserInput();
	sRender();
}

void GameState_Play::sMovement()
{
	// Implement all entity movement here
}

void GameState_Play::sAI()
{
	// Implement Follow behavior of NPCS
	// Implement Patrol behavior of NPCS
}

void GameState_Play::sLifespan()
{
	// Implement Lifespan
}

void GameState_Play::sCollision()
{
	// Implement Collision detection / resolution
}

void GameState_Play::sAnimation()
{
	// Implement updating and setting of all animations here
}

void GameState_Play::sUserInput()
{
	auto pInput = m_player->getComponent<CInput>();

	sf::Event event;
	while (m_game.window().pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_game.quit();
		}
		// this event is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape: { m_game.popState(); break; }
			case sf::Keyboard::W: { pInput->up = true; break; }
			case sf::Keyboard::A: { pInput->left = true; break; }
			case sf::Keyboard::S: { pInput->down = true; break; }
			case sf::Keyboard::D: { pInput->right = true; break; }
			case sf::Keyboard::Z: { init(m_levelPath); break; }
			case sf::Keyboard::R: { m_drawTextures = !m_drawTextures; break; }
			case sf::Keyboard::F: { m_drawCollision = !m_drawCollision; break; }
			case sf::Keyboard::Y: { m_follow = !m_follow; break; }
			case sf::Keyboard::P: { setPaused(!m_paused); break; }
			case sf::Keyboard::Space: { spawnSword(m_player); break; }
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W: { pInput->up = false; break; }
			case sf::Keyboard::A: { pInput->left = false; break; }
			case sf::Keyboard::S: { pInput->down = false; break; }
			case sf::Keyboard::D: { pInput->right = false; break; }
			case sf::Keyboard::Space: { pInput->shoot = false; pInput->canShoot = true; break; }
			}
		}
	}
}

void GameState_Play::sRender()
{
	m_game.window().clear(sf::Color(255, 192, 122));

	// TODO: set the window view correctly

	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
			auto transform = e->getComponent<CTransform>();

			if (e->hasComponent<CAnimation>())
			{
				auto animation = e->getComponent<CAnimation>()->animation;
				animation.getSprite().setRotation(transform->angle);
				animation.getSprite().setPosition(transform->pos.x, transform->pos.y);
				animation.getSprite().setScale(transform->scale.x, transform->scale.y);
				m_game.window().draw(animation.getSprite());
			}
		}
	}

	// draw all Entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto box = e->getComponent<CBoundingBox>();
				auto transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box->size.x - 1, box->size.y - 1));
				rect.setOrigin(sf::Vector2f(box->halfSize.x, box->halfSize.y));
				rect.setPosition(transform->pos.x, transform->pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (box->blockMove && box->blockVision) { rect.setOutlineColor(sf::Color::Black); }
				if (box->blockMove && !box->blockVision) { rect.setOutlineColor(sf::Color::Blue); }
				if (!box->blockMove && box->blockVision) { rect.setOutlineColor(sf::Color::Red); }
				if (!box->blockMove && !box->blockVision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game.window().draw(rect);
			}

			if (e->hasComponent<CPatrol>())
			{
				auto & patrol = e->getComponent<CPatrol>()->positions;
				for (size_t p = 0; p < patrol.size(); p++)
				{
					dot.setPosition(patrol[p].x, patrol[p].y);
					m_game.window().draw(dot);
				}
			}

			if (e->hasComponent<CFollowPlayer>())
			{
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position.x = e->getComponent<CTransform>()->pos.x;
				lines[0].position.y = e->getComponent<CTransform>()->pos.y;
				lines[0].color = sf::Color::Black;
				lines[1].position.x = m_player->getComponent<CTransform>()->pos.x;
				lines[1].position.y = m_player->getComponent<CTransform>()->pos.y;
				lines[1].color = sf::Color::Black;
				m_game.window().draw(lines);
				dot.setPosition(e->getComponent<CFollowPlayer>()->home.x, e->getComponent<CFollowPlayer>()->home.y);
				m_game.window().draw(dot);
			}
		}
	}

	m_game.window().display();
}
