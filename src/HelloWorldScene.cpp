#include "HelloWorldScene.h"
#include <sstream>

using namespace cocos2d;

#define PTM_RATIO 32		// Точек на метр

enum
{
	kTagCross = 1,
	kTagSun   = 2,
	kTagScore = 3
};

// Формирует строку формата "Score: n"
std::string ScoreString (int n)
{
	std::ostringstream buff;
	buff<<"Score: "<<n;
	return buff.str();
}

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());

        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////

		// Create a "close" menu item with close icon, it's an auto release object.
        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            this,
            menu_selector(HelloWorld::menuCloseCallback));
        CC_BREAK_IF(! pCloseItem);

			// Place the menu item bottom-right conner.
        pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));

			// Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);

			// Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);

			// Отобразить счет.
		score = 0;
		CCLabelTTF* scoreLabel = CCLabelTTF::create(ScoreString(0).c_str(), "Arial", 24);
        CC_BREAK_IF(! scoreLabel);
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        scoreLabel->setPosition(ccp(size.width - 70, size.height - 30));		// где-нибудь справа вверху
		scoreLabel->setTag(kTagScore);
        this->addChild(scoreLabel, 1);

			// Изображение на фоне
        CCSprite* background = CCSprite::create("background.png");
        CC_BREAK_IF(! background);
        background->setPosition(ccp(size.width/2, size.height/2));
        this->addChild(background, 0);

			// Спрайт солнца
		CCSprite* sunSprite = CCSprite::create("sun02_128x128.png");
		CC_BREAK_IF(! sunSprite);
		sunSprite->setScale(0.5f);		// Пока разрешение изображения великовато, используем масштабирование
		sunSprite->setPosition( ccp(size.width/2, size.height-sunSprite->getContentSize().height/2 *0.5) );
		sunSprite->setTag(kTagSun);
		this->addChild(sunSprite, 1);

			// Спрайт мяча
		CCSprite* ballSprite = CCSprite::create("ball_32x32.png");
		CC_BREAK_IF(! ballSprite);
		ballSprite->setPosition( ccp(150, 300) );
		this->addChild(ballSprite, 1);

			// Спрайт корзины
		/*CCSprite* basketSprite = CCSprite::create("Container.png");
		CC_BREAK_IF(! basketSprite);
		basketSprite->setPosition( ccp(size.width-100, basketSprite->getContentSize().height/2) );
		this->addChild(basketSprite);*/

			// Обрабатывать касания экрана
		setTouchEnabled(true);

		// Непонятно почему, но несмотря на то, что setTouchEnabled() регистрирует слой в диспетчере (читать "по идее должен"),
		// касания не воспринимаются (ccTouchBegan и далее не вызывается). Добавим заново.
		CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
		pDispatcher->addTargetedDelegate(this, 1, false);

		// Создание мира Box2D.
		b2Vec2 gravity;
		gravity.Set(0.0, -10.0);				// почти как в реальном мире

		_world = new b2World(gravity);
		_world->SetAllowSleeping(true);

			// Границы экрана для Box2D.
		b2BodyDef* edgesBodyDef = new b2BodyDef();
		edgesBodyDef->position.Set(0.0, 0.0);
		_edgesBody = _world->CreateBody(edgesBodyDef);
		delete edgesBodyDef;
		edgesBodyDef = NULL;

		b2EdgeShape* edge = new b2EdgeShape;
			// нижняя
		edge->Set( b2Vec2(0, 0), b2Vec2(size.width/PTM_RATIO, 0) );
		_edgesBody->CreateFixture(edge, 0);
			// верхняя
		edge->Set( b2Vec2(0, size.height/PTM_RATIO), b2Vec2(size.width/PTM_RATIO, size.height/PTM_RATIO) );
		_edgesBody->CreateFixture(edge, 0);
			// левая
		edge->Set( b2Vec2(0,0), b2Vec2(0, size.height/PTM_RATIO) );
		_edgesBody->CreateFixture(edge, 0);
			// правая
		edge->Set( b2Vec2(size.width/PTM_RATIO, 0), b2Vec2(size.width/PTM_RATIO, size.height/PTM_RATIO) );
		_edgesBody->CreateFixture(edge, 0);
		delete edge;
		edge = NULL;
		
			// Мяч - твердое тело, круглое
		b2CircleShape* circle = new b2CircleShape();
			// Српайт квадратный, поэтому можно считать, что половина его высоты есть радиус круга
		circle->m_radius      = ballSprite->getContentSize().height/2/PTM_RATIO;		

		b2BodyDef* ballBodyDef = new b2BodyDef();
		ballBodyDef->type      = b2_dynamicBody;
		ballBodyDef->position.Set( ballSprite->getPosition().x/PTM_RATIO, ballSprite->getPosition().y/PTM_RATIO );
		ballBodyDef->userData  = ballSprite;
		_ballBody			   = _world->CreateBody(ballBodyDef);

		b2FixtureDef* ballShape = new b2FixtureDef();
		ballShape->density      = 1.0f;
		ballShape->friction     = 0.1f;
		ballShape->restitution  = 0.7f;
		ballShape->shape        = circle;
		_ballBody->CreateFixture(ballShape);

		delete ballShape;
		delete ballBodyDef;
		ballBodyDef = NULL;
		ballShape   = NULL;

			// Корзина как цель для мяча - всего лишь пространство между двумя линиями,
			// непроницаемыми для мяча
		b2BodyDef* basketBodyDef = new b2BodyDef();
		basketBodyDef->position.SetZero();		// Временно
		


			// По умолчанию указатели на объекты Box2D != NULL
		mouseJoint = NULL;

		this->schedule( schedule_selector(HelloWorld::Tick) );

        bRet = true;
    } while (0);

    return bRet;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

void HelloWorld::Tick(float dt)
{
	int velocityIterations = 5;
	int positionIterations = 2;

	_world->Step(dt, velocityIterations, positionIterations);

		// Перебираем все объекты в мире и делаем анимацию движения тех,
		// к которым привязан какой-либо спрайт
	for(b2Body* b = _world->GetBodyList(); b; b = b->GetNext() )
	{
		if (b->GetUserData())
		{
			CCSprite* myActor = (CCSprite*)b->GetUserData();

			myActor->setPosition( CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) );
			myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );	// В Box2D и Cocos2d разные направления отсчета углов
		}
	}
}

bool HelloWorld::ccTouchBegan(CCTouch* pTouch, CCEvent* event)
{
		// Если mouseJoint уже есть, то нет смысла создавать еще одну
	if ( ! mouseJoint )
	{
			// Проверим, попало ли касание в область мяча.
		CCPoint location = pTouch->getLocation();
		b2Vec2 point     = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);		
		bool hit         = _ballBody->GetFixtureList()->TestPoint(point);

		if (hit)
		{
			CCSprite* crossMark = CCSprite::create("RedCross.png");
			crossMark->setTag(kTagCross);
			crossMark->setPosition(location);
			this->addChild(crossMark, 1);

				// Позиция мяча до оттягивания. Для вычисления возвращающей силы.
			b2Vec2 locInWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);
			prevLocationWorld = locInWorld;
			
				// Подогнать центр мяча в точку касания экрана
			float32 angle = _ballBody->GetAngle();
			_ballBody->SetTransform(point, angle);

			b2MouseJointDef* md = new b2MouseJointDef();
			md->bodyA        = _edgesBody;
			md->bodyB        = _ballBody;
			md->target       = locInWorld;
			md->dampingRatio = 1.0;
			md->maxForce	 = 50000;			// Без maxForce аномальное поведение

			mouseJoint = (b2MouseJoint*) _world->CreateJoint(md);
			delete md;
			md = NULL;

			return true;
		}
		else return false;	// В случае непопадания по мячу вызовы ccTouchMoved и Ended не нужны.
	}
	else return false;
}

void HelloWorld::ccTouchMoved(CCTouch* pTouch, CCEvent* event)
{
	if (! mouseJoint)	return;		

		// Мяч следует за пальцем/мышью
	CCPoint location  = pTouch->getLocation();
	b2Vec2 locInWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

		// Если попытаться утащить мяч за пределы экрана, то он "отваливается" от мыши.
	if (   (location.x < 1) || (location.x > screenSize.width-1) 
		|| (location.y < 1) || (location.y > screenSize.height-1) )
	{
		_world->DestroyJoint(mouseJoint);
		mouseJoint = NULL;
			// Здесь необходимо приложить импульс, иначе после обрыва связки
			// мяч может полететь дальше по инерции в направлении оттягивания.
		b2Vec2 linImpulse = b2Vec2(_ballBody->GetPosition().x, _ballBody->GetPosition().y);
		linImpulse	     -= prevLocationWorld;
		linImpulse       *= -3;							// Величина подобрана "на глаз"
		_world->ClearForces();
		_ballBody->SetLinearVelocity( b2Vec2(0,0) );
		_ballBody->ApplyLinearImpulse( linImpulse, b2Vec2(_ballBody->GetPosition().x, _ballBody->GetPosition().y) );
	}
	else
		mouseJoint->SetTarget(locInWorld);

	return;
}

void HelloWorld::ccTouchEnded(CCTouch* pTouch, CCEvent* event)
{
	this->removeChildByTag(kTagCross, true);

	if (mouseJoint){				
		_world->DestroyJoint(mouseJoint);
		_world->ClearForces();
		mouseJoint = NULL;

		b2Vec2 linImpulse = b2Vec2(_ballBody->GetPosition().x, _ballBody->GetPosition().y);
		linImpulse		 -= prevLocationWorld;
		linImpulse		 *= -3;						// Величина подобрана "на глаз"
		_world->ClearForces();
		_ballBody->SetLinearVelocity( b2Vec2(0,0) );
		_ballBody->ApplyLinearImpulse( linImpulse, b2Vec2(_ballBody->GetPosition().x, _ballBody->GetPosition().y) );
		
		// Действия, которые должны быть не здесь, но пусть пока побудут, как заготовки

			// Мигание солнца 3 раза
		CCBlink* sunBlink = CCBlink::create(1.2f, 3);
		CCSprite* sun	  = (CCSprite*) this->getChildByTag(kTagSun);
		sun->runAction(sunBlink);

			// Счет.
		score++;		// Защиты от переполнения нет.
		CCLabelTTF* scoreLabel = (CCLabelTTF*) this->getChildByTag(kTagScore);
		scoreLabel->setString( ScoreString(score).c_str() );
	}
}

HelloWorld::~HelloWorld()
{
	delete _world;
	_world = NULL;
		// Остальные указатели из private достаточно занулить.
		// Сами объекты уже уничтожены миром _world
	_edgesBody  = NULL;
	_ballBody   = NULL;
	//_basketBody = NULL;
	mouseJoint  = NULL;
}

