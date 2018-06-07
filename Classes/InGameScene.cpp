
#include "InGameScene.h"
#include "GameOverScene.h"
#include "Audio.h"

USING_NS_CC;

#define PLANE_BIT_MASK    0x1
#define ENEMY_BIT_MASK    0x2
#define BULLET_BIT_MASK    0x4
#define EDGE_BIT_MASK    0x8

inline float deg2rad(float deg)
{
	const float deg2rad_k = float(M_PI) / 180.0f;
	return deg * deg2rad_k;
}

inline float rad2deg(float deg)
{
	const float rad2deg_k = 180.0f / float(M_PI);
	return deg * rad2deg_k;
}

bool InGameScene::CPlane::init()
{
	bool ret = Sprite::initWithFile("plane.png");

	return ret;
}

bool InGameScene::CEnemy::init()
{
	static const char *enemy_pics[] = {

		"enemy1.png",
		"enemy2.png",
		"enemy3.png",
		"enemy4.png",
		"enemy5.png",
		"enemy6.png"
	};
	const int N = std::extent<decltype(enemy_pics)>::value;
	const char *pic = enemy_pics[cocos2d::random(0, N - 1)];

	bool ret = Sprite::initWithFile(pic);

	const float size = 100.0f;
	const float scale = size / std::max(this->getContentSize().width, this->getContentSize().height);
	this->setScale(scale);

	return ret;
}

bool InGameScene::CBullet::init()
{
	bool ret = Sprite::initWithFile("bullet.png");
	
	const float size = 30.0f;
	const float scale = size / std::max(this->getContentSize().width, this->getContentSize().height);
	this->setScale(scale);

	return ret;
}

Scene* InGameScene::createScene()
{
	auto scene = InGameScene::createWithPhysics();
	
	PhysicsWorld *phy = scene->getPhysicsWorld();
	phy->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	phy->setGravity({});
	
	auto layer = InGameScene::create();
	layer->setName("InGameScene");
	scene->addChild(layer);

	return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool InGameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	auto body = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 5.f);

	/*auto edgeNode = Node::create();
	edgeNode->setPosition({ visibleSize.width / 2,visibleSize.height / 2 });
	edgeNode->setPhysicsBody(body);
	this->addChild(edgeNode);*/


	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(InGameScene::OnReturnButtonClicked, this));
	if (closeItem == nullptr ||
		closeItem->getContentSize().width <= 0 ||
		closeItem->getContentSize().height <= 0)
	{
		problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
	}
	else
	{
		float x = origin.x + closeItem->getContentSize().width / 2;
		float y = origin.y + visibleSize.height - closeItem->getContentSize().height / 2;
		closeItem->setPosition(Vec2(x, y));
	}

	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

    // add background

	static const char *bg_pics[] = {

		"bg1.jpg",
		"bg2.jpg",
		"bg3.jpg",
		"bg4.jpg"
	};
	const int N = std::extent<decltype(bg_pics)>::value;
	const char *pic = bg_pics[cocos2d::random(0, N - 1)];

    auto bgmap = Sprite::create(pic);
    if (bgmap == nullptr)
    {
        problemLoading(pic);
    }
    else
    {
		auto winsize = Director::getInstance()->getWinSize();
		bgmap->setScale(std::max(visibleSize.width / bgmap->getContentSize().width, visibleSize.height / bgmap->getContentSize().height));

		float x = origin.x + bgmap->getContentSize().width;
		float y = origin.y + bgmap->getContentSize().height / 2;
		bgmap->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 2));
        // add the sprite as a child to this layer
        this->addChild(bgmap, 0);
    }

	// elements
	m_pPlane = CPlane::create();
	if (m_pPlane == nullptr)
	{
		problemLoading("'plane.png'");
	}
	else 
	{
		m_pPlane->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 2));

		auto planeBody = PhysicsBody::createCircle(256.0);
		planeBody->setVelocityLimit(0.f);
		planeBody->setContactTestBitmask(PLANE_BIT_MASK);
		m_pPlane->setPhysicsBody(planeBody);
		
		this->addChild(m_pPlane, 99);

		
		
	}

	auto label = Label::createWithTTF("New Space War", "fonts/Marker Felt.ttf", 36);
	if (label == nullptr)
	{
		problemLoading("'fonts/Marker Felt.ttf'");
	}
	else
	{
		// position the label on the center of the screen
		label->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height - label->getContentSize().height));

		// add the label as a child to this layer
		this->addChild(label, 1);
	}
	m_pScores = label;
	UpdateStats();

	scheduleUpdate();
	schedule(schedule_selector(InGameScene::OnCreateEnemy), 1.0f);
	m_iShotCount = m_iKilled = 0;

	
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(createMouseListener(), this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(createContactListener(), this);
    return true;
}

void InGameScene::OnReturnButtonClicked(Ref* pSender)
{
	Director::getInstance()->popScene();
}

void InGameScene::OnCreateEnemy(float dt)
{

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	float x = cocos2d::random(origin.x, origin.x + visibleSize.width);
	float y = origin.y + visibleSize.height;

	auto spr = CEnemy::create();
	if (!spr)
	{
		// throw TBD
		return;
	}
	
	spr->setPosition(x, y);

	auto sprBody = PhysicsBody::createBox(spr->getContentSize());
	sprBody->setContactTestBitmask(ENEMY_BIT_MASK);
	spr->setPhysicsBody(sprBody);

	addChild(spr);
	m_pEnemies.push_back(spr);
}

void InGameScene::PlaneShoot()
{
	++m_iShotCount;
	UpdateStats();
	
	auto src = m_pPlane->getPosition();

	float degrees = m_pPlane->getRotation();
	float rad = deg2rad(degrees);
	Vec2 dir{ sin(rad), cos(rad) };

	src += dir * 400.0f;
	auto spr = CBullet::create();
	if (!spr)
	{
		problemLoading("bullet.png");
		return;
	}

	spr->setRotation(degrees);
	spr->setPosition(src);

	auto sprBody = PhysicsBody::createBox(spr->getContentSize());
	spr->setPhysicsBody(sprBody);
	sprBody->setContactTestBitmask(BULLET_BIT_MASK);
	sprBody->setVelocity(dir * 1000.0f);

	addChild(spr);
	m_pBullets.push_back(spr);

}

void InGameScene::update(float dt)
{
	auto &dist = m_pPlane->getPosition();
	for (auto p : m_pEnemies)
	{
		auto &src = p->getPosition();
		auto vel = (dist - src).getNormalized() * 100.0f;
		p->getPhysicsBody()->setVelocity(vel);
	}

	for (auto iter = m_pBullets.begin(); iter != m_pBullets.end(); /**/)
	{
		auto p = *iter;
		
		float x = p->getPositionNormalized().x;
		float y = p->getPositionNormalized().y;
		if (x < 0 || x > 1 || y < 0 || y > 1)
		{
			m_pBullets.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

EventListenerMouse *InGameScene::createMouseListener()
{
	EventListenerMouse *listener = EventListenerMouse::create();

	static bool bLeftStatus = false;
	static bool bRightStatus = false;

	listener->onMouseDown = [&](EventMouse* event) {

		switch (event->getMouseButton())
		{
		case EventMouse::MouseButton::BUTTON_LEFT:
			bLeftStatus = true;
			
			break;
		case EventMouse::MouseButton::BUTTON_RIGHT:
			bRightStatus = true;
			break;
		}
	};

	listener->onMouseUp = [&](EventMouse* event) {

		switch (event->getMouseButton())
		{
		case EventMouse::MouseButton::BUTTON_LEFT:
			if(bLeftStatus)
				this->PlaneShoot();
			bLeftStatus = false;
			break;
		case EventMouse::MouseButton::BUTTON_RIGHT:
			if (bRightStatus)
			{
				auto visibleSize = Director::getInstance()->getVisibleSize();
				auto pos = event->getLocation();
				Vec2 dest = { pos.x, visibleSize.height - pos.y };
				m_pPlane->setPosition(dest);
			}
			bRightStatus = false;
			break;
		}
	};

	listener->onMouseMove = [&](EventMouse* event) {
		auto src = m_pPlane->getPosition();

		Vec2 dest = event->getLocation();

		m_pPlane->setRotation(fmod(rad2deg((dest - src).getAngle() + 180.0f), 360.0f) - 180.0f);
		
	};

	return listener;
}

EventListenerPhysicsContact *InGameScene::createContactListener()
{
	EventListenerPhysicsContact *listener = nullptr;
	listener = EventListenerPhysicsContact::create();
	listener->onContactBegin = [&](const PhysicsContact& contact) {
		Node *p1 = contact.getShapeA()->getBody()->getNode();
		Node *p2 = contact.getShapeB()->getBody()->getNode();

		// Note: Uses RTTI to check whether the object is right

		auto check_plane_hited = [&](Node *p1, Node *p2){
			auto pPlane = dynamic_cast<CPlane *>(p1);
			auto pEnemy = dynamic_cast<CEnemy *>(p2);

			if (!pPlane || !pEnemy)
				return;

			if (pPlane == m_pPlane)
				this->OnPlaneBeingHit();
		};
		check_plane_hited(p1, p2);
		check_plane_hited(p2, p1);
		
		auto check_bullet_hit_enemy = [&](Node *p1, Node *p2) {
		
			auto pBullet = dynamic_cast<CBullet *>(p1);
			auto pEnemy = dynamic_cast<CEnemy *>(p2);

			if (pBullet && pEnemy)
			{
				this->OnBulletHitEnemy(pBullet, pEnemy);
				return true;
			}
			return false;
		};

		// if f() deleted p, must return
		if (check_bullet_hit_enemy(p1, p2))
			return false;
		if (check_bullet_hit_enemy(p2, p1))
			return false;

		return true;
	};

	return listener;
}

void InGameScene::OnPlaneBeingHit()
{
	auto ps = GameOverScene::createScene();
	Director::getInstance()->popScene();
	Director::getInstance()->pushScene(ps);
	static_cast<GameOverScene *>(ps)->ShowScores(m_iShotCount, m_iKilled);
}


void InGameScene::OnBulletHitEnemy(CBullet *bullet, CEnemy *enemy)
{
	
	m_iKilled++;
	UpdateStats();

	// explode effect
	auto spr = Sprite::create("hit.png");
	if (spr)
	{
		spr->setPosition(bullet->getPosition());
		this->addChild(spr);
		std::function<void(float)> f = [this, spr](float) { this->removeChild(spr); };
		spr->scheduleOnce(f, 0.5f, "del");
	}

	auto iter1 = std::find(m_pBullets.begin(), m_pBullets.end(), bullet);
	if(iter1 != m_pBullets.end())
		m_pBullets.erase(iter1);

	auto iter2 = std::find(m_pEnemies.begin(), m_pEnemies.end(), enemy);
	if (iter2 != m_pEnemies.end())
		m_pEnemies.erase(iter2);

	/*std::function<void(float)> fRemove = [this, bullet, enemy](float) { 
		this->removeChild(bullet); 
		this->removeChild(enemy);
	};
	schedule(fRemove, "remove");*/
	this->removeChild(bullet);
	this->removeChild(enemy);
}

void InGameScene::UpdateStats()
{
	std::ostringstream oss;
	oss << "Score:" << "\t";
	oss << "Kill = " << m_iKilled << "\t";
	oss << "Shot = " << m_iShotCount << "\t";

	m_pScores->setString(oss.str());
}

void InGameScene::onEnter()
{
	Scene::onEnter();

	Audio_PlayBgm();
}

void InGameScene::onExit()
{
	Scene::onExit();

	Audio_StopBgm();
}