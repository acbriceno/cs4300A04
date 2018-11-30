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
	m_player->addComponent<CDraggable>();
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
	sDrag();
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
				if (t->hasComponent<CBoundingBox>())
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
	
	for (auto e : m_entityManager.getEntities("npc"))
	{
		for (auto tile : m_entityManager.getEntities())
		{
			if (tile->tag() == "tile")
			{
				Vec2 pOverlap = Physics::GetOverlap(tile, m_player);
				if (pOverlap.x > 0 && pOverlap.y > 0 && tile->getComponent<CBoundingBox>()->blockMove)
				{
					m_player->getComponent<CTransform>()->pos -= (m_player->getComponent<CTransform>()->facing * pOverlap);
				}
				Vec2 eOverlap = Physics::GetOverlap(tile, e);
				if (eOverlap.x > 0 && eOverlap.y > 0 && tile->getComponent<CBoundingBox>()->blockMove)
				{
					Vec2 tOverlap = Physics::GetPreviousOverlap(tile, e);

					if (tOverlap.x <= 0)
					{

						if (e->getComponent<CTransform>()->speed.x <= 0)
						{
							e->getComponent<CTransform>()->pos.x += eOverlap.x;
						}
						else
						{
							e->getComponent<CTransform>()->pos.x -= eOverlap.x;
						}

					}
					else
					{

						if (e->getComponent<CTransform>()->speed.y <= 0)
						{
							e->getComponent<CTransform>()->pos.y += eOverlap.y;
						}
						else
						{
							e->getComponent<CTransform>()->pos.y -= eOverlap.y;
						}

					}


				}
			}
			if (tile->tag() == "sword")
			{
				Vec2 overlap = Physics::GetOverlap(tile, e);
				if (overlap.x > 0 && overlap.y > 0)
				{
					auto exp = m_entityManager.addEntity("explosion");
					//auto pTransform = m_player->getComponent<CTransform>();
					exp->addComponent<CTransform>()->pos = e->getComponent<CTransform>()->pos;
					//exp->getComponent<CTransform>()->facing = eTransform->facing;
					//sword->addComponent<CBoundingBox>(m_game.getAssets().getAnimation("SwordUp").getSize(), false, false);
					exp->addComponent<CAnimation>(m_game.getAssets().getAnimation("Explosion"), false);
					e->destroy();
				}
			}
			if (tile->tag() == "player")
			{
				Vec2 overlap = Physics::GetOverlap(tile, e);
				if (overlap.x > 0 && overlap.y > 0)
				{
					m_player->destroy();
					spawnPlayer();
				}
			}
		}
		

		
	}
}

void GameState_Play::sDrag(){
    
    if (mouseClicked && !selected) {
   
        m_player->getComponent<CDraggable>()->draggable = Physics::PointInBounds(mousePosition, m_player);
        selected =  m_player->getComponent<CDraggable>()->draggable;
        mouseClicked = false;
    }
    
    if(mouseClicked && m_player->getComponent<CDraggable>()->draggable )
    {
        m_player->getComponent<CTransform>()->pos.x = mousePosition.x;
        m_player->getComponent<CTransform>()->pos.y = mousePosition.y;
        m_player->getComponent<CDraggable>()->draggable = false;
        mouseClicked = false;
        selected = false;
    }
    
    
//Drag and Hold
//    if(mouseClicked)
//    {
//        m_player->getComponent<CDraggable>()->draggable = Physics::PointInBounds(mousePosition, m_player);
//
//    }
//    if(mouseClickReleased && m_player->getComponent<CDraggable>()->draggable)
//    {
//        m_player->getComponent<CTransform>()->pos.x = mousePosition.x;
//        m_player->getComponent<CTransform>()->pos.y = mousePosition.y;
//        mouseClickReleased = false;
//    }
    
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
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
//                std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
//                std::cout << "Player Entity at (" << m_player->getComponent<CTransform>()->pos.x << "," << m_player->getComponent<CTransform>()->pos.x << ")\n";
//                std::cout << "Player Entity Bounding BOX at (" << m_player->getComponent<CBoundingBox>()->halfSize.x << "," <<  m_player->getComponent<CBoundingBox>()->halfSize.y << ")\n";
                mouseClicked = true;
                mousePosition.x = event.mouseButton.x;
                mousePosition.y = event.mouseButton.y;
    
            }
            
        }
        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
// Drag and Hold
//                std::cout << "Left Mouse Button Released at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
//                mousePosition.x = event.mouseButton.x;
//                mousePosition.y = event.mouseButton.y;
//                mouseClicked = false;
//                  mouseClickReleased = true;
            }
            
        }
    }
}

void GameState_Play::sRender()
{
    m_game.window().clear(sf::Color(255, 192, 122));

    roomY =0;
    roomX = 0;
    
if (m_player->getComponent<CTransform>()->pos.x >=0 && m_player->getComponent<CTransform>()->pos.x <=1280) {
        roomX = 0;
    }
    if (m_player->getComponent<CTransform>()->pos.x >=1281 && m_player->getComponent<CTransform>()->pos.x <=2561) {
        roomX = 1;
    }
    if (m_player->getComponent<CTransform>()->pos.x >=2562 && m_player->getComponent<CTransform>()->pos.x <=3842) {
        roomX = 2;
    }
    if (m_player->getComponent<CTransform>()->pos.x >=3843 && m_player->getComponent<CTransform>()->pos.x <=5123) {
        roomX = 3;
    }
    if (m_player->getComponent<CTransform>()->pos.x >= -1280 && m_player->getComponent<CTransform>()->pos.x <=-1) {
        roomX = -1;
    }
    if (m_player->getComponent<CTransform>()->pos.x >= -2561 && m_player->getComponent<CTransform>()->pos.x <= -1281) {
        roomX = -2;
    }
    if (m_player->getComponent<CTransform>()->pos.x >= -3842 && m_player->getComponent<CTransform>()->pos.x <= -2562) {
        roomX = -3;
    }
    
    if (m_player->getComponent<CTransform>()->pos.y >=0 && m_player->getComponent<CTransform>()->pos.y <=768) {
        roomY = 0;
    }
    if (m_player->getComponent<CTransform>()->pos.y >=769 && m_player->getComponent<CTransform>()->pos.y <=1537) {
        roomY = 1;
    }
    if (m_player->getComponent<CTransform>()->pos.y >=1538 && m_player->getComponent<CTransform>()->pos.y <=2306) {
        roomY = 2;
    }
    if (m_player->getComponent<CTransform>()->pos.y >=2307 && m_player->getComponent<CTransform>()->pos.y <=3075) {
        roomY = 3;
    }
    if (m_player->getComponent<CTransform>()->pos.y >= -768 && m_player->getComponent<CTransform>()->pos.y <= -1) {
        roomY = -1;
    }
    if (m_player->getComponent<CTransform>()->pos.y >= -1537 && m_player->getComponent<CTransform>()->pos.y <= -769) {
        roomY = -2;
    }
    if (m_player->getComponent<CTransform>()->pos.y >= -2306 && m_player->getComponent<CTransform>()->pos.y <= -1538) {
        roomY = -3;
    }


    
    // TODO: set the window view correctly
    sf::View view(sf::FloatRect(0.f, 0.f, m_game.window().getSize().x, m_game.window().getSize().y));
    if (m_follow)
    {
        view.setCenter(sf::Vector2f(m_player->getComponent<CTransform>()->pos.x, m_player->getComponent<CTransform>()->pos.y));
    }
    else
    {
      
            int centerX =m_game.window().getSize().x/2 + (roomX*  m_game.window().getSize().x);
            int centerY =m_game.window().getSize().y/2 + (roomY *  m_game.window().getSize().y);
             view.setCenter(centerX, centerY);
        
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
