#include "GameState_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

GameState_Play::GameState_Play(GameEngine & game, const std::string & levelPath)
    : GameState(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void GameState_Play::init(const std::string & levelPath)
{
    loadLevel(levelPath);
}

void GameState_Play::loadLevel(const std::string & filename)
{
    m_entityManager = EntityManager();

    // spawn a sample player and sword
    
    //spawnSword(m_player);
	std::ifstream fin(filename);
	std::string token, name, AI;
	int TX, TY, RX, RY;
	float S;
	bool BM, BV;

	while (fin.good()) {
		fin >> token;
		if (token == "Tile") {
			fin >> name >> RX >> RY >> TX >> TY >> BM >> BV;
			Vec2 room(RX, RY);
			Vec2 roompos(TX*64+32, TY*64+32);
			Vec2 pos(room.x*m_game.window().getSize().x + roompos.x, room.y*m_game.window().getSize().y + roompos.y);
			auto block = m_entityManager.addEntity("tile");
			block->addComponent<CTransform>(pos);
			block->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			block->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(name).getSize(),BM,BV);
		}
		//else if (token == "Dec") {
			
		//}
		else if (token == "Player") {
			fin >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED;
		}
		else if (token == "NPC")
		{
			fin >> name >> RX >> RY >> TX >> TY >> BM >> BV >> AI >> S;
			Vec2 room(RX, RY);
			Vec2 roompos(TX * 64 + 32, TY * 64 + 32);
			Vec2 pos(room.x*m_game.window().getSize().x + roompos.x, room.y*m_game.window().getSize().y + roompos.y);
			auto npc = m_entityManager.addEntity("npc");
			npc->addComponent<CTransform>(pos);
			npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			npc->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(name).getSize(), BM, BV);
			if (AI == "Patrol")
			{
				int n;
				fin >> n;
				std::vector<Vec2> pos;
				int x, y;
				for (int i = 0; i < n; i++)
				{
					fin >> x >> y;
					Vec2 spot(x*64 + 32, y*64 + 32);
					Vec2 spotpos(room.x*m_game.window().getSize().x + spot.x, room.y*m_game.window().getSize().y + spot.y);
					pos.push_back(spotpos);
				}
				npc->addComponent<CPatrol>(pos, S);
			}
			else{npc->addComponent<CFollowPlayer>(pos, S);}
		}
		else { std::cout << "Bad\n"; }
	}

	spawnPlayer();
}

void GameState_Play::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(Vec2(640, 384));
    m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("StandDown"), true);
    m_player->addComponent<CInput>();
    m_player->getComponent<CTransform>()->facing = Vec2(0, 1);
	Vec2 BB(m_playerConfig.CX, m_playerConfig.CY);
	m_player->addComponent<CBoundingBox>(BB, false, false);
	m_player->addComponent<CState>("Stand");
}

void GameState_Play::spawnSword(std::shared_ptr<Entity> entity)
{
	auto eTransform = entity->getComponent<CTransform>();

	auto sword = m_entityManager.addEntity("sword");
	sword->addComponent<CTransform>(eTransform->pos + eTransform->facing * 64);
	sword->getComponent<CTransform>()->facing = eTransform->facing;
	sword->addComponent<CBoundingBox>(m_game.getAssets().getAnimation("SwordUp").getSize(), false, false);
	sword->addComponent<CLifeSpan>(150);
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
	m_player->getComponent<CTransform>()->speed = Vec2(0, 0);

	auto pTransform = m_player->getComponent<CTransform>();
	auto pInput = m_player->getComponent<CInput>();
	bool hor = false;

	
	m_player->getComponent<CState>()->state = "Stand";
	if (pInput->right)
	{
		pTransform->scale.x = 1;
		pTransform->speed.x += m_playerConfig.SPEED;
		hor = true;
		pTransform->facing = Vec2(1,0);
		m_player->getComponent<CState>()->state = "Run";
	}
	if (pInput->left)
	{
		pTransform->scale.x = -1;
		pTransform->speed.x -= m_playerConfig.SPEED;
		hor = true;
		pTransform->facing = Vec2(-1, 0);
		m_player->getComponent<CState>()->state = "Run";
		
	}
	if (pInput->up && !hor)
	{
		
		pTransform->speed.y -= m_playerConfig.SPEED;
		pTransform->facing = Vec2(0,-1);
		m_player->getComponent<CState>()->state = "Run";
	}
	if (pInput->down && !hor)
	{
		
		pTransform->speed.y += m_playerConfig.SPEED;
		pTransform->facing = Vec2(0,1);
		m_player->getComponent<CState>()->state = "Run";

	}
	
	
	for (auto e : m_entityManager.getEntities())
	{
		e->getComponent<CTransform>()->pos += e->getComponent<CTransform>()->speed;
	}

	for (auto e : m_entityManager.getEntities("sword"))
	{
		pInput->shoot = false;
		e->getComponent<CTransform>()->facing = pTransform->facing;
		e->getComponent<CTransform>()->pos = pTransform->pos + Vec2(64, 64)*pTransform->facing;
		m_player->getComponent<CState>()->state = "Atk";
	}

	if (pInput->shoot && pInput->canShoot)
	{
		pInput->canShoot = false;
		spawnSword(m_player);
	}
}

void GameState_Play::sAI()
{
	
	for (auto e : m_entityManager.getEntities("npc"))
	{
		auto eTransform = e->getComponent<CTransform>();
		Vec2 dest(0, 0);
		float speed = 0;
		eTransform->speed = Vec2(0, 0);
		if (e->hasComponent<CPatrol>())
		{
			auto ePatrol = e->getComponent<CPatrol>();
			speed = ePatrol->speed;
			
			dest = ePatrol->positions[ePatrol->currentPosition];

			if (eTransform->pos.dist(dest) <= 5)
			{
				ePatrol->currentPosition += 1;
				if (ePatrol->positions.size() <= ePatrol->currentPosition)
				{
					ePatrol->currentPosition = 0;
				}
				dest = ePatrol->positions[ePatrol->currentPosition];
			}
		}
		else
		{
			bool cansee = true;
			speed = e->getComponent<CFollowPlayer>()->speed;
			for (auto t : m_entityManager.getEntities())
			{
				if (t->getComponent<CBoundingBox>()->blockVision && t != e && t != m_player)
				{
					if (Physics::EntityIntersect(m_player->getComponent<CTransform>()->pos, eTransform->pos, t))
					{
						cansee = false;
						break;
					}
				}
			}
			
			if (cansee) { dest = m_player->getComponent<CTransform>()->pos; }
			else { dest = e->getComponent<CFollowPlayer>()->home; }
		}
		if (eTransform->pos.dist(dest) <= 5)
		{
			eTransform->speed = Vec2(0, 0);
		}
		else
		{
			float dx = dest.x - eTransform->pos.x;
			float dy = dest.y - eTransform->pos.y;
			float ang = atan2(dy, dx);
			float vx = speed*cos(ang);
			float vy = speed*sin(ang);
			eTransform->speed = Vec2(vx, vy);
		}

	}
}

void GameState_Play::sLifespan()
{
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CLifeSpan>())
		{

			sf::Time time = e->getComponent<CLifeSpan>()->clock.getElapsedTime();
			if (time.asMilliseconds() >= e->getComponent<CLifeSpan>()->lifespan)
			{
				e->destroy();
			}
		}
	}
}

void GameState_Play::sCollision()
{
	auto pTransform = m_player->getComponent<CTransform>();
	for (auto tile : m_entityManager.getEntities("tile"))
	{
		Vec2 overlap = Physics::GetOverlap(tile, m_player);
		if (overlap.x > 0 && overlap.y > 0 && tile->getComponent<CBoundingBox>()->blockMove)
		{
			m_player->getComponent<CTransform>()->pos -= (m_player->getComponent<CTransform>()->facing * overlap);
		}
	}
}

void GameState_Play::sAnimation()
{
	auto pTransform = m_player->getComponent<CTransform>();
	
	std::string animation = m_player->getComponent<CState>()->state;

	if (pTransform->facing.x == 0)
	{
		if (pTransform->facing.y == 1)
		{
			animation += "Down";
		}
		else
		{
			animation += "Up";
		}
	}
	else
	{
		animation += "Right";
	}

	if (m_player->getComponent<CAnimation>()->animation.getName() != animation)
	{
		m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
	}

	for (auto e : m_entityManager.getEntities("sword"))
	{
		e->getComponent<CTransform>()->scale = Vec2(1, 1);
		if (abs(e->getComponent<CTransform>()->facing.y) == 1) 
		{
			e->addComponent<CAnimation>(m_game.getAssets().getAnimation("SwordUp"), true);
			e->getComponent<CTransform>()->scale.y = e->getComponent<CTransform>()->facing.y * -1;
		}
		else 
		{
			e->addComponent<CAnimation>(m_game.getAssets().getAnimation("SwordRight"), true); 
			e->getComponent<CTransform>()->scale.x = e->getComponent<CTransform>()->facing.x;
		}

		
	}

	for (auto e : m_entityManager.getEntities())
	{
		e->getComponent<CAnimation>()->animation.update();

		if (e->getComponent<CAnimation>()->animation.hasEnded() && !(e->getComponent<CAnimation>()->repeat))
		{
			e->destroy();
		}
	}
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
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:  { m_game.popState(); break; }
                case sf::Keyboard::W:       { pInput->up = true; break; }
                case sf::Keyboard::A:       { pInput->left = true; break; }
                case sf::Keyboard::S:       { pInput->down = true; break; }
                case sf::Keyboard::D:       { pInput->right = true; break; }
                case sf::Keyboard::Z:       { init(m_levelPath); break; }
                case sf::Keyboard::R:       { m_drawTextures = !m_drawTextures; break; }
                case sf::Keyboard::F:       { m_drawCollision = !m_drawCollision; break; }
                case sf::Keyboard::Y:       { m_follow = !m_follow; break; }
                case sf::Keyboard::P:       { setPaused(!m_paused); break; }
                case sf::Keyboard::Space:   { pInput->shoot = true; break; }
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:       { pInput->up = false; break; }
                case sf::Keyboard::A:       { pInput->left = false; break; }
                case sf::Keyboard::S:       { pInput->down = false; break; }
                case sf::Keyboard::D:       { pInput->right = false; break; }
                case sf::Keyboard::Space:   { pInput->shoot = false; pInput->canShoot = true; break; }
            }
        }
    }
}

void GameState_Play::sRender()
{
    m_game.window().clear(sf::Color(255, 192, 122));

    // TODO: set the window view correctly
    sf::View view(sf::FloatRect(0.f, 0.f, m_game.window().getSize().x, m_game.window().getSize().y));
    if (m_follow)
    {
        view.setCenter(sf::Vector2f(m_player->getComponent<CTransform>()->pos.x, m_player->getComponent<CTransform>()->pos.y));
    }
    else
    {
    }
    m_game.window().setView(view);
        
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
                rect.setSize(sf::Vector2f(box->size.x-1, box->size.y-1));
                rect.setOrigin(sf::Vector2f(box->halfSize.x, box->halfSize.y));
                rect.setPosition(transform->pos.x, transform->pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box->blockMove && box->blockVision)  { rect.setOutlineColor(sf::Color::Black); }
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
