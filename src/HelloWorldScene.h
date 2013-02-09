#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "Box2D/Box2D.h"

using namespace cocos2d;

class HelloWorld : public cocos2d::CCLayer
{
public:
	// —оздание мира Box2D, изначальна€ расстановка спрайтов на слое, регистраци€ сло€ в TouchDispatcher'е
    virtual bool init(); 

    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

	// ќбновление мира Box2D, обновление положени€ св€занных с ним спрайтов каждый фрейм
	void Tick(float dt);

	bool ccTouchBegan(CCTouch* pTouch, CCEvent* event);
	void ccTouchMoved(CCTouch* pTouch, CCEvent* event);
	void ccTouchEnded(CCTouch* pTouch, CCEvent* event);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	// ”даление мира Box2D, зануление указателей
	~HelloWorld();
private:
	int score;
		// ¬се дл€ симул€ции мира Box2D
	b2World* _world;
	b2Body* _edgesBody;			// √раницы экрана, за которые м€ч не должен вылетать
	b2Body* _basketBody;		//  орзина - цель дл€ м€ча
	b2Body* _ballBody;			// ћ€ч - единственный управл€емый пользователем объект
	b2MouseJoint* mouseJoint;	// ѕрив€зка к мыши дл€ управлени€ м€чом (отт€гивание м€ча)
	//b2WeldJoint* weldJoint;
	b2Vec2 prevLocationWorld;	// ѕоложение м€ча до начала отт€гивани€. ƒл€ расчета силы.
};

#endif  // __HELLOWORLD_SCENE_H__